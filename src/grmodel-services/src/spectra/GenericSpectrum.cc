
#include <grmodel-services/spectra/GenericSpectrum.h>

#include <data-structures/time/ModifiedJulianDate.h>

#include <rng-service/RNGService.h>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/Logging.h>
#include <hawcnest/RegisterService.h>

#include <cmath>

using namespace std;
using namespace HAWCUnits;

REGISTER_SERVICE(GenericSpectrum);

Configuration
GenericSpectrum::DefaultConfiguration()
{
  Configuration config;

  config.Parameter<double>("fluxNorm", 1e-10 / (cm2*second*TeV));
  config.Parameter<double>("energyNorm", 1*TeV);
  config.Parameter<double>("spIndex", -2.5);
  config.Parameter<double>("energyMin", 100*GeV);
  config.Parameter<double>("energyMax", 100*TeV);

  return config;
}

void
GenericSpectrum::Initialize(const Configuration& config)
{
  double I0;
  double E0;
  double idx;
  double Emax;

  config.GetParameter("fluxNorm", I0);
  config.GetParameter("energyNorm", E0);
  config.GetParameter("spIndex", idx);
  config.GetParameter("energyMin", Emin_);
  config.GetParameter("energyMax", Emax);

  spectrum_ = PowerLaw(E0, Emax, I0, E0, idx);
}

double
GenericSpectrum::GetFlux(const double E, const ModifiedJulianDate& /*mjd*/,
                          const ParticleType& /*type*/)
  const
{
  return spectrum_.Evaluate(E);
}

/// Calculate the weight needed at an energy E to sample the cosmic ray
/// spectral shape; Enorm fixes the flux
double
GenericSpectrum::GetFluxWeight(const double E, const ModifiedJulianDate& /*mjd*/,
                                const PowerLaw& p, const ParticleType& /*type*/)
  const
{
  return spectrum_.Reweight(p, E);
}

double
GenericSpectrum::GetRandomEnergy(const RNGService& rng,
                                 const double E0, const double E1,
                                 const ParticleType& /*type*/)
  const
{
  return rng.PowerLaw(spectrum_.GetSpectralIndex(E0), E0, E1);
}

/// Integrate the spectrum between a lower and upper energy range
double
GenericSpectrum::Integrate(const double E0, const double E1,
                           const ModifiedJulianDate& /*mjd*/,
                           const ParticleType& /*type*/)
  const
{
  return spectrum_.Integrate(E0, E1);
}


/// Get the probability to keep an event sampled from some power law
/// if we want it to obey this spectrum
double
GenericSpectrum::GetProbToKeep(const double E, const PowerLaw& pl,
                               const ModifiedJulianDate& /*mjd*/,
                               const ParticleType& /*type*/)
  const
{
  return spectrum_.GetProbToKeep(pl,E);
}


