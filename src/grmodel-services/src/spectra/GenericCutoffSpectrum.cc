
#include <grmodel-services/spectra/GenericCutoffSpectrum.h>
#include <data-structures/time/ModifiedJulianDate.h>

#include <rng-service/RNGService.h>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/Logging.h>
#include <hawcnest/RegisterService.h>

#include <cmath>

using namespace std;
using namespace HAWCUnits;

REGISTER_SERVICE(GenericCutoffSpectrum);

Configuration
GenericCutoffSpectrum::DefaultConfiguration()
{
  Configuration config;

  config.Parameter<double>("fluxNorm", 1e-10 / (cm2*second*TeV));
  config.Parameter<double>("energyNorm", 1*TeV);
  config.Parameter<double>("spIndex", -2.5);
  config.Parameter<double>("energyCutoff", 20*TeV);
  config.Parameter<double>("energyMin", 100*GeV);
  config.Parameter<double>("energyMax", 100*TeV);

  return config;
}

void
GenericCutoffSpectrum::Initialize(const Configuration& config)
{
  double Emin;
  double Emax;
  double I0;
  double E0;
  double idx;
  double Ec;

  config.GetParameter("fluxNorm", I0);
  config.GetParameter("energyNorm", E0);
  config.GetParameter("spIndex", idx);
  config.GetParameter("energyCutoff", Ec);
  config.GetParameter("energyMin", Emin);
  config.GetParameter("energyMax", Emax);

  spectrum_ = CutoffPowerLaw(Emin, Emax, I0, E0, idx, Ec);
}

double
GenericCutoffSpectrum::GetFlux(const double E, const ModifiedJulianDate& /*mjd*/,
                          const ParticleType& /*type*/)
  const
{
  return spectrum_.Evaluate(E);
}

/// Calculate the weight needed at an energy E to sample the cosmic ray
/// spectral shape; Enorm fixes the flux
double
GenericCutoffSpectrum::GetFluxWeight(const double E, const ModifiedJulianDate& /*mjd*/,
                                const PowerLaw& p, const ParticleType& /*type*/)
  const
{
  return spectrum_.Reweight(p, E);
}

double
GenericCutoffSpectrum::GetRandomEnergy(const RNGService& rng,
                                       const double E0, const double E1,
                                       const ParticleType& /*type*/)
  const
{
  double E;
  do {
    E = rng.PowerLaw(spectrum_.GetSpectralIndex(E0), E0, E1);
  } while (rng.Uniform() > exp(-E/spectrum_.GetCutoffX()));

  return E;
}

/// Integrate the spectrum between a lower and upper energy range
double
GenericCutoffSpectrum::Integrate(const double E0, const double E1,
                                 const ModifiedJulianDate& /*mjd*/,
                                 const ParticleType& /*type*/)
  const
{
  return spectrum_.Integrate(E0, E1);
}

/// Get the probability to keep an event sampled from some power law
/// if we want it to obey this spectrum
double
GenericCutoffSpectrum::GetProbToKeep(const double E, const PowerLaw& pl,
                                     const ModifiedJulianDate& /*mjd*/,
                                     const ParticleType& /*type*/)
  const
{
  return spectrum_.GetProbToKeep(pl,E);
}

