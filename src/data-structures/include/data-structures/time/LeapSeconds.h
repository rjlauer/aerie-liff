/*!
 * @file LeapSeconds.h
 * @brief Calculation of leap seconds.
 * @author Darko Veberic
 * @version $Id: LeapSeconds.h 16447 2013-07-30 15:11:33Z tweisgarber $
 */

#ifndef DATACLASSES_TIME_LEAPSECONDS_H_INCLUDED
#define DATACLASSES_TIME_LEAPSECONDS_H_INCLUDED

#include <ctime>
#include <map>
#include <string>

typedef boost::shared_ptr<std::string> TimeZonePtr;
typedef boost::shared_ptr<std::string> TimeZoneConstPtr;

/*!
 * @class LeapSeconds
 * @author Darko Veberic
 * @date 22 Jul 2009
 * @ingroup time
 * @brief Calculation of leap seconds given a system time or GPS time.
 *
 * Leap seconds are 1-second increments (or decrements) to the UTC time scale
 * that are used to account for changes in mean solar time.  In other words,
 * they are used to account for the changing rotational speed of the Earth.
 *
 * Since the start of the GPS epoch in 1980, there has been a leap second
 * update roughly every two years.  This class contains a table of leap seconds
 * that must be manually updated when a new leap second is announced.
 */
class LeapSeconds {

  private:

    typedef std::map<unsigned int, std::pair<int, int> > GPSToUnixMap;
    typedef GPSToUnixMap::const_iterator GPSToUnixIterator;

    typedef std::map<std::time_t, int> UnixToGPSMap;
    typedef UnixToGPSMap::const_iterator UnixToGPSIterator;

  public:

    static LeapSeconds& GetInstance() {
      static LeapSeconds instance;
      return instance;
    }

    /// Get the number of leap seconds given a system time
    int GetLeapSeconds(const std::time_t unixSec) const;

    /// Convert GPS seconds to a system time, accounting for leap seconds
    bool ConvertGPSToUnix(const unsigned int gpsSec, std::time_t& unixSec) const;

    /// Convert a system time to GPS seconds, accounting for leap seconds
    void ConvertUnixToGPS(const std::time_t unixSec, unsigned int& gpsSec) const;

  private:

    LeapSeconds();

    LeapSeconds(const LeapSeconds& ls);

   ~LeapSeconds() { UnsetUTC(); }

    void Init();

    void SetUTC();

    void UnsetUTC();

    const std::time_t gpsEpochInUnixSeconds_;

    GPSToUnixMap gpsToUnixLeaps_;
    UnixToGPSMap unixToGPSLeaps_;
    TimeZonePtr tz_;

};

#endif // DATACLASSES_TIME_LEAPSECONDS_H_INCLUDED

