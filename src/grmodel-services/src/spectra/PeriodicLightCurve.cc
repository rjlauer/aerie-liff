/*!
 * @file PeriodicLightCurve.cc
 * @brief A non-parametric, periodic light curve.
 * @author Segev BenZvi
 * @date 5 Oct 2014
 * @version $Id: PeriodicLightCurve.cc 22114 2014-10-06 03:33:38Z sybenzvi $
 */

#include <grmodel-services/spectra/PeriodicLightCurve.h>

#include <hawcnest/RegisterService.h>

#include <algorithm>

using namespace std;

REGISTER_SERVICE(PeriodicLightCurve);

Configuration
PeriodicLightCurve::DefaultConfiguration()
{
  Configuration config;
  config.Parameter<double>("mjd0");
  config.Parameter<double>("period");
  config.Parameter<vector<double> >("phase");
  config.Parameter<vector<double> >("value");
  return config;
}

void
PeriodicLightCurve::Initialize(const Configuration& config)
{
  double mjd0;
  config.GetParameter("mjd0", mjd0);

  double period;
  config.GetParameter("period", period);

  vector<double> phase;
  config.GetParameter("phase", phase);

  vector<double> value;
  config.GetParameter("value", value);

  lc_ = boost::make_shared<PeriodicFunction>(mjd0, period, phase, value);
  peakFlux_ = *max_element(value.begin(), value.end());
}

double
PeriodicLightCurve::GetFlux(const double tMJD)
  const
{
  return lc_->Evaluate(tMJD);
}

double
PeriodicLightCurve::GetFluxFraction(const double tMJD)
  const
{
  return lc_->Evaluate(tMJD) / peakFlux_;
}

