/*!
 * @file PINCResult.h 
 * @brief PINC of a reconstructed air shower.
 * @author Jim Braun
 * @version $Id: PINCResult.h 24730 2015-04-04 19:35:57Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_RECO_PINC_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RECO_PINC_RESULT_H_INCLUDED

#include <data-structures/reconstruction/RecoResult.h>
#include <hawcnest/HAWCNest.h>

/*!
 * @class PINCResult
 * @author Jim Braun
 * @ingroup core_fitters 
 * @brief PINC calculated for a shower, given a particular search radius
 */
class PINCResult : public RecoResult {

  public:

    PINCResult() : radius_(0.),
                   pincness_(0.),
                   xNch_(0) { }

    double GetRadius() const
    { return radius_; }

    double GetPINC() const
    { return pincness_; }

    unsigned GetNchExcluded() const
    { return xNch_; }

    void SetRadius(const double radius)
    { radius_ = radius; }

    void SetPINC(const double compact)
    { pincness_ = compact; }

    void SetNchExcluded(const unsigned xNch)
    { xNch_ = xNch; }

  protected:

    double radius_;             ///< Exclusion radius used to cut away core
    double pincness_;           ///< PINC = nFit/MaxPE
    unsigned xNch_;             ///< Number of channels within excl. radius

};

SHARED_POINTER_TYPEDEFS(PINCResult);

#endif // DATA_STRUCTURES_RECO_PINC_RESULT_H_INCLUDED

