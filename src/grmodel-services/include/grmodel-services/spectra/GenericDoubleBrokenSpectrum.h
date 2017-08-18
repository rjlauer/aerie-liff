
#ifndef GRMODEL_SERVICES_SPECTRA_GENERICDOUBLEBROKENSPECTRUM_H_INCLUDED
#define GRMODEL_SERVICES_SPECTRA_GENERICDOUBLEBROKENSPECTRUM_H_INCLUDED

#include <grmodel-services/spectra/CosmicRayService.h>

#include <data-structures/math/DoubleBrokenPowerLaw.h>

#include <hawcnest/Service.h>

class RNGService;

/*!
 * @class GenericDoubleBrokenSpectrum
 * @author Tom Weisgarber
 * @date 18 December 2012
 * @ingroup gr_models
 * @brief A generic gamma-ray source with a double broken power law spectrum
 */
class GenericDoubleBrokenSpectrum : public CosmicRayService {

  public:

    typedef CosmicRayService Interface;

    Configuration DefaultConfiguration();

    void Initialize(const Configuration& config);

    void Finish() { }

    /// Get the flux at some energy E from a particular particle type
    double GetFlux(const double E, const ModifiedJulianDate& mjd,
                   const ParticleType& type=Gamma) const;

    /// Calculate the weight needed at an energy E to convert a PowerLaw
    /// spectrum p into the shape described by this spectral model
    double GetFluxWeight(const double E, const ModifiedJulianDate& mjd,
                         const PowerLaw& p,
                         const ParticleType& type=Gamma) const;

    /// Minimum of the energy range of the model
    double GetMinEnergy(const ParticleType& type) const
    { return Emin_; }

    /// Maximum of the energy range of the model
    double GetMaxEnergy(const ParticleType& type) const
    { return spectrum_.GetMaxX(); }

    /// Randomly sample the spectrum between E0 and E1
    double GetRandomEnergy(const RNGService& rng,
                           const double E0, const double E1,
                           const ParticleType& type) const;

    /// Integrate the spectrum between a lower and upper energy range
    double Integrate(const double E0, const double E1,
                     const ModifiedJulianDate& mjd,
                     const ParticleType& type) const;

    /// Get the probability to keep an event sampled from some power law
    /// if we want it to obey this spectrum
    virtual double GetProbToKeep(const double E, const PowerLaw& pl,
                                 const ModifiedJulianDate& mjd,
                                 const ParticleType& type) const;

  private:

    DoubleBrokenPowerLaw spectrum_;   ///< Double broken power law spectrum
    double Emin_;                     ///< Minimum energy (not normalization)
    double throwIdx_;                 ///< Index for random number generation

};

#endif // GRMODEL_SERVICES_SPECTRA_GENERICDOUBLEBROKENSPECTRUM_H_INCLUDED

