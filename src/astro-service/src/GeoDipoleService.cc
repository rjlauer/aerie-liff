/*!
 * @file GeoDipoleService.cc
 * @brief Calculate the orientation of the geomagnetic dipole at some point,
 *        at some time.
 * @author Segev BenZvi
 * @date 9 Nov 2015
 * @version $Id: GeoDipoleService.cc 27616 2015-11-09 13:58:19Z sybenzvi $
 */

#include <astro-service/GeoDipoleService.h>

#include <data-structures/geometry/Point.h>
#include <data-structures/geometry/Vector.h>
#include <data-structures/geometry/Ellipsoid.h>

#include <data-structures/time/ModifiedJulianDate.h>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/RegisterService.h>

#include <cmath>

using namespace HAWCUnits;
using namespace std;

REGISTER_SERVICE(GeoDipoleService);

Configuration
GeoDipoleService::DefaultConfiguration()
{
  Configuration c;
  return c;
}

void
GeoDipoleService::Initialize(const Configuration& /*config*/)
{
}

void
GeoDipoleService::GetField(
  const Point& p, const ModifiedJulianDate& /*mjd*/, Vector& B)
  const
{
  double r = p.GetMag() / Ellipsoid::GetEquatorialRadius();
  double B0 = 31.2 * HAWCUnits::micro*tesla;
  double tilt = 11.5 * degree;

  double xp = p.GetX();
  double yp = p.GetY()*cos(tilt) - p.GetZ()*sin(tilt);
  double zp = p.GetY()*sin(tilt) + p.GetZ()*cos(tilt);
  double rp = sqrt(xp*xp + yp*yp + zp*zp);

  double theta = acos(zp/rp);
  double phi = atan2(yp, xp);

  double Br = -2 * B0 * pow(1./r,3) * cos(theta);
  double Btheta = -B0 * pow(1./r,3) * sin(theta);

  double Bx = sin(theta)*cos(phi)*Br + cos(theta)*cos(phi)*Btheta;
  double By = sin(theta)*sin(phi)*Br + cos(theta)*sin(phi)*Btheta;
  double Bz = cos(theta)*Br - sin(theta)*Btheta;

  B.SetXYZ(Bx, By, Bz);
}

