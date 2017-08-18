/*!
 * @file ModifiedJulianDate.h
 * @brief Calculation of Modified Julian Date.
 * @author Segev BenZvi
 * @version $Id: ModifiedJulianDate.h 37304 2017-02-02 18:45:48Z imc $
 */

#ifndef DATACLASSES_TIME_MODIFIEDJULIANDATE_H_INCLUDED
#define DATACLASSES_TIME_MODIFIEDJULIANDATE_H_INCLUDED

#include <iostream>

#include <hawcnest/HAWCNest.h>
#include <hawcnest/HAWCUnits.h>

#include <data-structures/time/TimeStamp.h>
#include <data-structures/time/UTCDateTime.h>

class TimeInterval;

/*!
 * @enum TimeScale
 * @ingroup time
 * @brief Enums for some standard time scales (useful in astronomical
 *        conversions)
 */
enum TimeScale {
  UTC,              ///< Coordinated Universal Time (1972 epoch)
  TAI,              ///< International Atomic Time (1958 epoch)
  TT                ///< Terrestrial time (a.k.a. TDT; formerly ET)
};

/*!
 * @class ModifiedJulianDate
 * @author Segev BenZvi
 * @date 28 May 2010
 * @ingroup time
 * @brief Store a modified Julian date (MJD), defined as the number of days
 *        (plus fractional days) since midnight of 17 Nov 1858.
 *
 * MJD is a replacement for Julian Date (mean solar days since noon of 1 Jan
 * -4712).  It can be expressed with respect to various standard time scales,
 * such as:
 * <ol>
 *   <li>Universal Time (UT1)</li>
 *   <li>Coordinated Universal Time (UTC)</li>
 *   <li>International Atomic Time (TAI)</li>
 *   <li>Terrestrial Time (TT)</li>
 * </ol>
 * These time scales are related by several fixed and changing offsets,
 * as shown here (from http://stjarnhimlen.se/comp/time.html):
 *
 * <pre>
 *                                                  ET 1960-1983
 *                                                 TDT 1984-2000
 *  UTC 1972-  GPS 1980-    TAI 1958-               TT 2001-
 * ----+---------+-------------+-------------------------+-----
 *     |         |             |                         |
 *     |<------ TAI-UTC ------>|<-----   TT-TAI    ----->|
 *     |         |             |      32.184s fixed      |
 *     |<GPS-UTC>|<- TAI-GPS ->|                         |
 *     |         |  19s fixed  |                         |
 *     |                                                 |
 *     <> delta-UT = UT1-UTC                             |
 *      | (max 0.9 sec)                                  |
 * -----+------------------------------------------------+-----
 *      |<-------------- delta-T = TT-UT1 -------------->|
 *     UT1 (UT)                                       TT/TDT/ET
 * </pre>
 * 
 * This class has a function to provide conversions between UTC, TT, and TAI.
 * The conversion between UTC and UT1, which can differ by up to 0.9 seconds,
 * is not provided.  Many astronomical observations are reported in Barycentric
 * Dynamical Time (TDB), which differs from TT by <2 ms during the next
 * millenium (IAU 2006 Resolution B3).  If very high astronomical accuracy is
 * required, please note that conversion to TDB is not provided here.
 */
class ModifiedJulianDate : public Baggable {

  public:

    /// Default constructor.
    /// Inititialized to 0 GPS seconds (1980-01-06 00:00:00 UTC)
    ModifiedJulianDate() :
      mjd_(44244 * HAWCUnits::day),
      nLeaps_(0)
    {}
  
    /// Construct MJD from a date and time, given in UTC
    ModifiedJulianDate(const UTCDateTime& utcTime);

    /// Construct MJD from GPS seconds and nanoseconds (from a GPS receiver)
    ModifiedJulianDate(const TimeStamp& gpsTime);

    /// Construct from MJD (in base time units), with respect to UTC.  E.g.,
    /// 55368.1 * day, which is 21 Jun 2010 02:24:00 UT.
    ModifiedJulianDate(const double& mjd);

    /// Set time from a date and time, given in UTC
    void SetDate(const UTCDateTime& utcTime);

    /// Set time using GPS seconds and nanoseconds (from a GPS receiver)
    void SetDate(const TimeStamp& gpsTime);

    /// Set time from MJD (in base time units), with respect to UTC.  E.g.,
    /// 55368.1 * day, which is 21 Jun 2010 02:24:00 UT.
    void SetDate(const double& mjd);

    /// Get the date (in base units) with respect to a given time scale
    double GetDate(const TimeScale& ts = UTC) const;

    /// Return the current UTC date and time for this MJD
    UTCDateTime GetUTCDateTime() const;

    /// Return the current GPS TimeStamp for this MJD
    TimeStamp GetTimeStamp() const;

    /// ModifiedJulianDate + TimeInterval = ModifiedJulianDate 
    ModifiedJulianDate operator+(const TimeInterval& ti) const;

    /// ModifiedJulianDate += TimeInterval = ModifiedJulianDate
    ModifiedJulianDate& operator+=(const TimeInterval& ti);

    /// ModifiedJulianDate - ModifiedJulianDate = TimeInterval
    TimeInterval operator-(const ModifiedJulianDate& ts) const;

    /// ModifiedJulianDate - TimeInterval = ModifiedJulianDate
    ModifiedJulianDate operator-(const TimeInterval& ti) const;

    /// ModifiedJulianDate -= TimeInterval = ModifiedJulianDate
    ModifiedJulianDate& operator-=(const TimeInterval& mjd);

  private:

    double mjd_;        ///< MJD, expressed with respect to UTC
    int nLeaps_;        ///< Leap seconds in GPS epoch (for UTC-TT conversions)

  friend std::ostream& operator<<(std::ostream&, const ModifiedJulianDate&);

};

// Convenient public definitions
const ModifiedJulianDate J2000_MJD(J2000_UTC);

SHARED_POINTER_TYPEDEFS(ModifiedJulianDate);

//Inline members

#include <data-structures/time/TimeInterval.h>

inline
ModifiedJulianDate
ModifiedJulianDate::operator+(const TimeInterval& ti)
  const
{
  return ModifiedJulianDate(this->GetTimeStamp() + ti);
}

inline
ModifiedJulianDate&
ModifiedJulianDate::operator+=(const TimeInterval& ti)
{
  SetDate(this->GetTimeStamp() + ti);
  return *this;
}

inline
ModifiedJulianDate
ModifiedJulianDate::operator-(const TimeInterval& ti)
  const
{
  return ModifiedJulianDate(this->GetTimeStamp() - ti);
}

inline
ModifiedJulianDate&
ModifiedJulianDate::operator-=(const TimeInterval& ti)
{
  SetDate(this->GetTimeStamp() - ti);
  return *this;
}

inline
TimeInterval
ModifiedJulianDate::operator-(const ModifiedJulianDate& mjd)
    const
{
  return this->GetTimeStamp() - mjd.GetTimeStamp();
}

#endif // DATACLASSES_TIME_MODIFIEDJULIANDATE_H_INCLUDED

