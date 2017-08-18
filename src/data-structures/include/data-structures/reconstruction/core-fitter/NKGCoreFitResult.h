/*!
 * @file NKGCoreFitResult.h 
 * @brief Storage for output of the Simple NKG core fitter.
 * @author Jim Braun
 * @version $Id: GaussCoreFitResult.h 18851 2014-02-18 17:08:50Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_RECO_NKG_CORE_FIT_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RECO_NKG_CORE_FIT_RESULT_H_INCLUDED

#include <data-structures/reconstruction/core-fitter/CoreFitResult.h>

/*!
 * @class NKGCoreFitResult
 * @author Jim Braun
 * @ingroup core_fitters
 * @brief Fit result from the Simple NKG core fitter, including a chi-squared
 *        figure of merit
 */
class NKGCoreFitResult : public CoreFitResult {

  public:

    NKGCoreFitResult() : ageParameter_(-1.),
                         ageErr_(-1.),
                         nIterations_(-1) { }

    int GetNIterations() const {return nIterations_;}

    double GetAge() const {return ageParameter_;}

    double GetAgeUncertainty() const {return ageErr_;}

    void SetNIterations(const int nIterations)
        {nIterations_ = nIterations;}

    void SetAge(const double age) {ageParameter_ = age;}

    void SetAgeUncertainty(const double ageErr) {ageErr_ = ageErr;}

    double GetLDF(const double r) const;

  protected:

    double ageParameter_;
    double ageErr_;
    int nIterations_;
};

SHARED_POINTER_TYPEDEFS(NKGCoreFitResult);

#endif // DATA_STRUCTURES_RECO_GAUSS_CORE_FIT_RESULT_H_INCLUDED

