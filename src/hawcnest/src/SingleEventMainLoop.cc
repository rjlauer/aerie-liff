/*!
 * @file SingleEventMainLoop.cc 
 * @brief Implementation of the single datum processing loop class.
 * @author Segev BenZvi
 * @date 22 Aug 2009 
 * @version $Id: SingleEventMainLoop.cc 23075 2014-12-01 18:27:32Z sybenzvi $
 */

#include <hawcnest/processing/SingleEventMainLoop.h>
#include <hawcnest/RegisterService.h>
#include <hawcnest/Logging.h>
#include <stdexcept>
#include <hawcnest/Service.h>
#include <hawcnest/processing/Module.h>
#include <hawcnest/processing/Source.h>
#include <signal.h>

using namespace std;

REGISTER_SERVICE(SingleEventMainLoop);

SingleEventMainLoop::SingleEventMainLoop(){}

Configuration SingleEventMainLoop::DefaultConfiguration(){
  // parameters for the list of modules to execute and the 
  // name of the one source to start things

  Configuration config;
  config.Parameter<vector<string> >("modulechain");
  config.Parameter<string>("source");
  return config;
}

void SingleEventMainLoop::Initialize(const Configuration& config){
  // retrieve the list of modules and the source that we're
  // configured to use

  config.GetParameter("source",sourceName_);
  config.GetParameter("modulechain",moduleNames_);

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
bool& SingleEventMainLoop_termination_flag(){
  static bool terminate = false;
  return terminate;
}

// global bool for flagging when the user issues a early termination signal
// needs to be global because the signal function is just a straight c function
// and can't have any state
bool& SingleEventMainLoop_early_termination_flag(){
  static bool early_terminate = false;
  return early_terminate;
}
 


// the signal function. passed to the unix signal(...) to terminate
// the event processing when the user sends SIGINT (Ctl-C)
// note that the current event when interrupted will finish processing
// to avoid putting modules in funny states
void SingleEventMainLoop_terminate(int signal){
  log_info("Terminating main loop early because we received a signal " << signal);
   SingleEventMainLoop_termination_flag() = true;
}

void
SingleEventMainLoop::Execute(const MainLoop::Direction dir)
{
  // set up termination signal
  signal(SIGINT, SingleEventMainLoop_terminate);
  SingleEventMainLoop_termination_flag() = false;

  // Default is to loop through all events
  SingleEventMainLoop_early_termination_flag() = false;

  if (!source_)
    log_fatal("no source specified.  aborting");
  if (modules_.size() == 0)
    log_fatal("no modules specified.  ");

  // Use the source to retrieve an event, then execute modules 
  // in order until the module gives 'filter' or the source
  // doesn't have any more events
  log_trace("getting event from source named '"<<sourceName_<<"'");

  // if the termination flag has been flipped since processing the last event
  // then go ahead and just stop processing
  if (SingleEventMainLoop_termination_flag()) {
    log_info("terminating loop early because it was interrupted by the user");
    return;
  }

  // if the early termination flag has been flipped since processing
  // the last event then go ahead and just stop processing...
  if (SingleEventMainLoop_early_termination_flag()) {
    log_trace("terminating loop early as requested by the user");
    return;
  }
   
  // ...otherwise check for a new event:
  BagPtr event;
  switch (dir) {
    case REVERSE:
      event = source_->Previous();
      break;
    case FORWARD:   // intentional fall-through; default is forward iteration
    default:
      event = source_->Next();
      break;
  }

  if (!event) {
    log_trace("Done processing events");
    return;
  }

  for (unsigned i = 0 ; i < modules_.size() ; i++) {
    log_trace("processing module named '"<<moduleNames_[i]<<"'");
    lastResult_ = modules_[i]->Process(event);
    if (lastResult_ == Module::Continue){
      log_trace("continuing to the next module");
    }
    else if (lastResult_ == Module::Filter) {
      log_trace("filtering event");
      return;
    }
    else if (lastResult_ == Module::Terminate) {
      log_trace("Terminating event early");
      SingleEventMainLoop_early_termination_flag() = true;
    }
    else {
      log_warn("problem with module return result.  filtering event");
      return;
    }
  }
  log_trace("done processing this event");
}

