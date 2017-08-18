/*!
 * @file Module.h 
 * @brief A generic interface for processing modules.
 * @author John Pretz 
 * @date 25 Aug 2008 
 * @version $Id: Module.h 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#ifndef HAWCNEST_PROCESSING_MODULE_H_INCLUDED
#define HAWCNEST_PROCESSING_MODULE_H_INCLUDED

#include <hawcnest/processing/Bag.h>

/*!
 * @author John Pretz
 * @brief A unique kind of service which is intended to work 
 *        on the event stream
 * @ingroup hawcnest_api
 */
class Module {

  public:

    enum Result{
      Continue = 0,
      Filter = 1,
      Terminate = 2,
    };

    virtual ~Module() { }

    virtual Result Process(BagPtr b) = 0;

};

SHARED_POINTER_TYPEDEFS(Module);

#endif // HAWCNEST_PROCESSING_MODULE_H_INCLUDED

