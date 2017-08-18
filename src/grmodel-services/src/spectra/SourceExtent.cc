/*!
 * @file SourceExtent.cc
 * @brief Simple parameterizations of source extent
 * @author Segev BenZvi
 * @date 15 Feb 2013
 * @version $Id: SourceExtent.cc 14868 2013-04-27 14:16:27Z sybenzvi $
 */

#include <grmodel-services/spectra/SourceExtent.h>

SourceExtent::SourceExtent() :
  shape_(NONE),
  length_(0.),
  width_(0.),
  angle_(0.)
{
}

SourceExtent::SourceExtent(const Shape s,
                           const double l, const double w, const double a) :
  shape_(s),
  length_(l),
  width_(w),
  angle_(a)
{
}

