/*!
 * @file SummaryResult.h 
 * @brief Storage for summary data of event.
 * @author Brian Baughman
 * @date 19 Nov 2014
 * @version $Id: SummaryResult.h 37857 2017-02-24 23:30:24Z johnm $
 */ 

#ifndef DATA_STRUCTURES_RECO_SUMMARY_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RECO_SUMMARY_RESULT_H_INCLUDED

#include <data-structures/reconstruction/RecoResult.h>

#include <hawcnest/HAWCNest.h>

#include <stdint.h>

/*!
 * @class SummaryResult
 * @author Brian Baughman
 * @date 19 Nov 2014
 * @brief Storage for output of summary of event
 */
class SummaryResult : public virtual RecoResult {

  public:

    typedef std::vector<double> dVec;
    typedef std::vector<std::string> sVec;
    /// Default constructor
    SummaryResult() :
      version_(0),
      eventID_(0),
      runID_(0),
      timeSliceID_(0),
      triggerFlags_(0),
      eventFlags_(0),
      gtcFlags_(0),
      gpsSec_(0),
      gpsNanosec_(0),
      nChannels_(0),
      nTanks_(0),
      nHits_(0),
      maxChannels_(0),
      windowHits_(0),
      nChTot_(0),
      nChAvail_(0),
      nHitTot_(0),
      nHit_(0),
      nHitSP10_(0),
      nHitSP20_(0),
      nTankTot_(0),
      nTankAvail_(0),
      nTankHitTot_(0),
      nTankHit_(0),
      cxPE10_(0.),
      cxPE20_(0.),
      cxPE30_(0.),
      cxPE40_(0.),
      cxPE50_(0.),
      allCxPERadii_(false),
      cxPE30PMT_(0),
      cxPE40PMT_(0),
      cxPE50PMT_(0),
      cxPE30XnCh_(0),
      cxPE40XnCh_(0),
      cxPE50XnCh_(0),
      cxPE30SPTime_(0.),
      cxPE40SPTime_(0.),
      cxPE50SPTime_(0.),
      pairOut_(false),
      pairRadius_(0.),
      pairCharge_(0.),
      pairNHit_(0),
      pincOut_(false),
      pinc_(0.),
      featuresNNOut_(false),
      DisMax_(0.),
      pmtChi2Out_(false),
      pmtChi2_(0.),
      zenithAngle_(0.),
      azimuthAngle_(0.),
      dec_(0.),
      ra_(0.),
      angleFitStatus_(0),
      angleFitType_(-1),
      angleFitUnc_(0.),
      angleFitChi2_(0.),
      angleFitNdof_(0),
      planeZenith_(0.),
      planeAzimuth_(0.),
      planeDec_(0.),
      planeRa_(0.),
      planeFitStatus_(0),
      coreX_(0.),
      coreXUnc_(0.),
      coreY_(0.),
      coreYUnc_(0.),
      logCoreAmplitude_(0.),
      logCoreAmplitudeUnc_(0.),
      showerAge_(0.),
      showerAgeUnc_(0.),
      coreFitType_(-1),
      coreFitUnc_(0.),
      coreFitChi2_(0.),
      coreFitNdof_(0),
      coreFitStatus_(0),
      logNnEnergy_(0.),
      fAnnulusCharge0_(0.),
      fAnnulusCharge1_(0.),
      fAnnulusCharge2_(0.),
      fAnnulusCharge3_(0.),
      fAnnulusCharge4_(0.),
      fAnnulusCharge5_(0.),
      fAnnulusCharge6_(0.),
      fAnnulusCharge7_(0.),
      fAnnulusCharge8_(0.),
      NNEOut_(false),
      logNPE_(0.),
      logMaxPE_(0.),
      LDFage_(0.),
      LDFamp_(0.),
      LDFchiSq_(0.),
      latOut_(false),
      GamCoreAge_(0.),
      GamCoreAmp_(0.),
      GamCoreChiSq_(0.),
      GamCorePackInt_(0.),
      GamCoreOut_(false),
      coreFiduScale_(0)
    { }

    virtual ~SummaryResult() { }

  public:

    uint64_t version_;
    uint64_t eventID_;
    uint64_t runID_;
    uint64_t timeSliceID_;
  
    uint64_t triggerFlags_;
    uint64_t eventFlags_;
    uint64_t gtcFlags_;
    
    uint64_t gpsSec_;
    uint64_t gpsNanosec_;
    
    uint64_t nChannels_;
    uint64_t nTanks_;
    uint64_t nHits_;
    uint64_t maxChannels_;
    uint64_t windowHits_;
    
    uint64_t nChTot_;
    uint64_t nChAvail_;
    uint64_t nHitTot_;
    uint64_t nHit_;
    uint64_t nHitSP10_;
    uint64_t nHitSP20_;
    uint64_t nTankTot_;
    uint64_t nTankAvail_;
    uint64_t nTankHitTot_;
    uint64_t nTankHit_;
    
    double   cxPE10_;
    double   cxPE20_;
    double   cxPE30_;
    double   cxPE40_;
    double   cxPE50_;
    bool     allCxPERadii_;
    
    uint64_t cxPE30PMT_;
    uint64_t cxPE40PMT_;
    uint64_t cxPE50PMT_;
    
    uint64_t cxPE30XnCh_;
    uint64_t cxPE40XnCh_;
    uint64_t cxPE50XnCh_;
    
    double   cxPE30SPTime_;
    double   cxPE40SPTime_;
    double   cxPE50SPTime_;
  
    bool     pairOut_;
    double   pairRadius_;
    double   pairCharge_;
    uint64_t pairNHit_;

    bool     pincOut_;
    double   pinc_;
    
    bool   	 featuresNNOut_;
		double 	 DisMax_;

    bool     pmtChi2Out_;
    double   pmtChi2_;
    
    double   zenithAngle_;
    double   azimuthAngle_;
    double   dec_;
    double   ra_;
    uint64_t angleFitStatus_;
    int      angleFitType_;
    double   angleFitUnc_;
    double   angleFitChi2_;
    uint64_t angleFitNdof_;
    
    double   planeZenith_;
    double   planeAzimuth_;
    double   planeDec_;
    double   planeRa_;
    uint64_t planeFitStatus_;
    
    double   coreX_;
    double   coreXUnc_;
    double   coreY_;
    double   coreYUnc_;
    double   logCoreAmplitude_;
    double   logCoreAmplitudeUnc_;
    double   showerAge_;
    double   showerAgeUnc_;
    int      coreFitType_;
    double   coreFitUnc_;
    double   coreFitChi2_;
    uint64_t coreFitNdof_;
    uint64_t coreFitStatus_;
    
    double   logNnEnergy_;
    double   fAnnulusCharge0_;
    double   fAnnulusCharge1_;
    double   fAnnulusCharge2_;
    double   fAnnulusCharge3_;
    double   fAnnulusCharge4_;
    double   fAnnulusCharge5_;
    double   fAnnulusCharge6_;
    double   fAnnulusCharge7_;
    double   fAnnulusCharge8_;
    bool     NNEOut_;

    double   logNPE_;
    double   logMaxPE_;

    double   LDFage_;
    double   LDFamp_;
    double   LDFchiSq_;
    bool     latOut_;
    
    double   GamCoreAge_;
    double   GamCoreAmp_;
    double   GamCoreChiSq_;
    double   GamCorePackInt_;
    bool     GamCoreOut_;

    unsigned int coreFiduScale_;

    void PrepFiducialChargeArrays(const int i)
    {
      nScales_ = i;
      fScales_ = dVec(i,0);
      fracQ_ = dVec(i,0);
    }
    
    void SetFiducialChargeArrays(const int i, const double fscale, const double fracQ)
    {
      fScales_[i] = fscale;
      fracQ_[i] = fracQ;
    }

    int GetnScales() const { return nScales_; }
    double GetfScale(const int i) const { return fScales_[i]; }
    double GetFracQ(const int i) const { return fracQ_[i]; }
   
    // Likelihood energy results
    void PrepLHEArrays(const int i)
    {
      nlhe_Species_ = i;
      lhe_Species_ = sVec(i);
      lhe_E_ = dVec(i,0);
      lhe_LH_ = dVec(i,0);
    }
    
    void SetLHEArrays(const int i, const std::string species, const double energy, const double llh)
    {
      lhe_Species_[i] = species;
      lhe_E_[i] = energy;
      lhe_LH_[i] = llh;
    }

    int GetNLHESpecies() const { return nlhe_Species_; }
    std::string Getlhe_Species(const int i) const { return lhe_Species_[i]; }
    double Getlhe_Energy(const int i) const { return lhe_E_[i]; }
    double Getlhe_LH(const int i) const { return lhe_LH_[i]; }

  protected:
    int      nScales_;
    dVec     fScales_;
    dVec     fracQ_;

    int      nlhe_Species_;
    sVec     lhe_Species_;
    dVec     lhe_E_;
    dVec     lhe_LH_;

};

SHARED_POINTER_TYPEDEFS(SummaryResult);

#endif // DATA_STRUCTURES_RECO_SUMMARY_RESULT_H_INCLUDED

