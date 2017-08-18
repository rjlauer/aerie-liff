/*!
 * @file LogParabola.cc
 * @brief Implementation of log-parabolic power law functions.
 * @author Segev BenZvi
 * @date 21 May 2012
 * @version $Id: LogParabola.cc 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#include <data-structures/math/LogParabola.h>
#include <data-structures/math/SpecialFunctions.h>

#include <cmath>

using namespace std;
using namespace SpecialFunctions;

LogParabola::LogParabola() :
  PowerLaw(1., HAWCUnits::infinity, 1., 1., -2.),
  idx2_(0.)
{ }

LogParabola::LogParabola(const double x0, const double x1,
                         const double A, const double xN,
                         const double a, const double b) :
  PowerLaw(x0, x1, A, xN, a),
  idx2_(b)
{
}

double
LogParabola::Evaluate(const double x)
  const
{
  return A_ * pow(x/xN_, idx1_ + idx2_*log(x/xN_));
}

double
LogParabola::GetNormWeight(const double x0, const double x1)
  const
{
  log_fatal("Not implemented.");
  return 0;   // for compiler
}

double
LogParabola::Reweight(const PowerLaw& pl, const double x)
  const
{
  return Evaluate(x)/pl.Evaluate(x)*pl.Integrate(pl.GetMinX(),pl.GetMaxX());
}

double
LogParabola::GetProbToKeep(const PowerLaw& pl, const double x)
  const
{
  log_fatal("Not implemented.");
  return 0;   // for compiler
}

double
LogParabola::GetEdgeX(unsigned int idx)
  const
{
  log_fatal("Not implemented.");
  return 0;   // for compiler
}

