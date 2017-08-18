/*!
 * @file PeriodicFunction.h
 * @brief A non-parametric periodic function defined by a data table
 * @author Segev BenZvi
 * @date 5 Oct 2014
 * @version $Id: PeriodicFunction.h 22111 2014-10-06 00:16:33Z sybenzvi $
 */

#ifndef DATACLASSES_MATH_PERIODICFUNCTION_H_INCLUDED
#define DATACLASSES_MATH_PERIODICFUNCTION_H_INCLUDED

#include <data-structures/math/TabulatedFunction.h>

#include <hawcnest/PointerTypedefs.h>

/*!
 * @class PeriodicFunction
 * @ingroup math
 * @author Segev BenZvi
 * @date 5 Oct 2014
 * @brief A non-parametric periodic function defined as a function of a single
 * cycle, period length, and offset.
 * 
 * This class implements a non-parametric periodic function.  The user defines
 * the behavior within a single cycle by giving a table of period cycle and
 * function values.  The cycle is expressed between 0 and 1 and rescaled using
 * the offset and period length.
 */
class PeriodicFunction {

  private:

    typedef std::vector<double> vecD;

  public:

    /*!
     * @brief Construct a periodic function
     * @param t0 Offset (phase) of the start of the period
     * @param period Length of the period
     * @param fphase Phase of the function within a period [0..1]
     * @param fnfunc Value of the function within a period.
     *        This parameter should define one complete cycle.
     */
    PeriodicFunction(const double t0, const double period,
                     const vecD& fphase, const vecD& fnfunc);

    virtual ~PeriodicFunction() { }

    /// Evaluate the PeriodicFunction at value t
    double Evaluate(const double& t) const;

    /// Allow the PeriodicFunction to behave as a functor evaluated at value t
    double operator()(const double& t) const
    { return Evaluate(t); }

  private:

    double t0_;       ///< Offset for start of new cycle
    double period_;   ///< Length of the period

    /// Table of function values vs. position in periodic cycle (phase)
    TabulatedFunction<double> func_;

};

SHARED_POINTER_TYPEDEFS(PeriodicFunction);

#endif // DATACLASSES_MATH_PERIODICFUNCTION_H_INCLUDED

