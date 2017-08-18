/*!
 * @file Integrator.h
 * @brief Interfaces for integration functions.
 * @author Segev BenZvi
 * @version $Id: Integrator.h 13144 2012-11-07 17:22:17Z sybenzvi $
 */

#ifndef DATACLASSES_MATH_INTEGRATOR_H_INCLUDED
#define DATACLASSES_MATH_INTEGRATOR_H_INCLUDED

/*!
 * @class Integrator
 * @brief Abstract interface for integrating functions in one dimension
 * @author Darko Veberic 
 * @author Segev BenZvi
 * @ingroup math
 * @tparam Functor Function object representing the function to integrate
 * @date 22 Apr 2012
 *
 * This class is based on the Integrator interface written by D. Veberic for
 * Auger Offline.  However, rather than containing several internal routines
 * for calculating 1D integrals using different algorithms, it simply provides
 * an interface for integration of function objects.  Users implement the
 * algorithm they want by inheriting from this class.
 *
 * For example, suppose trapezoidal rule integration is implemented in a child
 * class called TrapIntegrator.  The example of usage is:
 * @code
 *   // Define a function through the functor class
 *   class Sin {
 *     public:
 *       Sin(const double ampl, const double freq) : A_(ampl), f_(freq) { }
 *       double operator()(const double x) const { return A_*std::sin(f_*x); }
 *     private:
 *       const double A_;
 *       const double f_;
 *   };
 *
 *   ...
 *
 *   // One-time integration using the MakeIntegrator function:
 *   Sin sinf(2., 3.15149);
 *   const double intrgl = MakeIntegrator<TrapIntegrator>(sinf).Integrate(0, 1);
 *
 *   ...
 *
 *   // Repeated use of the integrator:
 *   double result = 0.;
 *   Sin sinf(10., 3.14159);
 *   TrapIntegrator<Sin> integrator = MakeIntegrator<TrapIntegrator>(sinf);
 *   for (double d = 0.; d < 1e2; d += 3.)
 *     result += integrator.Integrate(d, d+3.);
 * @endcode
 *
 * Simple functions that do not need initialization or additional parameters
 * can be used directly without needing a functor:
 * @code
 *   double Inv(const double x) { return 1./x; }
 *
 *   // Evaluate ln(e) numerically:
 *   const double r = MakeIntegrator<TrapIntegrator>(Inv).Integral(1., 2.718);
 * @endcode
 *
 * Note that some built-in functions do not have a static binding (like log,
 * exp, etc.) and have to be wrapped in a functor or a function before they can
 * be integrated.  E.g.,
 * @code
 *   double Log(const double x) { return std::log(x); }
 * @endcode
 */
template<class Functor>
class Integrator {

  public:

    Integrator(Functor& f) : functor_(f) { }

    virtual ~Integrator() { }

    /// Integrate function, implemented in derived classes
    virtual double Integrate(const double a, const double b,
                             const double tol = 1e-6) const = 0;

  protected:

    Functor& functor_;

};

/*!
 * @function MakeIntegrator
 * @brief Convenience function to create an Integrator-derived object
 * @ingroup math
 */
template<template<class> class IntegrationType, class Functor>
inline IntegrationType<Functor>
MakeIntegrator(Functor& f)
{
  return IntegrationType<Functor>(f);
}

#endif // DATACLASSES_MATH_INTEGRATOR_H_INCLUDED

