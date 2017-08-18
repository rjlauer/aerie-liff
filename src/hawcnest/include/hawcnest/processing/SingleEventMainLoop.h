/*!
 * @file SingleEventMainLoop.h 
 * @brief A processing loop which handles just one event from a Source.
 * @author Segev BenZvi
 * @date 19 Jan 2011
 * @version $Id: SingleEventMainLoop.h 23073 2014-12-01 17:23:33Z sybenzvi $
 */

#ifndef HAWCNEST_SINGLEEVENTMAINLOOP_H_INCLUDED
#define HAWCNEST_SINGLEEVENTMAINLOOP_H_INCLUDED

#include <hawcnest/processing/MainLoop.h>
#include <hawcnest/processing/Source.h>
#include <hawcnest/processing/Module.h>
#include <hawcnest/Configuration.h>

/*!
 * @class SingleEventMainLoop
 * @author Segev BenZvi
 * @ingroup hawcnest_api
 * @brief A class which runs an execution loop on just one event at a time
 */
class SingleEventMainLoop : public MainLoop{

  public:
    typedef MainLoop Interface;

    typedef std::vector<std::string> ModuleChain;

    SingleEventMainLoop();

    virtual void Execute(const Direction dir=FORWARD);

    Configuration DefaultConfiguration();

    void Initialize(const Configuration& config);
  
  private:

    std::string sourceName_;
    ModuleChain moduleNames_;
    SourcePtr source_;
    std::vector<ModulePtr> modules_;

};

#endif // HAWCNEST_SINGLEEVENTMAINLOOP_H_INCLUDED

