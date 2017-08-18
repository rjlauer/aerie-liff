/*!
 * @file SFCoreFitResult.cc
 * @version $Id: SFCoreFitResult.cc 24727 2015-04-04 15:18:20Z sybenzvi $
 */

#include <data-structures/reconstruction/core-fitter/LDModels.h>
#include <data-structures/reconstruction/core-fitter/SFCoreFitResult.h>

double
SFCoreFitResult::GetLDF(const double r)
  const
{
  return sfcf(r, amplitude_, sigma_, tailNorm_, rmol_);
}

