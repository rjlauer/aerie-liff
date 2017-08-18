/*!
 * @file Source.h 
 * @brief Generic interface for a source of data for processing.
 * @author John Pretz 
 * @date 25 Aug 2008 
 * @version $Id: Source.h 23073 2014-12-01 17:23:33Z sybenzvi $
 */

#ifndef HAWCNEST_SOURCE_H_INCLUDED
#define HAWCNEST_SOURCE_H_INCLUDED

#include <hawcnest/processing/Bag.h>
#include <boost/shared_ptr.hpp>

/*!
 * @author John Pretz
 * @ingroup hawcnest_api
 * @brief A unique kind of service that provides a stream of events
 */
class Source {

  public:

    virtual ~Source() { }

    /// Get next Bag in data stream; should return empty pointer when done
    virtual BagPtr Next() = 0;

    /// Get previous Bag in data stream; defaults to empty pointer
    virtual BagPtr Previous()
    { return BagPtr(); }

};

SHARED_POINTER_TYPEDEFS(Source);

#endif // HAWCNEST_SOURCE_H_INCLUDED

