/*!
 * @file WhileLoop.cc
 * @brief Processing loop over a set of modules inside another module chain.
 * @author Jim Braun
 * @date 20 Sep 2011
 * @version $Id: WhileLoop.cc 17766 2013-11-01 04:23:51Z sybenzvi $
 */

#include <hawcnest/processing/WhileLoop.h>
#include <hawcnest/RegisterService.h>

REGISTER_SERVICE(WhileLoop);

Configuration
WhileLoop::DefaultConfiguration()
{
  Configuration config;
  config.Parameter<std::vector<std::string> >("modulechain");
  return config;
}

void
WhileLoop::Initialize(const Configuration& config)
{
  config.GetParameter("modulechain", moduleNames_);

  for (unsigned i = 0; i < moduleNames_.size(); i++) {
    ModulePtr module = GetService<ModulePtr>(moduleNames_[i]);
    if (!module) {
      log_fatal("couldn't find module with name " << moduleNames_[i]);
    }
    modules_.push_back(module);
  }
}

Module::Result
WhileLoop::Process(BagPtr bag)
{
  // Initialize the loop -- take appropriate data from the Bag.
  InitializeLoop(bag);

  for (;;) {

    // Check for user-requested termination
    if (terminate_) {
      return Module::Terminate;
    }

    // Generate an empty local bag
    BagPtr localBag = boost::make_shared<Bag>();

    // Do pre-loop processing & control
    if (!PreLoop(bag, localBag)) {
      break;
    }

    // Do loop
    for (unsigned i = 0 ; i < modules_.size() ; i++) {
      log_trace("processing module named '" << moduleNames_[i] << "'");
      Module::Result result = modules_[i]->Process(localBag);
      if (result == Module::Continue){
        log_trace("continuing to the next module");
      }
      else if (result == Module::Filter) {
        log_trace("filtering event");
        break;
      }
      else if (result == Module::Terminate) {
        log_trace("Terminating event early");
        terminate_ = true;
      }
      else {
        log_warn("problem with module return result.  filtering event");
        break;
      }
    }
    log_trace("done processing this event");

    // Do pre-loop processing
    PostLoop(bag, localBag);
  }

  return Module::Continue;
}

