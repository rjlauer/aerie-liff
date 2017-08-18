/*!
 * @file EfficientModuleSequentialMainLoop.cc 
 * @brief Implementation of a sequential loop of EfficientModule algorithms.
 * @author John Pretz 
 * @date 3 Mar 2010 
 * @version $Id: EfficientModuleSequentialMainLoop.cc 23073 2014-12-01 17:23:33Z sybenzvi $
 */

#include <hawcnest/processing/EfficientModuleSequentialMainLoop.h>
#include <hawcnest/Logging.h>
#include <hawcnest/Service.h>

using namespace std;

EfficientModuleSequentialMainLoop::EfficientModuleSequentialMainLoop()
{
}

Configuration
EfficientModuleSequentialMainLoop::DefaultConfiguration()
{
  Configuration config;
  config.Parameter<vector<string> >("modulechain");
  return config;
}

void
EfficientModuleSequentialMainLoop::Initialize(const Configuration& config)
{
  config.GetParameter("modulechain",moduleNames_);

  for (unsigned i = 0 ; i < moduleNames_.size() ; i++) {
    EfficientModulePtr module = GetService<EfficientModulePtr>(moduleNames_[i]);
    if (!module)
      log_fatal("couldn't find module with name " << moduleNames_[i]);
    modules_.push_back(module);
  }
}

void
EfficientModuleSequentialMainLoop::Execute(const MainLoop::Direction dir)
{
  if (modules_.size() == 0)
    log_fatal("no modules specified.");

  BagPtr bag = boost::make_shared<Bag>();
  for (unsigned i = 0 ; i < modules_.size() ; i++) {
    modules_[i]->TemplateBag(bag);
  }

  bool running = true;
  while (running) {

    try {
      for (unsigned i = 0 ; i < modules_.size() ; i++) {

        switch (modules_[i]->Process()) {

          case Module::Terminate: running = false;  // End Execute()
          case Module::Filter: i = modules_.size(); // End the loop
          case Module::Continue: break;
        }
      }

    } catch (const exception& e) {
      log_fatal_nothrow("error.... " << e.what());
      break;
    }
  }
}

