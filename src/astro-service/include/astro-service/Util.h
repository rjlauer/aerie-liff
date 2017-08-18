/*!
 * @file Util.h 
 * @brief miscellaneous routines used by apps using astro service  
 * @author Israel Martinez
 * @date 02-Feb-2017
 * @version $Id$
 */

#include <data-structures/time/ModifiedJulianDate.h>

#include <data-structures/geometry/LatLonAlt.h>

#include <string>

#ifndef ASTRO_SERVICE_UTIL_H_INCLUDED
#define ASTRO_SERVICE_UTIL_H_INCLUDED

/*Get a MJD from a string.
  Formats:
  - GPS seconds: eg. 1009414815
  - Date and time: YYYY-MM-DDThh:mm:ss"
  - MJD: e.g. 55555.55 
  Note that it distinguish between an integer MJD and GPS seconds by the number 
  of digits, so the first 10000 seconds since 1980-01-06 00:00:00 UTC are not 
  accesiblle using GPS seconds*/
ModifiedJulianDate ParseTime(const std::string& ts, int zone=0);

/*Get current time. It uses POSIX time() */
UTCDateTime CurrentUTCDateTime();
ModifiedJulianDate CurrentMJD();

/*Convert a latitude/longitude or UTM string into a LatLonAlt position type.
  Altitude is in standard units.
  Formats for loc:
  - Latitude,Longitude [degree]: e.g. 18.99,-97.39
  - UTM easting, northing, zone, band [meter]: e.g. 678142,2101110,14,Q */
LatLonAlt ParseLatLonAlt(const std::string& loc, const double altitude=0.);

/*Parses to standard units an angle in the following formats:
  - Degrees: e.g. 18.99"
  - [+-]HHhMMmSS.SSs (sign is optional): e.g  +18h59m41.6s"
  - [+-]DDdMMmSS.SSd (sign is optional): e.g -17d34m12.5s */
double ParseAngle(const std::string& ang);

#endif //ASTRO_SERVICE_UTIL_H_INCLUDED
