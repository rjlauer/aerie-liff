/*!
 * @file S2Point.h
 * @brief Representation of points on the unit sphere.
 * @author Segev BenZvi
 * @version $Id: S2Point.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_GEOMETRY_S2POINT_H_INCLUDED
#define DATACLASSES_GEOMETRY_S2POINT_H_INCLUDED

#include <data-structures/geometry/Point.h>

/*!
 * @class S2Point
 * @author Segev BenZvi
 * @date 7 Nov 2009
 * @ingroup geometry_data
 * @brief Representation of a point on the unit sphere (or 2-sphere) given by
 *        a spherical polar zenith and azimuth angle
 */
class S2Point {

  public:

    S2Point();
    S2Point(const Point& p) : p_(p) { }
    S2Point(const double theta, const double phi);
    virtual ~S2Point() { }

    /// Get zenith angle (0 = +z, 180 = -z)
    double GetTheta() const { return p_.GetTheta(); }

    /// Get azimuth angle (0 = 360 = +x)
    double GetPhi() const { return p_.GetPhi(); }

    /// Set zenith angle (0 = +z, 180 = -z)
    void SetTheta(const double theta) { p_.SetTheta(theta); }

    /// Set azimuth.  Warning: this could fail if theta = 0 or 180 degrees
    void SetPhi(const double phi) { p_.SetPhi(phi); }

    /// "Safe" setter that never fails for any theta, phi
    void SetThetaPhi(const double theta, const double phi)
    { p_.SetRThetaPhi(1, theta, phi); }

    /// Return a representation as a unit vector
    const Point& GetPoint() const { return p_; }

    /// Calculate the angle with respect to another S2Point
    double Angle(const S2Point& s2) const { return p_.Angle(s2.p_); }

  private:

    Point p_;

  friend S2Point operator*(const R3Transform& matrix, const S2Point& s);

  friend std::ostream& operator<<(std::ostream& os, const S2Point& s);

};

SHARED_POINTER_TYPEDEFS(S2Point);

#endif // DATACLASSES_GEOMETRY_S2POINT_H_INCLUDED

