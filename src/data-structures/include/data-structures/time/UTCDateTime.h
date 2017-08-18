/*!
 * @file UTCDateTime.h
 * @brief Representation of a calendar date and time.
 * @author Darko Veberic
 * @version $Id: UTCDateTime.h 19473 2014-04-12 03:06:50Z sybenzvi $
 */

#ifndef DATACLASSES_TIME_UTCDATETIME_H_INCLUDED
#define DATACLASSES_TIME_UTCDATETIME_H_INCLUDED

#include <data-structures/time/TimeStamp.h>
#include <data-structures/time/UTCDate.h>

/*!
 * @class UTCDateTime
 * @author Darko Veberic
 * @date 22 Jul 2009
 * @ingroup time
 * @brief Storage of a date and time in UTC, initialized by year, month, day,
 *        hour, minute, and second.
 */
class UTCDateTime : public UTCDate {

  public:

    UTCDateTime() : UTCDate(), hour_(0), minute_(0), sec_(0), ns_(0) { }

    /// Construction from a date object and time values
    UTCDateTime(const UTCDate& date,
                const int hour = 0, const int minute = 0, const int second = 0,
                const int nanosecond = 0)
      : UTCDate(date)
    { SetHMS(hour, minute, second, nanosecond); }


    /// Construction from date values and time values 
    UTCDateTime(const int year, const int month, const int day,
                const int hour = 0, const int minute = 0, const int second = 0,
                const int nanosecond = 0)
      : UTCDate(year, month, day)
    { SetHMS(hour, minute, second, nanosecond); }

    /// Construction from a GPS timestamp
    UTCDateTime(const TimeStamp& time);

    /// Construction from UNIX time using a time_t type
    UTCDateTime(const time_t& time);

    /// Construction from C time struct; useful for date-time XML extraction
    UTCDateTime(const tm& time)
      : UTCDate(time.tm_year+1900, time.tm_mon+1, time.tm_mday)
    { SetHMS(time.tm_hour, time.tm_min, time.tm_sec, 0); }

    /// Construction from a string formatted as YYYY-MM-DDThh:mm:ssZ
    UTCDateTime(const std::string& ts);

    void SetDateTime(const int year, const int month, const int day,
                     const int hour, const int minute, const int second,
                     const int nanosecond = 0);

    TimeStamp GetTimeStamp() const;

    int GetHour() const { return hour_; }

    int GetMinute() const { return minute_; }

    int GetSecond() const { return sec_; }

    int GetNanoSecond() const { return ns_; }

    std::time_t GetUnixSecond() const
    { return UTCDate::GetUnixSecond(year_, month_, day_, hour_, minute_, sec_); }

  private:

    void SetHMS(const int hour, const int minute, const int sec, const int ns = 0);

    int hour_;
    int minute_;
    int sec_;
    int ns_;

  friend std::ostream& operator<<(std::ostream& os, const UTCDateTime& dt);

};

/*!
 * @fn GetCurrentUTCDateTime
 * @brief Return the current date and time in a UTCDateTime object.
 */
UTCDateTime GetCurrentTime();

// Convenient public definitions:
// J2000.0 = Julian Date 2451545.0 TT
//         = 1 Jan. 2000 12:00:00.000 TT
//         = 1 Jan. 2000 11:59:27.816 TAI
//         = 1 Jan. 2000 11:58:55.816 UTC
const UTCDateTime J2000_UTC(2000, 1, 1, 11, 58, 55, 816000000);

SHARED_POINTER_TYPEDEFS(UTCDateTime);

#endif // DATACLASSES_TIME_UTCDATETIME_H_INCLUDED

