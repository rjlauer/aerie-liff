/*!
 * @file AxialVector.cc 
 * @brief Implementation of an object that transforms like a pseudovector.
 * @author Segev BenZvi 
 * @date 18 Feb 2010 
 * @version $Id: AxialVector.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <data-structures/geometry/AxialVector.h>
#include <data-structures/geometry/R3Transform.h>

AxialVector&
AxialVector::Transform(const R3Transform& m)
{
  const double vX = x_;
  const double vY = y_;
  const double vZ = z_;
  const double xx = m.GetXX();
  const double xy = m.GetXY();
  const double xz = m.GetXZ();
  const double yx = m.GetYX();
  const double yy = m.GetYY();
  const double yz = m.GetYZ();
  const double zx = m.GetZX();
  const double zy = m.GetZY();
  const double zz = m.GetZZ();

  x_ = (yy*zz-yz*zy)*vX + (yz*zx-yx*zz)*vY + (yx*zy-yy*zx)*vZ;
  y_ = (zy*xz-zz*xy)*vX + (zz*xx-zx*xz)*vY + (zx*xy-zy*xx)*vZ;
  z_ = (xy*yz-xz*yy)*vX + (xz*yx-xx*yz)*vY + (xx*yy-xy*yx)*vZ;

  return *this;
}

AxialVector
operator*(const R3Transform& m, const AxialVector& v)
{
  const double vX = v.x_;
  const double vY = v.y_;
  const double vZ = v.z_;
  const double xx = m.GetXX();
  const double xy = m.GetXY();
  const double xz = m.GetXZ();
  const double yx = m.GetYX();
  const double yy = m.GetYY();
  const double yz = m.GetYZ();
  const double zx = m.GetZX();
  const double zy = m.GetZY();
  const double zz = m.GetZZ();

  return AxialVector((yy*zz-yz*zy)*vX + (yz*zx-yx*zz)*vY + (yx*zy-yy*zx)*vZ,
                     (zy*xz-zz*xy)*vX + (zz*xx-zx*xz)*vY + (zx*xy-zy*xx)*vZ,
                     (xy*yz-xz*yy)*vX + (xz*yx-xx*yz)*vY + (xx*yy-xy*yx)*vZ);
}

