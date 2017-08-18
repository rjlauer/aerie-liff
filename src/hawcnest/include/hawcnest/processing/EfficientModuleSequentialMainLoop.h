/*!
 * @file EfficientModuleSequentialMainLoop.h 
 * @brief A processing loop based which used EfficientModule slots.
 * @author John Pretz 
 * @date 3 Mar 2010 
 * @version $Id: EfficientModuleSequentialMainLoop.h 23073 2014-12-01 17:23:33Z sybenzvi $
 */

#ifndef HAWCNEST_EFFICIENTMODULESEQUENTIALMAINLOOP_H_INCLUDED
#define HAWCNEST_EFFICIENTMODULESEQUENTIALMAINLOOP_H_INCLUDED

#include <hawcnest/processing/MainLoop.h>
#include <hawcnest/processing/EfficientModule.h>
#include <hawcnest/Configuration.h>

class EfficientModuleSequentialMainLoop : public MainLoop{
 public:
  typedef MainLoop Interface;

  EfficientModuleSequentialMainLoop();

  virtual void Execute(const Direction dir=FORWARD);

  Configuration DefaultConfiguration();

  void Initialize(const Configuration& config);

 private:
  std::vector<std::string> moduleNames_;
  std::vector<EfficientModulePtr> modules_;

};

#endif // HAWCNEST_EFFICIENTMODULESEQUENTIALMAINLOOP_H_INCLUDED

