/*!
 * @file CompactnessResult.h 
 * @brief Compactness of a reconstructed air shower.
 * @author Jim Braun
 * @version $Id: CompactnessResult.h 24730 2015-04-04 19:35:57Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_RECO_COMPACTNESS_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RECO_COMPACTNESS_RESULT_H_INCLUDED

#include <data-structures/reconstruction/RecoResult.h>
#include <hawcnest/HAWCNest.h>

/*!
 * @class CompactnessResult
 * @author Jim Braun
 * @ingroup core_fitters 
 * @brief Compactness calculated for a shower, given a particular search radius
 */
class CompactnessResult : public RecoResult {

  public:

    CompactnessResult() : radius_(0.),
                          compactness_(0.),
                          maxPE_(0.),
                          tResi_(0.),
                          maxPEChannelID_(0) { }

    double GetRadius() const
    { return radius_; }

    double GetCompactness() const
    { return compactness_; }

    double GetMaxPE() const
    { return maxPE_; }
  
    double GetTResi() const
    { return tResi_; }
  
    unsigned GetMaxPEChannelID() const
    { return maxPEChannelID_; }

    unsigned GetNchExcluded() const
    { return xNch_; }

    void SetRadius(const double radius)
    { radius_ = radius; }

    void SetCompactness(const double compact)
    { compactness_ = compact; }

    void SetMaxPE(const double maxPE)
    { maxPE_ = maxPE; }

    void SetTResi(const double tResi)
    { tResi_ = tResi; }
  
    void SetMaxPEChannelID(const unsigned maxPEChannelID)
    { maxPEChannelID_ = maxPEChannelID; }

    void SetNchExcluded(const unsigned xNch)
    { xNch_ = xNch; }

  protected:

    double radius_;             ///< Exclusion radius used to find hottest PMT
    double compactness_;        ///< Compactness = nFit/MaxPE
    double maxPE_;              ///< Maximum PE outside exclusion radius
    double tResi_;              ///< Time residual of hit relative to plane
    unsigned maxPEChannelID_;   ///< Channel with max PE
    unsigned xNch_;             ///< Number of channels within excl. radius

};

SHARED_POINTER_TYPEDEFS(CompactnessResult);

#endif // DATA_STRUCTURES_RECO_COMPACTNESS_RESULT_H_INCLUDED

