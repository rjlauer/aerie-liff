/*!
 * @file R3Vector.cc 
 * @brief Implementation of a vector base class in R3.
 * @author Segev BenZvi 
 * @date 18 Feb 2010 
 * @version $Id: R3Vector.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <data-structures/geometry/R3Vector.h>

#include <hawcnest/Logging.h>
#include <hawcnest/HAWCUnits.h>

using namespace HAWCUnits;
using namespace std;

double
R3Vector::Angle(const R3Vector& v)
  const
{
  double arg = (x_*v.x_ + y_*v.y_ + z_*v.z_) / sqrt(GetMag2() * v.GetMag2());
  if (arg >= 1.)
    return 0.;
  if (arg <= -1.)
    return pi;
  return acos(arg);
}

R3Vector::R3Vector(const double v1, const double v2, const double v3,
                   const CoordinateSystem sys)
{
  switch (sys) {
    case CARTESIAN:      // (v1, v2, v3) = (x, y, z)
    {
      SetXYZ(v1, v2, v3);
      break;
    }
    case CYLINDRICAL:    // (v1, v2, v3) = (rho, phi, z)
    {
      SetRhoPhiZ(v1, v2, v3);
      break;
    }
    case SPHERICAL:      // (v1, v2, v3) = (r, theta, phi)
    {
      SetRThetaPhi(v1, v2, v3);
      break;
    }
  }
}

void
R3Vector::SetXYZ(const double x, const double y, const double z)
{
  x_ = x;
  y_ = y;
  z_ = z;
}

void 
R3Vector::SetRhoPhiZ(const double rho, const double phi, const double z)
{
  x_ = rho * cos(phi);
  y_ = rho * sin(phi);
  z_ = z;
}

void 
R3Vector::SetRThetaPhi(const double r, const double theta, const double phi)
{
  x_ = r * sin(theta) * cos(phi);
  y_ = r * sin(theta) * sin(phi);
  z_ = r * cos(theta);
}

void
R3Vector::SetPhi(const double phi)
{
  const double rho = GetRho();
  if (rho == 0) {
    log_warn("could not set phi: the vector is colinear with the z-axis");
    return;
  }
  else
    SetXYZ(rho * cos(phi), rho * sin(phi), z_);
}

void
R3Vector::SetTheta(const double theta)
{
  const double r = GetMag();
  if (r == 0) {
    log_warn("could not set theta: the vector has zero magnitude");
    return;
  }
  else {
    const double phi = GetPhi();
    SetXYZ(r*sin(theta)*cos(phi), r*sin(theta)*sin(phi), r*cos(theta));
  }
}

R3Vector
R3Vector::GetUnitVector() const
{
  const double r = GetMag();
  return r > 0 ? R3Vector(x_/r, y_/r, z_/r) : R3Vector();
}

R3Vector
R3Vector::GetOrthogonalVector() const
{
  const double dx = x_ < 0 ? -x_ : x_;
  const double dy = y_ < 0 ? -y_ : y_;
  const double dz = z_ < 0 ? -z_ : z_;
  if (dx < dy)
    return dx < dz ? R3Vector(0, z_, -y_) : R3Vector(y_, -x_, 0);
  else
    return dy < dz ? R3Vector(-z_, 0, x_) : R3Vector(y_, -x_, 0);
}

void
R3Vector::Normalize()
{
  double scale = 1. / GetMag();
  x_ *= scale;
  y_ *= scale;
  z_ *= scale;
}

ostream&
operator<<(ostream& os, const R3Vector& v)
{
  os << '(' << v.x_ << ", " << v.y_ << ", " << v.z_ << ')';
  return os;
}

