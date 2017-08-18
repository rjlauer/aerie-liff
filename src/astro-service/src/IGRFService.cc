/*!
 * @file IGRFService.cc
 * @brief Calculate the orientation of the IGRF at some point, at some time.
 * @author Segev BenZvi
 * @date 10 Jan 2013
 * @version $Id: IGRFService.cc 33809 2016-08-16 14:38:44Z zhampel $
 */

#ifdef COMPILE_FORTRAN_COMPONENTS

#include <astro-service/IGRFService.h>
#include <astro-service/Geopack-2008.h>

#include <data-structures/geometry/Point.h>
#include <data-structures/geometry/Vector.h>
#include <data-structures/geometry/Ellipsoid.h>

#include <data-structures/time/ModifiedJulianDate.h>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/RegisterService.h>

using namespace HAWCUnits;
using namespace std;

REGISTER_SERVICE(IGRFService);

Configuration
IGRFService::DefaultConfiguration()
{
  Configuration c;
  c.Parameter<bool>("dipoleOnly", false);
  return c;
}

void
IGRFService::Initialize(const Configuration& config)
{
  config.GetParameter("dipoleOnly", useDipole_);

  // Initialize the Geopack internal coordinate transformations:
  //   1) set solar wind to be radially outward along the Earth-Sun line.
  //   2) start calculation at the beginning of 2012
  float vgsex = -400.;
  float vgsey = 0;
  float vgsez = 0;
  int year = 2011;
  int day = 1;
  int hour = 0;
  int min = 0;
  int sec = 0;
  recalc_08_(&year, &day, &hour, &min, &sec, &vgsex, &vgsey, &vgsez);
}

void
IGRFService::GetField(
  const Point& p, const ModifiedJulianDate& /*mjd*/, Vector& B)
  const
{
  // Compute only the dipole part of the geomagnetic field
  if (useDipole_) {
    float xgeo = p.GetX() / Ellipsoid::GetEquatorialRadius();
    float ygeo = p.GetY() / Ellipsoid::GetEquatorialRadius();
    float zgeo = p.GetZ() / Ellipsoid::GetEquatorialRadius();
    float xgsw, ygsw, zgsw;
    int geo2gsw =  1;
    int gsw2geo = -1;

    float Bxgsw, Bygsw, Bzgsw;
    float Bxgeo, Bygeo, Bzgeo;

    // Convert geocentric input to GSW system, calculate B, then rotate back
    geogsw_08_(&xgeo, &ygeo, &zgeo, &xgsw, &ygsw, &zgsw, &geo2gsw);
    dip_08_(&xgsw, &ygsw, &zgsw, &Bxgsw, &Bygsw, &Bzgsw);
    geogsw_08_(&Bxgeo, &Bygeo, &Bzgeo, &Bxgsw, &Bygsw, &Bzgsw, &gsw2geo);

    B.SetXYZ(Bxgeo, Bygeo, Bzgeo);
  }
  // Alternatively, compute the full IGRF model
  else {
    float r = p.GetMag() / Ellipsoid::GetEquatorialRadius();

    float theta = p.GetTheta();
    const double sint = sin(theta);
    const double cost = cos(theta);

    float phi = p.GetPhi();
    const double sinp = sin(phi);
    const double cosp = cos(phi);

    float Br, Btheta, Bphi;
    igrf_geo_08_(&r, &theta, &phi, &Br, &Btheta, &Bphi);

    B.SetXYZ(
      (Br*sint + Btheta*cost)*cosp - Bphi*sinp,
      (Br*sint + Btheta*cost)*sinp + Bphi*cosp,
      Br*cost - Btheta*sint);
  }

  B *= HAWCUnits::nano*tesla;
}

#endif // COMPILE_FORTRAN_COMPONENTS

