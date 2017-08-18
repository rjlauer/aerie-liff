/*!
 * @file S2Point.cc 
 * @brief Implementation of coordinate doublets on the unit sphere.
 * @author Segev BenZvi 
 * @date 18 Feb 2010 
 * @version $Id: S2Point.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <data-structures/geometry/S2Point.h>
#include <data-structures/geometry/R3Transform.h>
#include <cmath>

using namespace std;

S2Point::S2Point() :
  p_(1, 0, 0)
{
}

S2Point::S2Point(const double theta, const double phi) :
  p_(Point(1, theta, phi, R3Vector::SPHERICAL))
{
}

S2Point
operator*(const R3Transform& matrix, const S2Point& s)
{
  return matrix*s.p_;
}

ostream&
operator<<(ostream& os, const S2Point& s)
{
  os << '(' << s.GetTheta() << ", " << s.GetPhi() << ')';
  return os;
}

