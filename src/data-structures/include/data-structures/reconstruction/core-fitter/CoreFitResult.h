/*!
 * @file CoreFitResult.h 
 * @brief Storage for output of core fitters.
 * @author Jim Braun
 * @version $Id: CoreFitResult.h 27732 2015-11-14 01:22:18Z joshwood $
 */ 

#ifndef DATA_STRUCTURES_RECO_CORE_FIT_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RECO_CORE_FIT_RESULT_H_INCLUDED

#include <data-structures/reconstruction/core-fitter/CoreFitTypes.h>
#include <data-structures/reconstruction/RecoResult.h>
#include <data-structures/geometry/Point.h>

#include <hawcnest/HAWCNest.h>

#include <cmath>

/*!
 * @class CoreFitResult
 * @author Jim Braun
 * @ingroup core_fitters
 * @brief Storage for output of core fitters
 */
class CoreFitResult : public virtual RecoResult {

  public:

    /// Default constructor
    CoreFitResult() : pos_(0.,0.,0.),
                      sigma_(-1.),
                      amplitude_(-1.),
                      errCalc_(false),
                      xErr_(-1.),
                      yErr_(-1.),
                      zErr_(-1.),
                      sigmaErr_(-1.),
                      amplitudeErr_(-1.),
                      chiSq_(-1.),
                      ndof_(0),
                      type_(-1) { }

    virtual ~CoreFitResult() { }

    /// Reconstructed X coordinate
    double GetX() const { return pos_.GetX(); }

    /// Reconstructed Y coordinate
    double GetY() const { return pos_.GetY(); }

    /// Reconstructed Z coordinate
    double GetZ() const { return pos_.GetZ(); }

    /// Underlying Point object
    const Point& GetPosition() const { return pos_; }

    /// Reconstructed width of the core
    double GetSigma() const { return sigma_; }

    /// Reconstructed amplitude of the shower
    double GetAmplitude() const { return amplitude_; }

    /// Indicates whether uncertainties have been calculated
    bool UncertaintiesCalculated() const { return errCalc_; }

    /// 1-sigma uncertainty in x location
    double GetXUncertainty() const { return xErr_; }

    /// 1-sigma uncertainty in y location
    double GetYUncertainty() const { return yErr_; }

    /// 1-sigma uncertainty in z location
    double GetZUncertainty() const { return zErr_; }

    /// 1-sigma uncertainty in xy plane
    double GetXYUncertainty() const { return sqrt(xErr_*xErr_+yErr_*yErr_); }

    /// 1-sigma uncertainty in reconstructed width of the core
    double GetSigmaUncertainty() const { return sigmaErr_; }

    /// 1-sigma uncertainty in reconstructed amplitude of the shower
    double GetAmplitudeUncertainty() const { return amplitudeErr_; }

    /// Set fit coordinates
    void SetXYZ(const double x, const double y, const double z) {
      pos_.SetXYZ(x, y, z);
    }

    void SetPosition(const Point& pos) { pos_ = pos; }

    void SetUncertaintiesCalculated(bool errCalc) {errCalc_ = errCalc; }

    void SetXUncertainty(const double err) { xErr_ = err; }

    void SetYUncertainty(const double err) { yErr_ = err; }

    void SetZUncertainty(const double err) { zErr_ = err; }

    void SetSigma(const double sigma) { sigma_ = sigma; }

    void SetAmplitude(const double amplitude) { amplitude_ = amplitude; }

    void SetSigmaUncertainty(const double sigmaErr) { sigmaErr_ = sigmaErr; }

    void SetAmplitudeUncertainty(const double amplitudeErr) {
      amplitudeErr_ = amplitudeErr;
    }

    /// Return the fitted lateral distribution as a function of radius from the
    /// core position
    virtual double GetLDF(const double r) const { return 0.; }

    /// Figure of merit for fit
    double GetChiSq() const { return chiSq_; }

    void SetChiSq(const double chiSq) { chiSq_ = chiSq; }

    int GetNdof() const { return ndof_; }

    int GetType() const { return type_; }

    void SetNdof(const int ndof) { ndof_ = ndof; }

    void SetType(const int type) { type_ = type; }

    /// Shower age, 0 by default unless set by core fitter
    virtual double GetAge() const { return 0; }

    virtual double GetAgeUncertainty() const { return 0; }

    virtual int GetNIterations() const { return 0; }

    virtual void SetAge(const double age) { }

    virtual void SetAgeUncertainty(const double ageErr) { }

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

    void ClearChannelsFit() {
      fitChannelIDs_.clear();
      fitChannelData_.clear();
    }

    class FitData {
      public:
        FitData() : x_(0), y_(0), pe_(0), dpe_(0) { }
        FitData(double x, double y, double pe, double dpe) :
          x_(x), y_(y), pe_(pe), dpe_(dpe) { }

        double x_;
        double y_;
        double pe_;
        double dpe_;
    };

    typedef std::vector<FitData> FitDataList;
    typedef FitDataList::iterator FitDataIterator;
    typedef FitDataList::const_iterator ConstFitDataIterator;

    void SetFitData(double x, double y, double pe, double dpe) {
      fitChannelData_.push_back(FitData(x,y,pe,dpe));
    }

    FitDataIterator FitDataBegin()
    { return fitChannelData_.begin(); }

    FitDataIterator FitDataEnd()
    { return fitChannelData_.end(); }

    ConstFitDataIterator FitDataBegin() const
    { return fitChannelData_.begin(); }

    ConstFitDataIterator FitDataEnd() const
    { return fitChannelData_.end(); }

  protected:

    Point  pos_;
    double sigma_;
    double amplitude_;
    bool   errCalc_;
    double xErr_;    
    double yErr_;
    double zErr_;
    double sigmaErr_;
    double amplitudeErr_;
    double chiSq_;
    int    ndof_;

    mutable bool sorted_;
    mutable std::vector<unsigned> fitChannelIDs_; ///< List of channel IDs

    int    type_;

    /// List of fit data (optionally filled by core fitters)
    mutable std::vector<FitData> fitChannelData_;

};

SHARED_POINTER_TYPEDEFS(CoreFitResult);

#endif // DATA_STRUCTURES_RECO_CORE_FIT_RESULT_H_INCLUDED

