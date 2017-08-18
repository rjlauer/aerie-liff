/*!
 * @file GQIntegrator.h
 * @brief Implementation of Gauss quadrature integration.
 * @author Segev BenZvi
 * @version $Id: GQIntegrator.h 13144 2012-11-07 17:22:17Z sybenzvi $
 */

#ifndef DATACLASSES_MATH_GQINTEGRATOR_H_INCLUDED
#define DATACLASSES_MATH_GQINTEGRATOR_H_INCLUDED

#include <data-structures/math/Integrator.h>

/*!
 * @class GQIntegrator
 * @brief Integration of a function using adaptive Gaussian quadrature
 * @author Segev BenZvi
 * @date 22 Apr 2012
 * @tparam Functor Function object representing the function to integrate
 * @ingroup math
 *
 * Integration by quadrature evaluates an integral via the approximation
 * \f[
 *   \int_a^b f(x)dx \approx \sum_{i=0}^n w_i f(x_i)
 * \f]
 * where the nodes \f$x_i\f$ and weights \f$w_i\f$ are pre-computed and
 * optimized for the type of integrand.  Gaussian quadrature will produce
 * accurate results if \f$f(x)\f$ is well-approximated by a polynomial
 * function, e.g.,
 * \f[
 *   f(x) = W(x) g(x)
 * \f]
 * where \f$g(x)\f$ is approximately polynomial and \f$W(x)\f$ are combinations
 * of orthogonal functions such as the Legendre polynomials.  The method works
 * best for functions that are relatively smooth, and will fail if \f$f(x)\f$
 * has a singularity.
 *
 * For more details see <a href="http://apps.nrbook.com/c/index.html">Numerical
 * Recipes in C</a>, W. Press et al. (1992), Chapter 4.5.
 */
template<class Functor>
class GQIntegrator : public Integrator<Functor> {

  public:

    GQIntegrator(Functor& f) : Integrator<Functor>(f) { }

    double
    Integrate(const double a, const double b, const double tol = 1e-12) const;

};

#include <data-structures/math/GQIntegrator-inl.h>

#endif // DATACLASSES_MATH_GQINTEGRATOR_H_INCLUDED

