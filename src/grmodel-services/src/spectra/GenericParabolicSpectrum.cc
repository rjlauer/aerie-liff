
#include <grmodel-services/spectra/GenericParabolicSpectrum.h>
#include <data-structures/time/ModifiedJulianDate.h>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/Logging.h>
#include <hawcnest/RegisterService.h>

using namespace std;
using namespace HAWCUnits;

REGISTER_SERVICE(GenericParabolicSpectrum);

Configuration
GenericParabolicSpectrum::DefaultConfiguration()
{
  Configuration config;

  config.Parameter<double>("fluxNorm", 1e-10 / (cm2*second*TeV));
  config.Parameter<double>("energyNorm", 1*TeV);
  config.Parameter<double>("indexA", -2.5);
  config.Parameter<double>("indexB", -0.1);
  config.Parameter<double>("energyMin", 100*GeV);
  config.Parameter<double>("energyMax", 100*TeV);

  return config;
}

void
GenericParabolicSpectrum::Initialize(const Configuration& config)
{
  double I0;
  double E0;
  double a;
  double b;
  double Emax;

  config.GetParameter("fluxNorm", I0);
  config.GetParameter("energyNorm", E0);
  config.GetParameter("indexA", a);
  config.GetParameter("indexB", b);
  config.GetParameter("energyMin", Emin_);
  config.GetParameter("energyMax", Emax);

  spectrum_ = LogParabola(Emin_, Emax, I0, E0, a, b);
}

double
GenericParabolicSpectrum::GetFlux(const double E,
                          const ModifiedJulianDate& /*mjd*/,
                          const ParticleType& /*type*/)
  const
{
  return spectrum_.Evaluate(E);
}

/// Calculate the weight needed at an energy E to sample the cosmic ray
/// spectral shape; Enorm fixes the flux
double
GenericParabolicSpectrum::GetFluxWeight(
  const double E, const ModifiedJulianDate& /*mjd*/,
  const PowerLaw& p, const ParticleType& /*type*/)
  const
{
  return spectrum_.Reweight(p, E);
}

/// Integrate the spectrum between a lower and upper energy range
double
GenericParabolicSpectrum::Integrate(const double E0, const double E1,
                                    const ModifiedJulianDate& /*mjd*/,
                                    const ParticleType& /*type*/)
  const
{
  return spectrum_.Integrate(E0, E1);
}

/// Get the probability to keep an event sampled from some power law
/// if we want it to obey this spectrum
double
GenericParabolicSpectrum::GetProbToKeep(const double E, const PowerLaw& pl,
                                        const ModifiedJulianDate& /*mjd*/,
                                        const ParticleType& /*type*/)
  const
{
  return spectrum_.GetProbToKeep(pl,E);
}

