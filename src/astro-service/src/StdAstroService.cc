
#include <iostream>
#include <iomanip>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/Logging.h>
#include <hawcnest/RegisterService.h>

#include <data-structures/geometry/R3Transform.h>
#include <data-structures/geometry/LatLonAlt.h>
#include <data-structures/geometry/Vector.h>
#include <data-structures/astronomy/EclPoint.h>
#include <data-structures/astronomy/EquPoint.h>
#include <data-structures/astronomy/GalPoint.h>
#include <data-structures/astronomy/HorPoint.h>
#include <data-structures/astronomy/AstroCoords.h>

#include <astro-service/StdAstroService.h>
#include <astro-service/Precession.h>
#include <astro-service/Nutation.h>
#include <astro-service/Moon.h>
#include <astro-service/Sun.h>

using namespace HAWCUnits;
using namespace std;

const double mjdJ2000 = 51544.5 * day;
const double century = 36525 * day;

REGISTER_SERVICE(StdAstroService);

Configuration
StdAstroService::DefaultConfiguration()
{
  Configuration config;
  config.Parameter<bool>("cachePrecession", true);
  return config;
}

void
StdAstroService::Initialize(const Configuration& config)
{
  config.GetParameter("cachePrecession", cachePrecess_);
  if (cachePrecess_) {
    cachedMJD_ = boost::make_shared<ModifiedJulianDate>(UTCDateTime(1995,1,1,0,0,0));
    cachedEpoch_ = boost::make_shared<ModifiedJulianDate>(J2000_MJD);
  }
}

// ___________________________
// Sidereal time calculations \_________________________________________________

double
StdAstroService::GetGMST(const ModifiedJulianDate& mjd)
  const
{
  // Interval between MJD and J2000, in centuries
  // NOTE: MJD is expected with respect to UT
  const double t0 = (mjd.GetDate() - mjdJ2000) / century;
  const double t1 = (mjd.GetDate() - mjdJ2000) / day;

  // IAU 1982 formula for 0h UT of a given MJD, adapted to handle any time.
  // See J. Meeus, Astronomical Algorithms (Willman-Bell 1998) p. 88, eq. 12.4
  double gmst = (280.46061837 + 360.98564736629 * t1
                 + t0*t0 * (0.000387933 - t0 / 38710000.)) * degree;

  gmst = fmod(gmst, twopi);
  return (gmst < 0.) ? gmst + twopi : gmst;
}

double
StdAstroService::GetAST(const ModifiedJulianDate& mjd)
  const
{
  const double T = (floor(mjd.GetDate()/day) - mjdJ2000/day) / 36525.;

  // IAU 1982 formula for 0h UT of a given MJD
  // See J. Meeus, Astronomical Algorithms (Willman-Bell 1998) p. 87, eq. 12.2
  const double t0 = (24110.54841 + (8640184.812866 + (0.093104 - 6.2e-6*T)*T)*T) * second;

  double ast = fmod(mjd.GetDate(), 1*day) - t0;
  ast = fmod(ast, 1*day);
  if (ast < 0.)
    ast += 1*day;
  ast *= 0.9972695663;

  return ast * 15*degree/hour;

  /*
  // IAU 1982 formula for 0h UT of a given MJD
  // See J. Meeus, Astronomical Algorithms (Willman-Bell 1998) p. 87, eq. 12.2
  double ast = (24110.54841 + (8640184.812866 + (0.093104 - 6.2e-6*T)*T)*T) * second;
  ast += 1.00273790935 * fmod(mjd.GetDate(), 1*day);
  ast *= 15*degree / hour;

  ast = fmod(ast, twopi);
  return (ast < 0.) ? ast + twopi : ast;
  */
}

// _________________________________
// Local/equatorial transformations \___________________________________________

void
StdAstroService::Loc2Equ(const ModifiedJulianDate& mjd, const LatLonAlt& llh,
                         const Vector& axis, EquPoint& equ, const TimeSystem s,
                         const bool toJ2000)
  const
{
  // Conversion from local to equatorial coordinates: see discussion in
  // J. Meeus, Astronomical Algorithms, Willman-Bell 1998, chapter 13.
  //
  // Note 1: this algorithm is adapted from the SLALIB routine sla_H2E in h2e.f
  //         (P. Wallace, GPLv2, 2005)
  // Note 2: a conversion from right to left-handed coordinates is needed
  // Note 3: a conversion from zenith to elevation angle is needed

  double a = twopi - axis.GetPhi();
  while (a < 0.)
    a += twopi;
  a = fmod(a + halfpi, twopi);

  // Choose time system for conversion (sidereal, anti-sidereal, or UT time)
  double lst = 0;
  switch (s) {
    case SIDEREAL:
      lst = GetGMST(mjd);
      break;
    case ANTISIDEREAL:
      lst = GetAST(mjd);
      break;
    case SOLAR:
      lst = fmod(mjd.GetDate(), 1*day) * 15*degree/hour;
      break;
    default:
      break;
  }

  const double sinA = sin(a);
  const double cosA = cos(a);
  const double sinE = cos(axis.GetTheta());
  const double cosE = sin(axis.GetTheta());
  const double sinL = sin(llh.GetLatitude());
  const double cosL = cos(llh.GetLatitude());

  const double x = -cosA*cosE*sinL + sinE*cosL;
  const double y = -sinA*cosE;
  const double z =  cosA*cosE*cosL + sinE*sinL;

  const double r = sqrt(x*x + y*y);
  double ha = (r != 0.) ? atan2(y, x) : 0.;
  while (ha < 0.)
    ha += twopi;
  double ra = lst + llh.GetLongitude() - ha;
  while (ra < 0.)
    ra += twopi;

  equ.SetRADec(fmod(ra, twopi), atan2(z, r));

  // Precess to J2000 from current epoch if requested
  if (toJ2000)
    Precess(J2000_MJD, mjd, equ);
}

void
StdAstroService::Equ2Loc(const ModifiedJulianDate& mjd, const LatLonAlt& llh,
                         const EquPoint& equ, Vector& axis, const TimeSystem s,
                         const bool fromJ2000)
  const
{
  // Conversion from horizon to equatorial coordinates: see discussion in
  // J. Meeus, Astronomical Algorithms, Willman-Bell 1998, chapter 13.
  //
  // Note 1: this algorithm is adapted from the SLALIB routine sla_E2H in e2h.f
  //         (P. Wallace, GPLv2, 2005)
  // Note 2: a conversion from left to right-handed coordinates is needed
  // Note 3: a conversion from elevation to zenith angle is needed

  // Choose time system for conversion (sidereal, anti-sidereal, or UT time)
  double lst = 0;
  switch (s) {
    case SIDEREAL:
      lst = GetGMST(mjd);
      break;
    case ANTISIDEREAL:
      lst = GetAST(mjd);
      break;
    case SOLAR:
      lst = fmod(mjd.GetDate(), 1*day) * 15*degree/hour;
      break;
    default:
      break;
  }

  // Precess from J2000 to current epoch if requested
  EquPoint equC = equ;
  if (fromJ2000)
    Precess(mjd, J2000_MJD, equC);

  const double ha = lst + llh.GetLongitude() - equC.GetRA();

  const double sinH = sin(ha);
  const double cosH = cos(ha);
  const double sinD = sin(equC.GetDec());
  const double cosD = cos(equC.GetDec());
  const double sinL = sin(llh.GetLatitude());
  const double cosL = cos(llh.GetLatitude());

  const double x = -cosH*cosD*sinL + sinD*cosL;
  const double y = -sinH*cosD;
  const double z =  cosH*cosD*cosL + sinD*sinL;

  const double r = sqrt(x*x + y*y);
  double a = (r != 0.) ? twopi - atan2(y, x) : 0.;
  while (a < 0.)
    a += twopi;

  axis.SetRThetaPhi(1., atan2(r,z), fmod(a+halfpi, twopi));
}

// ______________________________________
// Horizontal/equatorial transformations \______________________________________

void
StdAstroService::Hor2Equ(const ModifiedJulianDate& mjd, const LatLonAlt& llh,
                         const HorPoint& hor, EquPoint& equ)
  const
{
  // Conversion from horizon to equatorial coordinates: see discussion in
  // J. Meeus, Astronomical Algorithms, Willman-Bell 1998, chapter 13.
  //
  // Note: this algorithm is adapted from the SLALIB routine sla_H2E in h2e.f
  // (P. Wallace, GPLv2, 2005)

  const double sinA = sin(hor.GetAzimuth());
  const double cosA = cos(hor.GetAzimuth());
  const double sinE = sin(hor.GetElevation());
  const double cosE = cos(hor.GetElevation());
  const double sinL = sin(llh.GetLatitude());
  const double cosL = cos(llh.GetLatitude());

  const double x = -cosA*cosE*sinL + sinE*cosL;
  const double y = -sinA*cosE;
  const double z =  cosA*cosE*cosL + sinE*sinL;

  const double r = sqrt(x*x + y*y);
  double ha = (r != 0.) ? atan2(y, x) : 0.;
  while (ha < 0.)
    ha += twopi;
  double ra = GetGMST(mjd) + llh.GetLongitude() - ha;
  while (ra < 0.)
    ra += twopi;

  equ.SetRADec(fmod(ra, twopi), atan2(z, r));
}

void
StdAstroService::Equ2Hor(const ModifiedJulianDate& mjd, const LatLonAlt& llh,
                         const EquPoint& equ, HorPoint& hor)
  const
{
  // Conversion from horizon to equatorial coordinates: see discussion in
  // J. Meeus, Astronomical Algorithms, Willman-Bell 1998, chapter 13.
  //
  // Note: this algorithm is adapted from the SLALIB routine sla_E2H in e2h.f
  // (P. Wallace, GPLv2, 2005)

  const double ha = GetGMST(mjd) + llh.GetLongitude() - equ.GetRA();

  const double sinH = sin(ha);
  const double cosH = cos(ha);
  const double sinD = sin(equ.GetDec());
  const double cosD = cos(equ.GetDec());
  const double sinL = sin(llh.GetLatitude());
  const double cosL = cos(llh.GetLatitude());

  const double x = -cosH*cosD*sinL + sinD*cosL;
  const double y = -sinH*cosD;
  const double z =  cosH*cosD*cosL + sinD*sinL;

  const double r = sqrt(x*x + y*y);
  double a = (r != 0.) ? atan2(y, x) : 0.;
  while (a < 0.)
    a += twopi;

  hor.SetElevationAzimuth(atan2(z, r), fmod(a, twopi));
}

// ____________________________________
// Equatorial/ecliptic transformations \________________________________________

double
StdAstroService::GetMeanObliquity(const ModifiedJulianDate& mjd)
  const
{
  // Interval between J2000 and the current epoch, in centuries.
  // Note that the date is given in terrestrial time (a.k.a. ephemeris time)
  const double T = (mjd.GetDate(TT) - mjdJ2000) / century;

  // Mean obliquity, IAU 1984, without nutation correction.
  // See J. Meeus, Astronomical Algorithms (Willman-Bell 1998), p. 147
  const double eps0 = (84381.448 +
      T * (-46.8150 + T * (-0.00059 + T * 0.001813))) * arcsec;

  return eps0;
}

double
StdAstroService::GetMeanObliquityLaskar(const ModifiedJulianDate& mjd)
  const
{
  // Interval between J2000 and the current epoch, units of 10000 years.
  // Note that the date is given in terrestrial time (a.k.a. ephemeris time)
  const double u = (mjd.GetDate(TT) - mjdJ2000) / century;
  if (abs(u) > 1.)
    log_fatal("Bad MJD = " << mjd.GetDate() / day
              << "; Laskar formula only valid for 10000 years around J2000.0");

  // Mean obliquity, J. Laskar, A&A 157 (1984) 68, without nutation correction.
  // See J. Meeus, Astronomical Algorithms (Willman-Bell 1998), p. 147
  const double eps0 = (84381.448 + u * (-4680.93 + u * (-1.55 + u * (1999.25 +
                      u * (-51.38 + u * (-249.67 + u * (-39.05 +
                      u * (7.12 + u * (27.87 + u * (5.79 +
                      u * 2.45)))))))))) * arcsec;

  return eps0;
}

void
StdAstroService::Ecl2Equ(const ModifiedJulianDate& mjd, const EclPoint& ecl, EquPoint& equ)
  const
{
  const double eps0 = GetMeanObliquity(mjd);
  const Rotate ec2eqMtx(eps0, ORIGIN, XAXIS);
  equ.SetPoint(ec2eqMtx * ecl.GetPoint());
}

void
StdAstroService::Equ2Ecl(const ModifiedJulianDate& mjd, const EquPoint& equ, EclPoint& ecl)
  const
{
  const double eps0 = GetMeanObliquity(mjd);
  const Rotate eq2ecMtx(-eps0, ORIGIN, XAXIS);
  ecl.SetPoint(eq2ecMtx * equ.GetPoint());
}

// ____________________________________
// Equatorial/galactic transformations \________________________________________

// Galactic north pole in J2000.0 equatorial coordinates
const EquPoint gnpJ2000(HrMinSec(12*hour, 51*minute, 26.28*second),
                        DegMinSec(27*degree, 7*arcminute, 41.70*arcsecond));

// Galactic center in J2000.0 equatorial coordinates
const EquPoint gcnJ2000(HrMinSec(17*hour, 45*minute, 37.2*second),
                       -DegMinSec(28*degree, 56*arcminute, 10.22*arcsecond));

// Equatorial to galactic rotation matrix
const Rotate e2gMtx(gcnJ2000.GetPoint(), gnpJ2000.GetPoint(),
                    XAXIS, ZAXIS);

// Galactic to equatorial rotation matrix
const Rotate g2eMtx(XAXIS, ZAXIS,
                    gcnJ2000.GetPoint(), gnpJ2000.GetPoint());

void
StdAstroService::Equ2Gal(const EquPoint& equ, GalPoint& gal)
  const
{
  gal.SetPoint(e2gMtx * equ.GetPoint());
}

void
StdAstroService::Gal2Equ(const GalPoint& gal, EquPoint& equ)
  const
{
  equ.SetPoint(g2eMtx * gal.GetPoint());
}

void
StdAstroService::GetLunarGeocentricPosition(
  const ModifiedJulianDate& mjd, EquPoint& equ)
  const
{
  // Get the geocentric position of the moon and directly calculate RA and Dec
  Moon::GetGeocentricPoint(mjd, moonPos_);
  double ra = moonPos_.GetPhi();
  while (ra < 0)
    ra += 360*degree;
  equ.SetRADec(ra, 90*degree - moonPos_.GetTheta());
}

void
StdAstroService::GetLunarTopocentricPosition(
  const ModifiedJulianDate& mjd, const LatLonAlt& locale, EquPoint& equ)
  const
{
  // Get the geocentric position of the moon and translate it to the
  // topocentric equatorial frame with the origin at "locale"
  Moon::GetGeocentricPoint(mjd, moonPos_);
  moonPos_ -= Rotate(GetGMST(mjd), ORIGIN, ZAXIS) * locale.GetGeocentricPoint();
  double ra = moonPos_.GetPhi();
  while (ra < 0)
    ra += 360*degree;
  equ.SetRADec(ra, 90*degree - moonPos_.GetTheta());
}

void
StdAstroService::GetSolarGeocentricPosition(
  const ModifiedJulianDate& mjd, EquPoint& equ)
  const
{
  // Get the geocentric position of the sun and directly calculate RA and Dec
  Sun::GetGeocentricPoint(mjd, sunPos_);
  double ra = sunPos_.GetRA();
  while (ra < 0)
    ra += 360*degree;
  equ.SetRADec(ra, sunPos_.GetDec());
}

void
StdAstroService::Precess(
  const ModifiedJulianDate& epoch, const ModifiedJulianDate& mjd, EquPoint& equ)
  const
{
  // Cache the precession calculation, updating only after 1 day elapses;
  // the error in the correction is about 1e-6 degree/day.
  if (cachePrecess_) {
    bool reCache = false;

    if (abs(cachedMJD_->GetDate() - mjd.GetDate()) > 1*day) {
      log_debug("Resetting cached MJD " << *cachedMJD_ << " to " << mjd);
      cachedMJD_ = boost::make_shared<ModifiedJulianDate>(mjd);
      reCache = true;
    }

    if (abs(cachedEpoch_->GetDate() - epoch.GetDate()) > 1*day) {
      log_debug("Resetting cached MJD " << *cachedEpoch_ << " to " << epoch);
      cachedEpoch_ = boost::make_shared<ModifiedJulianDate>(mjd);
      reCache = true;
    }

    if (reCache) {
      if (mjd.GetDate() > epoch.GetDate()) {
        nupreMtx_ = Nutation::GetRotationMatrix(mjd) *
                    Precession::GetRotationMatrix(epoch, mjd);
        nupreMtx_.Invert();
      }
      else {
        nupreMtx_ = Nutation::GetRotationMatrix(epoch) *
                    Precession::GetRotationMatrix(mjd, epoch);
      }
    }
    log_debug("Using cached nutation/precession matrix");
  }
  // Don't cache the precession/nutation matrix -- just calculate it every call
  else {
    if (mjd.GetDate() > epoch.GetDate()) {
      nupreMtx_ = Nutation::GetRotationMatrix(mjd) *
                  Precession::GetRotationMatrix(epoch, mjd);
      nupreMtx_.Invert();
    }
    else {
      nupreMtx_ = Nutation::GetRotationMatrix(epoch) *
                  Precession::GetRotationMatrix(mjd, epoch);
    }
  }

  equ.SetPoint(nupreMtx_ * equ.GetPoint());
}

void
StdAstroService::PrecessFromJ2000ToEpoch(
  const ModifiedJulianDate& epoch, EquPoint& equ)
  const
{
  Precess(epoch, J2000_MJD, equ);
}

void
StdAstroService::PrecessFromEpochToJ2000(
  const ModifiedJulianDate& epoch, EquPoint& equ)
  const
{
  Precess(J2000_MJD, epoch, equ);
}
