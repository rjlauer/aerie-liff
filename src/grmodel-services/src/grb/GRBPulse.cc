/*!
 * @file GRBPulse.cc
 * @author Segev BenZvi
 * @date 14 Jul 2011
 * @brief GRB pulse parameterization based on a double-exponential flare
 *        with a band-function spectrum.
 * @version $Id: GRBPulse.cc 18002 2013-11-25 17:47:47Z sybenzvi $
 */

#include <grmodel-services/grb/GRBPulse.h>
#include <data-structures/time/ModifiedJulianDate.h>

#include <cmath>

using namespace HAWCUnits;
using namespace std;

// Anonymous namespace for file-level constants
namespace {

  const double E0 = 100 * keV;
  const double DT = 0.5;
  const double WE = 0.33;
  const double LOG100 = log(1e2);

}

GRBPulse::GRBPulse() :
  tPeak_(0),
  tStart_(0),
  tEnd_(0),
  sRise_(0.5*second),
  sDecay_(1*second),
  I_(1e-6 * erg / cm2),
  nu_(1.),
  Epeak_(100*keV),
  alpha_(-1.),
  beta_(-2.)
{
  Ec_ = (alpha_-beta_) * Epeak_ / (2+alpha_-WE);
}

double
GRBPulse::GetIntensity(const double E, const ModifiedJulianDate& mjd)
  const
{
  const double t = mjd.GetDate();
  if (t < tStart_ || t > tEnd_)
    return 0.;

  // Band function
  double bandf = 0.;
  if (E < Ec_)
    bandf = pow(E/E0, alpha_) * exp(-E/E0);
  else
    bandf = pow(Ec_/E0, alpha_-beta_) * exp(beta_-alpha_) * pow(E/E0, beta_);

  // Profile function from Norris et al., with energy dependent narrowing and
  // shift in peak time
  double efac = pow(E/E0, -WE);
  double rt = sRise_ * efac;
  double dt = sDecay_ * efac;
  double deltaT = DT * (sRise_ - rt) * pow(LOG100, 1./nu_);
  double tp = tPeak_ - deltaT;

  double pulse = 0.;
  if (t < tp)
    pulse = exp(-pow(abs(t-tp)/rt, nu_));
  else
    pulse = exp(-pow(abs(t-tp)/dt, nu_));

  return I_ * bandf * pulse;
}

