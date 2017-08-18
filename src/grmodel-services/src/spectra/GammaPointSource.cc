
#include <grmodel-services/spectra/GammaPointSource.h>
#include <grmodel-services/spectra/CosmicRayService.h>
#include <grmodel-services/spectra/TabulatedLightCurve.h>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/RegisterService.h>

using namespace HAWCUnits;
using namespace std;


REGISTER_SERVICE(GammaPointSource);

Configuration
GammaPointSource::DefaultConfiguration()
{
  Configuration config;
  config.Parameter<double>("sourceRA", 0.);
  config.Parameter<double>("sourceDec", 0.);
  config.Parameter<double>("redshift", 0.);
  config.Parameter<string>("sourceSpectrum");
  config.Parameter<string>("lightCurve", "");
  config.Parameter<int>("sourceShape", int(SourceExtent::NONE));
  config.Parameter<double>("sourceLength", 0.);
  config.Parameter<double>("sourceWidth", 0.);
  config.Parameter<double>("sourceAngle", 0.);
  return config;
}

void
GammaPointSource::Initialize(const Configuration& config)
{
  // Get the source position
  double ra, dec;
  config.GetParameter("sourceRA", ra);
  config.GetParameter("sourceDec", dec);
  sourceLoc_.SetRADec(ra, dec);

  config.GetParameter("redshift", redshift_);
  config.GetParameter("sourceSpectrum", spectrumServiceName_);
  config.GetParameter("lightCurve", lightCurveName_);

  particleType_ = Gamma;

  int ishape;
  config.GetParameter("sourceShape", ishape);
  SourceExtent::Shape shape = SourceExtent::Shape(ishape);

  if (shape != SourceExtent::NONE) {
    double l, w, a;
    config.GetParameter("sourceLength", l);
    config.GetParameter("sourceWidth", w);
    config.GetParameter("sourceAngle", a);
    extent_ = SourceExtent(shape, l, w, a);
  }
}

double
GammaPointSource::GetFlux(const double E, const ModifiedJulianDate& mjd)
  const
{
  const CosmicRayService& crs =
    GetService<CosmicRayService>(spectrumServiceName_);

  if (lightCurveName_.empty())
    return crs.GetFlux(E, mjd, particleType_);
  else {
    const LightCurve& lc = GetService<LightCurve>(lightCurveName_);
    return lc.GetFluxFraction(mjd) * crs.GetFlux(E, mjd, particleType_);
  }
}

double
GammaPointSource::GetFluxWeight(const double E, const ModifiedJulianDate& mjd, const PowerLaw& p)
  const
{
  const CosmicRayService& crs =
    GetService<CosmicRayService>(spectrumServiceName_);

  return crs.GetFluxWeight(E, mjd, p, particleType_);
}

double
GammaPointSource::GetMinEnergy()
  const
{
  const CosmicRayService& crs =
    GetService<CosmicRayService>(spectrumServiceName_);

  return crs.GetMinEnergy(particleType_);
}

double
GammaPointSource::GetMaxEnergy()
  const
{
  const CosmicRayService& crs =
    GetService<CosmicRayService>(spectrumServiceName_);

  return crs.GetMaxEnergy(particleType_);
}

/// Randomly sample an energy from the source spectrum in [E0, E1]
double
GammaPointSource::GetRandomEnergy(
    const RNGService& rng, const double E0, const double E1)
  const
{
  const CosmicRayService& crs =
    GetService<CosmicRayService>(spectrumServiceName_);

  return crs.GetRandomEnergy(rng, E0, E1, particleType_);
}

double
GammaPointSource::Integrate(const double E0, const double E1,
                       const ModifiedJulianDate& mjd)
  const
{
  const CosmicRayService& crs =
    GetService<CosmicRayService>(spectrumServiceName_);

  if (lightCurveName_.empty())
    return crs.Integrate(E0, E1, mjd, particleType_);
  else {
    const LightCurve& lc = GetService<LightCurve>(lightCurveName_);
    return lc.GetFluxFraction(mjd) * crs.Integrate(E0, E1, mjd, particleType_);
  }
}

