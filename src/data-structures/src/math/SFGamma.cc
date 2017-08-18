/*!
 * @file SFGamma.cc
 * @brief Implementation of various forms of the Gamma function.
 * @author Segev BenZvi
 * @date 17 Apr 2012
 * @version $Id: SFGamma.cc 26761 2015-08-25 20:53:58Z tweisgarber $
 */

#include <hawcnest/Logging.h>
#include <data-structures/math/SpecialFunctions.h>

#ifdef HAVE_GSL
//
#include <gsl/gsl_sf_erf.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_sf_psi.h>
#include <gsl/gsl_cdf.h>
//
#else
//
#include <boost/math/special_functions/gamma.hpp>
#include <boost/math/special_functions/expint.hpp>
#include <boost/math/special_functions/digamma.hpp>
#include <boost/math/special_functions/erf.hpp>
#include <cmath>
using namespace std;
//
#endif // HAVE_GSL

namespace SpecialFunctions {

  // The complete gamma function
  double
  Gamma::G(const double a)
  {
    if (a <= 0.)
      log_fatal("domain error: a = " << a << " <= 0");

    #ifdef HAVE_GSL
    return gsl_sf_gamma(a);
    #else
    return boost::math::tgamma(a);
    #endif
  }

  // The natural logarithm of the gamma function
  double
  Gamma::lnG(const double a)
  {
    if (a <= 0.)
      log_fatal("domain error: a <= 0");

    #ifdef HAVE_GSL
    return gsl_sf_lngamma(a);
    #else
    return boost::math::lgamma(a);
    #endif
  }

  // The lower incomplete gamma function
  double
  Gamma::g(const double a, const double x)
  {
    if (a <= 0. || x < 0.)
      log_fatal("domain error: a <= 0 || x < 0");
    return Gamma::G(a) * Gamma::P(a, x);
  }

  // The regularized lower incomplete gamma function
  double
  Gamma::P(const double a, const double x)
  {
    if (a <= 0. || x < 0.)
      log_fatal("domain error: a <= 0 || x < 0");

    #ifdef HAVE_GSL
    return gsl_sf_gamma_inc_P(a, x);
    #else
    return boost::math::gamma_p(a, x);
    #endif
  }

  // The upper incomplete gamma function
  double
  Gamma::G(const double a, const double x)
  {
    // Note: a < 0 allowed as long as x >= 0
    if (x < 0.)
      log_fatal("domain error: x < 0");

    #ifdef HAVE_GSL
    return gsl_sf_gamma_inc(a, x);
    #else
    if (a > 0.) {
      return boost::math::tgamma(a, x);
    }
    else if (a == 0.) {
      return boost::math::expint(1, x);
    }
    else {
      // Use the recurrence relation G(a+1,x) = a*G(a,x) + x^a*exp(-x)
      // to calculate the upper imcomplete gamma function for a < 0
      const double da = a - floor(a);
      double alpha = da;
      double ga = (da > 0. ? boost::math::tgamma(da, x)
                           : boost::math::expint(1, x));
      do {
        const double shift = exp(-x + (alpha -1.)*log(x));
        ga = (ga - shift) / (alpha - 1.);
        alpha -= 1.;
      } while (alpha > a);
      return ga;
    }
    #endif
  }

  // The regularized upper incomplete gamma function
  double
  Gamma::Q(const double a, const double x)
  {
    // Note: a < 0 not allowed here due to pole in the Gamma function
    if (a <= 0. || x < 0.)
      log_fatal("domain error: a <= 0 || x < 0");

    #ifdef HAVE_GSL
    return gsl_sf_gamma_inc_Q(a, x);
    #else
    return boost::math::gamma_q(a, x);
    #endif
  }

  // The error function
  double
  Gamma::Erf(const double x)
  {
    #ifdef HAVE_GSL
    return gsl_sf_erf(x);
    #else
    return boost::math::erf(x);
    #endif
  }

  // The complementary error function
  double
  Gamma::Erfc(const double x)
  {
    #ifdef HAVE_GSL
    return gsl_sf_erfc(x);
    #else
    return boost::math::erfc(x);
    #endif
  }

  // The inverse error function
  double
  Gamma::ErfInverse(const double x)
  {
    #ifdef HAVE_GSL
    //return gsl_sf_erfc(x);
    return gsl_cdf_ugaussian_Pinv(0.5*(1.0+x));
    #else
    return boost::math::erf_inv(x);
    #endif
  }

  // The digamma function
  double
  Gamma::DG(const double x)
  {
    #ifdef HAVE_GSL
    return gsl_sf_psi(x);
    #else
    return boost::math::digamma(x);
    #endif
  } 

}

