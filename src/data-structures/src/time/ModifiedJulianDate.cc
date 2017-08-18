/*!
 * @file ModifiedJulianDate.cc 
 * @brief Definition of the modified Julian date (MJD) class.
 * @author Segev BenZvi 
 * @date 18 Feb 2010 
 * @version $Id: ModifiedJulianDate.cc 23496 2015-01-07 19:45:23Z sybenzvi $
 */

#include <iomanip>

#include <hawcnest/HAWCUnits.h>

#include <data-structures/time/ModifiedJulianDate.h>
#include <data-structures/time/LeapSeconds.h>

#include <cmath>

using namespace HAWCUnits;
using namespace std;

namespace {

  // Convert seconds in the UNIX epoch (1 Jan 1970 00:00:00 UTC) to MJD
  inline
  double
  MJDFromUnix(const time_t unixSec)
  {
    return (double(unixSec) / 86400. + 40587.) * day;
  }

  // Convert MJD to seconds in the UNIX epoch (1 Jan 1970 00:00:00 UTC)
  inline
  time_t
  UnixFromMJD(const double mjd)
  {
    return lround((mjd/day - 40587.) * 86400);
  }

}

ModifiedJulianDate::ModifiedJulianDate(const UTCDateTime& d)
{
  SetDate(d);
}

ModifiedJulianDate::ModifiedJulianDate(const TimeStamp& gps)
{
  SetDate(gps);
}

ModifiedJulianDate::ModifiedJulianDate(const double& mjd)
{
  SetDate(mjd);
}

void
ModifiedJulianDate::SetDate(const UTCDateTime& d)
{
  const time_t unixSec(d.GetUnixSecond());
  nLeaps_ = LeapSeconds::GetInstance().GetLeapSeconds(unixSec);
  mjd_ = MJDFromUnix(unixSec) + d.GetNanoSecond() * nanosecond;
}

void
ModifiedJulianDate::SetDate(const TimeStamp& gps)
{
  time_t unixSec;
  LeapSeconds::GetInstance().ConvertGPSToUnix(gps.GetGPSSecond(), unixSec);
  nLeaps_ = LeapSeconds::GetInstance().GetLeapSeconds(unixSec);
  mjd_ = MJDFromUnix(unixSec) + gps.GetGPSNanoSecond() * nanosecond;
}

void
ModifiedJulianDate::SetDate(const double& mjd)
{
  mjd_ = mjd;
  // Add a warning if mjd is very small. This can happen if the user forgets to
  // use the HAWC base units during initialization.
  if (mjd < 1*day)
    log_warn("Initializing MJD to " << mjd/day << " days. Did you forget to include time units?");

  // Put in a check to see if we're in the GPS epoch
  if (mjd/day < 40587.)
    nLeaps_ = 0;
  else
    nLeaps_ = LeapSeconds::GetInstance().GetLeapSeconds(UnixFromMJD(mjd));
}

double
ModifiedJulianDate::GetDate(const TimeScale& ts)
  const
{
  // UTC is default
  double mjdTS = mjd_;

  // TAI: GPS offset (19 s) + leaps since GPS epoch (1-1-1980)
  if (ts == TAI)
    mjdTS += (19 + nLeaps_) * second;
  // TT: TAI offset (32.184 s) + GPS offset (19 s) + leaps since 1-1-1980
  else if (ts == TT)
    mjdTS += (51.184 + nLeaps_) * second;

  return mjdTS;
}

UTCDateTime
ModifiedJulianDate::GetUTCDateTime()
  const
{
  return UTCDateTime(GetTimeStamp());
}

TimeStamp
ModifiedJulianDate::GetTimeStamp()
  const
{
  const time_t unixSec = UnixFromMJD(mjd_);
  unsigned int gpsSec;
  LeapSeconds::GetInstance().ConvertUnixToGPS(unixSec, gpsSec);

  return TimeStamp(gpsSec);
}

ostream&
operator<<(ostream& os, const ModifiedJulianDate& mjd)
{
  os << setprecision(12) << mjd.mjd_ / day << " UT" << setprecision(6);
  return os;
}

