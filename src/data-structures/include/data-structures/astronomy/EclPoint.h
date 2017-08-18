/*!
 * @file EclPoint.h
 * @brief Calculations in the ecliptic coordinate system.
 * @author Segev BenZvi
 * @version $Id: EclPoint.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_ASTRONOMY_ECLPOINT_H_INCLUDED
#define DATACLASSES_ASTRONOMY_ECLPOINT_H_INCLUDED

#include <hawcnest/HAWCUnits.h>
#include <data-structures/geometry/S2Point.h>

class Rotate;

/*!
 * @class EclPoint
 * @author Segev BenZvi
 * @date 28 May 2010
 * @ingroup astronomy_data
 * @brief Ecliptic latitude and longitude
 *
 * This class encapsulates an ecliptic coordinate.  The ecliptic coordinate
 * system is the projection of the orbital plane of the Earth onto the
 * celestial sphere, so the ecliptic plane contains the path of the sun across
 * the sky (as well as most objects in the solar system).
 *
 * The ecliptic (or celestial) latitude and longitude of a body are denoted
 * \f$\beta\f$ and \f$\lambda\f$.  Longitude is measured along the ecliptic
 * from the position of the vernal equinox.
 */
class EclPoint {

  public:

    EclPoint() { }

    /// Location in ecliptic latitude (beta) and longitude (lambda)
    EclPoint(const double beta, const double lambda)  :
      p_(HAWCUnits::halfpi - beta, lambda) { }

    /// Retrieve ecliptic latitude (N = 90, S = -90)
    double GetBeta() const { return HAWCUnits::halfpi - p_.GetTheta(); }

    /// Retrieve ecliptic longitude
    double GetLambda() const
    { return p_.GetPhi() < 0 ? HAWCUnits::twopi + p_.GetPhi() : p_.GetPhi(); }

    void SetPoint(const S2Point& s) { p_ = s; }

    /// Set ecliptic latitude (N = 90, S = -90)
    void SetBeta(const double b) { p_.SetTheta(HAWCUnits::halfpi - b); }

    /// Set ecliptic longitude (may fail if latitude is 90 or -90)
    void SetLambda(const double l) { p_.SetPhi(l); }

    /// "Safe" setter that never fails for any pair of b, l.  If practical,
    /// users should call this function instead of SetBeta() or SetLambda()
    void SetBetaLambda(const double b, const double l)
    { p_.SetThetaPhi(HAWCUnits::halfpi - b, l); }

    /// Calculate the angle between this coordinate and another one
    double Angle(const EclPoint& c) const { return p_.Angle(c.p_); }

    /// Get the 3-tuple on the unit sphere for this coordinate
    const Point& GetPoint() const { return p_.GetPoint(); }

  private:

    S2Point p_;

  friend std::ostream& operator<<(std::ostream& os, const EclPoint& s);

};

SHARED_POINTER_TYPEDEFS(EclPoint);

#endif // DATACLASSES_ASTRONOMY_ECLPOINT_H_INCLUDED

