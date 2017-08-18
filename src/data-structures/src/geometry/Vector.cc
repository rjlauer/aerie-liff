/*!
 * @file Vector.cc 
 * @brief Implementation of a polar vector.
 * @author Segev BenZvi 
 * @date 18 Feb 2010 
 * @version $Id: Vector.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <data-structures/geometry/Vector.h>
#include <data-structures/geometry/R3Transform.h>

Vector&
Vector::Transform(const R3Transform& m)
{
  const double vX = x_;
  const double vY = y_;
  const double vZ = z_;

  x_ = m.GetXX()*vX + m.GetXY()*vY + m.GetXZ()*vZ;
  y_ = m.GetYX()*vX + m.GetYY()*vY + m.GetYZ()*vZ;
  z_ = m.GetZX()*vX + m.GetZY()*vY + m.GetZZ()*vZ;

  return *this;
}

Vector
operator*(const R3Transform& m, const Vector& v)
{
  const double vX = v.x_;
  const double vY = v.y_;
  const double vZ = v.z_;

  return Vector(m.GetXX()*vX + m.GetXY()*vY + m.GetXZ()*vZ,
                m.GetYX()*vX + m.GetYY()*vY + m.GetYZ()*vZ,
                m.GetZX()*vX + m.GetZY()*vY + m.GetZZ()*vZ);
}

