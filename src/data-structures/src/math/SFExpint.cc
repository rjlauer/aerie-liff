/*!
 * @file SFExpint.cc
 * @brief Implementation of forms of the exponential integral.
 * @author Segev BenZvi
 * @date 17 Apr 2012
 * @version $Id: SFExpint.cc 13185 2012-11-10 15:46:03Z sybenzvi $
 */

#include <hawcnest/Logging.h>
#include <data-structures/math/SpecialFunctions.h>

#ifdef HAVE_GSL
#include <gsl/gsl_sf_expint.h>
#else
#include <boost/math/special_functions/expint.hpp>
#endif

namespace SpecialFunctions {

  // The exponential integral
  double
  Expint::En(const int n, const double x)
  {
    if (n < 0)
      log_fatal("domain error: n < 0");

    if (x <= 0.)
      log_fatal("domain error: x <= 0");

    #ifdef HAVE_GSL
    return gsl_sf_expint_En(n, x);
    #else
    return boost::math::expint(n, x);
    #endif
  }

  // The principal value of the exponential integral
  double
  Expint::Ei(const double x)
  {
    if (x <= 0.)
      log_fatal("domain error: x <= 0");

    #ifdef HAVE_GSL
    return gsl_sf_expint_Ei(x);
    #else
    return boost::math::expint(x);
    #endif
  }

}

