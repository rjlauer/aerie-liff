/*!
 * @file IGRFService.h
 * @brief The International Geomagnetic Reference Field (IGRF).
 * @author Segev BenZvi
 * @date 10 Jan 2013
 * @version $Id: IGRFService.h 16648 2013-08-07 13:42:38Z pretz $
 */

#ifdef COMPILE_FORTRAN_COMPONENTS

#ifndef ASTROSERVICE_IGRFSERVICE_H_INCLUDED
#define ASTROSERVICE_IGRFSERVICE_H_INCLUDED

#include <astro-service/GeomagneticField.h>

#include <hawcnest/Service.h>

/*!
 * @class IGRFService
 * @author Segev BenZvi
 * @date 10 Jan 2013
 * @ingroup astro_xforms
 * @brief Calculate the orientation of the Earth's magnetic field as a function
 *        of geocentric position and time using the International Geomagnetic
 *        Reference Field.
 *
 * The International Geomagnetic Reference Field (IGRF) model describes the
 * scaler potential of the geomagnetic field as a function of position and
 * time.  The scaler potential is expressed in terms of a spherical harmonic
 * basis and is summed up to l=11.  The spherical harmonic coefficients are
 * hardcoded into the Fortran subroutines in the file Geopack-2008.f.
 *
 * Note that there are two components to the geomagnetic field beneath the
 * Earth's magnetopause:
 * <ol>
 *   <li>An internal field given by the IGRF.</li>
 *   <li>An external field affected by the heliosphere, not calculated in this
 *   model.</li>
 * </ol>
 * The external field is about 1% of the magnitude of the internal field.
 * Moreover, because it is strongly affected by the solar wind, which can
 * change on time scales of an hour, a proper calculation requires a connection
 * to a database containing tables of solar wind direction and magnitude as a
 * function of time.
 */
class IGRFService : public GeomagneticField {

  public:

    typedef GeomagneticField Interface;

    Configuration DefaultConfiguration();

    void Initialize(const Configuration& config);

    void Finish() { }

    /// Return field orientation at a geocentric location p and time MJD
    void GetField(const Point& p, const ModifiedJulianDate& mjd,
                  Vector& B) const;

  private:

    bool useDipole_;    ///< If true, only calculate dipole component of field

};


#endif // ASTROSERVICE_IGRFSERVICE_H_INCLUDED

#endif // COMPILE_FORTRAN_COMPONENTS

