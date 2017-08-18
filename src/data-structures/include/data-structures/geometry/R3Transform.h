/*!
 * @file R3Transform.h
 * @brief Vector transformation classes.
 * @author Segev BenZvi
 * @version $Id: R3Transform.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_GEOMETRY_R3TRANSFORM_H_INCLUDED
#define DATACLASSES_GEOMETRY_R3TRANSFORM_H_INCLUDED

#include <hawcnest/processing/Bag.h>

class Scale;
class Rotate;
class Translate;

class Point;
class Vector;

/*!
 * @class R3Transform
 * @ingroup geometry_data
 * @brief Generic transformation in R3.  Interface from CLHEP
 */
class R3Transform : public virtual Baggable {
  
  public:

    R3Transform();

    /// Transform according to scaling/rotation + translation
    R3Transform(
      const double xx, const double xy, const double xz, const double dx,
      const double yx, const double yy, const double yz, const double dy,
      const double zx, const double zy, const double zz, const double dz);

    /// Transformation of a basis (no reflection)
    R3Transform(const Point& fr1, const Point& fr2, const Point& fr3,
                const Point& to1, const Point& to2, const Point& to3);

    virtual ~R3Transform() { }

    /// Return the inverse of the transformation matrix, if possible
    R3Transform GetInverse() const;

    /// Invert the transformation matrix, if possible
    R3Transform& Invert();

    /// Multiply two matrices
    R3Transform operator*(const R3Transform& t) const;

    /// Decompose transformation into scaling, rotation, and translation
    void GetDecomposition(Scale& s, Rotate& r, Translate& t) const;

    bool operator==(const R3Transform& t) const;

    bool operator!=(const R3Transform& t) const;

    /// Scaling/rotation matrix element XX
    double GetXX() const { return xx_; }

    /// Scaling/rotation matrix element XY
    double GetXY() const { return xy_; }

    /// Scaling/rotation matrix element XZ
    double GetXZ() const { return xz_; }

    /// Scaling/rotation matrix element YX
    double GetYX() const { return yx_; }

    /// Scaling/rotation matrix element YY
    double GetYY() const { return yy_; }

    /// Scaling/rotation matrix element YZ
    double GetYZ() const { return yz_; }

    /// Scaling/rotation matrix element ZX
    double GetZX() const { return zx_; }

    /// Scaling/rotation matrix element ZY
    double GetZY() const { return zy_; }

    /// Scaling/rotation matrix element ZZ
    double GetZZ() const { return zz_; }

    /// Translation matrix element DX
    double GetDX() const { return dx_; }

    /// Translation matrix element DY
    double GetDY() const { return dy_; }

    /// Translation matrix element DZ
    double GetDZ() const { return dz_; }

    /// Convert transformation to an identity matrix
    void SetIdentity() {
      xx_ = yy_ = zz_ = 1;
      xy_ = xz_ = dx_ = yx_ = yz_ = dy_ = zx_ = zy_ = dz_ = 0;
    }

  protected:

    double xx_, xy_, xz_, dx_;
    double yx_, yy_, yz_, dy_;
    double zx_, zy_, zz_, dz_;

    void SetR3Transform(
      const double xx, const double xy, const double xz, const double dx,
      const double yx, const double yy, const double yz, const double dy,
      const double zx, const double zy, const double zz, const double dz);

  friend std::ostream& operator<<(std::ostream& os, const R3Transform& t);

};

SHARED_POINTER_TYPEDEFS(R3Transform);

/*!
 * @class Scale
 * @ingroup geometry_data
 * @brief Scaling transformation in R3
 */
class Scale : public R3Transform {
  
  public:

    Scale() : R3Transform() { }

    Scale(const double x, const double y, const double z)
      : R3Transform(x, 0, 0, 0,
                  0, y, 0, 0,
                  0, 0, z, 0) { }

    Scale(const double s)
      : R3Transform(s, 0, 0, 0,
                  0, s, 0, 0,
                  0, 0, s, 0) { }

};

SHARED_POINTER_TYPEDEFS(Scale);

/*!
 * @class Rotate
 * @ingroup geometry_data
 * @brief Coordinate rotation in R3
 */
class Rotate : public R3Transform {

  public:

    Rotate() : R3Transform() { }

    Rotate(const R3Transform& t) : R3Transform(t) { }

    /// Rotate by angle a around an axis given by endpoints p1 and p2
    Rotate(const double a, const Point& p1, const Point& p2);

    /// Rotate by angle a around a vector axis v
    Rotate(const double a, const Vector& v);

    /// Construct rotation from two axes using the endpoints of the original
    //axis and the endpoints of the final axis (no reflections)
    Rotate(const Point& fr1, const Point& fr2,
           const Point& to1, const Point& to2);

};

SHARED_POINTER_TYPEDEFS(Rotate);

/*!
 * @class Translate
 * @ingroup geometry_data
 * @brief Translation in R3
 */
class Translate : public R3Transform {
  
  public:

    Translate() : R3Transform() { }

    Translate(const double x, const double y, const double z)
      : R3Transform(1, 0, 0, x,
                  0, 1, 0, y,
                  0, 0, 1, z) { }

};

SHARED_POINTER_TYPEDEFS(Translate);

#endif // DATACLASSES_GEOMETRY_R3TRANSFORM_H_INCLUDED

