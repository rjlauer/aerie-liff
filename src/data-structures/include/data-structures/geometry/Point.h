/*!
 * @file Point.h
 * @brief Polar vector objects.
 * @author Segev BenZvi
 * @version $Id: Point.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_GEOMETRY_POINT_H_INCLUDED
#define DATACLASSES_GEOMETRY_POINT_H_INCLUDED

#include <data-structures/geometry/R3Vector.h>

class R3Transform;

/*!
 * @class Point
 * @ingroup geometry_data
 * @brief Object which can rotate/scale/reflect like a polar vector, and which
 *        can be translated with respect to a coordinate origin
 */
class Point : public R3Vector {

  public:

    Point(const R3Vector& v) : 
      R3Vector(v) { }

    /// Construct from a set of Cartesian coordinates x,y,z
    Point(const double x, const double y, const double z) :
      R3Vector(x, y, z) { }

    /// Construct from a set of parametric coordinates (cylindrical polar,
    /// spherical polar, etc.)
    Point(const double p1, const double p2, const double p3,
          const R3Vector::CoordinateSystem& coordSys) :
      R3Vector(p1, p2, p3, coordSys) { }

    Point() { }

    virtual ~Point() { }

    /// Transform a Point (rotate, scale, translate)
    Point& Transform(const R3Transform& matrix);

    /// Transform a Point (rotate, scale, translate)
    friend Point operator*(const R3Transform& matrix, const Point& p);

};

// Convenient public definitions
const Point ORIGIN(0, 0, 0);
const Point XAXIS(1, 0, 0);
const Point YAXIS(0, 1, 0);
const Point ZAXIS(0, 0, 1);

SHARED_POINTER_TYPEDEFS(Point);

#endif // DATACLASSES_GEOMETRY_POINT_H_INCLUDED

