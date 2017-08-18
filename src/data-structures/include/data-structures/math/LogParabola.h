/*!
 * @file LogParabola.h
 * @brief Representation of a log-parabolic power law function.
 * @author Segev BenZvi
 * @date 21 May 2012
 * @version $Id: LogParabola.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_MATH_LOGPARABOLA_H_INCLUDED
#define DATACLASSES_MATH_LOGPARABOLA_H_INCLUDED

#include <data-structures/math/PowerLaw.h>

/*!
 * @class LogParabola
 * @ingroup math
 * @author Segev BenZvi
 * @date 21 May 2012
 * @brief Class which encapsulates a power law with an exponential cutoff
 *
 * This class defines a log-parabolic power law function of form
 * \f[
 *   f(x) = A\cdot \left(\frac{x}{x_N}\right)^{\alpha+\beta\ln{(x/x_N)}},
 * \f]
 * where the input \f$ x \f$ is defined over the domain \f$ [x_0, x_1] \f$.
 */
class LogParabola : public PowerLaw {

  public:

    LogParabola();

    LogParabola(const double x0, const double x1,
                const double A, const double xN,
                const double a, const double b);

    virtual ~LogParabola() { }

    /// Get the normalization point of the constant A
    double GetNormX() const { return xN_; }

    /// Return the "slope" of the log parabolic spectral index
    double GetSpectralIndexSlope() const { return idx2_; }

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

  protected:

    double idx2_;   ///< "Slope" of log-parabolic power law spectrum

};

SHARED_POINTER_TYPEDEFS(LogParabola);

#endif // DATACLASSES_MATH_LOGPARABOLA_H_INCLUDED

