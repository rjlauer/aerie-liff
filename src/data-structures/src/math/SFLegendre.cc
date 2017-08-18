/*!
 * @file SFLegendre.cc
 * @brief Implementation of various forms of the Legendre polynomials.
 * @author Segev BenZvi
 * @date 16 Aug 2013
 * @version $Id: SFLegendre.cc 19729 2014-05-01 16:12:26Z sybenzvi $
 */

#include <hawcnest/Logging.h>
#include <data-structures/math/SpecialFunctions.h>

#ifdef HAVE_GSL
#include <gsl/gsl_sf_legendre.h>
#else
#include <boost/math/special_functions/legendre.hpp>
#include <boost/math/special_functions/spherical_harmonic.hpp>
#endif // HAVE_GSL

#include <cmath>

using namespace std;

namespace SpecialFunctions {

  double
  Legendre::P(const int l, const double x)
  {
    #ifdef HAVE_GSL
      return gsl_sf_legendre_Pl(l, x);
    #else
      return boost::math::legendre_p(l, x);
    #endif
  }

  complex<double>
  Legendre::Y(const int l, const int m, const double theta, const double phi)
  {
    #ifdef HAVE_GSL
      const int mm = abs(m);
      const double plm = gsl_sf_legendre_sphPlm(l, mm, cos(theta));
      double re, im;

      if (m >= 0) {
        re = plm * cos(mm*phi);
        im = plm * sin(mm*phi);
      }
      else {
        const double fac = ((mm % 2) == 0 ? 1. : -1.);
        re =  fac * plm * cos(mm*phi);
        im = -fac * plm * sin(mm*phi);
      }

      return complex<double>(re, im);
    #else
      return boost::math::spherical_harmonic(l, m, theta, phi);
    #endif
  }

  double
  Legendre::ReY(const int l, const int m, const double theta, const double phi)
  {
    #ifdef HAVE_GSL
      if (m >= 0)
        return gsl_sf_legendre_sphPlm(l, m, cos(theta)) * cos(m*phi);
      else {
        int mm = abs(m);
        return gsl_sf_legendre_sphPlm(l, mm, cos(theta)) * sin(mm*phi);
      }
    #else
      return boost::math::spherical_harmonic_r(l, m, theta, phi);
    #endif
  }

}

