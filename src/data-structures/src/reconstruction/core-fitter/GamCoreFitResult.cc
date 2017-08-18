/*!
 * @file GamCoreFitResult.cc
 * @version $Id: GamCoreFitResult.cc 24727 2015-04-04 15:18:20Z kam686 $
 */

#include <data-structures/reconstruction/core-fitter/LDModels.h>
#include <data-structures/reconstruction/core-fitter/GamCoreFitResult.h>

double
GamCoreFitResult::GetLDF(const double r)
  const
{
  return latGamCore(r, amp_, age_, chisquare_);
}

