/*!
 * @file MultiSource.h
 * @brief Interface to merge/synchronize two or more streams of Source data.
 * @author Segev BenZvi
 * @date 29 Nov 2012
 * @version $Id: MultiSource.h 16640 2013-08-07 04:06:39Z pretz $
 */

#ifndef HAWCNEST_PROCESSING_MULTISOURCE_H_INCLUDED
#define HAWCNEST_PROCESSING_MULTISOURCE_H_INCLUDED

#include <hawcnest/processing/Bag.h>
#include <hawcnest/processing/Source.h>
#include <hawcnest/processing/Module.h>
#include <hawcnest/Configuration.h>

/*!
 * @class MultiSource
 * @author Segev BenZvi
 * @date 29 Nov 2012
 * @ingroup hawcnest_api
 * @brief An interface which pulls together data from multiple Source
 *        instances, passes them through a local Module sequence, and then
 *        outputs a Bag.  Convenient for merging or synchronizing two or more
 *        streams of data.
 */
class MultiSource : public Source {

  public:

    typedef Source Interface;
    typedef std::vector<std::string> SourceChain;
    typedef std::vector<std::string> ModuleChain;

    Configuration DefaultConfiguration();

    void Initialize(const Configuration& config);

    virtual BagPtr Next();

    /// Initialize the internal Module processing loop
    virtual void InitializeLoop() { }

    /// Setup before running the internal Module sequence 
    virtual bool PreLoop() { return true; }

    /// Cleanup after running the internal Module sequence
    virtual void PostLoop() { }

    /// True if there are cached data events to push to the global Bag
    virtual bool HasNext() { return false; }

    /// Get the next cached event to push to the global Bag
    virtual BagPtr GetNext() { return BagPtr(); }

  protected:

    SourceChain sourceNames_;           ///< List of Source names
    std::vector<SourcePtr> sources_;    ///< Source list

    ModuleChain moduleNames_;           ///< List of Module names
    std::vector<ModulePtr> modules_;    ///< Module list

    std::vector<BagPtr> bags_;          ///< Local Bag list (from Source list)

};


#endif // HAWCNEST_PROCESSING_MULTISOURCE_H_INCLUDED

