/*!
 * @file Sun.h
 * @brief Calculate the position of the Sun.
 * @author Daniel Fiorino
 * @date 27 Jul 2013
 * @version $Id:
 */

#ifndef ASTROSERVICE_SUN_H_INCLUDED
#define ASTROSERVICE_SUN_H_INCLUDED

class ModifiedJulianDate;
class EquPoint;

/*!
 * @class Sun
 * @author Daniel Fiorino
 * @date 27 Jul 2013
 * @ingroup astro_xforms
 * @brief Calculate position, velocity, cycle of the sun
 *
 * This class is for the calculation of the position and velocity of the sun
 * in geocentric coordinates, which can be converted to topocentric coordinates
 * assuming a spherical Earth or using a suitable ellipsoidal model of the
 * Earth but hasn't been done yet.
 *
 * The algorithm comes from J. Meeus, Astronimical Algorithms, 2nd Ed, 163-165. 
 *
 * The accuracy of the geocentric algorithm is about 0.01 degrees according 
 * to Meeus. 
 */
class Sun {

  public:

    static void GetGeocentricPoint(const ModifiedJulianDate& mjd, EquPoint& ep);

};

#endif // ASTROSERVICE_SUN_H_INCLUDED

