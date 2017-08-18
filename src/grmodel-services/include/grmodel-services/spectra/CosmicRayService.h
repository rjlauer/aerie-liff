
#ifndef GRMODEL_SERVICES_SPECTRA_COSMICRAYSERVICE_H_INCLUDED
#define GRMODEL_SERVICES_SPECTRA_COSMICRAYSERVICE_H_INCLUDED

#include <data-structures/physics/Particle.h>

class PowerLaw;
class ModifiedJulianDate;
class RNGService;

/*!
 * @class CosmicRayService
 * @author Segev BenZvi
 * @date 23 Jan 2011
 * @ingroup gr_models
 * @brief Abstract interface to services which describe a spectrum from a
 *        cosmic ray source
 */
class CosmicRayService {

  public:

    virtual ~CosmicRayService() { }

    /// Calculate the flux of a given particle species at a given energy E
    virtual double GetFlux(const double E, const ModifiedJulianDate& mjd,
                           const ParticleType& type) const = 0;

    /// Calculate the weight needed at an energy E to convert a PowerLaw
    /// spectrum p into the shape described by this spectral model
    virtual double GetFluxWeight(const double E, const ModifiedJulianDate& mjd,
                                 const PowerLaw& p,
                                 const ParticleType& type) const = 0;

    /// Get the mimimum of the energy range of the model
    virtual double GetMinEnergy(const ParticleType& type) const = 0;

    /// Get the maximum of the energy range of the model
    virtual double GetMaxEnergy(const ParticleType& type) const = 0;

    /// Randomly sample an energy from the internal spectrum
    virtual double GetRandomEnergy(const RNGService& rng,
                                   const double E0, const double E1,
                                   const ParticleType& type) const
    { return 0.; }

    /// Integrate the spectrum between a lower and upper energy range
    virtual double Integrate(const double E0, const double E1,
                             const ModifiedJulianDate& mjd,
                             const ParticleType& type) const = 0;

    /// Get the probability to keep an event sampled from some power law
    /// if we want it to obey this spectrum
    virtual double GetProbToKeep(const double E, const PowerLaw& pl,
                                 const ModifiedJulianDate& mjd,
                                 const ParticleType& type) const = 0;

};

#endif // GRMODEL_SERVICES_SPECTRA_COSMICRAYSERVICE_H_INCLUDED

