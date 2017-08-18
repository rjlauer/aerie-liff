/*!
 * @file CutoffPowerLaw.cc
 * @brief Implementation of cutoff power law functions
 * @author Segev BenZvi
 * @date 18 Apr 2012
 * @version $Id: CutoffPowerLaw.cc 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#include <data-structures/math/CutoffPowerLaw.h>
#include <data-structures/math/SpecialFunctions.h>

#include <cmath>

using namespace std;
using namespace SpecialFunctions;

CutoffPowerLaw::CutoffPowerLaw() :
  PowerLaw(1., HAWCUnits::infinity, 1., 1., -2.),
  xC_(10.)
{ }

CutoffPowerLaw::CutoffPowerLaw(const double x0, const double x1,
                   const double A, const double xN,
                   const double idx, const double xC) :
  PowerLaw(x0, x1, A, xN, idx),
  xC_(xC)
{
}

double
CutoffPowerLaw::Evaluate(const double x)
  const
{
  return A_ * pow(x/xN_, idx1_) * exp(-x/xC_);
}

double
CutoffPowerLaw::GetNormWeight(const double x0, const double x1)
  const
{
  const double a = idx1_ + 1.;

  // Normalization, integrating from x0 to infinity
  if (x1 == HAWCUnits::infinity)
    return 1. / (pow(xC_, a) * Gamma::G(a, x0/xC_));

  // Normalization for finite upper limit
  return 1. / (pow(xC_, a) * (Gamma::G(a, x0/xC_) - Gamma::G(a, x1/xC_)));
}

double
CutoffPowerLaw::Reweight(const PowerLaw& pl, const double x)
  const
{
  return Evaluate(x)/pl.Evaluate(x)*pl.Integrate(pl.GetMinX(),pl.GetMaxX());
}

double
CutoffPowerLaw::GetProbToKeep(const PowerLaw& pl, const double x)
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
CutoffPowerLaw::GetEdgeX(unsigned int idx)
  const
{
  if(idx == 0)
    return x0_;
  return x1_;
}

double
CutoffPowerLaw::InvertIntegral(const double frac)
  const
{
  log_fatal("Not implemented.");
  return 0;   // for compiler
}

