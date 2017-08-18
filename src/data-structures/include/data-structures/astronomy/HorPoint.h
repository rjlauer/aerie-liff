/*!
 * @file HorPoint.h
 * @brief Calculations in the horizon coordinate system.
 * @author Segev BenZvi
 * @version $Id: HorPoint.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_ASTRONOMY_HORPOINT_H_INCLUDED
#define DATACLASSES_ASTRONOMY_HORPOINT_H_INCLUDED

#include <hawcnest/HAWCUnits.h>
#include <data-structures/geometry/S2Point.h>

/*!
 * @class HorPoint
 * @author Segev BenZvi
 * @date 28 May 2010
 * @ingroup astronomy_data
 * @brief Horizontal coordinates (azimuth and elevation)
 *
 * The horizontal coordinate system projects the local horizon onto the
 * celestial sphere.  Since the coordinate system is fixed with respect to a
 * point on the Earth, the position of celestial objects will drift with time
 * in horizontal coordinates.
 * 
 * Position above the horizon is called elevation (or altitude).  Azimuth is
 * in this implementation is measured in a left-handed sense with respect to
 * North, though many astronomical sources define azimuth with respect to South
 * (see, e.g., J. Meeus, Astronomical Algorithms, Willman-Bell 1998, p. 91).
 *
 * Note the difference from local detector coordinates, which are usually
 * measured in a right-handed sense with respect to East.  The coordinates are
 * left-handed because this tracks the apparent motion of stars due to the
 * Earth's rotation.  If the azimuth of an object is \f$>180^\circ\f$, it is
 * rising; if \f$<180^\circ\f$, the object is setting.
 */
class HorPoint {

  public:

    HorPoint() { }

    /// Set elevation (also known as altitude) and azimuth
    HorPoint(const double elev, const double azim);

    /// Return elevation angle (a.k.a. altitude): zenith = 90, antizenith = -90
    double GetElevation() const { return HAWCUnits::halfpi - p_.GetTheta(); }

    /// Retrieve azimuth angle
    double GetAzimuth() const;

    void SetPoint(const S2Point& s) { p_ = s; }

    /// Set elevation angle (zenith = 90, antizenith = -90)
    void SetElevation(const double el) { p_.SetTheta(HAWCUnits::halfpi - el); }

    /// Set azimuth.  Warning: may fail if elevation is 90 or -90!
    void SetAzimuth(const double az);

    /// "Safe" setter that works for any pair of elevation and azimuth.  If
    /// possible, call this function instead of SetElevation() and SetAzimuth()
    void SetElevationAzimuth(const double el, const double az);

    /// Calculate the angle between this coordinate and another one.
    double Angle(const HorPoint& c) const { return p_.Angle(c.p_); }

    /// Get the 3-tuple on the unit sphere for this coordinate
    const Point& GetPoint() const { return p_.GetPoint(); }

  private:

    S2Point p_;

  friend std::ostream& operator<<(std::ostream& os, const HorPoint& s);

};

SHARED_POINTER_TYPEDEFS(HorPoint);

#endif // DATACLASSES_ASTRONOMY_HORPOINT_H_INCLUDED

