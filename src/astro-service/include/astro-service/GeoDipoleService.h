/*!
 * @file GeoDipoleService.h
 * @brief The International Geomagnetic Reference Field (GeoDipole).
 * @author Segev BenZvi
 * @date 10 Jan 2013
 * @version $Id: GeoDipoleService.h 27613 2015-11-09 03:14:48Z sybenzvi $
 */

#ifndef ASTROSERVICE_GEODIPOLE_SERVICE_H_INCLUDED
#define ASTROSERVICE_GEODIPOLE_SERVICE_H_INCLUDED

#include <astro-service/GeomagneticField.h>

#include <hawcnest/Service.h>

/*!
 * @class GeoDipoleService
 * @author Segev BenZvi
 * @date 9 Nov 2015
 * @ingroup astro_xforms
 * @brief Calculate the orientation of the Earth's magnetic field as a function
 *        of geocentric position and time using a dipole field approximation.
 *
 * Note that there are two components to the geomagnetic field beneath the
 * Earth's magnetopause:
 * <ol>
 *   <li>An internal field given by the GeoDipole.</li>
 *   <li>An external field affected by the heliosphere, not calculated in this
 *   model.</li>
 * </ol>
 * The external field is about 1% of the magnitude of the internal field.
 * Moreover, because it is strongly affected by the solar wind, which can
 * change on time scales of an hour, a proper calculation requires a connection
 * to a database containing tables of solar wind direction and magnitude as a
 * function of time.
 */
class GeoDipoleService : public GeomagneticField {

  public:

    typedef GeomagneticField Interface;

    Configuration DefaultConfiguration();

    void Initialize(const Configuration& config);

    void Finish() { }

    /// Return field orientation at a geocentric location p and time MJD
    void GetField(const Point& p, const ModifiedJulianDate& mjd,
                  Vector& B) const;

};


#endif // ASTROSERVICE_GEODIPOLE_SERVICE_H_INCLUDED

