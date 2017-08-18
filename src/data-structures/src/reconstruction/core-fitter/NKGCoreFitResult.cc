/*!
 * @file NKGCoreFitResult.cc
 * @version $Id: NKGCoreFitResult.cc 24722 2015-04-04 07:43:59Z sybenzvi $
 */

#include <data-structures/reconstruction/core-fitter/LDModels.h>
#include <data-structures/reconstruction/core-fitter/NKGCoreFitResult.h>

double
NKGCoreFitResult::GetLDF(const double r)
  const
{
  return nkg(r, amplitude_, ageParameter_, true);
}

