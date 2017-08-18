/*!
 * @file PeriodicLightCurve.h
 * @brief A non-parametric, periodic light curve service based on an offset, a
 *        period, and a table of times and fluxes that define one periodic
 *        cycle.
 * @author Segev BenZvi
 * @date 5 Oct 2014
 * @version $Id: PeriodicLightCurve.h 22114 2014-10-06 03:33:38Z sybenzvi $
 */

#ifndef GRMODEL_SERVICES_SPECTRA_PERIODICLIGHTCURVE_H_INCLUDED
#define GRMODEL_SERVICES_SPECTRA_PERIODICLIGHTCURVE_H_INCLUDED

#include <grmodel-services/spectra/LightCurve.h>

#include <data-structures/math/PeriodicFunction.h>

#include <hawcnest/Service.h>

/*!
 * @class PeriodicLightCurve
 * @author Segev BenZvi
 * @date 5 Oct 2014
 * @ingroup gr_models
 * @brief Implementation of a time-dependent light curve based on a flux table.
 */
class PeriodicLightCurve : public LightCurve {

  public:
    
    typedef LightCurve Interface;

    Configuration DefaultConfiguration();

    void Initialize(const Configuration& config);

    void Finish() { }

    /// Return the light curve at some time tMJD
    double GetFlux(const double tMJD) const;

    /// Get the flux relative to the peak flux at some time tMJD
    double GetFluxFraction(const double tMJD) const;

  private:

    PeriodicFunctionPtr lc_;    ///< Light curve local storage
    double peakFlux_;           ///< Peak flux per period

};

#endif // GRMODEL_SERVICES_SPECTRA_PERIODICLIGHTCURVE_H_INCLUDED

