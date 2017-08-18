/*!
 * @file SequentialMainLoop.cc 
 * @brief Implementation of the sequential data processing loop class.
 * @author John Pretz 
 * @date 22 Aug 2009 
 * @version $Id: SequentialMainLoop.cc 24652 2015-03-27 20:42:38Z criviere $
 */

#include <hawcnest/processing/SequentialMainLoop.h>
#include <hawcnest/RegisterService.h>
#include <hawcnest/Logging.h>
#include <stdexcept>
#include <hawcnest/Service.h>
#include <hawcnest/processing/Module.h>
#include <hawcnest/processing/Source.h>
#include <signal.h>

using namespace std;

REGISTER_SERVICE(SequentialMainLoop);

SequentialMainLoop::SequentialMainLoop() : updateFrequency_(10000),
                                           nBags_(0),
                                           terminationLimit_(-1){
}

Configuration SequentialMainLoop::DefaultConfiguration(){
  // parameters for the list of modules to execute and the 
  // name of the one source to start things

  Configuration config;
  config.Parameter<vector<string> >("modulechain");
  config.Parameter<string>("source");
  config.Parameter<int>("updateFrequency",updateFrequency_);
  config.Parameter<int>("terminationLimit",terminationLimit_);
  return config;
}

void SequentialMainLoop::Initialize(const Configuration& config){
  // retrieve the list of modules and the source that we're
  // configured to use

  config.GetParameter("source",sourceName_);
  config.GetParameter("modulechain",moduleNames_);
  config.GetParameter("updateFrequency",updateFrequency_);
  config.GetParameter("terminationLimit",terminationLimit_);

  source_ = GetService<SourcePtr>(sourceName_);
  if (!source_)
    log_fatal("no source specified.  aborting");
  for (unsigned i = 0 ; i < moduleNames_.size() ; i++) {
    ModulePtr module = GetService<ModulePtr>(moduleNames_[i]);
    if (!module)
      log_fatal("couldn't find module with name " << moduleNames_[i]);
    modules_.push_back(module);
  }
}

// global bool for flagging when a signal to terminate the looop is caught
// needs to be global because the signal function is just a straight c function
// and can't have any state
bool& SequentialMainLoop_termination_flag(){
  static bool terminate = false;
  return terminate;
}

// global bool for flagging when the user issues a early termination signal
// needs to be global because the signal function is just a straight c function
// and can't have any state
bool& SequentialMainLoop_early_termination_flag(){
  static bool early_terminate = false;
  return early_terminate;
}
 


// the signal function. passed to the unix signal(...) to terminate
// the event processing when the user sends SIGINT (Ctl-C)
// note that the current event when interrupted will finish processing
// to avoid putting modules in funny states
void SequentialMainLoop_terminate(int signal){
  log_info("Terminating main loop early because we received a signal " << signal);
   SequentialMainLoop_termination_flag() = true;
}

void
SequentialMainLoop::Execute(const MainLoop::Direction dir)
{
  // set up termination signal
  signal(SIGINT,SequentialMainLoop_terminate);
  SequentialMainLoop_termination_flag() = false;

  //Default is to loop through all events
  SequentialMainLoop_early_termination_flag() = false;


  if (!source_)
    log_fatal("no source specified.  aborting");
  if (modules_.size() == 0)
    log_fatal("no modules specified.  ");

  // Use the source to retrieve an event, then execute modules 
  // in order until the module gives 'filter' or the source
  // doesn't have any more events
  while (1) {
    log_trace("getting event from source named '"<<sourceName_<<"'");

    if(nBags_ >= terminationLimit_ && terminationLimit_ > 0){
      log_info("terminating loop because we reached the "
               "termination limit of "<<terminationLimit_);
      break;
    }

    if(nBags_ % updateFrequency_ == 0){
      log_info("processing bag number "<<nBags_);
    }

    // if the termination flag has been flipped since processing the last event
    // then go ahead and just stop processing
    if(SequentialMainLoop_termination_flag()){
      log_info("terminating loop early because it was interrupted by the user");
      break;
    }

    // if the early termination flag has been flipped since processing
    // the last event then go ahead and just stop processing
    if(SequentialMainLoop_early_termination_flag()){
      log_trace("terminating loop early as requested by the user");
      break;
    }

    // otherwise check for a new event
    BagPtr event = source_->Next();
    nBags_ = nBags_ + 1;
    if (!event) {
      log_trace("Done processing events");
      break;
    }
    for (unsigned i = 0 ; i < modules_.size() ; i++) {
      log_trace("processing module named '"<<moduleNames_[i]<<"'");
      lastResult_ = modules_[i]->Process(event);
      if (lastResult_ == Module::Continue){
        log_trace("continuing to the next module");
      }
      else if (lastResult_ == Module::Filter) {
        log_trace("filtering event");
        break;
      }
      else if (lastResult_ == Module::Terminate) {
          log_trace("Terminating event early");
          SequentialMainLoop_early_termination_flag() = true;
      }
      else {
        log_warn("problem with module return result.  filtering event");
        break;
      }
    }
    log_trace("done processing this event");
  }
}
