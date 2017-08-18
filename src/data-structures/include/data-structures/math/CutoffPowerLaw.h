/*!
 * @file CutoffPowerLaw.h
 * @brief Representation of a power law function with an exponential cutoff
 * @author Segev BenZvi
 * @date 18 Apr 2012
 * @version $Id: CutoffPowerLaw.h 28837 2016-01-12 20:47:28Z sybenzvi $
 */

#ifndef DATACLASSES_MATH_CUTOFFPOWERLAW_H_INCLUDED
#define DATACLASSES_MATH_CUTOFFPOWERLAW_H_INCLUDED

#include <data-structures/math/PowerLaw.h>

/*!
 * @class CutoffPowerLaw
 * @ingroup math
 * @author Segev BenZvi
 * @date 18 Apr 2012
 * @brief Class which encapsulates a power law with an exponential cutoff
 *
 * This class defines a power law function of form
 * \f[
 *   f(x) = A\cdot \left(\frac{x}{x_N}\right)^{\alpha}e^{x/x_c},
 * \f]
 * where the input \f$ x \f$ is defined over the domain \f$ [x_0, x_1] \f$.
 *
 * The class provides a normalization function that allows a PowerLaw to be
 * reweighted into this CutoffPowerLaw.  If the upper limit is infinite
 * the constant which normalizes the integral flux is given by
 * \f[
 *    A = \frac{x_N^\alpha}{x_c^{\alpha+1}\Gamma(\alpha+1, x_0/x_c)},
 * \f]
 * where \f$\Gamma(a,x)\f$ is the upper incomplete gamma function.
 * Alternatively, if the upper limit is finite the constant which normalizes
 * the integral flux is 
 * \f[
 *    A = \frac{x_N^\alpha}{x_c^{\alpha+1}\Gamma(\alpha+1, x_0/x_c) -
 *                                        \Gamma(\alpha+1, x_1/x_c)}.
 * \f]
 */
class CutoffPowerLaw : public PowerLaw {

  public:

    CutoffPowerLaw();

    CutoffPowerLaw(const double x0, const double x1,
                   const double A, const double xN,
                   const double i, const double xC);

    virtual ~CutoffPowerLaw() { }

    /// Get the exponential cutoff point
    double GetCutoffX() const { return xC_; }

    /// Evaluate the power law at some value
    virtual double Evaluate(const double x) const;

    /// Get the factor or "weight" needed to normalize the integral of the
    /// power law between x0 and x1
    virtual double GetNormWeight(const double x0,
                                 const double x1=HAWCUnits::infinity) const;

    /// Reweight a value x from another power law to obey this power law
    virtual double Reweight(const PowerLaw& pl, const double x) const;

    /// Get the probability to keep an event sampled from some other power
    /// law so that it obeys this power law
    virtual double GetProbToKeep(const PowerLaw& pl, const double x) const;

    /// Get number of transition points in the spectrum (including bounds)
    virtual unsigned int GetNEdges() const { return 2; }

    /// Get the x-value of transition idx
    virtual double GetEdgeX(unsigned int idx) const;

    /// Get the value of x between x0 and x1 at which the integral obtains
    /// some fraction of its total value
    virtual double InvertIntegral(const double frac) const;

  protected:

    double xC_;     ///< Exponential cutoff point

};

SHARED_POINTER_TYPEDEFS(CutoffPowerLaw);

#endif // DATACLASSES_MATH_CUTOFFPOWERLAW_H_INCLUDED

