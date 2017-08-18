/*!
 * @file UTCDateTime.cc 
 * @brief Representation of a calendar date and time.
 * @author Darko Veberic
 * @author Segev BenZvi 
 * @date 17 Feb 2010 
 * @version $Id: UTCDateTime.cc 19473 2014-04-12 03:06:50Z sybenzvi $
 */

#include <data-structures/time/UTCDateTime.h>
#include <data-structures/time/LeapSeconds.h>

#include <hawcnest/Logging.h>

#include <ctime>
#include <cstdio>
#include <iomanip>

using namespace std;

UTCDateTime::UTCDateTime(const TimeStamp& ts)
{
  time_t unixSecond;
  const bool leap =
    LeapSeconds::GetInstance().ConvertGPSToUnix(ts.GetGPSSecond(), unixSecond);
  if (leap)
    --unixSecond;

  const tm* const gm = gmtime(&unixSecond);
  UTCDate::SetDate(gm->tm_year + 1900, gm->tm_mon + 1, gm->tm_mday);
  hour_ = gm->tm_hour;
  minute_ = gm->tm_min;
  sec_ = (leap ? 60 : gm->tm_sec);
  ns_ = ts.GetGPSNanoSecond();
}

UTCDateTime::UTCDateTime(const time_t& t)
{
  struct tm* gmt = gmtime(&t);
  SetDateTime(gmt->tm_year + 1900, gmt->tm_mon + 1, gmt->tm_mday,
              gmt->tm_hour, gmt->tm_min, gmt->tm_sec);
}

UTCDateTime::UTCDateTime(const string& ts)
{
  int Y, M, D, h, m, s;
  if (sscanf(ts.c_str(), "%04d-%02d-%02dT%02d:%02d:%02dZ",
                         &Y, &M, &D, &h, &m, &s) != 6)
  {
    log_fatal("Input time " << ts << " is not of format YYYY-MM-DDThh:mm:ssZ");
  }
  SetDateTime(Y, M, D, h, m, s);
}

void
UTCDateTime::SetDateTime(const int yr, const int mn, const int dy,
                         const int hour, const int minute, const int second,
                         const int nanosecond)
{
  SetDate(yr, mn, dy);
  SetHMS(hour, minute, second, nanosecond);
}

void
UTCDateTime::SetHMS(const int hour, const int minute, const int second,
                    const int nanosecond)
{
  if (hour < 0 || hour > 23)
    log_fatal("Hour " << hour << " not in range 0 - 23");

  if (minute < 0 || minute > 59)
    log_fatal("Minute " << minute << " not in range 0 - 59");

  if (second < 0 || second > 60)
    log_fatal("Second " << second << " not in range 0 - 59(60)");

  if (nanosecond < 0 || nanosecond > 999999999)
    log_fatal("Nanosecond " << nanosecond << " not in range 0 - 999999999");

  hour_ = hour;
  minute_ = minute;
  sec_ = second;
  ns_ = nanosecond;

  if (second != 60)
    return;

  time_t unixSecond = GetUnixSecond();
  unsigned int gpsSecond;
  LeapSeconds::GetInstance().ConvertUnixToGPS(unixSecond, gpsSecond);
  --gpsSecond;
  const bool isLeap =
    LeapSeconds::GetInstance().ConvertGPSToUnix(gpsSecond, unixSecond);
  if (!isLeap)
    log_fatal("UTCDateTime unix=" << unixSecond << " gps=" << gpsSecond
              << ": second is 60 but time is not a leap second");
}

TimeStamp
UTCDateTime::GetTimeStamp()
  const
{
  const time_t unixSecond = GetUnixSecond();
  unsigned int gpsSecond;
  LeapSeconds::GetInstance().ConvertUnixToGPS(unixSecond, gpsSecond);
  if (sec_ == 60)
    --gpsSecond;
  return TimeStamp(gpsSecond, (unsigned int)(ns_));
}

ostream&
operator<<(ostream& os, const UTCDateTime& dt)
{
  os << dt.year_ << '-' << setfill('0')
     << setw(2) << dt.month_ << '-'
     << setw(2) << dt.day_ << ' '
     << setw(2) << dt.hour_ << ':'
     << setw(2) << dt.minute_ << ':'
     << setw(2) << dt.sec_;
  return os;
}

// Get current date and time 
UTCDateTime
GetCurrentTime()
{
  return UTCDateTime(time(0));
}

