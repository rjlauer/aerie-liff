/*!
 * @file Moon.h
 * @brief Calculate the position of the Moon.
 * @author Segev BenZvi
 * @date 25 Jul 2010
 * @version $Id: Moon.h 14859 2013-04-27 14:14:39Z sybenzvi $
 */

#ifndef ASTROSERVICE_MOON_H_INCLUDED
#define ASTROSERVICE_MOON_H_INCLUDED

class ModifiedJulianDate;
class Point;

/*!
 * @class Moon
 * @author Segev BenZvi
 * @date 25 Jul 2010
 * @ingroup astro_xforms
 * @brief Calculate position, velocity, cycle of the moon
 *
 * This class is for the calculation of the position and velocity of the moon
 * in geocentric coordinates, which can be converted to topocentric coordinates
 * assuming a spherical Earth or using a suitable ellipsoidal model of the
 * Earth.
 *
 * The algorithm comes from J. Meeus, l'Astronomie, June 1984, 348, and the
 * implementation is taken from the SLALIB routine in dmoon.f (see
 * http://starlink.jach.hawaii.edu/).  
 *
 * The accuracy of the algorithm is about 10" in longitude, 3" in latitude, and
 * 20 km in distance, according to Meeus.  These should not be interpreted as
 * extreme values, but as something more akin to a typical spread of errors.
 */
class Moon {

  public:

    static void GetGeocentricPoint(const ModifiedJulianDate& mjd, Point& p);

};

#endif // ASTROSERVICE_MOON_H_INCLUDED

