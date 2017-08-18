/*!
 * @file TabulatedSpectrum.h
 * @brief Implementation of a flux table.
 * @author Brian Baughman
 * @date 6 Dec 2012
 * @version $Id: TabulatedSpectrum.h 16675 2013-08-08 22:30:36Z pretz $
 */

#ifndef GRMODEL_SERVICES_SPECTRA_TABULATEDSPECTRUM_H_INCLUDED
#define GRMODEL_SERVICES_SPECTRA_TABULATEDSPECTRUM_H_INCLUDED

#include <grmodel-services/spectra/CosmicRayService.h>

#include <data-structures/math/TabulatedFunction.h>

#include <hawcnest/Service.h>

class RNGService;

/*!
 * @class TabulatedSpectrum
 * @author Brian Baughman
 * @author Segev BenZvi
 * @date 6 December 2012
 * @ingroup gr_models
 * @brief A generic tabulated spectrum.  Values are interpolated linearly in
 *        log(F) vs. log(E).
 */
class TabulatedSpectrum : public CosmicRayService {

  public:

    typedef CosmicRayService Interface;

    Configuration DefaultConfiguration();

    void Initialize(const Configuration& config);

    double GetFlux(const double E, const ModifiedJulianDate& mjd,
                   const ParticleType& type=Gamma) const;

    double GetFluxWeight(const double E, const ModifiedJulianDate& mjd,
                         const PowerLaw& p,
                         const ParticleType& type=Gamma) const;
    
    double GetMinEnergy(const ParticleType& type=Gamma) const;

    double GetMaxEnergy(const ParticleType& type=Gamma) const;

    double GetRandomEnergy(const RNGService& rng,
                           const double E0, const double E1,
                           const ParticleType& type) const;

    double Integrate(const double E0, const double E1,
                     const ModifiedJulianDate& mjd,
                     const ParticleType& type=Gamma) const;

    double GetProbToKeep(const double E, const PowerLaw& pl,
                         const ModifiedJulianDate& mjd,
                         const ParticleType& type=Gamma) const;

  private:

    TabulatedFunction<double> logFvslogE_;  ///< Spectrum table in log space
    int nIntegrationBins_;                  ///< Binning in numerical integrals

    // Helper function to get units for fields in the file
    void GetUnits(std::string& line, std::vector<double>& units);

};

#endif // GRMODEL_SERVICES_SPECTRA_TABULATEDSPECTRUM_H_INCLUDED

