/*!
 * @file Ellipsoid.h
 * @brief An ellipsoidal model of the Earth's surface.
 * @author Segev BenZvi
 * @version $Id: Ellipsoid.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_GEOMETRY_ELLIPSOID_H_INCLUDED
#define DATACLASSES_GEOMETRY_ELLIPSOID_H_INCLUDED

#include <hawcnest/HAWCUnits.h>

#include <cmath>

/*!
 * @namespace Ellipsoid
 * @author Segev BenZvi
 * @date 8 Jul 2010
 * @ingroup geometry_data
 * @brief Parameters of the Earth's ellipsoid (WGS84 model)
 *
 * The functions in Ellipsoid define the parameters of the WGS84 model of the
 * Earth's ellipsoid.  The key parameters are:
 * <ul>
 *   <li>Earth eccentricity \f$e\f$</li>
 *   <li>Flattening \f$f = 1 - \sqrt{1-e^2}\f$</li>
 *   <li>Equatorial radius \f$a\f$<li>
 *   <li>Polar radius \f$b = a(1-f)\f$
 *   <li>Radius of curvature at geodetic latitude \f$\phi\f$:
 *       \f[
 *         R_{N} = \frac{a}{\sqrt{1-e^2\sin^2{\phi}}}
 *       \f]
 *   </li>
 * </ul>
 *
 * This code is based on the ReferenceEllipsoid class written for the Auger
 * Offline software by Lukas Nellen.  The WGS84 parameters can be found at
 * http://www.colorado.edu/geography/gcraft/notes/datum/gif/ellipse.gif
 * 
 * An explanation of the ellipsoid and its use in conversion between geocentric
 * and geodetic coordinates can be found at
 * http://www.oc.nps.navy.mil/oc2902w/coord/coordcvt.pdf
 */
namespace Ellipsoid {

  /// First eccentricity of the Earth (squared) in WGS84 model
  inline double GetEccentricity2()
  { return 0.00669437999013; }

  /// First eccentricity of the Earth in WGS84 model
  inline double GetEccentricity()
  { return std::sqrt(GetEccentricity2()); }

  /// Get the flattening of the Earth ellipsoid
  inline double GetFlattening()
  { return 1. - std::sqrt(1. - GetEccentricity2()); }

  /// Get radius of the Earth at the equator
  inline double GetEquatorialRadius()
  { return 6378137 * HAWCUnits::meter; }

  /// Get radius of the Earth at the poles
  inline double GetPolarRadius()
  { return (1. - GetFlattening()) * GetEquatorialRadius(); }

  /// Radius of ellipsoid curvature in the prime vertical (i.e., with respect
  /// to the center of the geodetic coordinate system)
  inline double GetRn(const double p) {
    const double sp = std::sin(p);
    return GetEquatorialRadius() / std::sqrt(1. - GetEccentricity2()*sp*sp);
  }

}

#endif // DATACLASSES_GEOMETRY_ELLIPSOID_H_INCLUDED

