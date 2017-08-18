/*!
 * @file SequentialMainLoop.h 
 * @brief A processing loop which churns through all events in a Source.
 * @author John Pretz 
 * @date 22 Aug 2009 
 * @version $Id: SequentialMainLoop.h 23073 2014-12-01 17:23:33Z sybenzvi $
 */

#ifndef HAWCNEST_SEQUENTIALMAINLOOP_H_INCLUDED
#define HAWCNEST_SEQUENTIALMAINLOOP_H_INCLUDED

#include <hawcnest/processing/MainLoop.h>
#include <hawcnest/processing/Source.h>
#include <hawcnest/processing/Module.h>
#include <hawcnest/Configuration.h>

/*!
 * @class SequentialMainLoop
 * @author John Pretz
 * @ingroup hawcnest_api
 * @brief A class which is the main execution loop. Retrieves a list
 * of Module services and one Source service and executes them in 
 * order.
 */
class SequentialMainLoop : public MainLoop{

  public:
    typedef MainLoop Interface;

    typedef std::vector<std::string> ModuleChain;

    SequentialMainLoop();

    virtual void Execute(const Direction dir=FORWARD);

    Configuration DefaultConfiguration();

    void Initialize(const Configuration& config);

  private:

    std::string sourceName_;
    ModuleChain moduleNames_;
    SourcePtr source_;
    std::vector<ModulePtr> modules_;

    int updateFrequency_;
    int nBags_;
    int terminationLimit_;
};

#endif // HAWCNEST_SEQUENTIALMAINLOOP_H_INCLUDED

