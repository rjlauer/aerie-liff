/*!
 * @file HorPoint.cc 
 * @brief Implementation of a horizon (left-handed local) coordinate class.
 * @author Segev BenZvi 
 * @date 30 May 2010 
 * @version $Id: HorPoint.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <cmath>

#include <hawcnest/HAWCUnits.h>
#include <data-structures/astronomy/HorPoint.h>

using namespace HAWCUnits;
using namespace std;

namespace {

  // Handle left/right conversion of the azimuth
  double ConvertAzimuth(const double azimuth)
  {
    double azi = twopi - azimuth;       // Convert left/right or right/left
    while (azi < 0.)
      azi += twopi;
    return fmod(azi + halfpi, twopi);   // Shift 0 point to E or N
  }

}

HorPoint::HorPoint(const double elev, const double azim) :
  p_(halfpi - elev, ConvertAzimuth(azim))
{
}

double
HorPoint::GetAzimuth()
  const
{
  return ConvertAzimuth(p_.GetPhi());
}

void
HorPoint::SetAzimuth(const double az)
{
  p_.SetPhi(ConvertAzimuth(az));
}

void
HorPoint::SetElevationAzimuth(const double el, const double az)
{
  p_.SetThetaPhi(HAWCUnits::halfpi - el, ConvertAzimuth(az));
}

ostream&
operator<<(ostream& os, const HorPoint& s)
{
  os << "(Elev = " << s.GetElevation() / degree
     << ", Azim = " << s.GetAzimuth() / degree << ')';
  return os;
}

