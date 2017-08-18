/*!
 * @file Math.h
 * @brief Some convenient functions for use inside many different programs
 * @author Segev BenZvi
 * @date 26 Jan 2013
 * @version $Id: Math.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_MATH_MATH_H_INCLUDED
#define DATACLASSES_MATH_MATH_H_INCLUDED

#include <boost/type_traits/integral_constant.hpp>

#include <cmath>
#include <limits>

/*!
 * @namespace Math
 * @author Segev BenZvi
 * @date 26 Jan 2013
 * @ingroup math
 * @brief Convenient simple math functions not defined in standard libraries
 */
namespace Math {

  /// Square a value
  template<typename T>
  inline T sqr(const T& x)
  { return x*x; }

  template<typename T>
  inline int sign(const T& x, const boost::integral_constant<bool, false>)
  { return T(0) < x; }

  template<typename T>
  inline int sign(const T& x, const boost::integral_constant<bool, true>)
  { return (T(0) < x) - (x < T(0)); }

  /// Sign function: +1 if x > 0, -1 if x < 0.
  template<typename T>
  inline int sign(const T& x)
  { return sign(x, boost::integral_constant<bool, std::numeric_limits<T>::is_signed>()); }

  /// Solve the quadratic formula ax^2 + bx + c = 0 for the two roots x0, x1
  template<typename T>
  bool solveQuadratic(const T& a, const T& b, const T& c, T& x0, T& x1)
  {
    T discr = b*b - 4*a*c;
    if (discr < 0)
      return false;
    else if (discr == 0)
      x0 = x1 = -0.5*b/a;
    else {
      T q = (b > 0) ? -0.5*(b + std::sqrt(discr)) : -0.5*(b - std::sqrt(discr));
      x0 = q/a;
      x1 = c/q;
    }
    if (x0 > x1)
      std::swap(x0, x1);
    return true;
  }

}

#endif // DATACLASSES_MATH_MATH_H_INCLUDED

