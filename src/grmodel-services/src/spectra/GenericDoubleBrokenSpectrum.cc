
#include <grmodel-services/spectra/GenericDoubleBrokenSpectrum.h>

#include <rng-service/RNGService.h>

#include <data-structures/math/PowerLaw.h>
#include <data-structures/time/ModifiedJulianDate.h>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/Logging.h>
#include <hawcnest/RegisterService.h>

#include <cmath>

using namespace std;
using namespace HAWCUnits;

REGISTER_SERVICE(GenericDoubleBrokenSpectrum);

Configuration
GenericDoubleBrokenSpectrum::DefaultConfiguration()
{
  Configuration config;

  config.Parameter<double>("fluxNorm", 1e-10 / (cm2*second*TeV));
  config.Parameter<double>("energyNorm", 1*TeV);
  config.Parameter<double>("spIndex1", -2.5);
  config.Parameter<double>("energyBreak1", 20*TeV);
  config.Parameter<double>("spIndex2", -3.5);
  config.Parameter<double>("energyBreak2", 40*TeV);
  config.Parameter<double>("spIndex3", -3.0);
  config.Parameter<double>("energyMin", 100*GeV);
  config.Parameter<double>("energyMax", 100*TeV);

  return config;
}

void
GenericDoubleBrokenSpectrum::Initialize(const Configuration& config)
{
  double I0;
  double E0;
  double idx1;
  double idx2;
  double idx3;
  double Eb1;
  double Eb2;
  double Emax;

  config.GetParameter("fluxNorm", I0);
  config.GetParameter("energyNorm", E0);
  config.GetParameter("spIndex1", idx1);
  config.GetParameter("energyBreak1", Eb1);
  config.GetParameter("spIndex2", idx2);
  config.GetParameter("energyBreak2", Eb2);
  config.GetParameter("spIndex3", idx3);
  config.GetParameter("energyMin", Emin_);
  config.GetParameter("energyMax", Emax);

  throwIdx_ = max(-2., idx1);
  throwIdx_ = max(throwIdx_, idx2);
  throwIdx_ = max(throwIdx_, idx3);

  spectrum_ = DoubleBrokenPowerLaw(Emin_, Emax, I0, E0, idx1, Eb1, idx2,
                                   Eb2, idx3);
}

double
GenericDoubleBrokenSpectrum::GetFlux(const double E, const ModifiedJulianDate& /*mjd*/,
                          const ParticleType& /*type*/)
  const
{
  return spectrum_.Evaluate(E);
}

/// Calculate the weight needed at an energy E to sample the cosmic ray
/// spectral shape; Enorm fixes the flux
double
GenericDoubleBrokenSpectrum::GetFluxWeight(const double E, const ModifiedJulianDate& /*mjd*/,
                                const PowerLaw& p, const ParticleType& /*type*/)
  const
{
  return spectrum_.Reweight(p, E);
}

double
GenericDoubleBrokenSpectrum::GetRandomEnergy(const RNGService& rng,
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
GenericDoubleBrokenSpectrum::Integrate(const double E0, const double E1,
                                 const ModifiedJulianDate& /*mjd*/,
                                 const ParticleType& /*type*/)
  const
{
  return spectrum_.Integrate(E0, E1);
}

/// Get the probability to keep an event sampled from some power law
/// if we want it to obey this spectrum
double
GenericDoubleBrokenSpectrum::GetProbToKeep(const double E, const PowerLaw& pl,
                                     const ModifiedJulianDate& /*mjd*/,
                                     const ParticleType& /*type*/)
  const
{
  return spectrum_.GetProbToKeep(pl,E);
}

