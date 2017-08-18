/*!
 * @file BrokenPowerLaw.cc
 * @brief Implementation of broken power law function
 * @author Segev BenZvi
 * @date 10 Jan 2011
 * @version $Id: BrokenPowerLaw.cc 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#include <data-structures/math/BrokenPowerLaw.h>

#include <cmath>

using namespace std;

BrokenPowerLaw::BrokenPowerLaw() :
  PowerLaw(1., HAWCUnits::infinity, 1., 1., -2),
  xB1_(10.),
  idx2_(-2.5)
{ }

BrokenPowerLaw::BrokenPowerLaw(
    const double x0, const double x1, const double A, const double xN,
    const double idx1, const double xB, const double idx2) :
  PowerLaw(x0, x1, A, xN, idx1),
  xB1_(xB),
  idx2_(idx2)
{ }

double
BrokenPowerLaw::Evaluate(const double x)
  const
{
  if (x < xB1_)
    return A_ * pow(x/xN_, idx1_);
  else
    return A_ * pow(xB1_/xN_, idx1_ - idx2_) * pow(x/xN_, idx2_);
}

double
BrokenPowerLaw::GetNormWeight(const double x0, const double x1)
  const
{
  // Put in a sanity check
  if (x0 >= x1)
    log_fatal("Attempt to get non-positive-definite normalization");

  double norm = 1.;

  // Infinite upper limit
  if (x1 == HAWCUnits::infinity) {
    if (idx2_ >= -1.)
      log_fatal("Power law with index " << idx2_
                << " cannot be used with infinite upper limit.");

    // Normalization below the break point: integration over two components
    if (x0 < xB1_) {
      if((idx1_+1.0) == 0.0)
        norm = log(xB1_/x0) - (pow(xB1_,idx1_+1.0)) / (idx2_+1.0);
      else
        norm = (pow(xB1_, idx1_+1) - pow(x0, idx1_+1)) / (idx1_+1) -
               (pow(xB1_, idx1_+1) / (idx2_+1));
      norm = 1. / norm;
    }
    // Normalization above the break point: integration over one component
    else {
      // fixed this so that it matches the other normalizations
      norm = -(idx2_+1.0)*pow(x0,-(idx2_+1.0))*pow(xB1_,idx2_-idx1_);
    }
  }
  // Finite upper limit
  else {
    // Lower limit below the break point
    if (x0 < xB1_) {
      if (x1 < xB1_) {
        // account for the possibility that idx1_ == -1
        if((idx1_+1.0) == 0.0)
          return 1.0/log(x1/x0);
        norm = (pow(x1, idx1_+1) - pow(x0, idx1_+1)) / (idx1_+1);
      }
      else {
        // account for the possibility that idx1_ == -1 or idx2_ == -1
        double term1 = ((idx1_+1.0) == 0) ? log(xB1_/x0) :
                       ((pow(xB1_, idx1_+1) - pow(x0, idx1_+1)) / (idx1_+1));
        double term2 = ((idx2_+1.0) == 0) ? pow(xB1_,idx1_-idx2_)*log(x1/xB1_) :
                       ((pow(xB1_, idx1_-idx2_)*pow(x1, idx2_+1)
                       - pow(xB1_, idx1_+1)) / (idx2_+1));
        norm = term1+term2;
      }
    }
    // Lower limit above the break point
    else {
      // account for the possibility that idx2_ == -1 and fix a bug
      if((idx2_+1.0) == 0.0)
        return pow(xB1_,idx2_-idx1_)/log(x1/x0);
      norm = pow(xB1_,idx1_ - idx2_)*(pow(x1,idx2_+1) - pow(x0,idx2_+1))
             / (idx2_+1);
    }

    norm = 1. / norm;
  }

  return norm;
}

double
BrokenPowerLaw::Reweight(const PowerLaw& pl, const double x)
  const
{
  return Evaluate(x)/pl.Evaluate(x)*pl.Integrate(pl.GetMinX(),pl.GetMaxX());
}

double
BrokenPowerLaw::GetProbToKeep(const PowerLaw& pl, const double x)
  const
{
  // Get the maximum possible weight
  // (this must occur at the edges for each power law)
  double wmax = Reweight(pl,x0_);
  for(unsigned int ctr = 0; ctr < pl.GetNEdges(); ++ctr)
  {
    double wtest = Reweight(pl,pl.GetEdgeX(ctr));
    if(wtest > wmax)
      wmax = wtest;
  }
  for(unsigned int ctr = 0; ctr < GetNEdges(); ++ctr)
  {
    double wtest = Reweight(pl,GetEdgeX(ctr));
    if(wtest > wmax)
      wmax = wtest;
  }

  // Scale the weight to be less than unity with some safety margin
  double margin = 0.95;
  return Reweight(pl,x)*margin/wmax;
}

double
BrokenPowerLaw::GetEdgeX(unsigned int idx)
  const
{
  if (idx == 0)
    return x0_;
  if (idx == 1)
    return xB1_;
  return x1_;
}

double
BrokenPowerLaw::InvertIntegral(const double frac)
  const
{
  if (frac <= 0.0)
    return x0_;
  if (frac >= 1.0)
    return x1_;

  double int1 = GetNormWeight(x0_,x1_)/GetNormWeight(x0_,xB1_);
  double int2 = GetNormWeight(x0_,x1_)/GetNormWeight(xB1_,x1_);
  if(frac <= int1) {
    // the answer lies between x0_ and xB1_
    double newfrac = frac/int1;
    return PowerLaw(x0_,xB1_,1.0,xN_,idx1_).InvertIntegral(newfrac);
  }
  else {
    // the answer lies between xB1_ and x1_
    double newfrac = (frac-int1)/int2;
    return PowerLaw(xB1_,x1_,1.0,xN_,idx2_).InvertIntegral(newfrac);
  }
  return x0_; // for compiler
}

