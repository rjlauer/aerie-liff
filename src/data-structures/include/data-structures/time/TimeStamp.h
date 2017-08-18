/*!
 * @file TimeStamp.h
 * @brief Representation of a GPS time stamp.
 * @author Tom Paul
 * @author Darko Veberic
 * @version $Id: TimeStamp.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_TIME_TIMESTAMP_H_INCLUDED
#define DATACLASSES_TIME_TIMESTAMP_H_INCLUDED

#include <hawcnest/processing/Bag.h>
#include <hawcnest/HAWCUnits.h>

#include <iostream>

class TimeInterval;

/*!
 * @class TimeStamp
 * @brief A TimeStamp holds GPS second and nanosecond
 * @author Tom Paul
 * @author Darko Veberic
 * @date 19 Feb 2003
 * @ingroup time
 *
 * This class holds a GPS second and nanosecond and can be converted to UTC
 * or modified Julian date using helper functions in the data classes.  For a
 * discussion of various time systems and their connection to this class,
 * see http://tycho.usno.navy.mil/gpstt.html.
 */
class TimeStamp : public Baggable {

  public:

    TimeStamp() : sGPS_(0), nsGPS_(0) { }

    /// Constructor from a GPS second and nanosecond
    TimeStamp(const unsigned int sec, const unsigned int nsec = 0)
      : sGPS_(sec), nsGPS_(nsec) { }

    TimeStamp(const int sec, const int nsec = 0)
    { SetNormalized(sec, nsec); }

    unsigned int GetGPSSecond() const { return sGPS_; }

    unsigned int GetGPSNanoSecond() const { return nsGPS_; }

    void SetGPSTime(const unsigned int sec, const unsigned int nsec = 0)
    { sGPS_ = sec; nsGPS_ = nsec; }

    /// Set GPS second, ensuring that it is in the valid GPS epoch
    void SetNormalized(int sec, int nsec);

    /// TimeStamp + TimeInterval = TimeStamp
    TimeStamp operator+(const TimeInterval& ti) const;

    /// TimeStamp += TimeInterval = TimeStamp
    TimeStamp& operator+=(const TimeInterval& ti);

    /// TimeStamp - TimeStamp = TimeInterval
    TimeInterval operator-(const TimeStamp& ts) const;

    /// TimeStamp - TimeInterval = TimeStamp
    TimeStamp operator-(const TimeInterval& ti) const;

    /// TimeStamp -= TimeInterval = TimeStamp
    TimeStamp& operator-=(const TimeInterval& ti);

    bool operator==(const TimeStamp& ts) const
    { return sGPS_ == ts.sGPS_ && nsGPS_ == ts.nsGPS_; }

    bool operator!=(const TimeStamp& ts) const
    { return !(*this == ts); }

    bool operator>(const TimeStamp& ts) const
    { return sGPS_ > ts.sGPS_ || (sGPS_ == ts.sGPS_ && nsGPS_ > ts.nsGPS_); }

    bool operator>=(const TimeStamp& ts) const
    { return !(*this < ts); }

    bool operator<(const TimeStamp& ts) const
    { return sGPS_ < ts.sGPS_ || (sGPS_ == ts.sGPS_ && nsGPS_ < ts.nsGPS_); }

    bool operator<=(const TimeStamp& ts) const
    { return !(*this > ts); }

    /// Valid up to 68 years from Jan 6, 1980
    static TimeStamp Max() { return TimeStamp(2147483647U, 999999999U); }

  private:

    unsigned int sGPS_;
    unsigned int nsGPS_;

  friend std::ostream& operator<<(std::ostream& os, const TimeStamp& ts);

};

SHARED_POINTER_TYPEDEFS(TimeStamp);

#include <data-structures/time/TimeInterval.h>

inline
TimeStamp
TimeStamp::operator+(const TimeInterval& ti)
  const
{
  return TimeStamp(int(sGPS_ + ti.GetSecond()), int(nsGPS_ + ti.GetNanoSecond()));
}

inline
TimeStamp&
TimeStamp::operator+=(const TimeInterval& ti)
{
  SetNormalized(sGPS_ + ti.GetSecond(), nsGPS_ + ti.GetNanoSecond());
  return *this;
}

inline
TimeStamp
TimeStamp::operator-(const TimeInterval& ti)
  const
{
  // Constructor will normalize this properly
  return TimeStamp(int(sGPS_) - int(ti.GetSecond()),
                   int(nsGPS_) - int(ti.GetNanoSecond()));
}

inline
TimeInterval
TimeStamp::operator-(const TimeStamp& ts)
  const
{
  return TimeInterval((int(sGPS_) - int(ts.sGPS_)) * HAWCUnits::second +
                      (int(nsGPS_) - int(ts.nsGPS_)) * HAWCUnits::nanosecond
  );
}

inline
TimeStamp&
TimeStamp::operator-=(const TimeInterval& ti)
{
  SetNormalized(int(sGPS_) - int(ti.GetSecond()),
                int(nsGPS_) - int(ti.GetNanoSecond()));
  return *this;
}

#endif // DATACLASSES_TIME_TIMESTAMP_H_INCLUDED

