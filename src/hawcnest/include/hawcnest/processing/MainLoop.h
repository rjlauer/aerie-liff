/*!
 * @file MainLoop.h 
 * @brief Interface for the module processing loop.
 * @author John Pretz 
 * @date 22 Aug 2009 
 * @version $Id: MainLoop.h 23073 2014-12-01 17:23:33Z sybenzvi $
 */

#ifndef HAWCNEST_MAINLOOP_H_INCLUDED
#define HAWCNEST_MAINLOOP_H_INCLUDED

#include <hawcnest/PointerTypedefs.h>
#include <hawcnest/processing/Module.h>

/*!
 * @class MainLoop
 * @author John Pretz
 * @ingroup hawcnest_api
 * @brief A class which is the main execution loop. Very little more than
 * a hook to 'do your thing'.
 */
class MainLoop {

  public:

    // Enable bi-directional iteration through a data stream
    enum Direction {
      FORWARD,
      REVERSE
    };

    // This is the function that executes the main loop.  Derived
    // classes must decide exactly what the main execution loop does.
    virtual void Execute(const Direction dir=FORWARD) = 0;

    virtual ~MainLoop(){}

    Module::Result GetLastResult(void) const {return lastResult_;}
  
  protected:

    Module::Result lastResult_;

};

SHARED_POINTER_TYPEDEFS(MainLoop);

#endif // HAWCNEST_MAINLOOP_H_INCLUDED

