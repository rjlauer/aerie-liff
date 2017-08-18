/*!
 * @file LatLonAlt.cc 
 * @brief Storage and transformation of latitude/longitude/altitude triplets.
 * @author Segev BenZvi 
 * @date 08 Jul 2010 
 * @version $Id: LatLonAlt.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <data-structures/astronomy/AstroCoords.h>

#include <data-structures/geometry/LatLonAlt.h>
#include <data-structures/geometry/UTMPoint.h>
#include <data-structures/geometry/Ellipsoid.h>

#include <cmath>

using namespace HAWCUnits;
using namespace std;

LatLonAlt::LatLonAlt()
{
  SetLatitudeLongitudeHeight(90*degree, 0, 0);
}

LatLonAlt::LatLonAlt(const Point& geocentricPoint)
{
  SetGeocentricPoint(geocentricPoint);
}

LatLonAlt::LatLonAlt(const double lat, const double lon, const double ht)
{
  SetLatitudeLongitudeHeight(lat, lon, ht);
}

LatLonAlt::LatLonAlt(const UTMPoint& u)
{
  double lat, lon, ht;

  u.GetLatitudeLongitude(lat, lon);
  ht = u.GetHeight();

  SetLatitudeLongitudeHeight(lat, lon, ht);
}

// Geodetic conversion due to J.R. Clynch, Naval Postgraduate School,
// http://www.oc.nps.navy.mil/oc2902w/coord/coordcvt.pdf 
// From Auger Offline code by Lukas Nellen
void
LatLonAlt::SetLatitudeLongitudeHeight(double lat, double lon, double ht)
{
  latitude_ = lat;
  longitude_ = lon;
  height_ = ht;

  const double e2 = Ellipsoid::GetEccentricity2();
  const double Rn = Ellipsoid::GetRn(lat);

  p_.SetXYZ(
    (Rn + ht) * cos(lat)*cos(lon),
    (Rn + ht) * cos(lat)*sin(lon),
    (Rn*(1.-e2) + ht) * sin(lat));
}

// Iterative geocentric conversion by J.R. Clynch, Naval Postgraduate School,
// http://www.oc.nps.navy.mil/oc2902w/coord/coordcvt.pdf.
// From Auger Offline code by Lukas Nellen.  Note that there is a typo in the
// document when calculating the first guess for latitude
void
LatLonAlt::SetGeocentricPoint(const Point& pt)
{
  p_ = pt;

  const double e2 = Ellipsoid::GetEccentricity2();
  const double rho = p_.GetRho();
  const double z = p_.GetZ();
  longitude_ = p_.GetPhi();

  // First guess at latitude
  latitude_ = atan(z / rho);

  // If at poles, height is easy to estimate
  if (rho == 0) {
    height_ = abs(z) - Ellipsoid::GetPolarRadius();
  }
  // Else, solve iteratively for latitude and height (Clynch recommends 4 steps)
  else {
    double Rn;
    for (int i = 0; i < 4; ++i) {
      Rn = Ellipsoid::GetRn(latitude_);
      height_ = rho / cos(latitude_) - Rn;
      latitude_ = atan(z / rho / (1 - e2*(Rn/(Rn + height_))));
    }
  }
}

ostream&
operator<<(std::ostream& os, const LatLonAlt& t)
{
  bool N = t.latitude_ > 0;
  bool E = t.longitude_ > 0;

  os << "(" << DegMinSec(abs(t.latitude_)) << (N ? " N" : " S")
     << ", " << DegMinSec(abs(t.longitude_)) << (E ? " E" : " W")
     << ", Alt = " << t.height_ / meter << " m)";
  return os;
}

