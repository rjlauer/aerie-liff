/*!
 * @file PhysicsConstants.h
 * @brief A list of constants derived from the AERIE base units.
 * @author Segev BenZvi
 * @version $Id: PhysicsConstants.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATA_STRUCTURES_PHYSICSCONSTANTS_H_INCLUDED
#define DATA_STRUCTURES_PHYSICSCONSTANTS_H_INCLUDED

#include <hawcnest/HAWCUnits.h>

/*!
 * @namespace PhysicsConstants
 * @brief List of physical constants derived from the AERIE base units.
 */
namespace PhysicsConstants {

  using namespace HAWCUnits;

  // Expand the X-macro definitions in constant_defs.h to provide a list of
  // constants of form "const double name = value;"
  #define X(name, value, desc) static const double name = value;
  #include <data-structures/physics/constant_defs.h>
  #undef X

}

#endif // DATA_STRUCTURES_PHYSICSCONSTANTS_H_INCLUDED

