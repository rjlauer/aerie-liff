/*!
 * @file AngleFitResult.h 
 * @brief Storage for reconstructed showers.
 * @author Jim Braun
 * @version $Id: AngleFitResult.h 27732 2015-11-14 01:22:18Z joshwood $
 */ 

#ifndef DATA_STRUCTURES_RECO_ANGLE_FIT_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RECO_ANGLE_FIT_RESULT_H_INCLUDED

#include <data-structures/reconstruction/track-fitter/AngleFitTypes.h>
#include <data-structures/reconstruction/track-fitter/PropagationPlane.h>
#include <data-structures/reconstruction/RecoResult.h>
#include <data-structures/geometry/Vector.h>

#include <hawcnest/HAWCNest.h>

#include <vector>
#include <algorithm>

/*!
 * @class AngleFitResult
 * @author Jim Braun
 * @ingroup track_fitters
 * @brief Storage of reconstructed shower axis and its uncertainties
 */
class AngleFitResult : public virtual RecoResult {

  public:

    /// Default constructor
    AngleFitResult() : errCalc_(false),
                       t0Err_(-1.),
                       angleErr_(-1.),
                       chiSq_(-1.),
                       ndof_(0),
                       sorted_(true),
                       type_(-1) {

      p_.SetAxis(0.,0.,-1.);
    }

    void SetPropagationPlane(const PropagationPlane& p) {p_ = p;}

    const PropagationPlane& GetPropagationPlane() const {return p_;}

    /// Time shower plane crosses detector plane at core
    double GetReferenceTime() const {return p_.GetReferenceTime();}

    /// Reconstructed zenith angle
    double GetTheta() const {return p_.GetAxis().GetTheta();}

    /// Reconstructed azimuth angle
    double GetPhi() const {return p_.GetAxis().GetPhi();}

    /// Reconstructed shower axis
    const Vector& GetAxis() const {return p_.GetAxis();}

    /// Indicates whether uncertainties have been calculated
    bool UncertaintiesCalculated() const {return errCalc_;}

    /// 1-sigma Gaussian uncertainty in t0
    double GetTimeUncertainty() const {return t0Err_;}

    /// 1-sigma Polar Gaussian uncertainty (i.e. 39.4% containment in angle
    double GetAngleUncertainty() const {return angleErr_;}

    void SetUncertaintiesCalculated(bool errCalc) {errCalc_ = errCalc;}

    void SetTimeUncertainty(const double t0Err) {t0Err_ = t0Err;}

    void SetAngleUncertainty(const double angleErr) {angleErr_ = angleErr;}

    /// Figure of merit for fit, expressed as chi2 = -2 log(likelihood)
    double GetChiSq() const { return chiSq_; }

    void SetChiSq(const double chiSq) { chiSq_ = chiSq; }

    int GetNdof() const { return ndof_; }

    void SetNdof(const int ndof) { ndof_ = ndof; }

    int GetType() const { return type_; }

    void SetType(const int type) { type_ = type; }

    void SetChannelFit(const unsigned gridId) {
      sorted_ = false;
      fitChannelIDs_.push_back(gridId);
    }

    bool IsChannelFit(const unsigned gridId) const {
      if (!sorted_) {
        std::sort(fitChannelIDs_.begin(), fitChannelIDs_.end());
        sorted_ = true;
      }
      return std::binary_search(fitChannelIDs_.begin(),
                                fitChannelIDs_.end(), gridId);
    }

    void ClearChannelsFit() {fitChannelIDs_.clear();}

  protected:

    PropagationPlane p_;
    bool   errCalc_;
    double t0Err_;
    double angleErr_;
    double chiSq_;
    int ndof_;

    mutable bool sorted_;
    mutable std::vector<unsigned> fitChannelIDs_;

    int type_;
};

SHARED_POINTER_TYPEDEFS(AngleFitResult);

#endif // DATA_STRUCTURES_RECO_ANGLE_FIT_RESULT_H_INCLUDED

