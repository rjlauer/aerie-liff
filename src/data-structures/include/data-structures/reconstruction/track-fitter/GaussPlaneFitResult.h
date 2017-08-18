/*!
 * @file GaussPlaneFitResult.h
 * @brief Storage for result of Gaussian plane fit.
 * @author Jim Braun
 * @version $Id: GaussPlaneFitResult.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_RECO_GAUSS_PLANE_FIT_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RECO_GAUSS_PLANE_FIT_RESULT_H_INCLUDED

#include <data-structures/reconstruction/track-fitter/AngleFitResult.h>

/*!
 * @class GaussPlaneFitResult
 * @author Jim Braun
 * @ingroup track_fitters
 * @brief Result of the Gaussian planar shower front fit, with a chi-squared
 *        figure of merit
 */
class GaussPlaneFitResult : public virtual AngleFitResult {

  public:

    /// Default constructor
    GaussPlaneFitResult() { }

};

SHARED_POINTER_TYPEDEFS(GaussPlaneFitResult);

#endif // DATA_STRUCTURES_RECO_GAUSS_PLANE_FIT_RESULT_H_INCLUDED

