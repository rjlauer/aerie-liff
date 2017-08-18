/*!
 * @file EquPoint.h
 * @brief Calculations in the equatorial coordinate system.
 * @author Segev BenZvi
 * @version $Id: EquPoint.h 35396 2016-10-25 15:27:54Z imc $
 */

#ifndef DATACLASSES_ASTRONOMY_EQUPOINT_H_INCLUDED
#define DATACLASSES_ASTRONOMY_EQUPOINT_H_INCLUDED

#include <hawcnest/HAWCUnits.h>
#include <data-structures/geometry/S2Point.h>

class Rotate;

/*!
 * @class EquPoint
 * @author Segev BenZvi
 * @date 28 May 2010
 * @ingroup astronomy_data
 * @brief Right ascension and declination coordinates
 *
 * This class encapsulates an equatorial coordinate.  The equatorial coordinate
 * system is centered on the Earth, and projects the equator and poles onto the
 * celestial sphere.  The longitudinal angle is called the right ascension,
 * denoted \f$RA\f$ or \f$\alpha\f$.  The latitude is called declination,
 * and is denoted \f$\delta\f$.
 *
 * This class does not specify whether or not the equatorial coordinate has
 * been calculated using a standard epoch, such as J2000.0, or the epoch of the
 * day.  It is currently up to the user to make sure the proper epoch is being
 * used for a particular calculation.
 * 
 * @todo Add explicit data member to specify the astronomical epoch?
 */
class EquPoint {

  public:

    EquPoint() { }

    /// Location in RA and declination
    EquPoint(const double ra, const double dec) :
      p_(HAWCUnits::halfpi - dec, ra) { }

    /// Return right ascension
    double GetRA() const 
    { return p_.GetPhi() < 0 ? HAWCUnits::twopi + p_.GetPhi() : p_.GetPhi(); }

    /// Return declination
    double GetDec() const
    { return HAWCUnits::halfpi - p_.GetTheta(); }

    /// In traditional form
    void GetRATraditional(int& h, int& m, double& s);
    void GetDecTraditional(int& d, int& m, double& s);
  
    void SetPoint(const S2Point& s) { p_ = s; }

    /// Set right ascension; may fail if declination is 90 or -90!
    void SetRA(const double ra) { p_.SetPhi(ra); }

    /// Set declination (N = 90, S = -90)
    void SetDec(const double dec) { p_.SetTheta(HAWCUnits::halfpi - dec); }

    /// "Safe" setter that never fails for any pair of ra, dec.  If possible,
    /// users should call this function instead of SetRA() and SetDec()
    void SetRADec(const double ra, const double dec)
    { p_.SetThetaPhi(HAWCUnits::halfpi - dec, ra); }

    /// Calculate the angle between this coordinate and another one.
    double Angle(const EquPoint& c) const { return p_.Angle(c.p_); }

    /// Get the 3-tuple on the unit sphere for this coordinate
    const Point& GetPoint() const { return p_.GetPoint(); }

  private:

    S2Point p_;

  friend std::ostream& operator<<(std::ostream& os, const EquPoint& s);

};

SHARED_POINTER_TYPEDEFS(EquPoint);

#endif // DATACLASSES_ASTRONOMY_EQUPOINT_H_INCLUDED

