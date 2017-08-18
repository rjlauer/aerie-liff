/*!
 * @file LatDistFitResult.cc
 * @version $Id: LatDistFitResult.cc 24727 2015-04-04 15:18:20Z kam686 $
 */

#include <data-structures/reconstruction/core-fitter/LDModels.h>
#include <data-structures/reconstruction/core-fitter/LatDistFitResult.h>

double
LatDistFitResult::GetLDF(const double r)
  const
{
  return lat(r, amp_, age_, chisquare_);
}

