/*!
 * @file SFCoreFitResult.h 
 * @brief Storage for output of the super-fast core fitter (SFCF)
 * @author Segev BenZvi
 * @version $Id: SFCoreFitResult.h 37960 2017-03-02 02:05:17Z zhampel $
 */ 

#ifndef DATA_STRUCTURES_RECO_SF_CORE_FIT_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RECO_SF_CORE_FIT_RESULT_H_INCLUDED

#include <data-structures/reconstruction/core-fitter/CoreFitResult.h>

/*!
 * @class SFCoreFitResult
 * @author Jim Braun
 * @ingroup core_fitters
 * @brief Fit result from the Simple SF core fitter, including a chi-squared
 *        figure of merit
 */
class SFCoreFitResult : public CoreFitResult {

  public:

    SFCoreFitResult() : rmol_(-1.),
                        tailNorm_(-1.),
                        nIterations_(-1) { }

    int GetNIterations() const
    { return nIterations_; }

    void SetNIterations(const int nIterations)
    { nIterations_ = nIterations; }

    double GetTailNorm() const
    { return tailNorm_; }

    void SetTailNorm(const double tn)
    { tailNorm_ = tn; }

    void SetMoliereRadius(const double rm)
    { rmol_ = rm; }

    double GetLDF(const double r) const;

  protected:

    double rmol_;
    double tailNorm_;
    int nIterations_;

};

SHARED_POINTER_TYPEDEFS(SFCoreFitResult);

#endif // DATA_STRUCTURES_RECO_SF_CORE_FIT_RESULT_H_INCLUDED

