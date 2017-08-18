/*!
 * @file TabulatedLightCurve.cc
 * @brief A time-dependent light curve based on a table of times and fluxes.
 * @author Segev BenZvi
 * @date 5 Oct 2014
 * @version $Id: TabulatedLightCurve.cc 22113 2014-10-06 02:17:25Z sybenzvi $
 */

#include <grmodel-services/spectra/TabulatedLightCurve.h>

#include <hawcnest/RegisterService.h>

using namespace std;

REGISTER_SERVICE(TabulatedLightCurve);

Configuration
TabulatedLightCurve::DefaultConfiguration()
{
  Configuration config;
  config.Parameter<vector<double> >("mjd");
  config.Parameter<vector<double> >("flux");
  return config;
}

void
TabulatedLightCurve::Initialize(const Configuration& config)
{
  vector<double> mjd;
  config.GetParameter("mjd", mjd);

  vector<double> flux;
  config.GetParameter("flux", flux);

  for (size_t i = 0; i < mjd.size(); ++i) {
    lc_.PushBack(mjd[i], flux[i]);
    peakFlux_ = max(peakFlux_, flux[i]);
  }
}

double
TabulatedLightCurve::GetFlux(const double tMJD)
  const
{
  if (lc_.IsEmpty())
    return 0.;
  return lc_.Evaluate(tMJD);
}

double
TabulatedLightCurve::GetFluxFraction(const double tMJD)
  const
{
  if (lc_.IsEmpty())
    return 1.;
  return lc_.Evaluate(tMJD) / peakFlux_;
}

