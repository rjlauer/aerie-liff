/*!
 * @file WhileLoop.h
 * @brief Processing loop over a set of modules inside another module chain.
 * @author Jim Braun
 * @date 20 Sep 2011
 * @version $Id: WhileLoop.h 17766 2013-11-01 04:23:51Z sybenzvi $
 */

#ifndef HAWCNEST_WHILELOOP_H_INCLUDED
#define HAWCNEST_WHILELOOP_H_INCLUDED

#include <hawcnest/HAWCNest.h>

/*!
 * @class WhileLoop
 * @author Jim Braun
 * @ingroup hawcnest_api
 * @brief A class which loops over a given set of modules, and can be
 * embedded within other processing loops. Modules added to the loop will have
 * a fresh local copy of the Bag.  Loop control as well as access between the
 * local and global Bags is provided by virtual Pre/PostLoop calls.
 * @todo SequentialMainLoop should be a child of this class
 */
class WhileLoop : public Module {

  public:

    typedef Module Interface;

    WhileLoop() : terminate_(false) { }

    virtual ~WhileLoop() { }

    Configuration DefaultConfiguration();

    void Initialize(const Configuration& config);

    Module::Result Process(BagPtr bag);

    virtual void Execute() {
      BagPtr bag = boost::make_shared<Bag>();
      Process(bag);
    }

    /// Initial access to provide initialization from Bag data.
    virtual void InitializeLoop(BagPtr bag) { }

    /// Pre-loop call to set up the local bag from the global Bag.
    /// Returning 'false' will end the loop.
    virtual bool PreLoop(BagPtr bag, BagPtr localBag) {return true;}

    /// Post-loop call intended to remove data from the local bag.
    virtual void PostLoop(BagPtr bag, BagPtr localBag) { }

  protected:

    std::vector<std::string> moduleNames_;
    std::vector<ModulePtr> modules_;

    bool terminate_;

};


#endif // HAWCNEST_WHILELOOP_H_INCLUDED
