/*!
 * @file GalPoint.h
 * @brief Calculations in the galactic coordinate system.
 * @author Segev BenZvi
 * @version $Id: GalPoint.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_ASTRONOMY_GALPOINT_H_INCLUDED
#define DATACLASSES_ASTRONOMY_GALPOINT_H_INCLUDED

#include <hawcnest/HAWCUnits.h>
#include <data-structures/geometry/S2Point.h>

class Rotate;

/*!
 * @class GalPoint
 * @author Segev BenZvi
 * @date 28 May 2010
 * @ingroup astronomy_data
 * @brief Galactic latitude and longitude
 *
 * This class encapsulates a galactic coordinate.  The galactic coordinate
 * system is centered on the sun, with longitude measured along the galactic
 * plane.  The galactic center corresponds to a longitude of zero.  Latitude
 * and longitude are denoted \f$b\f$ and \f$\ell\f$.
 */
class GalPoint {

  public:

    GalPoint() { }

    /// Location in latitude (b) and longitude (l)
    GalPoint(const double b, const double l) :
      p_(HAWCUnits::halfpi - b, l) { }

    /// Retrieve galactic latitude (N = 90, S = -90)
    double GetB() const { return HAWCUnits::halfpi - p_.GetTheta(); }

    /// Retrieve galactic longitude
    double GetL() const { return p_.GetPhi(); }

    void SetPoint(const S2Point& s) { p_ = s; }

    /// Set latitude (N = 90, S = -90)
    void SetB(const double b) { p_.SetTheta(HAWCUnits::halfpi - b); }

    /// Set longitude; may fail if latitude is 90 or -90!
    void SetL(const double l) { p_.SetPhi(l); }

    /// "Safe" setter that never fails for any pair of b, l.  If possible,
    /// users should always use this function instead of SetB() and SetL()
    void SetBL(const double b, const double l)
    { p_.SetThetaPhi(HAWCUnits::halfpi - b, l); }

    /// Calculate the angle between this coordinate and another one
    double Angle(const GalPoint& c) const { return p_.Angle(c.p_); }

    /// Get the 3-tuple on the unit sphere for this coordinate
    const Point& GetPoint() const { return p_.GetPoint(); }

  private:

    S2Point p_;

  friend std::ostream& operator<<(std::ostream& os, const GalPoint& s);

};

SHARED_POINTER_TYPEDEFS(GalPoint);

#endif // DATACLASSES_ASTRONOMY_GALPOINT_H_INCLUDED

