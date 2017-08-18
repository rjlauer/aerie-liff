/*!
 * @file sourcesight.cc
 * @brief Give zenith/azimuth of given source position for given time
 * @author Dan Fiorino
 * @author Segev BenZvi
 * @date 20 Nov 2015
 * @ingroup utility_apps
 *
 * This program will take a local position on Earth's surface (either UTM or
 * latitute/longitude) and a time and calculate the topocentric position of
 * any object.  Note that there is no correction for atmospheric scattering of
 * light.
 *
 * If no arguments are given, the current position of the Crab at the location
 * of HAWC is written to the console.
 */

#include <data-structures/time/TimeStamp.h>
#include <data-structures/time/UTCDateTime.h>
#include <data-structures/time/ModifiedJulianDate.h>

#include <data-structures/astronomy/EquPoint.h>
#include <data-structures/astronomy/AstroCoords.h>

#include <data-structures/geometry/LatLonAlt.h>
#include <data-structures/geometry/UTMPoint.h>
#include <data-structures/geometry/Vector.h>

#include <astro-service/StdAstroService.h>

#include <hawcnest/CommandLineConfigurator.h>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include <iostream>
#include <string>

using namespace std;
using namespace HAWCUnits;

ModifiedJulianDate mjd_from_string(const std::string& ts, int zone=0);
ModifiedJulianDate mjd_from_current_time(int zone=0);
LatLonAlt locale_from_string(const std::string& loc, const double altitude=0.);

int main(int argc, char* argv[])
{
  CommandLineConfigurator cl(
    "Take a local position on Earth's surface (UTM or latitude/longitude)\n"
    "and a time and calculate the topocentric position of an object with\n"
    "right ascension RA and declination Dec (J2000).");

  cl.AddOption<string>("time,t",
                       "Time in GPS seconds: e.g., 1009411215, or\n"
                       "Date and time [YYYY-MM-DDThh:mm:ss]\n"
                       "(current time is used by default)\n");
  cl.AddOption<int>("zone,z",
                       -6,
                       "Time zone: UTC +/- hrs [-6 = Puebla, MX]\n");
  cl.AddOption<string>("loc,l",
                       "Site latitude and longitude: l,b [deg]\n"
                       "    E.g., 18.99,-97.39,\n"
                       "or UTM easting, northing, zone, band [meter]\n"
                       "    E.g., 678142,2101110,14,Q\n");
  cl.AddOption<double>("altitude,a",
                       4096.,
                       "Site altitude [meter]");
  cl.AddOption<double>("ra,r",
                       83.63,
                       "Right Ascension [degree]");
  cl.AddOption<double>("dec,d",
                       22.01,
                       "Declination [degree]");

  if (!cl.ParseCommandLine(argc, argv))
    return 1;

  // Configure astronomical transformation service
  try {
    HAWCNest nest;
    nest.Service("StdAstroService", "astroX");
    nest.Configure();

    const AstroService& astroX = GetService<AstroService>("astroX");

    // Get time (either specified by user, or current time)
    ModifiedJulianDate mjd(mjd_from_current_time(cl.GetArgument<int>("zone")));

    if (cl.HasFlag("time")) {
      mjd = mjd_from_string(cl.GetArgument<string>("time"),
                            cl.GetArgument<int>("zone"));
    }

    // Location (either specified by user, or HAWC position by default)
    const double altitude = cl.GetArgument<double>("altitude") * meter;
    double latitude  =  DegMinSec(18*degree, 59*arcminute, 41.63*arcsecond);
    double longitude = -DegMinSec(97*degree, 18*arcminute, 27.39*arcsecond);
    LatLonAlt locale(latitude, longitude, altitude);

    if (cl.HasFlag("loc"))
      locale = locale_from_string(cl.GetArgument<string>("loc"), altitude);

    // Convert source position to local coordinates; make sure to convert from
    // J2000 to get the true local position.
    EquPoint eT(cl.GetArgument<double>("ra") * degree,
                cl.GetArgument<double>("dec") * degree);
    Vector axis;

    astroX.Equ2Loc(mjd, locale, eT, axis, AstroService::SIDEREAL, true);
    double gmst = astroX.GetGMST(mjd);

    // Rising or Setting?
    double lmst = gmst + locale.GetLongitude();
    if (lmst >= twopi)
      lmst -= twopi;
    if (lmst < 0.)
      lmst += twopi;
    double lha  = lmst - eT.GetRA();
    if ( lha < -pi )
      lha += twopi;
    if ( lha > pi )
      lha -= twopi;
    string status;
    if ( lha < 0. )
      status = "rising";
    else
      status = "setting";

    log_info("\n==============================================\n"
         << "  Source at Zenith of " << axis.GetTheta() / degree << "°   and "
         << status 
         << "\n==============================================\n\n"
         << "Local coords ...... " << locale << '\n'
         << "Mod. Julian Day ... " << mjd << '\n'
         << "UTC Date Time   ... " << mjd.GetUTCDateTime() << '\n'
         << "GM Sidereal Time .. " << gmst/degree << "°, " << gmst/degree/15.<< " hrs" << "\n"
         << "Loc Sidereal Time . " << lmst/degree << "°, " << lmst/degree/15.<< " hrs" << "\n"
         << "\nSource position:\n"   
         << " - RA, Dec ........ " << eT << '\n'
         << " - RA, Dec ........ " << "(" << eT.GetRA()/degree << "°, " << eT.GetDec()/degree << "°)" << '\n'
         << " - Local HA ....... " << lha/degree << "°" << '\n'
         << " - Local Zenith ... " << axis.GetTheta() / degree << "°" << '\n'
         << " - Local Azimuth .. " << axis.GetPhi() / degree << "°");
  }
  catch (...) {
  }

  return 0;
}

// Convert a GPS/UTC time string to Modified Julian Day
ModifiedJulianDate
mjd_from_string(const std::string& ts, int zone)
{
  boost::regex eG("\\d{1,10}");
  boost::regex eU("\\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}");
  boost::regex eM("\\d+\\.?\\d*");
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
  // Time in MJD
  else if (boost::regex_match(ts,eM)) {
    // Parse mjd 
    double mjd = boost::lexical_cast<double>(ts);
    return ModifiedJulianDate(mjd * day);
  }
  // Anything else:
  else {
    log_fatal("Unrecognized time format: '" << ts << "'");
  }

  return ModifiedJulianDate(t);
}

// Convert current system time into MJD
ModifiedJulianDate
mjd_from_current_time(int zone)
{
  time_t curTime = time(0);
  struct tm* gmt = gmtime(&curTime);
  UTCDateTime utc(*gmt);

  return ModifiedJulianDate(utc);
}

// Convert a latitude/longitude or UTM string into a LatLonAlt position type
LatLonAlt
locale_from_string(const std::string& loc, const double altitude)
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
