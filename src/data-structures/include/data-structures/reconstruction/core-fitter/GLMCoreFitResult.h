/*!
 * @file Chi2CoreFitResult.h 
 * @brief Storage for output of the Chi2 core fitter.
 * @author Hugo Ayala
 * @version $Id: GaussCoreFitResult.h 19271 2014-03-31 14:57:08Z hayalaso $
 */ 

#ifndef DATA_STRUCTURES_RECO_CHI2_CORE_FIT_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RECO_CHI2_CORE_FIT_RESULT_H_INCLUDED

#include <data-structures/reconstruction/core-fitter/CoreFitResult.h>
#include <data-structures/reconstruction/core-fitter/NKGCoreFitResult.h>

/*!
 * @class Chi2CoreFitResult
 * @author Hugo Ayala
 * @ingroup core_fitters
 * @brief Fit result from the Chi2 core fitter. 
 */
class GLMCoreFitResult : public NKGCoreFitResult{

  public:

    GLMCoreFitResult() { }

    double GetLDF(const double r) const { return 0; }

};

SHARED_POINTER_TYPEDEFS(GLMCoreFitResult);

#endif // DATA_STRUCTURES_RECO_GAUSS_CORE_FIT_RESULT_H_INCLUDED

