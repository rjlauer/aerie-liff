/*!
 * @file PeriodicFunction.cc
 * @brief A non-parametric periodic function defined by a data table
 * @author Segev BenZvi
 * @date 5 Oct 2014
 * @version $Id: PeriodicFunction.cc 22111 2014-10-06 00:16:33Z sybenzvi $
 */

#include <data-structures/math/PeriodicFunction.h>

#include <hawcnest/Logging.h>

#include <cmath>

using namespace std;

PeriodicFunction::PeriodicFunction(const double t0, const double period,
                                   const vecD& fphase, const vecD& fnfunc) :
  t0_(t0),
  period_(period)
{
  // Initialize the table, starting with some sanity checks and normalizations
  if (fphase.size() != fnfunc.size())
    log_fatal("Periodic function coords and abscissa are different sizes.");

  // Phase of cycle should fall in [0,1]
  const double last = fphase.back();

  // Initialize the table
  for (size_t i = 0; i < fphase.size(); ++i)
    func_.PushBack(fphase[i]/last, fnfunc[i]);
}

double
PeriodicFunction::Evaluate(const double& t)
  const
{
  // Calculate the relative position in the periodic cycle
  double dphase = fmod((t - t0_), period_) / period_;
  if (dphase < 0)
    dphase += 1;

  // Linearly interpolate to return the function value
  return func_.Evaluate(dphase);
}

