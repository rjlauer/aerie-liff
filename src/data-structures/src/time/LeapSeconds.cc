/*!
 * @file LeapSeconds.cc 
 * @brief Definition of leap seconds and conversion of dates.
 * @author Darko Veberic
 * @author Segev BenZvi 
 * @date 17 Feb 2010 
 * @version $Id: LeapSeconds.cc 33282 2016-07-13 20:10:02Z sybenzvi $
 */

#include <ctime>

#include <hawcnest/Logging.h>

#include <data-structures/time/LeapSeconds.h>
#include <data-structures/time/UTCDate.h>

using namespace std;

LeapSeconds::LeapSeconds() : gpsEpochInUnixSeconds_(UTCDate::GetGPSEpoch().GetUnixSecond())
{
  Init();
  SetUTC();
}

bool
LeapSeconds::ConvertGPSToUnix(const unsigned int gpsSec, time_t& unixSec)
  const
{
  bool leap = false;
  GPSToUnixIterator low = gpsToUnixLeaps_.lower_bound(gpsSec);
  if (low != gpsToUnixLeaps_.end())
    leap = (low->first == gpsSec && low->second.first == 1);
  --low;
  unixSec = gpsEpochInUnixSeconds_ + gpsSec + low->second.second;
  return leap;
}

void
LeapSeconds::ConvertUnixToGPS(const time_t unixSec, unsigned int& gpsSec)
  const
{
  UnixToGPSIterator up = unixToGPSLeaps_.upper_bound(unixSec);
  --up;
  gpsSec = unixSec - gpsEpochInUnixSeconds_ + up->second;
}

int
LeapSeconds::GetLeapSeconds(const time_t unixSec)
  const
{
  UnixToGPSIterator up = unixToGPSLeaps_.upper_bound(unixSec);
  --up;
  return up->second;
}

void
LeapSeconds::Init()
{
  struct DateLeap {
    UTCDate nextDay_;
    int leap_;
  };

  const DateLeap leaps[] = {
    { UTCDate(1981, UTCDate::JUL, 1), +1 },
    { UTCDate(1982, UTCDate::JUL, 1), +1 },
    { UTCDate(1983, UTCDate::JUL, 1), +1 },
    { UTCDate(1985, UTCDate::JUL, 1), +1 },
    { UTCDate(1988, UTCDate::JAN, 1), +1 },
    { UTCDate(1990, UTCDate::JAN, 1), +1 },
    { UTCDate(1991, UTCDate::JAN, 1), +1 },
    { UTCDate(1992, UTCDate::JUL, 1), +1 },
    { UTCDate(1993, UTCDate::JUL, 1), +1 },
    { UTCDate(1994, UTCDate::JUL, 1), +1 },
    { UTCDate(1996, UTCDate::JAN, 1), +1 },
    { UTCDate(1997, UTCDate::JUL, 1), +1 },
    { UTCDate(1999, UTCDate::JAN, 1), +1 },
    { UTCDate(2006, UTCDate::JAN, 1), +1 },
    { UTCDate(2009, UTCDate::JAN, 1), +1 },
    { UTCDate(2012, UTCDate::JUL, 1), +1 },
    { UTCDate(2015, UTCDate::JUL, 1), +1 },
    { UTCDate(2017, UTCDate::JAN, 1), +1 }
  };
  const int n = sizeof(leaps) / sizeof(leaps[0]);

  int totalLeaps = 0;
  gpsToUnixLeaps_[0] = make_pair(0, 0);
  unixToGPSLeaps_[0] = 0;

  for (int i = 0; i < n; ++i) {
    const time_t unixSec = leaps[i].nextDay_.GetUnixSecond();
    const unsigned int gpsSec = unixSec - gpsEpochInUnixSeconds_ + totalLeaps;
    const int leap = leaps[i].leap_;
    totalLeaps += leap;
    gpsToUnixLeaps_[gpsSec] = make_pair(leap, -totalLeaps);
    unixToGPSLeaps_[unixSec] = totalLeaps;
  }
}

void
LeapSeconds::SetUTC()
{
  const char* const tz = getenv("TZ");
  if (tz)
    tz_ = boost::make_shared<std::string>(tz);
  setenv("TZ", "", 1);
  tzset();
}

void
LeapSeconds::UnsetUTC()
{
  if (tz_)
    setenv("TZ", tz_->c_str(), 1);
  else
    unsetenv("TZ");
  tzset();
}

