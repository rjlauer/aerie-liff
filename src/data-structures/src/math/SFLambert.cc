/*!
 * @file SFLambert.cc
 * @brief Implementation of the Lambert W function (real-valued).
 * @author Segev BenZvi
 * @date 10 Nov 2012
 * @version $Id: SFLambert.cc 13184 2012-11-10 15:45:25Z sybenzvi $
 */

#include <hawcnest/Logging.h>
#include <data-structures/math/SpecialFunctions.h>

#ifdef HAVE_GSL
//
#include <gsl/gsl_sf_lambert.h>
//
#else
//
#include <cmath>
#include <limits>

using namespace std;
//
#endif

namespace SpecialFunctions {

#ifndef HAVE_GSL
/*!
 * @namespace LambertWDetail
 * @author Darko Veberic
 * @date 25 Jun 2009
 * @brief Implementation of the Lambert W function, for use when GSL is not
 *        available
 */
namespace LambertWDetail {

  const double kInvE = 1/M_E;

  template<int n>
  inline double BranchPointPolynomial(const double p);

  template<>
  inline
  double
  BranchPointPolynomial<1>(const double p)
  {
    return
      -1 + p;
  }

  template<>
  inline
  double
  BranchPointPolynomial<2>(const double p)
  {
    return
      -1 + p*(1 + p*(-1./3));
  }

  template<>
  inline
  double
  BranchPointPolynomial<3>(const double p)
  {
    return
      -1 + p*(1 + p*(-1./3 + p*(11./72)));
  }

  template<>
  inline
  double
  BranchPointPolynomial<4>(const double p)
  {
    return
      -1 + p*(1 + p*(-1./3 + p*(11./72 + p*(-43./540))));
  }

  template<>
  inline
  double
  BranchPointPolynomial<5>(const double p)
  {
    return
      -1 + p*(1 + p*(-1./3 + p*(11./72 + p*(-43./540 + p*(769./17280)))));
  }

  template<>
  inline
  double
  BranchPointPolynomial<6>(const double p)
  {
    return
      -1 + p*(1 + p*(-1./3 + p*(11./72 + p*(-43./540 + p*(769./17280
      + p*(-221./8505))))));
  }

  template<>
  inline
  double
  BranchPointPolynomial<7>(const double p)
  {
    return
      -1 + p*(1 + p*(-1./3 + p*(11./72 + p*(-43./540 + p*(769./17280
      + p*(-221./8505 + p*(680863./43545600)))))));
  }

  template<>
  inline
  double
  BranchPointPolynomial<8>(const double p)
  {
    return
      -1 + p*(1 + p*(-1./3 + p*(11./72 + p*(-43./540 + p*(769./17280
      + p*(-221./8505 + p*(680863./43545600 + p*(-1963./204120))))))));
  }

  template<>
  inline
  double
  BranchPointPolynomial<9>(const double p)
  {
    return
      -1 + p*(1 + p*(-1./3 + p*(11./72 + p*(-43./540 + p*(769./17280
      + p*(-221./8505 + p*(680863./43545600 + p*(-1963./204120
      + p*(226287557./37623398400.)))))))));
  }

  template<int order>
  inline double AsymptoticExpansion(const double a, const double b);

  template<>
  inline
  double
  AsymptoticExpansion<0>(const double a, const double b)
  {
    return a - b;
  }

  template<>
  inline
  double
  AsymptoticExpansion<1>(const double a, const double b)
  {
    return a - b + b / a;
  }

  template<>
  inline
  double
  AsymptoticExpansion<2>(const double a, const double b)
  {
    const double ia = 1 / a;
    return a - b + b / a * (1 + ia * 0.5*(-2 + b));
  }

  template<>
  inline
  double
  AsymptoticExpansion<3>(const double a, const double b)
  {
    const double ia = 1 / a;
    return a - b + b / a *
      (1 + ia *
        (0.5*(-2 + b) + ia *
           1/6.*(6 + b*(-9 + b*2))));
  }

  template<>
  inline
  double
  AsymptoticExpansion<4>(const double a, const double b)
  {
    const double ia = 1 / a;
    return a - b + b / a *
      (1 + ia *
        (0.5*(-2 + b) + ia *
          (1/6.*(6 + b*(-9 + b*2)) + ia *
            1/12.*(-12 + b*(36 + b*(-22 + b*3))))));
  }

  template<>
  inline
  double
  AsymptoticExpansion<5>(const double a, const double b)
  {
    const double ia = 1 / a;
    return a - b + b / a *
      (1 + ia *
        (0.5*(-2 + b) + ia *
          (1/6.*(6 + b*(-9 + b*2)) + ia *
            (1/12.*(-12 + b*(36 + b*(-22 + b*3))) + ia *
              1/60.*(60 + b*(-300 + b*(350 + b*(-125 + b*12))))))));
  }

  template<int branch>
  class Branch {

  public:
    template<int order>
    static double BranchPointExpansion(const double x)
    { return BranchPointPolynomial<order>(eSign * sqrt(2*(M_E*x+1))); }

    // Asymptotic expansion
    // Corless et al. 1996, de Bruijn (1981)
    template<int order>
    static
    double
    AsymptoticExpansion(const double x)
    {
      const double logsx = log(eSign * x);
      const double logslogsx = log(eSign * logsx);
      return LambertWDetail::AsymptoticExpansion<order>(logsx, logslogsx);
    }

    template<int n>
    static inline double RationalApproximation(const double x);

    // Logarithmic recursion
    template<int order>
    static inline double LogRecursion(const double x)
    { return LogRecursionStep<order>(log(eSign * x)); }

    // generic approximation valid to at least 5 decimal places
    static inline double Approximation(const double x);

  private:
    // enum { eSign = 2*branch + 1 }; this doesn't work on gcc 3.3.3
    static const int eSign = 2*branch + 1;

    template<int n>
    static inline double LogRecursionStep(const double logsx)
    { return logsx - log(eSign * LogRecursionStep<n-1>(logsx)); }
  };

  // Rational approximations

  template<>
  template<>
  inline
  double
  Branch<0>::RationalApproximation<0>(const double x)
  {
    return x*(60 + x*(114 + 17*x)) / (60 + x*(174 + 101*x));
  }

  template<>
  template<>
  inline
  double
  Branch<0>::RationalApproximation<1>(const double x)
  {
    // branch 0, valid for [-0.31,0.3]
    return
      x * (1 + x *
        (5.931375839364438 + x *
          (11.392205505329132 + x *
            (7.338883399111118 + x*0.6534490169919599)))) /
      (1 + x *
        (6.931373689597704 + x *
          (16.82349461388016 + x *
            (16.43072324143226 + x*5.115235195211697))));
  }

  template<>
  template<>
  inline
  double
  Branch<0>::RationalApproximation<2>(const double x)
  {
    // branch 0, valid for [-0.31,0.5]
    return
      x * (1 + x *
        (4.790423028527326 + x *
          (6.695945075293267 + x * 2.4243096805908033))) /
      (1 + x *
        (5.790432723810737 + x *
          (10.986445930034288 + x *
            (7.391303898769326 + x * 1.1414723648617864))));
  }

  template<>
  template<>
  inline
  double
  Branch<0>::RationalApproximation<3>(const double x)
  {
    // branch 0, valid for [0.3,7]
    return
      x * (1 + x *
        (2.4450530707265568 + x *
          (1.3436642259582265 + x *
            (0.14844005539759195 + x * 0.0008047501729129999)))) /
      (1 + x *
        (3.4447089864860025 + x *
          (3.2924898573719523 + x *
            (0.9164600188031222 + x * 0.05306864044833221))));
  }

  template<>
  template<>
  inline
  double
  Branch<-1>::RationalApproximation<4>(const double x)
  {
    // branch -1, valid for [-0.3,-0.05]
    return
      (-7.814176723907436 + x *
        (253.88810188892484 + x * 657.9493176902304)) /
      (1 + x *
        (-60.43958713690808 + x *
          (99.98567083107612 + x *
            (682.6073999909428 + x *
              (962.1784396969866 + x * 1477.9341280760887)))));
  }

  // stopping conditions

  template<>
  template<>
  inline
  double
  Branch<0>::LogRecursionStep<0>(const double logsx)
  {
    return logsx;
  }

  template<>
  template<>
  inline
  double
  Branch<-1>::LogRecursionStep<0>(const double logsx)
  {
    return logsx;
  }

  template<>
  inline
  double
  Branch<0>::Approximation(const double x)
  {
    if (x < -0.32358170806015724) {
      if (x < -kInvE)
        return numeric_limits<double>::quiet_NaN();
      else if (x < -kInvE+1e-5)
        return BranchPointExpansion<5>(x);
      else
        return BranchPointExpansion<9>(x);
    } else {
      if (x < 0.14546954290661823)
        return RationalApproximation<1>(x);
      else if (x < 8.706658967856612)
        return RationalApproximation<3>(x);
      else
        return AsymptoticExpansion<5>(x);
    }
  }

  template<>
  inline
  double
  Branch<-1>::Approximation(const double x)
  {
    if (x < -0.051012917658221676) {
      if (x < -kInvE+1e-5) {
        if (x < -kInvE)
          return numeric_limits<double>::quiet_NaN();
        else
          return BranchPointExpansion<5>(x);
      } else {
        if (x < -0.30298541769)
          return BranchPointExpansion<9>(x);
        else
          return RationalApproximation<4>(x);
      }
    } else {
      if (x < 0)
        return LogRecursion<9>(x);
      else if (x == 0)
        return -numeric_limits<double>::infinity();
      else
        return numeric_limits<double>::quiet_NaN();
    }
  }

  // iterations

  inline
  double
  HalleyStep(const double x, const double w)
  {
    const double ew = exp(w);
    const double wew = w * ew;
    const double wewx = wew - x;
    const double w1 = w + 1;
    return w - wewx / (ew * w1 - (w + 2) * wewx/(2*w1));
  }

  inline
  double
  FritschStep(const double x, const double w)
  {
    const double z = log(x/w) - w;
    const double w1 = w + 1;
    const double q = 2 * w1 * (w1 + (2/3.)*z);
    const double eps = z / w1 * (q - z) / (q - 2*z);
    return w * (1 + eps);
  }

  template<double IterationStep(const double x, const double w)>
  inline
  double
  Iterate(const double x, double w, const double eps = 1e-6)
  {
    for (int i = 0; i < 100; ++i) {
      const double ww = IterationStep(x, w);
      if (fabs(ww - w) <= eps)
        return ww;
      w = ww;
    }
    cerr << "convergence not reached." << endl;
    return w;
  }

  template<double IterationStep(const double x, const double w)>
  struct Iterator {

    static
    double
    Do(const int n, const double x, const double w)
    {
      for (int i = 0; i < n; ++i)
        w = IterationStep(x, w);
      return w;
    }

    template<int n>
    static
    double
    Do(const double x, const double w)
    {
      for (int i = 0; i < n; ++i)
        w = IterationStep(x, w);
      return w;
    }

    template<int n, class = void>
    struct Depth {
      static double Recurse(const double x, double w)
      { return Depth<n-1>::Recurse(x, IterationStep(x, w)); }
    };

    // stop condition
    template<class T>
    struct Depth<1, T> {
      static double Recurse(const double x, const double w)
      { return IterationStep(x, w); }
    };

    // identity
    template<class T>
    struct Depth<0, T> {
      static double Recurse(const double x, const double w)
      { return w; }
    };

  };

} // namespace LambertWDetail

template<int branch>
double
LambertWApproximation(const double x)
{
  return LambertWDetail::Branch<branch>::Approximation(x);
}

// instantiations
template double LambertWApproximation<0>(const double x);
template double LambertWApproximation<-1>(const double x);

#endif

namespace Lambert {

  // The Lambert W function, principal real-valued branch
  template<>
  double
  W<0>(const double x)
  {
    #ifdef HAVE_GSL
    return gsl_sf_lambert_W0(x);
    #else
    if (fabs(x) > 1e-6 && x > -LambertWDetail::kInvE + 1e-5)
      return
        LambertWDetail::
          Iterator<LambertWDetail::FritschStep>::
            Depth<1>::
              Recurse(x, LambertWApproximation<0>(x));
    else
      return LambertWApproximation<0>(x);
    #endif
  }

  // The Lambert W function, secondary real-valued branch
  template<>
  double
  W<-1>(const double x)
  {
    #ifdef HAVE_GSL
    return gsl_sf_lambert_Wm1(x);
    #else
    if (x > -LambertWDetail::kInvE + 1e-5)
      return
        LambertWDetail::
          Iterator<LambertWDetail::FritschStep>::
            Depth<1>::
              Recurse(x, LambertWApproximation<-1>(x));
    else
      return LambertWApproximation<-1>(x);
    #endif
  }

  template double W<0>(const double x);
  template double W<-1>(const double x);

} // namespace Lambert

} // namespace SpecialFunctions

