/*!
 * @file SignpostModule.h 
 * @brief A generic interface for processing modules.
 * @author John Pretz 
 * @date 20 Oct 2014
 * @version $Id:$
 */

#ifndef HAWCNEST_PROCESSING_SIGNPOSTMODULE_H_INCLUDED
#define HAWCNEST_PROCESSING_SIGNPOSTMODULE_H_INCLUDED

#include <hawcnest/processing/Bag.h>

/*!
 * @author John Pretz
 * @brief A specialized module which makes a decision which fork an event should
 * go down. Utilized, for instance, by the TwoForkMainLoop
 * @ingroup hawcnest_api
 */
class SignpostModule {

  public:
    virtual ~SignpostModule() { }

    virtual int Direction(BagPtr b) = 0;

};

SHARED_POINTER_TYPEDEFS(SignpostModule);

#endif // HAWCNEST_PROCESSING_SIGNPOSTMODULE_H_INCLUDED

