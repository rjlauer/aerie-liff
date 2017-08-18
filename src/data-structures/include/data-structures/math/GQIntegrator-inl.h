/*!
 * @file GQIntegrator-inl.h
 * @brief Implementation of Gauss quadrature integration.
 * @author Segev BenZvi
 * @version $Id: GQIntegrator-inl.h 13144 2012-11-07 17:22:17Z sybenzvi $
 */

#ifndef DATACLASSES_MATH_GQINTEGRATOR_INL_H_INCLUDED
#define DATACLASSES_MATH_GQINTEGRATOR_INL_H_INCLUDED

#include <cmath>

template<class Functor>
double
GQIntegrator<Functor>::Integrate(
  const double a, const double b, const double tol)
  const
{
  // This is a 12-point quadruature based on the DGAUSS routine of CERNLIB.  It
  // was converted to C++ by Rene Brun for the ROOT MathCore library.  Ideally
  // we could have just wrapped the call to the ROOT gaussian quadrature
  // class, but it was full of ROOT stuff that made the bare function call
  // hard to use.
  static const double w[12] = {
    0.101228536290376, 0.222381034453374, 0.313706645877887,
    0.362683783378362, 0.027152459411754, 0.062253523938648,
    0.095158511682493, 0.124628971255534, 0.149595988816577,
    0.169156519395003, 0.182603415044924, 0.189450610455069
  };
  static const double x[12] = {
    0.960289856497536, 0.796666477413627, 0.525532409916329,
    0.183434642495650, 0.989400934991650, 0.944575023073233,
    0.865631202387832, 0.755404408355003, 0.617876244402644,
    0.458016777657227, 0.281603550779259, 0.095012509837637
  };

  static const double kHF = 0.5;
  static const double kCST = 5./1000;

  const Functor& f = this->functor_;

  double h, aconst, bb, aa, c1, c2, u, s8, s16, f1, f2;
  double xx;
  int i;

  h = 0;
  if (b == a)
    return h;
  aconst = kCST/std::abs(b - a);
  bb = a;
CASE1:
  aa = bb;
  bb = b;
CASE2:
  c1 = kHF*(bb + aa);
  c2 = kHF*(bb - aa);
  s8 = 0;
  for (i = 0; i < 4; ++i) {
    u  = c2*x[i];
    xx = c1 + u;
    f1 = f(xx);
    xx = c1-u;
    f2 = f(xx);
    s8 += w[i]*(f1 + f2);
  }
  s16 = 0;
  for (i = 4; i < 12; ++i) {
    u  = c2*x[i];
    xx = c1+u;
    f1 = f(xx);
    xx = c1-u;
    f2 = f(xx);
    s16  += w[i]*(f1 + f2);
  }
  s16 = c2*s16;
  if (std::abs(s16 - c2*s8) <= tol*(1. + std::abs(s16))) {
    h += s16;
    if (bb != b)
      goto CASE1;
  } else {
    bb = c1;
    if (1. + aconst*std::abs(c2) != 1)
      goto CASE2;
    h = s8;  //this is a crude approximation (cernlib function returned 0 !)
  }

  return h;
}

#endif // DATACLASSES_MATH_GQINTEGRATOR_INL_H_INCLUDED

