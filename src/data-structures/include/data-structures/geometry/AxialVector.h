/*!
 * @file AxialVector.h
 * @brief Representation of pseudovectors.
 * @author Segev BenZvi
 * @version $Id: AxialVector.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef __AxialVector_h__
#define __AxialVector_h__

#include <data-structures/geometry/R3Vector.h>

class R3Transform;

/*!
 * @class AxialVector
 * @ingroup geometry_data
 * @brief Object which scales/rotates/reflects like a pseudovector
 */
class AxialVector : public R3Vector {

  public:

    AxialVector(const R3Vector& v) : 
      R3Vector(v) { }

    /// Construct from three Cartesian coordinates x,y,z
    AxialVector(const double x, const double y, const double z) :
      R3Vector(x, y, z) { }

    /// Construct from three parametric coordinates (cylindrical polar, 
    /// spherical polar, etc.)
    AxialVector(const double p1, const double p2, const double p3,
          const R3Vector::CoordinateSystem& coordSys) :
      R3Vector(p1, p2, p3, coordSys) { }

   ~AxialVector() { }

    /// Transform the vector (scale or rotate)
    AxialVector& Transform(const R3Transform& matrix);

    /// Transform the vector (scale or rotate)
    friend AxialVector operator*(const R3Transform& matrix, const AxialVector& v);

};

SHARED_POINTER_TYPEDEFS(AxialVector);

#endif // __AxialVector_h__

