/*!
 * @file LightCurve.h
 * @brief A time-dependent light curve.
 * @author Segev BenZvi
 * @date 23 Jan 2011
 * @version $Id: LightCurve.h 22113 2014-10-06 02:17:25Z sybenzvi $
 */

#ifndef GRMODEL_SERVICES_SPECTRA_LIGHTCURVE_H_INCLUDED
#define GRMODEL_SERVICES_SPECTRA_LIGHTCURVE_H_INCLUDED

#include <hawcnest/PointerTypedefs.h>

class ModifiedJulianDate;

/*!
 * @class LightCurve
 * @author Segev BenZvi
 * @date 23 Jan 2011
 * @ingroup gr_models
 * @brief Interface class for a time-dependent light curve.
 */
class LightCurve {

  public:
    
    LightCurve() { }

    virtual ~LightCurve() { }

    /// Return the light curve at some time tMJD
    double GetFlux(const ModifiedJulianDate& tMJD) const;

    /// Return the light curve at some time tMJD
    virtual double GetFlux(const double tMJD) const = 0;

    /// Get the flux as a fraction of the peak flux at some time tMJD
    double GetFluxFraction(const ModifiedJulianDate& tMJD) const;

    /// Get the flux as a fraction of the peak flux at some time tMJD
    virtual double GetFluxFraction(const double tMJD) const = 0;

};

SHARED_POINTER_TYPEDEFS(LightCurve);

#endif // GRMODEL_SERVICES_SPECTRA_LIGHTCURVE_H_INCLUDED

