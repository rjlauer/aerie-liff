/*!
 * @file Nutation.h
 * @brief Calculate wobble of Earth about its axis (nutation).
 * @author Segev BenZvi 
 * @date 29 May 2010
 * @version $Id: Nutation.h 21279 2014-08-05 18:40:02Z sybenzvi $
 */

#ifndef ASTROSERVICE_NUTATION_H_INCLUDED
#define ASTROSERVICE_NUTATION_H_INCLUDED

#include <data-structures/geometry/R3Transform.h>

class ModifiedJulianDate;

/*!
 * @class Nutation
 * @author Segev BenZvi
 * @date 29 May 2010
 * @ingroup astro_xforms
 * @brief Calculate a rotation matrix for the oscillation of the Earth about
 *        its axis (nutation)
 *
 * Nutation, or the oscillation of the Earth about its rotation axis, is a
 * tidal effect due to the influence of the moon, sun, and other planets.
 * There is also a contribution due to the non-alignment of the rotation axes
 * of the Earth's solid core and liquid mantle (called Free Core Nutation).
 *
 * The Earth's nutation can be decomposed into two components:
 * <ol>
 *  <li>\f$\Delta\psi\f$, along the ecliptic plane (nutation in longitude)</li>
 *  <li>\f$\Delta\varepsilon\f$, perpendicular to the ecliptic plane (nutation
 *      in obliquity)</li>
 * </ol>
 * The correction for nutation is expressible as an Euler rotation, where the
 * Euler angles are
 * <ol>
 *  <li>\f$\varepsilon_0\f$ (mean obliquity of the ecliptic) about \f$x\f$</li>
 *  <li>\f$-\Delta\psi\f$ about \f$z\f$</li>
 *  <li>\f$-\varepsilon_0-\Delta\varepsilon\f$ about \f$x\f$</li>
 * </ol>
 *
 * This calculation corrects for nutation using a 63-parameter series given in
 * the 1980 IAU Theory of Nutation from the 1984 Astronomical Almanac.  For
 * details, see that source or chapter 22 of "Astronomical Algorithms" by
 * Jean Meeus, 2nd ed. (1998).
 */
class Nutation {

  public:

    /// Nutation matrix at some date (mjd)
    static const Rotate& GetRotationMatrix(const ModifiedJulianDate& mjd);

  private:

    static Rotate nutMatrix_;

};

#endif // ASTROSERVICE_NUTATION_H_INCLUDED

