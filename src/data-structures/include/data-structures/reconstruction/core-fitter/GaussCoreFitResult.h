/*!
 * @file GaussCoreFitResult.h 
 * @brief Storage for output of the Gaussian core fitter.
 * @author Jim Braun
 * @version $Id: GaussCoreFitResult.h 24721 2015-04-04 06:00:44Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_RECO_GAUSS_CORE_FIT_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RECO_GAUSS_CORE_FIT_RESULT_H_INCLUDED

#include <data-structures/reconstruction/core-fitter/CoreFitResult.h>

/*!
 * @class GaussCoreFitResult
 * @author Jim Braun
 * @ingroup core_fitters
 * @brief Fit result from the Gaussian core fitter, including a chi-squared
 *        figure of merit
 */
class GaussCoreFitResult : public CoreFitResult {

  public:

    GaussCoreFitResult() : nIterations_(-1) { }

    void SetNIterations(const int nIterations)
        {nIterations_ = nIterations;}

    double GetLDF(const double r) const { return 0; }
  
  protected:
     int nIterations_;
};

SHARED_POINTER_TYPEDEFS(GaussCoreFitResult);

#endif // DATA_STRUCTURES_RECO_GAUSS_CORE_FIT_RESULT_H_INCLUDED

