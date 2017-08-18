
#include <grmodel-services/spectra/GenericBrokenSpectrum.h>

#include <rng-service/RNGService.h>

#include <data-structures/time/ModifiedJulianDate.h>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/Logging.h>
#include <hawcnest/RegisterService.h>

#include <cmath>

using namespace std;
using namespace HAWCUnits;

REGISTER_SERVICE(GenericBrokenSpectrum);

Configuration
GenericBrokenSpectrum::DefaultConfiguration()
{
  Configuration config;

  config.Parameter<double>("fluxNorm", 1e-10 / (cm2*second*TeV));
  config.Parameter<double>("energyNorm", 1*TeV);
  config.Parameter<double>("spIndex1", -2.5);
  config.Parameter<double>("energyBreak", 20*TeV);
  config.Parameter<double>("spIndex2", -3.5);
  config.Parameter<double>("energyMin", 100*GeV);
  config.Parameter<double>("energyMax", 100*TeV);

  return config;
}

void
GenericBrokenSpectrum::Initialize(const Configuration& config)
{
  double I0;
  double E0;
  double idx1;
  double idx2;
  double Eb;
  double Emax;

  config.GetParameter("fluxNorm", I0);
  config.GetParameter("energyNorm", E0);
  config.GetParameter("spIndex1", idx1);
  config.GetParameter("energyBreak", Eb);
  config.GetParameter("spIndex2", idx2);
  config.GetParameter("energyMin", Emin_);
  config.GetParameter("energyMax", Emax);

  throwIdx_ = max(-2., idx1);
  throwIdx_ = max(throwIdx_, idx2);

  spectrum_ = BrokenPowerLaw(Emin_, Emax, I0, E0, idx1, Eb, idx2);
}

double
GenericBrokenSpectrum::GetFlux(const double E, const ModifiedJulianDate& /*mjd*/,
                          const ParticleType& /*type*/)
  const
{
  return spectrum_.Evaluate(E);
}

/// Calculate the weight needed at an energy E to sample the cosmic ray
/// spectral shape; Enorm fixes the flux
double
GenericBrokenSpectrum::GetFluxWeight(const double E, const ModifiedJulianDate& /*mjd*/,
                                const PowerLaw& p, const ParticleType& /*type*/)
  const
{
  return spectrum_.Reweight(p, E);
}

double
GenericBrokenSpectrum::GetRandomEnergy(const RNGService& rng,
                                       const double E0, const double E1,
                                       const ParticleType& type)
  const
{
  double E;
  static const ModifiedJulianDate mjd(55555*day);
  static const double F0 = GetFlux(E0,mjd,type);
  static const PowerLaw pl(E0, E1, F0, E0, throwIdx_);

  while (true) {
    E = rng.PowerLaw(throwIdx_, E0, E1);
    if (rng.Uniform() <= GetFlux(E,mjd,type)/pl.Evaluate(E))
      break;
  }

  return E;
}

/// Integrate the spectrum between a lower and upper energy range
double
GenericBrokenSpectrum::Integrate(const double E0, const double E1,
                                 const ModifiedJulianDate& /*mjd*/,
                                 const ParticleType& /*type*/)
  const
{
  return spectrum_.Integrate(E0, E1);
}

/// Get the probability to keep an event sampled from some power law
/// if we want it to obey this spectrum
double
GenericBrokenSpectrum::GetProbToKeep(const double E, const PowerLaw& pl,
                                     const ModifiedJulianDate& /*mjd*/,
                                     const ParticleType& /*type*/)
  const
{
  return spectrum_.GetProbToKeep(pl,E);
}

