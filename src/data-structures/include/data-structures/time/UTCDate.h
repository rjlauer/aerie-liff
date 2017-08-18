/*!
 * @file UTCDate.h
 * @brief Representation of a calendar date.
 * @author Darko Veberic
 * @version $Id: UTCDate.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_TIME_UTCDATE_H_INCLUDED
#define DATACLASSES_TIME_UTCDATE_H_INCLUDED

#include <hawcnest/processing/Bag.h>
#include <hawcnest/PointerTypedefs.h>

#include <ctime>

/*!
 * @class UTCDate
 * @author Darko Veberic
 * @date 22 Jul 2009
 * @ingroup time
 * @brief Storage of a date, initialized by calendar year, month, and day.
 */
class UTCDate : virtual public Baggable {

  public:

    enum Month {
      JAN = 1, FEB, MAR, APR, MAY, JUN,
      JUL, AUG, SEP, OCT, NOV, DEC
    };

    UTCDate() : year_(0), month_(0), day_(0) { }

    /// For month, January = 1, December = 12
    UTCDate(const int year, const int month, const int day)
    { SetDate(year, month, day); }

    void SetDate(const int year, const int month, const int day);

    int GetYear() const { return year_; }

    int GetMonth() const { return month_; }

    int GetDay() const { return day_; }

    bool operator==(const UTCDate& ts) const
    { return year_ == ts.year_ && month_ == ts.month_ && day_ == ts.day_; }

    bool operator!=(const UTCDate& ts) const
    { return !(*this == ts); }

    static UTCDate GetGPSEpoch() { return UTCDate(1980, JAN, 6); }

    static UTCDate GetUnixEpoch() { return UTCDate(1970, JAN, 1); }

    std::time_t GetUnixSecond() const
    { return GetUnixSecond(year_, month_, day_, 0, 0, 0); }

  protected:
    
    static std::time_t GetUnixSecond(const int yr, const int mon, const int day,
                                     const int hr, const int min, const int sec);

  private:

    static bool IsLeapYear(const int year) 
    { return (!(year % 4) && (year % 100)) || !(year % 400); }

    static int DaysPerMonth(const int year, const int month) {
      switch (month) {
        case JAN:
        case MAR:
        case MAY:
        case JUL:
        case AUG:
        case OCT:
        case DEC:
          return 31;
        case FEB:
          return IsLeapYear(year) ? 29 : 28;
        case APR:
        case JUN:
        case SEP:
        case NOV:
          return 30;
        default:
          return 0;
      }
    }

  protected:

    int year_;
    int month_;
    int day_;

};

SHARED_POINTER_TYPEDEFS(UTCDate);

#endif // DATACLASSES_TIME_UTCDATE_H_INCLUDED

