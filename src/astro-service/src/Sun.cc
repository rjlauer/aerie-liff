/*!
 * @file Sun.cc
 * @author Dan Fiorino
 * @date 18 Jul 2013
 * @brief Sun-tracking code, copied from the Milinda software.
 * @version $Id$
 */

#include <astro-service/Sun.h>

#include <data-structures/astronomy/EquPoint.h>
#include <data-structures/time/ModifiedJulianDate.h>

#include <hawcnest/HAWCUnits.h>

#include <cmath>

  // Algorithm taken directly from:
  // J. Meeus, "Astronomical Algorithms," Willman-Bell 1998, p. 163-165.

using namespace HAWCUnits;
using namespace std;

void
Sun::GetGeocentricPoint(const ModifiedJulianDate& mjd, EquPoint& ep)
{
  // get time in correct units 
  double MJD = mjd.GetDate()/day;
  double JD  = MJD + 2400000.5;
  double T   = (JD - 2451545.0) / 36525.;

  // geometric mean longitude of the Sun
  double Lo  = 280.46646 + 36000.76983*T + 0.0003032*T*T;
  Lo *= degree;
  while (Lo < 0.)
    Lo += twopi;
  while (Lo > twopi)
    Lo -= twopi;

  // mean anomaly of the Sun
  double M   = 357.52911 + 35999.05029*T - 0.0001537*T*T;
  M  *= degree;
  while (M < 0.)
    M += twopi;
  while (M > twopi)
    M -= twopi;

  // equation of the center of the Sun
  double C   =   (1.914602 - 0.004817*T - 0.000014*T*T)*sin(M)
               + (0.019993 - 0.000101*T)*sin(2*M)
               + 0.000289*sin(3*M);
  C  *= degree;

  // true longitude of the Sun
  double LoT = Lo + C;

  // oliquity of ellipse
  double eta_o = 23.439291111 - 0.013004167*T - 0.0000001638*T*T
               + 0.0000005036*T*T*T;
  eta_o *= degree;

  // low order correction to apparent true longitude for nutation and aberration
  double omega = 125.04 - 1934.136*T;
  omega *= degree;

  // apparent true longitude of Sun corrected for nutation/aberration
  double lambda = LoT - 0.00569*degree - 0.00478*degree*sin(omega);

  // oliquity of ellipse corrected for nutation/aberration
  double eta = eta_o + 0.00256*degree*cos(omega);

  // apparent equatorial position of Sun corrected for nutation/aberration
  double ra_app  = atan2( (cos(eta) * sin(lambda)), cos(lambda) );
  while (ra_app < 0.)
    ra_app += twopi;
  while (ra_app > twopi)
    ra_app -= twopi;
  double dec_app = asin( sin(eta) * sin(lambda));

  ep.SetRADec(ra_app,dec_app);
}

