/*!
 * @file DoubleBrokenPowerLaw.cc
 * @brief Power law functions with two break points.
 * @author Segev BenZvi
 * @date 10 Jan 2011
 * @version $Id: DoubleBrokenPowerLaw.cc 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#include <data-structures/math/DoubleBrokenPowerLaw.h>

#include <cmath>

using namespace std;

DoubleBrokenPowerLaw::DoubleBrokenPowerLaw() :
  BrokenPowerLaw(1., 1e10, 1., 1., -2., 10., -2.),
  xB2_(100.),
  idx3_(-2.)
{
}

DoubleBrokenPowerLaw::DoubleBrokenPowerLaw(
    const double x0, const double x1, const double A, const double xN,
    const double idx1, const double xB1, const double idx2, const double xB2,
    const double idx3) :
  BrokenPowerLaw(x0, x1, A, xN, idx1, xB1, idx2),
  xB2_(xB2),
  idx3_(idx3)
{
}

double
DoubleBrokenPowerLaw::GetSpectralIndex(const double x)
  const
{
  if (x < xB1_)
    return idx1_;
  else if (x >= xB1_ && x < xB2_)
    return idx2_;
  else
    return idx3_;
}

double
DoubleBrokenPowerLaw::Evaluate(const double x)
  const
{
  if (x < xB1_)
    return A_ * pow(x/xN_, idx1_);
  else if (x >= xB1_ && x < xB2_) 
    return A_ * pow(xB1_/xN_, idx1_-idx2_) * pow(x/xN_, idx2_);
  else
    return A_ * pow(xB1_/xN_, idx1_-idx2_) * pow(xB2_/xN_, idx2_-idx3_)
              * pow(x/xN_, idx3_);
}

double
DoubleBrokenPowerLaw::GetNormWeight(const double x0, const double x1)
  const
{
  // Put in a sanity check
  if(x0 >= x1)
    log_fatal("Attempt to get non-positive-definite normalization");

  double norm = 1.;

  // Infinite upper limit
  if (x1 == HAWCUnits::infinity) {
    if (idx3_ >= -1.)
      log_fatal("Power law with index " << idx3_ 
                << " cannot be used with infinite upper limit.");

    // Normalization when starting below first break point
    if (x0 < xB1_) {
      double factor2 = pow(xB1_,idx1_ - idx2_);
      double term1 = ((idx1_ + 1.0) == 0.0) ? log(xB1_/x0) :
                     ((pow(xB1_,idx1_ + 1.0) - pow(x0,idx1_ + 1.0)) / (idx1_ + 1.0));
      double term2 = ((idx2_ + 1.0) == 0.0) ? log(xB2_/xB1_) :
                     ((pow(xB2_,idx2_ + 1.0) - pow(xB1_,idx2_ + 1.0)) / (idx2_ + 1.0));
      double term3 = -pow(xB2_,idx2_ + 1.0) * pow(xB1_, idx1_ - idx2_) / (idx3_ + 1.0);
      norm = term1 + factor2*term2 + term3;
      norm = 1. / norm;
    }
    // Normalization when starting between first and second break points
    else if (x0 < xB2_) {
      double term1 = ((idx2_ + 1.0) == 0.0) ? log(xB2_/x0) :
                     ((pow(xB2_,idx2_ + 1.0) - pow(x0,idx2_ + 1.0)) / (idx2_ + 1.0));
      double term2 = -pow(xB2_,idx2_ + 1.0) / (idx3_ + 1.0);
      norm = pow(xB1_,idx1_ - idx2_) * (term1 + term2);
      norm = 1. / norm;
    }
    // Normalization after the second break point
    else {
      norm = -pow(xB1_,idx2_ - idx1_) * pow(xB2_,idx3_ - idx2_) *
             (idx3_+1.) * pow(x0, -(idx3_+1.));
    }
  }
  // Finite upper limits
  else {
    if(x0 < xB1_) {
      if(x1 < xB1_) {
        if((idx1_+1.0) == 0.0)
          norm = log(x1/x0);
        else
          norm = (pow(x1,idx1_ + 1.0)-pow(x0,idx1_ + 1.0)) / (idx1_ + 1.0);
      }
      else if(x1 < xB2_) {
        double factor2 = pow(xB1_,idx1_ - idx2_);
        double term1 = ((idx1_ + 1.0) == 0.0) ? log(xB1_/x0) :
                       ((pow(xB1_,idx1_ + 1.0) - pow(x0,idx1_ + 1.0)) / (idx1_ + 1.0));
        double term2 = ((idx2_ + 1.0) == 0.0) ? log(x1/xB1_) :
                       ((pow(x1,idx2_ + 1.0) - pow(xB1_,idx2_ + 1.0)) / (idx2_ + 1.0));
        norm = term1 + factor2*term2;
      }
      else {
        double factor2 = pow(xB1_,idx1_ - idx2_);
        double factor3 = factor2 * pow(xB2_,idx2_ - idx3_);
        double term1 = ((idx1_ + 1.0) == 0.0) ? log(xB1_/x0) :
                       ((pow(xB1_,idx1_ + 1.0) - pow(x0,idx1_ + 1.0)) / (idx1_ + 1.0));
        double term2 = ((idx2_ + 1.0) == 0.0) ? log(xB2_/xB1_) :
                       ((pow(xB2_,idx2_ + 1.0) - pow(xB1_,idx2_ + 1.0)) / (idx2_ + 1.0));
        double term3 = ((idx3_ + 1.0) == 0.0) ? log(x1/xB2_) :
                       ((pow(x1,idx3_ + 1.0) - pow(xB2_,idx3_ + 1.0)) / (idx3_ + 1.0));
        norm = term1 + factor2*term2 + factor3*term3;
      }
    }
    else if(x0 < xB2_) {
      if(x1 < xB2_) {
        if((idx2_+1.0) == 0.0) {
          norm = pow(xB1_,idx1_ - idx2_) * log(x1/x0);
        }
        else {
          norm = pow(xB1_,idx1_ - idx2_) *
          (pow(x1,idx2_ + 1.0)-pow(x0,idx2_ + 1.0)) / (idx2_ + 1.0);
        }
      }
      else {
        double factor2 = pow(xB2_,idx2_ - idx3_);
        double term1 = ((idx2_ + 1.0) == 0.0) ? log(xB2_/x0) :
                       ((pow(xB2_,idx2_ + 1.0) - pow(x0,idx2_ + 1.0)) / (idx2_ + 1.0));
        double term2 = ((idx3_ + 1.0) == 0.0) ? log(x1/xB2_) :
                       ((pow(x1,idx3_ + 1.0) - pow(xB2_,idx3_ + 1.0)) / (idx3_ + 1.0));
        norm = pow(xB1_,idx1_ - idx2_) * (term1 + factor2*term2);
      }
    }
    else {
      if((idx3_+1.0) == 0.0) {
        norm = pow(xB1_,idx1_ - idx2_) * pow(xB2_,idx2_ - idx3_) * log(x1/x0);
      }
      else {
        norm = pow(xB1_,idx1_ - idx2_) * pow(xB2_,idx2_ - idx3_) *
               (pow(x1,idx3_ + 1.0)-pow(x0,idx3_ + 1.0)) / (idx3_ + 1.0);
      }
    }
    norm = 1.0/norm;
  }

  return norm;
}

double
DoubleBrokenPowerLaw::Reweight(const PowerLaw& pl, const double x)
  const
{
  return Evaluate(x)/pl.Evaluate(x)*pl.Integrate(pl.GetMinX(),pl.GetMaxX());
}

double
DoubleBrokenPowerLaw::GetProbToKeep(const PowerLaw& pl, const double x)
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
DoubleBrokenPowerLaw::GetEdgeX(unsigned int idx)
  const
{
  if (idx == 0)
    return x0_;
  if (idx == 1)
    return xB1_;
  if (idx == 2)
    return xB2_;
  return x1_;
}

double
DoubleBrokenPowerLaw::InvertIntegral(const double frac)
  const
{
  if (frac <= 0.0)
    return x0_;
  if (frac >= 1.0)
    return x1_;

  double int1 = GetNormWeight(x0_,x1_)/GetNormWeight(x0_,xB1_);
  double int2 = GetNormWeight(x0_,x1_)/GetNormWeight(xB1_,xB2_);
  double intfrac = GetNormWeight(x0_,x1_)/GetNormWeight(x0_,xB2_);
  double int3 = GetNormWeight(x0_,x1_)/GetNormWeight(xB2_,x1_);

  if (frac <= int1) {
    // the answer lies between x0_ and xB1_
    double newfrac = frac/int1;
    return PowerLaw(x0_,xB1_,1.0,xN_,idx1_).InvertIntegral(newfrac);
  }
  else if (frac <= intfrac) {
    // the answer lies between xB1_ and xB2_
    double newfrac = (frac-int1)/int2;
    return PowerLaw(xB1_,xB2_,1.0,xN_,idx2_).InvertIntegral(newfrac);
  }
  else {
    // the answer lies between xB2_ and x1_
    double newfrac = (frac-int1-int2)/int3;
    return PowerLaw(xB2_,x1_,1.0,xN_,idx3_).InvertIntegral(newfrac);
  }
  return x0_; // for compiler
}

