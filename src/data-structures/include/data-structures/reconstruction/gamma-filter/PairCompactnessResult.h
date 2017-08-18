/*!
 * @file PairCompactnessResult.h 
 * @brief Pair-compactness of a reconstructed air shower.
 * @author Zig Hampel
 * @version $Id: PairCompactnessResult.h 24784 2015-04-11 13:06:06Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_RECO_PAIR_COMPACTNESS_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RECO_PAIR_COMPACTNESS_RESULT_H_INCLUDED

#include <data-structures/reconstruction/RecoResult.h>
#include <hawcnest/HAWCNest.h>

/*!
 * @class PairCompactnessResult
 * @author Zig Hampel
 * @ingroup gamma_filters
 * @brief PairCompactness calculated for a shower, given a particular search radius
 */
class PairCompactnessResult : public RecoResult {

  public:

    PairCompactnessResult() :
      radius_(0.),
      charge_(0.),
      nHitSelect_(0)
    { }

    double GetRadius() const
    { return radius_; }

    double GetCharge() const
    { return charge_; }

    int GetNHitSelect() const
    { return nHitSelect_; }

    void SetRadius(const double radius)
    { radius_ = radius; }

    void SetCharge(const double charge)
    { charge_ = charge; }

    void SetNHitSelect(const int nHitSel)
    { nHitSelect_ = nHitSel; }

  protected:

    double radius_;
    double charge_;
    int nHitSelect_;

};

SHARED_POINTER_TYPEDEFS(PairCompactnessResult);

#endif // DATA_STRUCTURES_RECO_PAIR_COMPACTNESS_RESULT_H_INCLUDED

