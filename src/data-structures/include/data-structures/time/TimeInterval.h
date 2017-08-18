/*!
 * @file TimeInterval.h
 * @brief Storage class for elapsed time.
 * @author Tom Paul
 * @author Darko Veberic
 * @version $Id: TimeInterval.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_TIME_TIMEINTERVAL_H_INCLUDED
#define DATACLASSES_TIME_TIMEINTERVAL_H_INCLUDED

#include <hawcnest/processing/Bag.h>

/*!
 * @class TimeInterval
 * @brief Represent the time elapsed between two events.
 * @author Tom Paul
 * @author Darko Veberic
 * @date 19 Feb 2003
 * @ingroup time
 *
 * From a TimeInterval, one can extract the lower bound on the number of
 * seconds in the interval (the seconds floor) as well as the number of
 * nanoseconds past the seconds floor.  When subtracting TimeIntervals
 * or TimeStamps to yield a TimeInterval, there are two cases to consider:
 * <ul>
 * <li>The differences is positive; second and nanosec are positive</li>
 * <li>The differences is negative; second is negative, nanosec is positive</li>
 * </ul>
 *
 * Note that this class uses a double-precision float to represent the elapsed
 * time between two events.  Since 52 bit are allocated for the mantissa of
 * a double-precision number, this interval is precise to about 1 part in
 * 5.4e15.  If, for example, 1 ns precision were required, one could use this
 * class to describe intervals of up to 62 days.
 */
class TimeInterval : public Baggable {

  public:

    TimeInterval(const double interval = 0.)
      : interval_(interval) { }

    TimeInterval(const TimeInterval& ti)
      : interval_(ti.interval_) { }

    TimeInterval& operator=(const TimeInterval& ti) 
    { interval_ = ti.interval_; return *this; }

    TimeInterval& operator=(const double& d)
    { interval_ = d; return *this; }

    /// Get the seconds floor for the interval (< 0 if interval is negative).
    int GetSecond() const;

    /// Get integer number of nanoseconds past the boundary (always > 0).
    unsigned int GetNanoSecond() const;

    /// Get the time interval as a double (in base time units).
    double GetInterval() const { return interval_; }

    TimeInterval operator+(const TimeInterval& ti) const
    { return TimeInterval(interval_ + ti.interval_); }

    TimeInterval operator-(const TimeInterval& ti) const
    { return TimeInterval(interval_ - ti.interval_); }

    /// Unary minus
    TimeInterval operator-() const
    { return TimeInterval(-interval_); }

    TimeInterval operator*(const double& d) const
    { return TimeInterval(d * interval_); }

    friend TimeInterval operator*(const double& d, const TimeInterval& ti)
    { return TimeInterval(d * ti.interval_); }

    TimeInterval operator/(const TimeInterval& ti) const
    { return TimeInterval(interval_ / ti.interval_); }

    TimeInterval operator/(const double& d) const
    { return TimeInterval(interval_ / d); }

    TimeInterval& operator+=(const TimeInterval& ti)
    { interval_ += ti.interval_; return *this; }

    TimeInterval& operator-=(const TimeInterval& ti)
    { interval_ -= ti.interval_; return *this; }

    TimeInterval& operator*=(const double& d)
    { interval_ *= d; return *this; }

    TimeInterval& operator/=(const double& d)
    { interval_ /= d; return *this; }

    bool operator>(const TimeInterval& ti) const
    { return interval_ > ti.interval_; }

    bool operator>=(const TimeInterval& ti) const
    { return interval_ >= ti.interval_; }

    bool operator<(const TimeInterval& ti) const
    { return interval_ < ti.interval_; }

    bool operator<=(const TimeInterval& ti) const
    { return interval_ <= ti.interval_; }

    bool operator==(const TimeInterval& ti) const
    { return interval_ == ti.interval_; }

    bool operator!=(const TimeInterval& ti) const
    { return interval_ != ti.interval_; }

    static TimeInterval Min() { return -4.294967296e9; }

    static TimeInterval Max() { return +4.294967296e9; }

  private:

    double interval_;

};

SHARED_POINTER_TYPEDEFS(TimeInterval);

#endif // DATACLASSES_TIME_TIMEINTERVAL_H_INCLUDED

