/*!
 * @file SFBeta.cc
 * @brief Implementation of various forms of the beta function.
 * @author Segev BenZvi
 * @date 17 Apr 2012
 * @version $Id: SFBeta.cc 13185 2012-11-10 15:46:03Z sybenzvi $
 */

#include <hawcnest/Logging.h>
#include <data-structures/math/SpecialFunctions.h>

#ifdef HAVE_GSL
#include <gsl/gsl_sf_gamma.h>
#else
#include <boost/math/special_functions/beta.hpp>
#endif // HAVE_GSL

#include <cmath>

using namespace std;

namespace {
  
  bool
  isNegativeInteger(const double x)
  {
    if (x <= 0.)
      return trunc(x) == x;
    return false;
  }

}

namespace SpecialFunctions {

  // The Beta function
  double
  Beta::B(const double a, const double b)
  {
    // If a, b are integers <= 0 we run into the poles of the gamma function
    if (isNegativeInteger(a) || isNegativeInteger(b))
      log_fatal("domain error: a and b cannot be negative integers.");

    #ifdef HAVE_GSL
    return gsl_sf_beta(a, b);
    #else
    using namespace boost::math;
    if (a > 0. && b > 0.)
      return beta(a, b);
    else
      return std::exp(lgamma(a) + lgamma(b) - lgamma(a + b));
    #endif
  }

  // The natural logarithm of the Beta function
  double
  Beta::lnB(const double a, const double b)
  {
    // If a, b are integers <= 0 we run into the poles of the gamma function
    if (isNegativeInteger(a) || isNegativeInteger(b))
      log_fatal("domain error: a and b cannot be negative integers.");

    #ifdef HAVE_GSL
    return gsl_sf_lnbeta(a, b);
    #else
    using namespace boost::math;
    return lgamma(a) + lgamma(b) - lgamma(a + b);
    #endif
  }

  // The normalized incomplete Beta function
  double
  Beta::I(const double a, const double b, const double x)
  {
    if (a <= 0. || b <= 0.)
      log_fatal("domain error: a, b cannot be negative or zero.");

    if (x < 0. || x > 1.)
      log_fatal("domain error: x not in [0,1].")

    #ifdef HAVE_GSL
    return gsl_sf_beta_inc(a, b, x);
    #else
    using namespace boost::math;
    return ibeta(a, b, x);
    #endif
  }

}

