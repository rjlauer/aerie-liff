/*!
 * @file Util.cc 
 * @brief miscellaneous routines used by apps using astro service  
 * @author Israel Martinez
 * @date 02-Feb-2017
 * @version $Id$
 */

#include <data-structures/time/TimeStamp.h>
#include <data-structures/time/UTCDateTime.h>

#include <data-structures/geometry/UTMPoint.h>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include <data-structures/astronomy/AstroCoords.h>
#include <astro-service/Util.h>

using namespace std;
using namespace HAWCUnits;

ModifiedJulianDate ParseTime(const std::string& ts, int zone)
{
  boost::regex eM("\\d{1,5}\\.?\\d{0,15}");
  boost::regex eG("\\d{6,10}");
  boost::regex eU("\\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}");
  TimeStamp t;

  // Time in GPS seconds:
  if (boost::regex_match(ts, eG)) {
    int gpsSec = boost::lexical_cast<int>(ts);
    t.SetGPSTime(gpsSec);
  }
  // Time in UTC:
  else if (boost::regex_match(ts, eU)) {
    // Parse datetime string
    int yr  = boost::lexical_cast<int>(ts.substr(0, 4));
    int mon = boost::lexical_cast<int>(ts.substr(5, 2));
    int day = boost::lexical_cast<int>(ts.substr(8, 2));
    int hr  = boost::lexical_cast<int>(ts.substr(11, 2));
    int min = boost::lexical_cast<int>(ts.substr(14, 2));
    int sec = boost::lexical_cast<int>(ts.substr(17, 2));

    // Convert to UT and add in the time zone
    UTCDateTime utc(yr, mon, day, hr, min, sec);
    t = TimeStamp(utc.GetTimeStamp());
    t -= TimeInterval(zone * HAWCUnits::hour);
  }
  // Time in MJD:
  else if (boost::regex_match(ts, eM)) {

    if(boost::regex_match(ts, boost::regex("\\d{1,5}")))
       log_warn("Assuming time is in MJD, not GPS seconds");
    
    return ModifiedJulianDate(boost::lexical_cast<double>(ts) * day);
    
  }
  // Anything else:
  else {
    log_fatal("Unrecognized time format: '" << ts << "'");
  }
  
  return ModifiedJulianDate(t);
}

UTCDateTime CurrentUTCDateTime() {
  
  time_t curTime = time(0);
  struct tm* gmt = gmtime(&curTime);
  return UTCDateTime(*gmt);

}

ModifiedJulianDate CurrentMJD() {
  return ModifiedJulianDate(CurrentUTCDateTime());
}

LatLonAlt ParseLatLonAlt(const std::string& loc, const double altitude)
{
  const boost::regex eL("[+-]?\\d+\\.?\\d*,[+-]?\\d+\\.?\\d*");
  const boost::regex eU("\\d+\\.?\\d*,\\d+\\.?\\d*,\\d{1,2},[A-Z]");

  LatLonAlt locale;

  // Argument is a latitude/longitude
  if (boost::regex_match(loc, eL)) {
    size_t pos = loc.find(",");
    double lat = boost::lexical_cast<double>(loc.substr(0, pos)) * degree;
    double lon = boost::lexical_cast<double>(loc.substr(pos + 1)) * degree;
    locale = LatLonAlt(lat, lon, altitude);
  }
  // Argument is a UTM coordinate
  else if (boost::regex_match(loc, eU)) {
    size_t pos;
    string u(loc);

    pos = u.find(",");
    double Eing = boost::lexical_cast<double>(u.substr(0, pos)) * meter;
    u = u.substr(pos + 1);

    pos = u.find(",");
    double Ning = boost::lexical_cast<double>(u.substr(0, pos)) * meter;
    u = u.substr(pos + 1);

    pos = u.find(",");
    int zone = boost::lexical_cast<int>(u.substr(0, pos));
    u = u.substr(pos + 1);

    char band = boost::lexical_cast<char>(u);
    locale = LatLonAlt(UTMPoint(Eing, Ning, altitude, zone, band));
  }
  // No other positions are valid
  else
    log_fatal("Unrecognized location format '" << loc << "'");

  return locale;
}

double ParseAngle(const std::string& ang)
{
  const boost::regex eH("\\d+h\\d+m\\d+\\.?\\d*s");
  const boost::regex eD("[+-]?\\d+d\\d+m\\d+\\.?\\d*s");
//  const boost::regex eR("[+-]?\\d+\\.?\\d*");
//  const boost::regex eR("[+-]?\\d+\\.?\\d*\\(e[+-]?[0-9]{1,3}\\)?");
//  const boost::regex eR("[+-]?\\d+\\.?\\d*e?[+-]?\\d{0,3}");
  const boost::regex eR("[+-]?\\d+\\.?\\d*(e[+-]?\\d{1,3})?");

  double angle = 0.;

  // Angle is in the format XXhYYmZZ.ZZs
  if (boost::regex_match(ang, eH)) {
    string a(ang);

    size_t pos = a.find("h");
    double hr = boost::lexical_cast<double>(a.substr(0, pos));
    a = a.substr(pos + 1);

    pos = a.find("m");
    double mn = boost::lexical_cast<double>(a.substr(0, pos));
    a = a.substr(pos + 1);

    pos = a.find("s");
    double sc = boost::lexical_cast<double>(a.substr(0, pos));

    angle = copysign(1., hr) * HrMinSec(abs(hr)*hour, mn*minute, sc*second);
  }
  // Angle is in the format +-XXdYYmZZ.ZZs
  else if (boost::regex_match(ang, eD)) {
    string a(ang);

    size_t pos = a.find("d");
    double dg = boost::lexical_cast<double>(a.substr(0, pos));
    a = a.substr(pos + 1);

    pos = a.find("m");
    double mn = boost::lexical_cast<double>(a.substr(0, pos));
    a = a.substr(pos + 1);

    pos = a.find("s");
    double sc = boost::lexical_cast<double>(a.substr(0, pos));

    angle = copysign(1., dg) * DegMinSec(abs(dg)*degree, mn*arcminute, sc*arcsecond);
  }
  // Angle is in the format +-XX.XYZ
  else if (boost::regex_match(ang, eR)) {
    angle = boost::lexical_cast<double>(ang) * degree;
  }
  else {
    log_fatal("Unrecognized angle format '" << ang << "'");
  }

  return angle;
}

