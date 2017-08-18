/*!
 * @file UTMPoint.cc 
 * @brief Implementation of Universal Transverse Mercator coordinates.
 * @author Segev BenZvi 
 * @date 08 Jul 2010 
 * @version $Id: UTMPoint.cc 22385 2014-10-23 19:01:21Z criviere $
 */

#include <data-structures/astronomy/AstroCoords.h>

#include <data-structures/geometry/UTMPoint.h>
#include <data-structures/geometry/LatLonAlt.h>
#include <data-structures/geometry/Ellipsoid.h>

#include <hawcnest/Logging.h>

#include <boost/format.hpp>

#include <cmath>

using namespace HAWCUnits;
using namespace std;

namespace {

  // UTM zones
  const double ZONE_WIDTH  = 6*deg;
  const double ZONE_OFFSET = 180*deg; // offset to push longitude into [0..360]

  const char   BAND_LETTER[]     = "CDEFGHJKLMNPQRSTUVWX";
  const int    NBANDS    = sizeof(BAND_LETTER) / sizeof(char) - 1;
  const double BAND_WIDTH      = 8*deg;
  //const double southernmostUTMBand = -80*deg;
  const char   firstNorthernBand   = 'N';  // 11th zone

  //const int ZONE_MIN = 1;
  const int ZONE_MAX = 60;

  // UTM definitions from transformation to latitude/longitude
  double K0 = 0.9996;

}

UTMPoint::UTMPoint(const double lat, const double lon, const double ht)
{
  SetLatitudeLongitudeHeight(lat, lon, ht);
}

UTMPoint::UTMPoint(const LatLonAlt& l)
{
  SetLatitudeLongitudeHeight(l.GetLatitude(), l.GetLongitude(), l.GetHeight());
}

// Converstion of UTM to latitude/longitude.  From L. Nellen and B. Revenu,
// based on Auger technical note GAP-2001-038
void
UTMPoint::GetLatitudeLongitude(double& lat, double& lon)
  const
{
  const double a = Ellipsoid::GetEquatorialRadius();
  const double e2 = Ellipsoid::GetEccentricity2();
  const double e1 = (1. - sqrt(1.-e2)) / (1. + sqrt(1.-e2));

  // Remove 5e5 meter offset for longitude
  const double x = easting_ - 500000.;
  double y = northing_;

  const bool inNorthernHemisphere = band_ >= firstNorthernBand;
  if (!inNorthernHemisphere)
    y -= 10000000.;  // remove 1e7 meter offset used for southern hemisphere

  const double LongitudeOrigin = (zone_ - 0.5) * ZONE_WIDTH - ZONE_OFFSET;

  const double ep2 = e2 / (1. - e2);
  const double e4 = e2*e2;
  const double e6 = e4*e2;
  const double e8 = e4*e4;
  const double e1_2 = e1*e1;

  const double M = y / K0;
  //const double mu = M / (a * (1. - e2/4 - 3*e4/64 - 5*e6/256 - 175*e8/16384));
  const double mu = M / (a * (1. - (4096*e2 - 768*e4 - 320*e6 - 175*e8)/16384));
  const double mur = mu/rad;
  const double phi1 = mur
    + (48.*e1 - 27.*e1_2*e1)/32. * sin(2*mur)
    + (42.*e1_2 - 55.*e1_2*e1_2)/32. * sin(4*mur)
    + (151.*e1*e1_2/96.) * sin(6*mur)
    + (1097.*e1_2*e1_2/512.) * sin(8*mur);

  const double phi1r = phi1/rad;
  const double cosPhi1 = cos(phi1r);
  const double sinPhi1 = sin(phi1r);
  const double tanPhi1 = sinPhi1/cosPhi1;

  const double N1 = a / sqrt(1. - e2 * sinPhi1*sinPhi1);
  const double T1 = tanPhi1*tanPhi1;
  const double C1 = ep2 * cosPhi1*cosPhi1;
  const double R1 = a * (1. - e2) / pow(1. - e2 * sinPhi1*sinPhi1, 1.5);
  const double D = x / (N1*K0);
  const double D2 = D*D;
  const double D4 = D2*D2;
  const double C1_2 = C1*C1;
  const double T1_2 = T1*T1;

  lat = phi1 - N1*tanPhi1/R1 *
        (0.5*D2
        - (5. + 3.*T1 + 10.*C1 - 4.*C1_2 - 9.*ep2) * D4/24.
        + (61. + 90.*T1 + 298.*C1 + 45.*T1_2 - 252.*ep2 - 3.*C1_2) * D2*D4/720);

  lon = (D
         - (1. + 2.*T1 + C1) * D*D2/6.
         + (5. - 2.*C1 + 28.*T1 - 3.*C1_2 + 8.*ep2 + 24.*T1_2) * D*D4 / 120.)
         / cosPhi1 + LongitudeOrigin;
}

// Converstion of latitude/longitude to UTM.  From L. Nellen and B. Revenu,
// based on Auger technical note GAP-2001-038
void
UTMPoint::SetLatitudeLongitudeHeight(double lat, double lon, double ht)
{
  height_ = ht;
  zone_ = ZoneFromLatitudeLongitude(lat, lon);
  band_ = BandFromLatitude(lat);

  const double LongitudeOrigin = (zone_-0.5)*ZONE_WIDTH - ZONE_OFFSET;

  //converts lat/long to UTM coords.  Equations from USGS Bulletin
  //1532 East Longitudes are positive, West longitudes are negative.
  //North latitudes are positive, South latitudes are negative.
  const double e2 = Ellipsoid::GetEccentricity2();
  const double e4 = e2 * e2;
  const double e6 = e4 * e2;
  const double e8 = e4 * e4;

  const double a = Ellipsoid::GetEquatorialRadius();

  const double cosLatitude = cos(lat);
  const double sinLatitude = sin(lat);
  const double tanLatitude = sinLatitude / cosLatitude;

  const double ep2 = e2 / (1. - e2);

  const double N = a / sqrt(1 - e2 * sinLatitude*sinLatitude);
  const double T = tanLatitude*tanLatitude;
  const double C = ep2 * cosLatitude*cosLatitude;
  const double A = cosLatitude * (lon/rad - LongitudeOrigin/rad);

  const double c1 = 1. - (4096*e2 + 768*e4 + 320*e6 + 175*e8) / 16384.;
  const double c2 = -(1536*e2 + 384*e4 + 180*e6 + 105*e8) / 4096.;
  const double c3 = (960*e4 + 720*e6 + 525*e8) / 16384.;
  const double c4 = (140*e6 + 175*e8) / 12288.;
  const double c5 = 315*e8 / 131072.;

  const double M = a * (c1*lat + c2*sin(2*lat) + c3*sin(4*lat)
                               + c4*sin(6*lat) + c5*sin(8*lat));
  const double A2 = A*A;
  const double A3 = A2*A;
  const double T2 = T*T;

  easting_ = K0*N*(A + (1. - T + C)*A3/6.
                     + (5. - 18.*T + T2 + 72.*C - 58.*ep2) * A2*A3 / 120.)
             + 500000.;

  northing_ = K0*(M + N*tanLatitude *
                  (0.5*A2 + (5. - T + 9.*C + 4.*C*C) * A*A3 / 24.
                   + (61. - 58.*T + T2 + 600.*C - 330.*ep2) * A3*A3 / 720.));

  if (lat < 0.)
    northing_ += 10000000.; // 10 000 000 meter offset for southern hemisphere
}

int
UTMPoint::ZoneFromLatitudeLongitude(const double lat, const double lon)
{
  using std::floor;
  int zone = int(floor(lon / ZONE_WIDTH)) + ZONE_MAX/2 + 1;

  // southern Norway zone exception
  if (56*degree <= lat && lat < 64*degree &&
      3*degree <= lon && lat < 12*degree) {
    zone = 32;
  }
  else if (72*degree <= lat && lat < 84*degree) {
    // Svalbard (Norway) zone exceptions
    if (0 <= lon && lon < 9*degree)
      zone = 31;
    else if (9*degree <= lon && lon < 21*degree)
      zone = 33;
    else if (21*degree <= lon && lon < 33*degree)
      zone = 35;
    else if (33*degree <= lon && lon < 42*degree)
      zone = 37;
  }

  return zone;
}

char
UTMPoint::BandFromLatitude(const double lat)
{
  using std::floor;
  int index = int(floor(lat / BAND_WIDTH)) + NBANDS/2;
  if (0 <= index && index < NBANDS)
    return BAND_LETTER[index];
  else if (72*degree <= lat && lat < 84*degree)
    return 'X';
  else {
    log_fatal("Latitude " << lat/deg << " deg out of range [-80 .. 84]. "
              "Cannot calculate band letter.");
    return ' '; // to keep the compiler happy
  }
}

ostream&
operator<<(std::ostream& os, const UTMPoint& u)
{
  os << boost::format("%7.1f E %7.1f N %d%c (UTM), Alt = %g m")
        % (u.easting_/meter)
        % (u.northing_/meter)
        % u.zone_
        % u.band_
        % (u.height_/meter);

  return os;
}

