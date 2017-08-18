/*!
 * @file BrokenPowerLaw.h
 * @brief Representation of a power law function with a break point
 * @author Segev BenZvi
 * @date 10 Jan 2011
 * @version $Id: BrokenPowerLaw.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */
 
#ifndef DATACLASSES_MATH_BROKENPOWERLAW_H_INCLUDED
#define DATACLASSES_MATH_BROKENPOWERLAW_H_INCLUDED

#include <data-structures/math/PowerLaw.h>

/*!
 * @class BrokenPowerLaw
 * @ingroup math
 * @author Segev BenZvi
 * @date 10 Jan 2011
 * @brief Class which encapsulates a broken power law with two spectral indices
 * @todo Check for domain errors when evaluating the power law at some x?
 *
 * This class defines a broken power law function of form
 * \f[
 *   f(x) = 
 *   \begin{cases}
 *     A\cdot \left(\frac{x}{x_N}\right)^{\gamma_1}
 *        & :\quad x < x_B \\
 *     A\cdot \left(\frac{x_B}{x_N}\right)^{\gamma_1-\gamma_2}\
 *            \left(\frac{x}{x_N}\right)^{\gamma_2}
 *        & :\quad x\geq x_B
 *   \end{cases}
 * \f]
 * where the input \f$ x \f$ is defined over the domain \f$ [x_0, x_1] \f$.
 *
 * The class reweighting functions can be used to reweight data from a PowerLaw
 * into a BrokenPowerLaw.  The normalization can assume that the upper limit is
 * infinite.  In this case, the normalization constant is given by
 * \f[
 *  A = 
 *  \begin{cases}
 *    \left((x_B^{\gamma_1+1}-x_0^{\gamma_1+1}) / x_N^{\gamma_1}(\gamma_1+1) -
 *    (x_B^{\gamma_2+1}) / x_N^{\gamma_2}(\gamma_2+1)\right)^{-1} 
 *          & : \quad x < x_B \\
 *     -(\gamma_2+1)\cdot x_N^{\gamma_2}\cdot x_0^{-(\gamma_2 + 1)}
 *          & : \quad x\geq x_B
 *  \end{cases}
 * \f]
 * 
 * If the upper limit is finite then the normalization constant becomes
 * \f[
 *  A = 
 *  \begin{cases}
 *    \left((x_1^{\gamma_1+1}-x_0^{\gamma_1+1}) / (\gamma_1+1)\right)^{-1}
 *      & : \quad x_0 < x_B\text{ and } x_1 < x_B \\
 *    \left((x_B^{\gamma_1+1}-x_0^{\gamma_1+1}) / (\gamma_1+1) -
 *    (x_B^{\gamma_1-\gamma_2}x_1^{\gamma_2+1} - x_B^{\gamma_1+1}) /
 *    (\gamma_2+1)\right)^{-1}
 *      & : \quad x_0 < x_B\text{ and } x_1 > x_B \\
 *   -(\gamma_2+1)\cdot x_0^{-(\gamma_2 + 1)}
 *      & : \quad x_0\geq x_B
 *  \end{cases}
 * \f]
 */
class BrokenPowerLaw : public PowerLaw {

  public:

    BrokenPowerLaw();

    BrokenPowerLaw(const double x0, const double x1, 
                   const double A, const double xN,
                   const double idx1, const double xB, const double idx2);

    virtual ~BrokenPowerLaw() { }

    /// Return the power law spectral index as a function of x
    virtual double GetSpectralIndex(const double x) const
    { return (x < xB1_) ? idx1_ : idx2_; }

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
    virtual unsigned int GetNEdges() const { return 3; }

    /// Get the x-value of transition idx
    virtual double GetEdgeX(unsigned int idx) const;

    /// Get the value of x between x0 and x1 at which the integral obtains
    /// some fraction of its total value
    virtual double InvertIntegral(const double frac) const;

    double xB1_;    ///< Location of break point
    double idx2_;   ///< Spectral index above break point

};

SHARED_POINTER_TYPEDEFS(BrokenPowerLaw);

#endif // DATACLASSES_MATH_BROKENPOWERLAW_H_INCLUDED

