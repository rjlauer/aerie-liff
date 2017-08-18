/*!
 * @file TimeRange.h
 * @brief Storage class for elapsed time between two TimeStamp objects.
 * @author Darko Veberic
 * @version $Id: TimeRange.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_TIME_TIMERANGE_H_INCLUDED
#define DATACLASSES_TIME_TIMERANGE_H_INCLUDED

#include <data-structures/time/TimeStamp.h>
#include <data-structures/time/TimeInterval.h>

#include <hawcnest/processing/Bag.h>

/*!
 * @class TimeRange
 * @brief A TimeRange defines a time interval between two TimeStamp values
 * @author Darko Veberic
 * @date 13 Feb 2007
 * @ingroup time
 *
 * Define an interval using a start and a stop time (inclusive).
 */
class TimeRange : public Baggable {

  public:

    TimeRange() { }

    /// Set a range using a start and a stop time
    TimeRange(const TimeStamp& t0, const TimeStamp& t1)
    { SetSorted(t0, t1); }

    /// Set a range using a start time and a time interval
    TimeRange(const TimeStamp& t0, const TimeInterval& duration)
    { SetSorted(t0, t0 + duration); }

    /// Get the total duration of the range
    TimeInterval GetDuration() const { return t1_ - t0_; }

    const TimeStamp& GetStartTime() const { return t0_; }
    const TimeStamp& GetStopTime() const { return t1_; }

    /// Set a range using a start and a stop time
    void SetTimeRange(const TimeStamp& t0, const TimeStamp& t1)
    { SetSorted(t0, t1); }

    /// Set a range using a start time and a time interval
    void SetTimeRange(const TimeStamp& t0, const TimeInterval& duration)
    { SetSorted(t0, t0 + duration); }

    bool operator==(const TimeRange& tr) const
    { return t0_ == tr.t0_ && t1_ == tr.t1_; }

    bool operator!=(const TimeRange& tr) const
    { return !(*this == tr); }

    /// Less-than comparison using start times
    bool operator<(const TimeRange& tr) const
    { return t0_ < tr.t0_; }

    /// Greater-than comparison using stop times
    bool operator>(const TimeRange& tr) const
    { return t1_ > tr.t1_; }

    /// Is TimeStamp in the time range [t0, t1)?
    bool IsInRange(const TimeStamp& ts) const
    { return t0_ <= ts && ts < t1_; }

    /// Is TimeStamp in the time range [t0, t1]?
    bool IsInRangeInclusive(const TimeStamp& ts) const
    { return t0_ <= ts && ts <= t1_; }

    /// Is TimeStamp in the time range [t0, t1)?
    bool operator==(const TimeStamp& ts) const
    { return IsInRange(ts); }

    /// Is TimeStamp not in the time range [t0, t1)?
    bool operator!=(const TimeStamp& ts) const
    { return !(*this == ts); }

    /// Is stopping time less than the TimeStamp?
    bool operator<(const TimeStamp& ts) const
    { return t1_ < ts; }

    /// Is starting time greater than the TimeStamp?
    bool operator>(const TimeStamp& ts) const
    { return t0_ > ts; }

    /// Does this time range overlap with a second one?
    bool HasCommonTime(const TimeRange& tr) const
    { return tr.t1_ >= t0_ && tr.t0_ <= t1_; }

  private:

    TimeStamp t0_;  ///< Beginning of the range
    TimeStamp t1_;  ///< End of the range

    /// Initialize range such that t0 < t1
    void SetSorted(const TimeStamp& t0, const TimeStamp& t1);

};

SHARED_POINTER_TYPEDEFS(TimeRange);

inline bool operator==(const TimeStamp& ts, const TimeRange& tr)
{ return tr == ts; }

inline bool operator!=(const TimeStamp& ts, const TimeRange& tr)
{ return tr != ts; }

inline bool operator<(const TimeStamp& ts, const TimeRange& tr)
{ return tr < ts; }

inline bool operator>(const TimeStamp& ts, const TimeRange& tr)
{ return tr > ts; }

#endif // DATACLASSES_TIME_TIMERANGE_H_INCLUDED

