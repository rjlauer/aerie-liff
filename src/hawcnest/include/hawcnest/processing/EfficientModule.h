/*!
 * @file EfficientModule.h 
 * @brief Interface for "efficient" modules which cache memory.
 * @author John Pretz 
 * @date 2 Mar 2010 
 * @version $Id: EfficientModule.h 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#ifndef HAWCNEST_EFFICIENTMODULE_H_INCLUDED
#define HAWCNEST_EFFICIENTMODULE_H_INCLUDED

#include <hawcnest/processing/Bag.h>
#include <hawcnest/processing/Module.h>

class EfficientModule {

  public:

    virtual ~EfficientModule() { }

    virtual Module::Result Process() = 0;

    virtual void TemplateBag(BagPtr bag) = 0;

};

SHARED_POINTER_TYPEDEFS(EfficientModule);

#endif // HAWCNEST_EFFICIENTMODULE_H_INCLUDED

