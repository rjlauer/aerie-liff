/*!
 * @file PowerLaw.cc
 * @brief Implementation of power law functions.
 * @author Segev BenZvi
 * @date 10 Jan 2011
 * @version $Id: PowerLaw.cc 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#include <data-structures/math/PowerLaw.h>

#include <cmath>

using namespace std;

PowerLaw::PowerLaw() :
  x0_(1.),
  x1_(HAWCUnits::infinity),
  A_(1.),
  xN_(1.),
  idx1_(-2.)
{ }

PowerLaw::PowerLaw(const double x0, const double x1,
                   const double A, const double xN, const double idx) :
  x0_(x0),
  x1_(x1),
  A_(A),
  xN_(xN),
  idx1_(idx)
{
}

double
PowerLaw::Evaluate(const double x)
  const
{
  return A_ * pow(x/xN_, idx1_);
}

double
PowerLaw::Integrate(const double x0, const double x1)
  const
{
  return A_ * pow(xN_, -idx1_) / GetNormWeight(x0,x1);
}

double
PowerLaw::GetNormWeight(const double x0, const double x1)
  const
{
  // Put in a sanity check
  if (x0 >= x1)
    log_fatal("Attempt to get non-positive-definite normalization");

  // Normalization, integrating from x0 to infinity
  if (x1 == HAWCUnits::infinity) {
    if (idx1_ >= -1.)
      log_fatal("Power law with index " << idx1_
                << " cannot be used with infinite upper limit.");
    return -(idx1_+1) * pow(x0, -(idx1_+1));
  }

  // Account for the possibility that idx1_ == -1
  if ((idx1_ + 1.) == 0.0)
    return 1. / log(x1/x0);

  // Normalization for finite upper limit
  return (idx1_+1)/(pow(x1, idx1_+1) - pow(x0, idx1_+1));
}

double
PowerLaw::Reweight(const PowerLaw& pl, const double x)
  const
{
  return Evaluate(x) / pl.Evaluate(x) * pl.Integrate(pl.GetMinX(),pl.GetMaxX());
}

double
PowerLaw::GetProbToKeep(const PowerLaw& pl, const double x)
  const
{
  // Get the maximum possible weight
  // (this must occur at the edges for each power law)
  double wmax = Reweight(pl, x0_);
  for (unsigned int ctr = 0; ctr < pl.GetNEdges(); ++ctr)
  {
    double wtest = Reweight(pl,pl.GetEdgeX(ctr));
    if(wtest > wmax)
      wmax = wtest;
  }
  for (unsigned int ctr = 0; ctr < GetNEdges(); ++ctr)
  {
    double wtest = Reweight(pl,GetEdgeX(ctr));
    if (wtest > wmax)
      wmax = wtest;
  }

  // Scale the weight to be less than unity with some safety margin
  double margin = 0.95;
  return Reweight(pl,x)*margin/wmax;
}

double
PowerLaw::GetEdgeX(unsigned int idx)
  const
{
  if (idx == 0)
    return x0_;
  return x1_;
}

double
PowerLaw::InvertIntegral(const double frac)
  const
{
  if (frac <= 0.0)
    return x0_;
  if (frac >= 1.0)
    return x1_;

  double prefactor = 1.0/GetNormWeight(x0_,x1_);
  if ((idx1_+1.0) == 0.0) {
    return x0_*exp(prefactor*frac);
  }
  else {
    double term1 = prefactor*(idx1_+1.0)*frac;
    double term2 = pow(x0_,idx1_+1.0);
    double power = 1.0/(idx1_+1.0);
    return pow(term1+term2,power);
  }
  return x0_; // for compiler
}

