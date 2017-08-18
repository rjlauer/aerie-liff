/*!
 * @file SpecialFunctions.h
 * @brief Special functions that come up often during integration.
 * @author Segev BenZvi
 * @date 17 Apr 2012
 * @version $Id: SpecialFunctions.h 26761 2015-08-25 20:53:58Z tweisgarber $
 */

#ifndef DATACLASSES_MATH_SPECIALFUNCTIONS_H_INCLUDED
#define DATACLASSES_MATH_SPECIALFUNCTIONS_H_INCLUDED

#include <complex>

/*!
 * @namespace SpecialFunctions
 * @author Segev BenZvi
 * @date 17 Apr 2012
 * @ingroup math
 * @brief Special functions that cannot be built out of the standard C math
 *        library
 *
 * These are mathematical functions useful for integrals and probability
 * calculations, e.g., Gamma functions, Beta functions, etc.
 *
 * Here we provide wrappers to an external library which provides the
 * functions, e.g., ROOT or GSL.  We have chosen to wrap the library function
 * calls to keep the AERIE code independent of a particular external API.  (GSL
 * is excellent but its C interface is clunky.)  If we choose to change the
 * back-end in the future it will not break user code.
 */
namespace SpecialFunctions {

  namespace Expint {

    /// The exponential integral of order n, defined by
    /// \f[ E_n(x) = \Re \int_1^\infty \frac{e^{-xt}}{t^n}\ dt,\quad
    //               x>0,\ n=0,1,\ldots \f]
    double En(const int n, const double x);

    /// The exponential integral defined by
    /// \f[ \text{Ei}(x) = -\int_{-x}^\infty \frac{e^{-t}}{t}\ dt,\ x>0 \f]
    double Ei(const double x);

  }

  namespace Beta {

    /// The Beta function defined by
    /// \f[ B(a,b) = \Gamma(a)\Gamma(b)/\Gamma(a+b),\ (a,b>0) \f]
    double B(const double a, const double b);

    /// The natural logarithm of the Beta function
    double lnB(const double a, const double b);

    /// The normalized incomplete beta function
    /// \f[ I_x(a,b)=B_x(a,b)/B(a,b) where B_x(a,b) 
    ///     = \int_0^x t^{a-1} (1-t)^{b-1} dt\ \text{for} 0 <= x <= 1 \f]
    double I(const double a, const double b, const double x);

  }

  namespace Gamma {

    /// The complete gamma function, defined by the integral
    /// \f[ \Gamma(a) = \int_0^\infty t^{a-1}e^{-t}\ dt \f]
    double G(const double a);

    /// Natural logarithm of the complete gamma function.
    double lnG(const double a);

    /// The lower incomplete gamma function, defined by the integral
    /// \f[ \gamma(a,x) = \int_0^x t^{a-1}e^{-t}\ dt \f]
    double g(const double a, const double x);

    /// The upper incomplete gamma function, defined by the integral
    /// \f[ \Gamma(a,x) = \int_x^\infty t^{a-1}e^{-t}\ dt \f]
    double G(const double a, const double x);

    /// The regularized lower incomplete gamma function, defined by
    /// \f[ P(a,x) = \frac{\gamma(a,x)}{\Gamma(a)} \f]
    double P(const double a, const double x);

    /// The regularized upper incomplete gamma function, defined by
    /// \f[ Q(a,x) = \frac{\Gamma(a,x)}{\Gamma(a)} \f]
    double Q(const double a, const double x);

    /// The error function, a special case of the incomplete gamma function:
    /// \f[ \text{erf}(x) = P(1/2,x^2) = (2/\sqrt(\pi)) \int_0^x e^{-t^2} dt \f]
    double Erf(const double x);

    /// The complementary error function:
    /// \f[ \text{erfc}(x) = 1 - \text{erf}(x) = Q(1/2,x^2) \f]
    double Erfc(const double x);

    /// The inverse error function
    double ErfInverse(const double x);

    /// Digamma function to get derivatives of a gamma function
    /// \f[ \psi(x) = \frac{\Gamma'(x)}{\Gamma(x)} \f]
    double DG(const double a);

  }

  namespace Lambert {

    /// The Lambert W function, defined as the solution to the equation
    /// \f[ z = W(z)e^{W(z)} \f].  The solution is multi-valued with two real
    /// branches \f$W_0(x)\f$ and \f$W_{-1}(x)\f$
    template<int branch>
    double W(const double x);

  }

  namespace Legendre {

    /// The Legendre polynomial of the first kind \f$P_{l}(x)\f$ where
    /// \f$|x|\leq1\f$
    double P(const int l, const double x);

    /// The spherical harmonic function \f$Y_l^m(\theta,\varphi)\f$, where
    /// \f$\theta\in[0,\pi]\f$ is zenith and \f$\varphi\in[0,2\pi]\f$ is azimuth
    std::complex<double>
    Y(const int l, const int m, const double theta, const double phi);

    /// The real-valued spherical harmonic function \f$Y_l^m(\theta,\varphi)\f$
    /// where \f$\theta\in[0,\pi]\f$ is zenith and \f$\varphi\in[0,2\pi]\f$ is
    /// azimuth
    double ReY(const int l, const int m, const double theta, const double phi);

  }

}

#endif // DATACLASSES_MATH_SPECIALFUNCTIONS_H_INCLUDED

