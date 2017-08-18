/*!
 * @file Precession.h
 * @brief Calculation precession of Earth's axis.
 * @author Segev BenZvi
 * @date 29 May 2010
 * @version $Id: Precession.h 14859 2013-04-27 14:14:39Z sybenzvi $
 */

#ifndef ASTROSERVICE_PRECESSION_H_INCLUDED
#define ASTROSERVICE_PRECESSION_H_INCLUDED

#include <data-structures/geometry/R3Transform.h>

class ModifiedJulianDate;

/*!
 * @class Precession
 * @author Segev BenZvi
 * @date 29 May 2010
 * @ingroup astro_xforms
 * @brief Calculate a rotation matrix for precession of the equinoxes
 */
class Precession {

  public:

    /// Precession matrix between a start epoch (e0) and an end epoch (e1);
    /// used in the sense v(e1) = R(e0, e1) * v(e0)
    static const Rotate& GetRotationMatrix(
      const ModifiedJulianDate& e0, const ModifiedJulianDate& e1);

  private:

    static Rotate precMatrix_;

};

#endif // ASTROSERVICE_PRECESSION_H_INCLUDED

