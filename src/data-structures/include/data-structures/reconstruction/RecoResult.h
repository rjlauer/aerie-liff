/*!
 * @file RecoResult.h
 * @brief Reconstruction result interface.
 * @author Jim Braun
 * @version $Id: RecoResult.h 27464 2015-11-02 14:45:10Z sybenzvi $
 */

#ifndef DATA_STRUCTURES_RECO_RECO_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RECO_RECO_RESULT_H_INCLUDED

#include <data-structures/reconstruction/Reco.h>

#include <hawcnest/HAWCNest.h>

/*!
 * @class RecoResult
 * @author Jim Braun
 * @ingroup modules
 * @brief Interface for printing reconstruction result structures and storing
 *        recontruction algorithm status for a particular task
 */
class RecoResult : public virtual Baggable {

  public:

    RecoResult() : status_(RECO_UNKNOWN), nFit_(-1) { }

    virtual ~RecoResult() { }

    /// Status of reconstruction result
    RecoStatus GetStatus() const {return status_;}

    /// Number of PMTs used in the fit
    int GetNFit() const { return nFit_; }
    void SetNFit(const int nFit) { nFit_ = nFit; }

    /// Number of channels in DAQ passing good channel selection
    unsigned int GetNchGood() const { return nChGood_; }
    void SetNchGood(const unsigned int n) { nChGood_ = n; }

    /// Number of channels with hits
    unsigned int GetNchHit() const { return nChHit_; }
    void SetNchHit(const unsigned int n) { nChHit_ = n; }

    /// Number of channels with hits after hit selection
    unsigned int GetNchGoodHit() const { return nChGoodHit_; }
    void SetNchGoodHit(const unsigned int n) { nChGoodHit_ = n; }

    /// Number of good channels without hits
    unsigned int GetNchGoodZero() const { return nChGoodZero_; }
    void SetNchGoodZero(const unsigned int n) { nChGoodZero_ = n; }

    /// Number of good channels without hits
    unsigned int GetNchAvail() const { return nChAvail_; }
    void SetNchAvail(const unsigned int n) { nChAvail_ = n; }

    void SetStatus(const RecoStatus status) {status_ = status;}


  protected:

    RecoStatus status_;
    int nFit_;

    unsigned int nChGood_;      ///< Channels in DAQ with good status
    unsigned int nChHit_;       ///< Channels with hits
    unsigned int nChGoodHit_;   ///< Channels with hits passing hit selection
    unsigned int nChGoodZero_;  ///< Good channels without hits
    unsigned int nChAvail_;     ///< Depends; could be good hits + good zeros

};

SHARED_POINTER_TYPEDEFS(RecoResult);

#endif // DATA_STRUCTURES_RECO_RECO_RESULT_H_INCLUDED

