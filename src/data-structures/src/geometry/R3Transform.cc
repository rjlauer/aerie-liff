/*!
 * @file R3Transform.cc 
 * @brief Implementation of coordinate transformations in R3.
 * @author Segev BenZvi 
 * @date 18 Feb 2010 
 * @version $Id: R3Transform.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <data-structures/geometry/R3Transform.h>
#include <data-structures/geometry/Point.h>
#include <data-structures/geometry/Vector.h>

#include <hawcnest/Logging.h>

#include <iomanip>
#include <cmath>

using namespace std;

R3Transform::R3Transform() :
  xx_(0), xy_(0), xz_(0), dx_(0),
  yx_(0), yy_(0), yz_(0), dy_(0),
  zx_(0), zy_(0), zz_(0), dz_(0)
{
}

R3Transform::R3Transform(
  const double xx, const double xy, const double xz, const double dx,
  const double yx, const double yy, const double yz, const double dy,
  const double zx, const double zy, const double zz, const double dz) :

  xx_(xx), xy_(xy), xz_(xz), dx_(dx),
  yx_(yx), yy_(yy), yz_(yz), dy_(dy),
  zx_(zx), zy_(zy), zz_(zz), dz_(dz)
{
}

R3Transform::R3Transform(
  const Point& fr0, const Point& fr1, const Point& fr2,
  const Point& to0, const Point& to1, const Point& to2)
{
  // Construct two basis vectors in the first coordinate system
  Vector x1 = (fr1 - fr0).GetUnitVector();
  Vector y1 = (fr2 - fr0).GetUnitVector();

  // Construct two basis vectors in the second coordinate system
  Vector x2 = (to1 - to0).GetUnitVector();
  Vector y2 = (to2 - to0).GetUnitVector();

  // Check angles
  const double cos1 = x1 * y1;
  const double cos2 = x2 * y2;
  const double tol = 1e-6;

  if (abs(1. - cos1) <= tol || abs(1. - cos2) <= tol) {
    // Issue warning about zero angle between axes
    SetIdentity();
  } else {
    if (abs(cos1 - cos2) > tol) {
      // Issue warning about angles not being equal
    }

    // Complete the basis vectors
    Vector z1 = (x1.Cross(y1)).GetUnitVector();
    y1 = z1.Cross(x1);

    Vector z2 = (x2.Cross(y2)).GetUnitVector();
    y2 = z2.Cross(x2);

    // Create the transformation
    const double detxx =  (y1.GetY()*z1.GetZ() - z1.GetY()*y1.GetZ());
    const double detxy = -(y1.GetX()*z1.GetZ() - z1.GetX()*y1.GetZ());
    const double detxz =  (y1.GetX()*z1.GetY() - z1.GetX()*y1.GetY());
    const double detyx = -(x1.GetY()*z1.GetZ() - z1.GetY()*x1.GetZ());
    const double detyy =  (x1.GetX()*z1.GetZ() - z1.GetX()*x1.GetZ());
    const double detyz = -(x1.GetX()*z1.GetY() - z1.GetX()*x1.GetY());
    const double detzx =  (x1.GetY()*y1.GetZ() - y1.GetY()*x1.GetZ());
    const double detzy = -(x1.GetX()*y1.GetZ() - y1.GetX()*x1.GetZ());
    const double detzz =  (x1.GetX()*y1.GetY() - y1.GetX()*x1.GetY());

    const double txx = x2.GetX()*detxx + y2.GetX()*detyx + z2.GetX()*detzx;
    const double txy = x2.GetX()*detxy + y2.GetX()*detyy + z2.GetX()*detzy;
    const double txz = x2.GetX()*detxz + y2.GetX()*detyz + z2.GetX()*detzz;
    const double tyx = x2.GetY()*detxx + y2.GetY()*detyx + z2.GetY()*detzx;
    const double tyy = x2.GetY()*detxy + y2.GetY()*detyy + z2.GetY()*detzy;
    const double tyz = x2.GetY()*detxz + y2.GetY()*detyz + z2.GetY()*detzz;
    const double tzx = x2.GetZ()*detxx + y2.GetZ()*detyx + z2.GetZ()*detzx;
    const double tzy = x2.GetZ()*detxy + y2.GetZ()*detyy + z2.GetZ()*detzy;
    const double tzz = x2.GetZ()*detxz + y2.GetZ()*detyz + z2.GetZ()*detzz;

    const double dx1 = fr0.GetX();
    const double dy1 = fr0.GetY();
    const double dz1 = fr0.GetZ();
    const double dx2 = to0.GetX();
    const double dy2 = to0.GetY();
    const double dz2 = to0.GetZ();

    SetR3Transform(
      txx, txy, txz, dx2 - txx*dx1 - txy*dy1 - txz*dz1,
      tyx, tyy, tyz, dy2 - tyx*dx1 - tyy*dy1 - tyz*dz1,
      tzx, tzy, tzz, dz2 - tzx*dx1 - tzy*dy1 - tzz*dz1);
  }
}

R3Transform
R3Transform::GetInverse()
  const
{
  double detxx = yy_*zz_ - yz_*zy_;
  double detxy = yx_*zz_ - yz_*zx_;
  double detxz = yx_*zy_ - yy_*zx_;
  double det   = xx_*detxx - xy_*detxy + xz_*detxz;

  if (det == 0) {
    log_error("R3Transform::inverse error: zero determinant");
    return R3Transform();
  }

  det = 1./det;
  detxx *= det;
  detxy *= det;
  detxz *= det;
  double detyx = (xy_*zz_ - xz_*zy_) * det;
  double detyy = (xx_*zz_ - xz_*zx_) * det;
  double detyz = (xx_*zy_ - xy_*zx_) * det;
  double detzx = (xy_*yz_ - xz_*yy_) * det;
  double detzy = (xx_*yz_ - xz_*yx_) * det;
  double detzz = (xx_*yy_ - xy_*yx_) * det;

  return R3Transform(
     detxx, -detyx,  detzx, -detxx*dx_ + detyx*dy_ - detzx*dz_,
    -detxy,  detyy, -detzy,  detxy*dx_ - detyy*dy_ + detzy*dz_,
     detxz, -detyz,  detzz, -detxz*dx_ + detyz*dy_ - detzz*dz_);
}

R3Transform&
R3Transform::Invert()
{
  double detxx = yy_*zz_ - yz_*zy_;
  double detxy = yx_*zz_ - yz_*zx_;
  double detxz = yx_*zy_ - yy_*zx_;
  double det   = xx_*detxx - xy_*detxy + xz_*detxz;

  if (det == 0) {
    log_error("R3Transform inversion error: zero determinant");
    return *this;
  }

  det = 1./det;
  detxx *= det;
  detxy *= det;
  detxz *= det;
  double detyx = (xy_*zz_ - xz_*zy_) * det;
  double detyy = (xx_*zz_ - xz_*zx_) * det;
  double detyz = (xx_*zy_ - xy_*zx_) * det;
  double detzx = (xy_*yz_ - xz_*yy_) * det;
  double detzy = (xx_*yz_ - xz_*yx_) * det;
  double detzz = (xx_*yy_ - xy_*yx_) * det;
  double dx = dx_;
  double dy = dy_;
  double dz = dz_;

  xx_ = detxx;
  xy_ = -detyx;
  xz_ = detzx;
  dx_ = -detxx*dx + detyx*dy - detzx*dz;
  yx_ = -detxy;
  yy_ = detyy;
  yz_ = -detzy;
  dy_ = detxy*dx - detyy*dy + detzy*dz;
  zx_ = detxz;
  zy_ = -detyz;
  zz_ = detzz;
  dz_ = -detxz*dx_ + detyz*dy_ - detzz*dz_;

  return *this;
}

R3Transform
R3Transform::operator*(const R3Transform& t)
  const
{
  return R3Transform(
    xx_*t.xx_ + xy_*t.yx_ + xz_*t.zx_, xx_*t.xy_ + xy_*t.yy_ + xz_*t.zy_,
    xx_*t.xz_ + xy_*t.yz_ + xz_*t.zz_, xx_*t.dx_ + xy_*t.dy_ + xz_*t.dz_ + dx_,

    yx_*t.xx_ + yy_*t.yx_ + yz_*t.zx_, yx_*t.xy_ + yy_*t.yy_ + yz_*t.zy_,
    yx_*t.xz_ + yy_*t.yz_ + yz_*t.zz_, yx_*t.dx_ + yy_*t.dy_ + yz_*t.dz_ + dy_,

    zx_*t.xx_ + zy_*t.yx_ + zz_*t.zx_, zx_*t.xy_ + zy_*t.yy_ + zz_*t.zy_,
    zx_*t.xz_ + zy_*t.yz_ + zz_*t.zz_, zx_*t.dx_ + zy_*t.dy_ + zz_*t.dz_ + dz_);
}

void
R3Transform::GetDecomposition(Scale& scale, Rotate& rotate, Translate& translate)
  const
{
  double sx = sqrt(xx_*xx_ + yx_*yx_ + zx_*zx_);
  double sy = sqrt(xy_*xy_ + yy_*yy_ + zy_*zy_);
  double sz = sqrt(xz_*xz_ + yz_*yz_ + zz_*zz_);

  if (xx_*(yy_*zz_-yz_*zy_) - xy_*(yx_*zz_-yz_*zx_) + xz_*(yx_*zy_-yy_*zx_) < 0)
    sz = -sz;

  scale.SetR3Transform(sx, 0,  0,  0,
                     0, sy,  0,  0,
                     0,  0, sz,  0);

  rotate.SetR3Transform(xx_/sx, xy_/sy, xz_/sz, 0,
                      yx_/sx, yy_/sy, yz_/sz, 0,
                      zx_/sx, zy_/sy, zz_/sz, 0);

  translate.SetR3Transform(1, 0, 0, dx_,
                         0, 1, 0, dy_,
                         0, 0, 1, dz_);
}

bool
R3Transform::operator==(const R3Transform& t)
  const
{
  return (this == &t) ? true :
    (xx_==t.xx_ && xy_==t.xy_ && xz_==t.xz_ && dx_==t.dx_ &&
     yx_==t.yx_ && yy_==t.yy_ && yz_==t.yz_ && dy_==t.dy_ &&
     zx_==t.zx_ && zy_==t.zy_ && zz_==t.zz_ && dz_==t.dz_);
}

bool
R3Transform::operator!=(const R3Transform& t)
  const
{
  return !(*this == t);
}

void
R3Transform::SetR3Transform(
  const double xx, const double xy, const double xz, const double dx,
  const double yx, const double yy, const double yz, const double dy,
  const double zx, const double zy, const double zz, const double dz)
{
  xx_ = xx;  xy_ = xy;  xz_ = xz;  dx_ = dx;
  yx_ = yx;  yy_ = yy;  yz_ = yz;  dy_ = dy;
  zx_ = zx;  zy_ = zy;  zz_ = zz;  dz_ = dz;
}

ostream&
operator<<(ostream& os, const R3Transform& t)
{
  size_t wt = os.precision() + 7;
  os << setfill(' ')
     << setw(wt) << t.xx_ << setw(wt) << t.xy_ << setw(wt) << t.xz_
     << setw(wt) << t.dx_ << '\n'
     << setw(wt) << t.yx_ << setw(wt) << t.yy_ << setw(wt) << t.yz_
     << setw(wt) << t.dy_ << '\n'
     << setw(wt) << t.zx_ << setw(wt) << t.zy_ << setw(wt) << t.zz_
     << setw(wt) << t.dz_;

  return os;
}

Rotate::Rotate(const double a, const Point& p1, const Point& p2)
{
  if (a == 0)
    return;

  double cx = p2.GetX() - p1.GetX();
  double cy = p2.GetY() - p1.GetY();
  double cz = p2.GetZ() - p1.GetZ();

  const double ll = sqrt(cx*cx + cy*cy + cz*cz);
  if (ll > 0) {
    const double cosa = cos(a);
    const double sina = sin(a);
    cx /= ll;
    cy /= ll;
    cz /= ll;

    const double txx = cosa + (1 - cosa)*cx*cx;
    const double txy =        (1 - cosa)*cx*cy - sina*cz;
    const double txz =        (1 - cosa)*cx*cz + sina*cy;

    const double tyx =        (1 - cosa)*cy*cx + sina*cz;
    const double tyy = cosa + (1 - cosa)*cy*cy;
    const double tyz =        (1 - cosa)*cy*cz - sina*cx;

    const double tzx =        (1 - cosa)*cz*cx - sina*cy;
    const double tzy =        (1 - cosa)*cz*cy + sina*cx;
    const double tzz = cosa + (1 - cosa)*cz*cz;

    const double tdx = p1.GetX();
    const double tdy = p1.GetY();
    const double tdz = p1.GetZ();

    SetR3Transform(
      txx, txy, txz, tdx - txx*tdx - txy*tdy - txz*tdz,
      tyx, tyy, tyz, tdy - tyx*tdx - tyy*tdy - tyz*tdz,
      tzx, tzy, tzz, tdz - tzx*tdx - tzy*tdy - tzz*tdz);
  }
}

Rotate::Rotate(const double a, const Vector& v)
{
  *this = Rotate(a, Point(0, 0, 0), Point(v.GetX(), v.GetY(), v.GetZ()));
}

Rotate::Rotate(const Point& fr1, const Point& fr2,
               const Point& to1, const Point& to2)
  : R3Transform(Point(0, 0, 0), fr1, fr2, Point(0, 0, 0), to1, to2)
{
}

