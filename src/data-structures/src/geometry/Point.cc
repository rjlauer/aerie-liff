/*!
 * @file Point.cc 
 * @brief Implementation of a 3D coordinate triplet.
 * @author Segev BenZvi 
 * @date 18 Feb 2010 
 * @version $Id: Point.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <data-structures/geometry/Point.h>
#include <data-structures/geometry/R3Transform.h>

using namespace std;

Point&
Point::Transform(const R3Transform& m)
{
  const double pX = x_;
  const double pY = y_;
  const double pZ = z_;

  x_ = m.GetXX()*pX + m.GetXY()*pY + m.GetXZ()*pZ + m.GetDX();
  y_ = m.GetYX()*pX + m.GetYY()*pY + m.GetYZ()*pZ + m.GetDY();
  z_ = m.GetZX()*pX + m.GetZY()*pY + m.GetZZ()*pZ + m.GetDZ();

  return *this;
}

Point
operator*(const R3Transform& m, const Point& p)
{
  const double pX = p.x_;
  const double pY = p.y_;
  const double pZ = p.z_;

  return Point(m.GetXX()*pX + m.GetXY()*pY + m.GetXZ()*pZ + m.GetDX(),
               m.GetYX()*pX + m.GetYY()*pY + m.GetYZ()*pZ + m.GetDY(),
               m.GetZX()*pX + m.GetZY()*pY + m.GetZZ()*pZ + m.GetDZ());
}

