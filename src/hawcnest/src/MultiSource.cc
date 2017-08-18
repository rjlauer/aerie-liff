/*!
 * @file MultiSource.cc
 * @brief Interface to merge/synchronize two or more streams of Source data.
 * @author Segev BenZvi
 * @date 29 Nov 2012
 * @version $Id: MultiSource.cc 16640 2013-08-07 04:06:39Z pretz $
 */

#include <hawcnest/processing/MultiSource.h>

#include <hawcnest/HAWCNest.h>
#include <hawcnest/Logging.h>
#include <hawcnest/RegisterService.h>

using namespace std;

REGISTER_SERVICE(MultiSource);

Configuration
MultiSource::DefaultConfiguration()
{
  Configuration c;
  c.Parameter<SourceChain>("sourcechain");
  c.Parameter<SourceChain>("modulechain", ModuleChain());
  return c;
}

void
MultiSource::Initialize(const Configuration& c)
{
  c.GetParameter<SourceChain>("sourcechain", sourceNames_);
  c.GetParameter<ModuleChain>("modulechain", moduleNames_);

  // Set up Source list
  for (size_t i = 0; i < sourceNames_.size(); ++i) {
    SourcePtr s = GetService<SourcePtr>(sourceNames_[i]);
    if (!s)
      log_fatal("Could not find source with name " << sourceNames_[i]);
    sources_.push_back(s);
  }

  // Set up Module list
  for (size_t i = 0; i < moduleNames_.size(); ++i) {
    ModulePtr m = GetService<ModulePtr>(moduleNames_[i]);
    if (!m)
      log_fatal("Could not find module with name " << moduleNames_[i]);
    modules_.push_back(m);
  }
}

BagPtr
MultiSource::Next()
{
  // Get any cached events
  if (HasNext())
    return GetNext();

  // Initialize the processing loop
  InitializeLoop();

  if (!PreLoop())
    return BagPtr();

  // Process data from every Source
  for (size_t i = 0; i < bags_.size(); ++i) {
    BagPtr& localBag = bags_[i];
    if (localBag == BagPtr())
      continue;
    
    for (size_t j = 0; j < modules_.size(); ++j) {
      log_trace("Processing module '" << moduleNames_[i] << "'");
      Module::Result result = modules_[j]->Process(localBag);
      if (result == Module::Continue) {
        log_trace("Continuing to next module");
      }
      else if (result == Module::Filter) {
        log_trace("Filtering event");
        break;
      }
      else if (result == Module::Terminate) {
        log_trace("Terminating event early");
        return BagPtr();
      }
      else {
        log_warn("Problem with module output.  Filtering event.");
        break;
      }
    }
    log_trace("Done processing bag; " << *localBag);
  }

  // Clean up and move to the next event
  PostLoop();
  return GetNext();
}

