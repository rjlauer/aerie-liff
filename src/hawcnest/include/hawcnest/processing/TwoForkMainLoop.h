/*!
 * @file TwoForkMainLoop.h 
 * @brief A processing loop which churns through all events in a Source.
 * @author John Pretz 
 * @date 20 Oct 20014
 * @version $Id:$
 */

#ifndef HAWCNEST_TWOFORKMAINLOOP_H_INCLUDED
#define HAWCNEST_TWOFORKMAINLOOP_H_INCLUDED

#include <hawcnest/processing/MainLoop.h>
#include <hawcnest/processing/Source.h>
#include <hawcnest/processing/Module.h>
#include <hawcnest/processing/SignpostModule.h>
#include <hawcnest/Configuration.h>

/*!
 * @class TwoForkMainLoop
 * @author John Pretz
 * @ingroup hawcnest_api
 * @brief A MainLoop implementation in which the events traverse one of 
 * two forks, alternate reconstruction paths, for instance
 */
class TwoForkMainLoop : public MainLoop{

  public:
    typedef MainLoop Interface;

    typedef std::vector<std::string> ModuleChain;

    TwoForkMainLoop();

    virtual void Execute(const Direction dir=FORWARD);

    Configuration DefaultConfiguration();

    void Initialize(const Configuration& config);

  private:

    std::string sourceName_;
    ModuleChain preforkModuleNames_;
    ModuleChain fork1ModuleNames_;
    ModuleChain fork2ModuleNames_;
    ModuleChain postforkModuleNames_;

    SourcePtr source_;
    std::vector<ModulePtr> preforkModules_;
    std::vector<ModulePtr> fork1Modules_;
    std::vector<ModulePtr> fork2Modules_;
    std::vector<ModulePtr> postforkModules_;

    std::string signpostModuleName_;
    SignpostModulePtr signpostModule_;

    int updateFrequency_;
    int nBags_;
    int terminationLimit_;
};

#endif // HAWCNEST_SEQUENTIALMAINLOOP_H_INCLUDED

