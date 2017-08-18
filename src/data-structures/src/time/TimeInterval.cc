/*!
 * @file TimeInterval.cc 
 * @brief Storage for some elapsed time.
 * @author Tom Paul
 * @author Segev BenZvi 
 * @date 18 Feb 2010 
 * @version $Id: TimeInterval.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <hawcnest/HAWCUnits.h>

#include <data-structures/time/TimeInterval.h>

using namespace HAWCUnits;
using namespace std;

int
TimeInterval::GetSecond()
  const
{
  if (interval_ < 0)
    return int(interval_ / second - 1.);
  else
    return int(interval_ / second);
}

unsigned int
TimeInterval::GetNanoSecond()
  const
{
  return (unsigned int)((interval_/second - double(GetSecond())) * (second/nanosecond) + 0.5);
}

