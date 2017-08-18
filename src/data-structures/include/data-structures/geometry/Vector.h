/*!
 * @file Vector.h
 * @brief Representation of polar vectors.
 * @author Segev BenZvi
 * @version $Id: Vector.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_GEOMETRY_VECTOR_H_INCLUDED
#define DATACLASSES_GEOMETRY_VECTOR_H_INCLUDED

#include <data-structures/geometry/R3Vector.h>

class R3Transform;

/*!
 * @class Vector
 * @ingroup geometry_data
 * @brief Object which scales/rotates/reflects like a polar vector
 */
class Vector : public R3Vector {

  public:

    Vector() :
      R3Vector() { }

    Vector(const R3Vector& v) : 
      R3Vector(v) { }

    /// Construct using Cartesian coordinates x,y,z
    Vector(const double x, const double y, const double z) :
      R3Vector(x, y, z) { }

    /// Construct using parametric coordinates (cylindrical, spherical, etc.)
    Vector(const double p1, const double p2, const double p3,
          const R3Vector::CoordinateSystem& coordSys) :
      R3Vector(p1, p2, p3, coordSys) { }

   ~Vector() { }

    /// Transform the Vector (scale or rotate)
    Vector& Transform(const R3Transform& matrix);

    /// Transform the Vector (scale or rotate)
    friend Vector operator*(const R3Transform& matrix, const Vector& v);

};

SHARED_POINTER_TYPEDEFS(Vector);

#endif // DATACLASSES_GEOMETRY_VECTOR_H_INCLUDED

