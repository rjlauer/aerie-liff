/*!
 * @file LatDistFitResult.h 
 * @brief Storage for output of the lateral distribution in the shower plane
 * @author Kelly Malone
 * @version $Id: LatDistFitResult.h 24727 2015-04-04 15:18:20Z kam686 $
 */ 

#ifndef DATA_STRUCTURES_RECO_LAT_DIST_FIT_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RECO_LAT_DIST_FIT_RESULT_H_INCLUDED

#include <data-structures/reconstruction/core-fitter/CoreFitResult.h>

/*!
 * @class SFCoreFitResult
 * @author Jim Braun
 * @ingroup core_fitters
 * @brief Fit result from the Simple SF core fitter, including a chi-squared
 *        figure of merit
 */
class LatDistFitResult : public CoreFitResult {

  public:

    LatDistFitResult() : amp_(-1.),
                        age_(-1.),
                        chisquare_(-1) { }

    double GetAge() const
    { return age_; }

    void SetAge(const double age)
    { age_ = age; }

    double GetAmplitude() const
    { return amp_; }

    void SetAmplitude(const double amp)
    { amp_ = amp; }

    void SetChiSquare(const double chisquare)
    { chisquare_ = chisquare; }

    double GetChiSquare() const
    { return chisquare_; }
    
    double GetLDF(const double r) const;

  protected:

    double amp_;
    double age_;
    double chisquare_;

};

SHARED_POINTER_TYPEDEFS(LatDistFitResult);

#endif // DATA_STRUCTURES_RECO_LAT_DIST_FIT_RESULT_H_INCLUDED

