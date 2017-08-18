/*!
 * @file R3Vector.h
 * @brief Vector triplet representation.
 * @author Segev BenZvi
 * @version $Id: R3Vector.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_GEOMETRY_R3VECTOR_H_INCLUDED
#define DATACLASSES_GEOMETRY_R3VECTOR_H_INCLUDED

#include <hawcnest/processing/Bag.h>

#include <cmath>
#include <iostream>

/*!
 * @class R3Vector
 * @author S. BenZvi
 * @date 7 Nov 2009
 * @ingroup geometry_data
 * @brief Representation of a Cartesian coordinate triplet in R3.  Based
 *        loosely on the CLHEP and ROOT 3-vector classes
 */
class R3Vector : public virtual Baggable {

  public:

    enum CoordinateSystem {
      CARTESIAN,
      CYLINDRICAL,
      SPHERICAL
    };

    R3Vector()
      : x_(0), y_(0), z_(0) { }

    /// Construct from a Cartesian coordinate triplet x,y,z
    R3Vector(const double x, const double y, const double z)
      : x_(x), y_(y), z_(z) { }

    /// Construct from parametric coordinates (cylindrical, spherical, etc.)
    R3Vector(const double v1, const double v2, const double v3,
             const CoordinateSystem sys);

    virtual ~R3Vector() { }

    /// Cartesian x-component
    double GetX() const { return x_; }

    /// Cartesian y-component
    double GetY() const { return y_; }

    /// Cartesian z-component
    double GetZ() const { return z_; }

    /// Squared rho-component in cylindrical coordinates
    double GetRho2() const { return x_*x_ + y_*y_; }

    /// Rho-component in cylindrical coordinates
    double GetRho() const { return std::sqrt(GetRho2()); }

    /// Squared length of the vector
    double GetMag2() const { return x_*x_ + y_*y_ + z_*z_; }

    /// GetMagnitude of the vector
    double GetMag() const { return std::sqrt(GetMag2()); }

    /// Spherical polar r-coordinate
    double GetR() const { return GetMag(); }

    /// Spherical polar zenith angle
    double GetTheta() const
    { return x_ == 0 && y_ == 0 && z_ == 0 ? 0 : std::atan2(GetRho(), z_); }

    /// Spherical/cylindrical polar azimuth angle
    double GetPhi() const
    { return x_ == 0 && y_ == 0 ? 0 : std::atan2(y_, x_); }

    /// Set cartesian coordinates
    void SetXYZ(const double x, const double y, const double z);

    void SetX(const double x) { x_ = x; }

    void SetY(const double y) { y_ = y; }

    void SetZ(const double z) { z_ = z; }

    /// Set cylindrical polar coordinates
    void SetRhoPhiZ(const double rho, const double phi, const double z);

    /// Set spherical polar coordinates
    void SetRThetaPhi(const double r, const double theta, const double phi);

    /// Set theta-component while holding vector magnitude and azimuth constant
    void SetTheta(const double theta);

    /// Set phi-component while holding vector magnitude and zenith constant
    void SetPhi(const double phi);

    /// Return the unit vector corresponding to this vector
    R3Vector GetUnitVector() const;

    /// Return a vector orthogonal to this one
    R3Vector GetOrthogonalVector() const;

    /// Reset vector length to one
    void Normalize();

    /// Evaluate the angle between this and another vector
    double Angle(const R3Vector& v) const;

    /// Scalar product with another vector
    double Dot(const R3Vector& v) const { 
      return x_*v.x_ + y_*v.y_ + z_*v.z_; }

    /// Vector product with another vector
    R3Vector Cross(const R3Vector& v) const
    { return R3Vector(y_*v.z_ - z_*v.y_, z_*v.x_ - x_*v.z_, x_*v.y_ - y_*v.x_); } 

    /// Add two vectors together
    R3Vector operator+(const R3Vector& v) const
    { return R3Vector(x_ + v.x_, y_ + v.y_, z_ + v.z_); }

    /// Addition and assignment
    R3Vector& operator+=(const R3Vector& v) {
      x_ += v.x_; y_ += v.y_; z_ += v.z_;
      return *this;
    }

    /// Unary minus
    R3Vector operator-() const
    { return R3Vector(-x_, -y_, -z_); }

    /// Subtract two vectors
    R3Vector operator-(const R3Vector& v) const
    { return R3Vector(x_ - v.x_, y_ - v.y_, z_ - v.z_); }

    /// Subtraction and assignment
    R3Vector& operator-=(const R3Vector& v) {
      x_ -= v.x_; y_ -= v.y_; z_ -= v.z_;
      return *this;
    }

    /// Multiply vector by a scalar
    R3Vector operator*(const double a) const 
    { return R3Vector(a * x_, a * y_, a * z_); }

    /// Multiplication by a scalar, and assignment
    R3Vector& operator*=(const double a) {
      x_ *= a; y_ *= a; z_ *= a;
      return *this;
    }

    /// Scalar product of vectors
    double operator*(const R3Vector& v) const
    { return Dot(v); }

    /// Divide vector by a scalar
    R3Vector operator/(const double a) const 
    { return R3Vector(x_ / a, y_ / a, z_ / a); }

    /// Division by a scalar, and assignment
    R3Vector& operator/=(const double a) {
      x_ /= a; y_ /= a; z_ /= a;
      return *this;
    }

    /// Check vectors for equivalence
    bool operator==(const R3Vector& v) const
    { return (x_ == v.x_ && y_ == v.y_ && z_ == v.z_); }

    /// Check vectors for inequivalence
    bool operator!=(const R3Vector& v) const 
    { return !(*this == v); }

  protected:

    double x_;
    double y_;
    double z_;

  /// Multiply a scalar by a vector
  friend R3Vector operator*(const double a, const R3Vector& v) {
    return R3Vector(a * v.x_, a * v.y_, a * v.z_);
  }

  /// Serialize vector contents to an output stream
  friend std::ostream& operator<<(std::ostream& os, const R3Vector& v);

};

SHARED_POINTER_TYPEDEFS(R3Vector);

#endif // DATACLASSES_GEOMETRY_R3VECTOR_H_INCLUDED

