/*!
 * @file DoubleBrokenPowerLaw.h
 * @brief Power law functions with two break points.
 * @author Segev BenZvi
 * @date 10 Jan 2011
 * @version $Id: DoubleBrokenPowerLaw.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_MATH_DOUBLEBROKENPOWERLAW_H_INCLUDED
#define DATACLASSES_MATH_DOUBLEBROKENPOWERLAW_H_INCLUDED

#include <data-structures/math/BrokenPowerLaw.h>

/*!
 * @class DoubleBrokenPowerLaw
 * @ingroup math
 * @author Segev BenZvi
 * @date 10 Jan 2011
 * @brief Class which encapsulates a double broken power law with three
 *        spectral indices
 * @todo Normalize between xMin and xMax rather than xMin and infinity?
 * @todo Check for domain errors when evaluating the power law at some x?
 *
 * This class defines a double broken power law function of form
 * \f[
 *   f(x) = 
 *   \begin{cases}
 *     A\cdot \left(\frac{x}{x_N}\right)^{\gamma_1}
 *         & :\quad x < x_{B_1} \\
 *     A\cdot \left(\frac{x_{B_1}}{x_N}\right)^{\gamma_1-\gamma_2}\
 *         \left(\frac{x}{x_N}\right)^{\gamma_2}
 *         & :\quad x_{B_1}\leq  x<x_{B_2} \\
 *     A\cdot \left(\frac{x_{B_1}}{x_N}\right)^{\gamma_1-\gamma_2}\
 *         \left(\frac{x_{B_1}}{x_N}\right)^{\gamma_2-\gamma_3}\
 *         \left(\frac{x}{x_N}\right)^{\gamma_3}
 *         & :\quad x\geq x_{B_2}
 * \end{cases}
 * \f]
 * where the input \f$ x \f$ is defined over the domain \f$ [x_0, x_1] \f$.
 */
class DoubleBrokenPowerLaw : public BrokenPowerLaw {

  public:

    DoubleBrokenPowerLaw();

    DoubleBrokenPowerLaw(const double x0, const double x1, 
                         const double A, const double xN,
                         const double idx1, const double xB1,
                         const double idx2, const double xB2,
                         const double idx3);

    virtual ~DoubleBrokenPowerLaw() { }

    /// Return the power law spectral index as a function of x
    virtual double GetSpectralIndex(const double x) const;

    /// Evaluate the power law at some value
    virtual double Evaluate(const double x) const;

    /// Get the factor or "weight" needed to normalize the integral of the
    /// power law between x0 and infinity
    virtual double GetNormWeight(const double x0,
                                 const double x1 = HAWCUnits::infinity) const;

    /// Reweight a value x from another power law to obey this power law
    virtual double Reweight(const PowerLaw& pl, const double x) const;

    /// Get the probability to keep an event sampled from some other power
    /// law so that it obeys this power law
    virtual double GetProbToKeep(const PowerLaw& pl, const double x) const;

    /// Get number of transition points in the spectrum (including bounds)
    virtual unsigned int GetNEdges() const { return 4; }

    /// Get the x-value of transition idx
    virtual double GetEdgeX(unsigned int idx) const;

    /// Get the value of x between x0 and x1 at which the integral obtains
    /// some fraction of its total value
    virtual double InvertIntegral(const double frac) const;

  // Changing from private to protected just in case
  protected:

    double xB2_;    ///< Location of second break point
    double idx3_;   ///< Spectral index above second break point

};

SHARED_POINTER_TYPEDEFS(DoubleBrokenPowerLaw);

#endif // DATACLASSES_MATH_DOUBLEBROKENPOWERLAW_H_INCLUDED

