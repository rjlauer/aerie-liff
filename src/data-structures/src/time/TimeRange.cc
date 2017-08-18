/*!
 * @file TimeRange.cc 
 * @brief Define a time interval between two TimeStamp values.
 * @author Darko Veberic
 * @author Segev BenZvi 
 * @date 20 Sep 2011 
 * @version $Id: TimeRange.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <data-structures/time/TimeRange.h>

void
TimeRange::SetSorted(const TimeStamp& t0, const TimeStamp& t1)
{
  if (t0 < t1) {
    t0_ = t0;
    t1_ = t1;
  }
  else {
    t0_ = t1;
    t1_ = t0;
  }
}

