/*!
 * @file TwoForkMainLoop.cc 
 * @brief Implementation of the sequential data processing loop class.
 * @author John Pretz 
 * @date 20 Oct 2014
 * @version $Id:$
 */

#include <hawcnest/processing/TwoForkMainLoop.h>
#include <hawcnest/RegisterService.h>
#include <hawcnest/Logging.h>
#include <stdexcept>
#include <hawcnest/Service.h>
#include <hawcnest/processing/Module.h>
#include <hawcnest/processing/Source.h>
#include <signal.h>

using namespace std;

REGISTER_SERVICE(TwoForkMainLoop);

TwoForkMainLoop::TwoForkMainLoop() : updateFrequency_(10000),
                                           nBags_(0),
                                           terminationLimit_(-1){
}

Configuration TwoForkMainLoop::DefaultConfiguration(){
  // parameters for the list of modules to execute and the 
  // name of the one source to start things

  Configuration config;
  config.Parameter<vector<string> >("preforkmodulechain");
  config.Parameter<vector<string> >("postforkmodulechain");
  config.Parameter<string>("signpost");
  config.Parameter<vector<string> >("fork1modulechain");
  config.Parameter<vector<string> >("fork2modulechain");
  config.Parameter<string>("source");
  config.Parameter<int>("updateFrequency",updateFrequency_);
  config.Parameter<int>("terminationLimit",terminationLimit_);
  return config;
}

void TwoForkMainLoop::Initialize(const Configuration& config){
  // retrieve the list of modules and the source that we're
  // configured to use

  config.GetParameter("source",sourceName_);
  config.GetParameter("preforkmodulechain",preforkModuleNames_);
  config.GetParameter("postforkmodulechain",postforkModuleNames_);
  config.GetParameter("signpost",signpostModuleName_);
  config.GetParameter("fork1modulechain",fork1ModuleNames_);
  config.GetParameter("fork2modulechain",fork2ModuleNames_);

  config.GetParameter("updateFrequency",updateFrequency_);
  config.GetParameter("terminationLimit",terminationLimit_);

  source_ = GetService<SourcePtr>(sourceName_);
  if (!source_)
    log_fatal("no source specified.  aborting");

  signpostModule_ = GetService<SignpostModulePtr>(signpostModuleName_);
  if (!signpostModule_)
    log_fatal("no signpost module specified.  aborting");

  // All the pre-fork modules
  for (unsigned i = 0 ; i < preforkModuleNames_.size() ; i++) {
    ModulePtr module = GetService<ModulePtr>(preforkModuleNames_[i]);
    if (!module)
      log_fatal("couldn't find module with name " << preforkModuleNames_[i]);
    preforkModules_.push_back(module);
  }

  // All the post-fork modules
  for (unsigned i = 0 ; i < postforkModuleNames_.size() ; i++) {
    ModulePtr module = GetService<ModulePtr>(postforkModuleNames_[i]);
    if (!module)
      log_fatal("couldn't find module with name " << postforkModuleNames_[i]);
    postforkModules_.push_back(module);
  }

  // All the fork1 modules
  for (unsigned i = 0 ; i < fork1ModuleNames_.size() ; i++) {
    ModulePtr module = GetService<ModulePtr>(fork1ModuleNames_[i]);
    if (!module)
      log_fatal("couldn't find module with name " << fork1ModuleNames_[i]);
    fork1Modules_.push_back(module);
  }

  // All the fork2 modules
  for (unsigned i = 0 ; i < fork2ModuleNames_.size() ; i++) {
    ModulePtr module = GetService<ModulePtr>(fork2ModuleNames_[i]);
    if (!module)
      log_fatal("couldn't find module with name " << fork2ModuleNames_[i]);
    fork2Modules_.push_back(module);
  }

}






// global bool for flagging when a signal to terminate the looop is caught
// needs to be global because the signal function is just a straight c function
// and can't have any state
bool& TwoForkMainLoop_termination_flag(){
  static bool terminate = false;
  return terminate;
}

// global bool for flagging when the user issues a early termination signal
// needs to be global because the signal function is just a straight c function
// and can't have any state
bool& TwoForkMainLoop_early_termination_flag(){
  static bool early_terminate = false;
  return early_terminate;
}
 
// the signal function. passed to the unix signal(...) to terminate
// the event processing when the user sends SIGINT (Ctl-C)
// note that the current event when interrupted will finish processing
// to avoid putting modules in funny states
void TwoForkMainLoop_terminate(int signal){
  log_info("Terminating main loop early because we received a signal " << signal);
   TwoForkMainLoop_termination_flag() = true;
}

void
TwoForkMainLoop::Execute(const MainLoop::Direction dir)
{
  // set up termination signal
  signal(SIGINT,TwoForkMainLoop_terminate);
  TwoForkMainLoop_termination_flag() = false;

  //Default is to loop through all events
  TwoForkMainLoop_early_termination_flag() = false;


  if (!source_)
    log_fatal("no source specified.  aborting");



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
    if(TwoForkMainLoop_termination_flag()){
      log_info("terminating loop early because it was interrupted by the user");
      break;
    }

    // if the early termination flag has been flipped since processing
    // the last event then go ahead and just stop processing
    if(TwoForkMainLoop_early_termination_flag()){
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

    // The pre-fork modules
    log_trace("processing pre-fork modules");
    for (unsigned i = 0 ; i < preforkModules_.size() ; i++) {
      log_trace("processing module named '"<<preforkModuleNames_[i]<<"'");
      Module::Result result = preforkModules_[i]->Process(event);
      if (result == Module::Continue){
        log_trace("continuing to the next module");
      }
      else if (result == Module::Filter) {
        log_trace("filtering event");
        break;
      }
      else if (result == Module::Terminate) {
          log_trace("Terminating event early");
          TwoForkMainLoop_early_termination_flag() = true;
      }
      else {
        log_warn("problem with module return result.  filtering event");
        break;
      }
    }

    // The decision
    int forkNum = signpostModule_->Direction(event);
    ModuleChain* chosenForkNames = NULL;
    vector<ModulePtr>* chosenFork = NULL;
    if(forkNum == 1){
      chosenForkNames = &fork1ModuleNames_;
      chosenFork = &fork1Modules_;
      log_trace("Event goes down fork 1");
    }
    else if(forkNum == 2){
      chosenForkNames = &fork2ModuleNames_;
      chosenFork = &fork2Modules_;
      log_trace("Event goes down fork 2");
    }
    else{
      log_fatal("TwoForkMainLoop can only handle forks number 1 or 2. Got "<<
                forkNum<<
                " from SignpostModule "<<
                signpostModuleName_);
    }

    // The fork modules
    log_trace("processing fork "<<forkNum<<" modules");
    for (unsigned i = 0 ; i < chosenFork->size() ; i++) {
      log_trace("processing module named '"<<(*chosenForkNames)[i]<<"'");
      Module::Result result = (*chosenFork)[i]->Process(event);
      if (result == Module::Continue){
        log_trace("continuing to the next module");
      }
      else if (result == Module::Filter) {
        log_trace("filtering event");
        break;
      }
      else if (result == Module::Terminate) {
          log_trace("Terminating event early");
          TwoForkMainLoop_early_termination_flag() = true;
      }
      else {
        log_warn("problem with module return result.  filtering event");
        break;
      }
    }

    // The post-fork modules
    log_trace("processing post-fork modules");
    for (unsigned i = 0 ; i < postforkModules_.size() ; i++) {
      log_trace("processing module named '"<<postforkModuleNames_[i]<<"'");
      Module::Result result = postforkModules_[i]->Process(event);
      if (result == Module::Continue){
        log_trace("continuing to the next module");
      }
      else if (result == Module::Filter) {
        log_trace("filtering event");
        break;
      }
      else if (result == Module::Terminate) {
          log_trace("Terminating event early");
          TwoForkMainLoop_early_termination_flag() = true;
      }
      else {
        log_warn("problem with module return result.  filtering event");
        break;
      }
    }

    log_trace("done processing this event");
  }

}
