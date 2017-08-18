/*!
 * @file ResponseBin.h
 * @author Robert Lauer
 * @date 25 Jul 2014
 * @brief Histograms and functions for use in a DetectorResponse bin.
 * @version $Id: ResponseBin.h 40126 2017-08-10 19:29:00Z criviere $
 */

#include <TH1D.h>
#include <TF1.h>

#include <hawcnest/Logging.h>
#include <hawcnest/PointerTypedefs.h>
#include <liff/BinList.h>

#ifndef RESPONSE_BIN_H_INCLUDED
#define RESPONSE_BIN_H_INCLUDED

SHARED_POINTER_TYPEDEFS(TH1D);
SHARED_POINTER_TYPEDEFS(TF1);

/*!
 * @class ResponseBin
 * @author Robert Lauer
 * @date 25 Jul 2014
 * @ingroup
 * @brief histograms/functions for use in DetectorResponse for a dec/nHit bin
 */

class ResponseBin {

  public:

    friend class DetectorResponse;

    ResponseBin(const int decbin, const BinName& nhbin) :
      // decbin_(decbin),
      // nhitbin_(nhbin),
      suffix_(Form("_dec%d_nh%s", decbin, nhbin.c_str())) {}

    ///Returns energy binning
    const std::vector<double>& GetLogEnBins() {
      return logEnBins_;
    }

    ///Reweight energy histograms according to vector with diff flux values
    void ReweightEnergies(const std::vector<double> fluxes) {
      if (fluxes.size() != simFluxes_.size()) {
        log_fatal("Size of vector<double> fluxes is not number of logEn "
                      << "histogram bins.");
      }
      const TH1DPtr enSig = GetEnSigHist(true); //'true' means reset
      for (int b = 0; b < enSig->GetNbinsX(); ++b) {
        double scale = fluxes[b] / simFluxes_[b];
        enSig->SetBinContent(b, enSig->GetBinContent(b) * scale);
      }
      sigExp_ = enSig->Integral();
    }

    /// Return Point Spread Function histogram
    TH1DPtr GetPsfHist(const bool reset = false) {
      if (reset || !psfHist_) {
        const std::string name = "PSF" + suffix_;
        psfHist_ = TH1DPtr(simPsfHist_);
        psfHist_->SetNameTitle(name.c_str(), name.c_str());
      }
      return psfHist_;
    }

    /// Return Energy Distribution histogram for Signal
    TH1DPtr GetEnSigHist(const bool reset = false) {
      if (reset || !enSigHist_) {
        const std::string name = "EnSig" + suffix_;
        enSigHist_ = TH1DPtr(new TH1D(*simEnSigHist_));
        enSigHist_->SetNameTitle(name.c_str(), name.c_str());
      }
      return enSigHist_;
    }

    /// Return Energy Distribution histogram for Background
    TH1DPtr GetEnBgHist(const bool reset = false) {
      if (reset || !enBgHist_) {
        const std::string name = "EnBg" + suffix_;
        enBgHist_ = TH1DPtr(simEnBgHist_);
        enBgHist_->SetNameTitle(name.c_str(), name.c_str());
      }
      return enBgHist_;
    }

    /// Return PSF distribution function
    TF1Ptr GetPsfFunction(const bool reset = false) {
      if (reset || !psfFunc_) {
        const std::string name = "PSF" + suffix_ + "_fit";
        psfFunc_ = TF1Ptr(simPsfFunc_);
        psfFunc_->SetNameTitle(name.c_str(), name.c_str());
      }
      return psfFunc_;
    }

    /// Return energy distribution function for signal
    TF1Ptr GetEnSigFunction(const bool reset = false) {
      if (reset || !enSigFunc_) {
        const std::string name = "EnSig" + suffix_ + "_fit";
        enSigFunc_ = TF1Ptr(simEnSigFunc_);
        enSigFunc_->SetNameTitle(name.c_str(), name.c_str());
      }
      return enSigFunc_;
    }

    /// Return energy distribution function for background
    TF1Ptr GetEnBgFunction(const bool reset = false) {
      if (reset || !enBgFunc_) {
        const std::string name = "EnBg" + suffix_ + "_fit";
        enBgFunc_ = TF1Ptr(simEnBgFunc_);
        enBgFunc_->SetNameTitle(name.c_str(), name.c_str());
      }
      return enBgFunc_;
    }

    /// Set PSF function, keep old range if setRange=false (default)
    void SetPsfFunction(TF1Ptr func, bool setRange = false) {
      std::string name = GetPsfFunction()->GetName();
      //const char* name = GetPsfFunction()->GetName(); //Doesn't work, why?
      double xmin, xmax;
      psfFunc_->GetRange(xmin, xmax);
      psfFunc_ = TF1Ptr(new TF1(*func));
      psfFunc_->SetNameTitle(name.c_str(), name.c_str());
      if (!setRange) psfFunc_->SetRange(xmin, xmax);
    }

    /// Set PSF TF1 and fit corresponding histogram
    void FitPsfWithTF1(TF1Ptr func) {
      SetPsfFunction(func, false);
      GetPsfHist()->Fit(psfFunc_.get(), "Q");
    }

    /// Set EnSig function, keep old range if setRange=false (default)
    void SetEnSigFunction(TF1Ptr func, bool setRange = false) {
      std::string name = GetEnSigFunction()->GetName();
      double xmin, xmax;
      enSigFunc_->GetRange(xmin, xmax);
      enSigFunc_ = TF1Ptr(new TF1(*func));
      enSigFunc_->SetNameTitle(name.c_str(), name.c_str());
      if (!setRange) enSigFunc_->SetRange(xmin, xmax);
    }

    /// Set EnSig TF1 and fit corresponding histogram
    void FitEnSigWithTF1(TF1Ptr func) {
      SetEnSigFunction(func, false);
      GetEnSigHist()->Fit(enSigFunc_.get(), "Q");
    }

    /// Set EnBg function, keep old range if setRange=false (default)
    void SetEnBgFunction(TF1Ptr func, bool setRange = false) {
      std::string name = GetEnBgFunction()->GetName();
      double xmin, xmax;
      enBgFunc_->GetRange(xmin, xmax);
      enBgFunc_ = TF1Ptr(new TF1(*func));
      enBgFunc_->SetNameTitle(name.c_str(), name.c_str());
      if (!setRange) enBgFunc_->SetRange(xmin, xmax);
    }

    /// Set EnBg TF1 and fit corresponding histogram
    void FitEnBgWithTF1(TF1Ptr func) {
      SetEnBgFunction(func, false);
      GetEnBgHist()->Fit(enBgFunc_.get(), "Q");
    }

    /// Return expected number of gamma-ray events in this bin
    double GetExpectedSignal() {
      return sigExp_;
    }

    /// Return Expected number of background events in this bin
    double GetExpectedBackground() {
      return bgExp_;
    }

    /// Set expected number of gamma-ray events in this bin
    void SetExpectedSignal(double ngr) {
      /// WARNING: EnSig-Function is not rescaled
      sigExp_ = ngr;
      //rescale current (possibly reweighted) energy hist:
      double oldexp = GetEnSigHist()->Integral();
      if (oldexp > 0) GetEnSigHist()->Scale(sigExp_ / oldexp);
      //rescale base reference energy hist used for future reweightings:
      oldexp = simEnSigHist_->Integral();
      if (oldexp > 0) simEnSigHist_->Scale(sigExp_ / oldexp);
    }

    /// Set expected number of background events in this bin
    void SetExpectedBackground(double nbg) {
      /// WARNING: EnBg-Function is not rescaled
      bgExp_ = nbg;
      double oldexp = simEnBgHist_->Integral();
      simEnBgHist_->Scale(bgExp_ / oldexp);
      GetEnBgHist(true);
    }


  private:

    // const int decbin_;
    // const BinName nhitbin_;
    //
    const std::string suffix_;

    double sigExp_;
    double bgExp_;

    TH1DPtr psfHist_;
    TH1DPtr enSigHist_;
    TH1DPtr enBgHist_;

    TF1Ptr psfFunc_;
    TF1Ptr enSigFunc_;
    TF1Ptr enBgFunc_;

    TH1DPtr simPsfHist_;
    TH1DPtr simEnSigHist_;
    TH1DPtr simEnBgHist_;

    TF1Ptr simPsfFunc_;
    TF1Ptr simEnSigFunc_;
    TF1Ptr simEnBgFunc_;

    std::vector<double> simFluxes_;
    std::vector<double> logEnBins_;
};
SHARED_POINTER_TYPEDEFS(ResponseBin);

#endif // RESPONSE_BIN_H_INCLUDED
