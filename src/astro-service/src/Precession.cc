
#include <hawcnest/HAWCUnits.h>

#include <data-structures/geometry/Point.h>
#include <data-structures/time/ModifiedJulianDate.h>

#include <astro-service/Precession.h>

using namespace HAWCUnits;
using namespace std;

Rotate Precession::precMatrix_ = Rotate();

const Rotate&
Precession::GetRotationMatrix(
  const ModifiedJulianDate& e0, const ModifiedJulianDate& e1)
{
  // Time interval (centuries) between J2000.0 and the starting epoch
  const double T = (e0.GetDate()/day - 51544.5) / 36525.;

  // Time interval (centuries) between the starting epoch and the final epoch
  const double t = (e1.GetDate() - e0.GetDate()) / (36525*day);

  // Euler angles: J. Meeus, Astro. Algorithms (Willman-Bell 1998) p. 134
  // Note: this is accurate to 0.1 arcsec up to 2040AD
  double zeta;
  double z;
  double theta;

  if (T == 0) {
    zeta  = t*(2306.2181 + t*(0.30188 + t*0.017998)) * arcsec;
    z     = t*(2306.2181 + t*(1.09468 + t*0.018203)) * arcsec;
    theta = t*(2004.3109 - t*(0.42665 + t*0.041833)) * arcsec;
  }
  else {
    const double u = 2306.2181 + T*(1.39656 - T*0.000139);
    const double v = 2004.3109 - T*(0.85330 + T*0.000217);
    zeta  = t*(u + t*((0.30188 - 3.44e-4*T) + t*0.017998)) * arcsec;
    z     = t*(u + t*((1.09468 + 6.60e-5*T) + t*0.018203)) * arcsec;
    theta = t*(v - t*((0.42665 + 2.17e-4*T) + t*0.041833)) * arcsec;
  }

  // Rotate by zeta about Z; by -theta about Y; and by z about Z.
  precMatrix_ = Rotate(z, ORIGIN, ZAXIS) *
                Rotate(-theta, ORIGIN, YAXIS) *
                Rotate(zeta, ORIGIN, ZAXIS);

  return precMatrix_;
}

