/*!
 * @file GamCoreFitResult.h 
 * @brief Storage for output of the lateral distribution in the shower plane
 * @author John Matthews
 * @version $Id: GamCoreFitResult.h 24727 2015-04-04 15:18:20Z kam686 $
 */ 

#ifndef DATA_STRUCTURES_RECO_GAM_CORE_FIT_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RECO_GAM_CORE_FIT_RESULT_H_INCLUDED

#include <data-structures/reconstruction/core-fitter/CoreFitResult.h>

class GamCoreFitResult : public CoreFitResult {

  public:

    GamCoreFitResult() : amp_(-1.),
                         age_(-1.),
                         chisquare_(-1.),
                         packint_(-1.) { }

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

    void SetPackInt(const double packint)
    { packint_ = packint; }

    double GetPackInt() const
    { return packint_; }
    
    double GetLDF(const double r) const;

  protected:

    double amp_;
    double age_;
    double chisquare_;
    double packint_;

};

SHARED_POINTER_TYPEDEFS(GamCoreFitResult);

#endif // DATA_STRUCTURES_RECO_GAM_CORE_FIT_RESULT_H_INCLUDED

