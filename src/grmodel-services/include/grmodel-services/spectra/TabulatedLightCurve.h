/*!
 * @file TabulatedLightCurve.h
 * @brief A time-dependent light curve service based on a table of times and 
 *        fluxes.
 * @author Segev BenZvi
 * @date 5 Oct 2014
 * @version $Id: TabulatedLightCurve.h 22114 2014-10-06 03:33:38Z sybenzvi $
 */

#ifndef GRMODEL_SERVICES_SPECTRA_TABULATEDLIGHTCURVE_H_INCLUDED
#define GRMODEL_SERVICES_SPECTRA_TABULATEDLIGHTCURVE_H_INCLUDED

#include <grmodel-services/spectra/LightCurve.h>

#include <data-structures/math/TabulatedFunction.h>

#include <hawcnest/Service.h>

/*!
 * @class TabulatedLightCurve
 * @author Segev BenZvi
 * @date 5 Oct 2014
 * @ingroup gr_models
 * @brief Implementation of a time-dependent light curve based on a flux table.
 */
class TabulatedLightCurve : public LightCurve {

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

    TabulatedFunction<double> lc_;    ///< Light curve local storage
    double peakFlux_;                 ///< Peak flux over observation period

};

#endif // GRMODEL_SERVICES_SPECTRA_TABULATEDLIGHTCURVE_H_INCLUDED

