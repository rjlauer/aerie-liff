/*!
 * @file UTCDate.cc 
 * @brief Representation of a calendar date.
 * @author Darko Veberic
 * @author Segev BenZvi 
 * @date 17 Feb 2010 
 * @version $Id: UTCDate.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <ctime>

#include <hawcnest/Logging.h>

#include <data-structures/time/UTCDate.h>

using namespace std;

void
UTCDate::SetDate(const int year, const int month, const int day)
{
  if (year < 1980 || (year == 1980 && month == 1 && day < 6))
    log_fatal("Date before GPS epoch");

  if (month < 1 || month > 12)
    log_fatal("Month " << month << " not in range 1 - 12");

  if (day < 1 || day > DaysPerMonth(year, month))
    log_fatal("Day " << day << " not in range 1-" << DaysPerMonth(year, month));

  year_ = year;
  month_ = month;
  day_ = day;
}

time_t
UTCDate::GetUnixSecond(const int year, const int month, const int day,
                       const int hour, const int minute, const int second)
{
  tm theTime = {
    second,
    minute,
    hour,
    day,
    month - 1,
    year - 1900,
    0, 
    0, 
    0
#ifdef _BSD_SOURCE
    , 0, 0
#endif
  };

  const time_t t1 = mktime(&theTime);
  const time_t dgm = mktime(gmtime(&t1));
  return t1 - (dgm - t1);
}

