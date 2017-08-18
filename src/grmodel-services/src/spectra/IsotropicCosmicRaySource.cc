
#include <grmodel-services/spectra/CosmicRayService.h>
#include <grmodel-services/spectra/IsotropicCosmicRaySource.h>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/RegisterService.h>

using namespace HAWCUnits;
using namespace std;

REGISTER_SERVICE(IsotropicCosmicRaySource);

Configuration
IsotropicCosmicRaySource::DefaultConfiguration()
{
  Configuration config;
  config.Parameter<string>("sourceSpectrum");
  config.Parameter<string>("particleType");
  return config;
}

void
IsotropicCosmicRaySource::Initialize(const Configuration& config)
{
  // Get the source spectrum and particle type
  config.GetParameter("sourceSpectrum", spectrumServiceName_);

  string pname;
  config.GetParameter("particleType", pname);

  particleType_ = Particle::GetParticleType(pname);
}

double
IsotropicCosmicRaySource::GetFlux(const double E, const ModifiedJulianDate& mjd)
  const
{
  const CosmicRayService& crs =
    GetService<CosmicRayService>(spectrumServiceName_);

  return crs.GetFlux(E, mjd, particleType_);
}

double
IsotropicCosmicRaySource::GetFluxWeight(const double E, const ModifiedJulianDate& mjd, const PowerLaw& p)
  const
{
  const CosmicRayService& crs =
    GetService<CosmicRayService>(spectrumServiceName_);

  return crs.GetFluxWeight(E, mjd, p, particleType_);
}

double
IsotropicCosmicRaySource::GetMinEnergy()
  const
{
  const CosmicRayService& crs =
    GetService<CosmicRayService>(spectrumServiceName_);

  return crs.GetMinEnergy(particleType_);
}

double
IsotropicCosmicRaySource::GetMaxEnergy()
  const
{
  const CosmicRayService& crs =
    GetService<CosmicRayService>(spectrumServiceName_);

  return crs.GetMaxEnergy(particleType_);
}

/// Randomly sample an energy from the source spectrum in [E0, E1]
double
IsotropicCosmicRaySource::GetRandomEnergy(
    const RNGService& rng, const double E0, const double E1)
  const
{
  const CosmicRayService& crs =
    GetService<CosmicRayService>(spectrumServiceName_);

  return crs.GetRandomEnergy(rng, E0, E1, particleType_);
}

double
IsotropicCosmicRaySource::Integrate(const double E0, const double E1,
                                    const ModifiedJulianDate& mjd)
  const
{
  const CosmicRayService& crs =
    GetService<CosmicRayService>(spectrumServiceName_);

  return crs.Integrate(E0, E1, mjd, particleType_);
}

