/*!
 * @file LikeHAWC.h
 * @author Patrick Younk
 * @date 25 Jul 2014
 * @brief Store a collection of CalcBin objects to define a complete likelihood.
 * @version $Id: LikeHAWC.h 40100 2017-08-09 00:33:33Z henrike $
 */

#ifndef LIKEHAWC_H
#define LIKEHAWC_H

#include <liff/BinList.h>
#include <liff/CalcBin.h>
#include <liff/ROI.h>

#include <TMinuit.h>

/*!
 * @class LikeHAWC
 * @author Patrick Younk
 * @date 25 Jul 2014
 * @ingroup 
 * @brief Wraps several CalcBin objects together for a likelihood analysis
 */

class LikeHAWC {

 public:
 
  LikeHAWC();

  ~LikeHAWC();

  LikeHAWC(SkyMapCollection *Data, const std::string& DetRes,
           const BinList& binList);

  LikeHAWC(SkyMapCollection *Data, const std::string& DetRes,
           threeML::ModelInterface &model,
           const BinList& binList);

  LikeHAWC(SkyMapCollection *Data, const std::string& DetRes,
           threeML::ModelInterface &model,
           double roiRA, double roiDec, double roiRadius,
           bool roiFixed,
           const BinList& binList);

  LikeHAWC(SkyMapCollection *Data, const std::string& DetRes,
           InternalModelPtr &internal,
           const BinList& binList);

  LikeHAWC(SkyMapCollection *Data, const std::string& DetRes,
           threeML::ModelInterface &model, InternalModelPtr &internal,
           const BinList& binList);

  LikeHAWC(const std::string& mapTreeFile, const std::string& DetRes,
           threeML::ModelInterface &model,
           const BinList& binList, bool loadAllSky = false);

//  LikeHAWC(const std::string& mapTreeFile, double nTransits,
//           const std::string& DetRes, threeML::ModelInterface &model,
//           const BinList& binList, const BinName& binID,
//           bool loadAllSky = false, bool gpdon = false);
  LikeHAWC(const std::string& mapTreeFile, double nTransits,
           const std::string& DetRes, threeML::ModelInterface &model,
           const BinList& binList, const BinName& binID,
           bool loadAllSky = false, bool galactic = false);
  
  LikeHAWC(const std::string& mapTreeFile, double nTransits,
           const std::string& DetRes, threeML::ModelInterface &model,
           const BinList& binList, bool loadAllSky = false);

  LikeHAWC(const std::string& mapTreeFile, const std::string& DetRes,
           const BinList& binList);

  LikeHAWC(const std::string& mapTreeFile, double nTransits,
           const std::string& DetRes, const BinList& binList);

  ///Sets data pointer to SkyMapCollection object
  void SetData(SkyMapCollection *Data);

  ///Sets data pointer to SkyMapCollection object
  void SetData(const std::string& mapTreeFile,
               std::vector<SkyPos> roi, const BinList& binList);

  ///Sets data pointer to SkyMapCollection object, sets number of transits
  void SetData(const std::string& mapTreeFile, double nTransits,
               std::vector<SkyPos> roi, const BinList& binList);

  ///Sets DetectorResponse via file name
  void SetDetectorResponse(const std::string& DetRes);

  ///Sets a specific InternalModel
  void SetInternalModel(InternalModelPtr internal);

  ///Sets up a CalcBin for each analysis bin
  void SetupCalcBins(const BinList& binList);

  ///Define source list from ModelInterface pointer, padding in deg. for ROI
  void ResetSources(threeML::ModelInterface &model, double padding = 5);

  ///Update existing sources to propagate changes in ModelInterface
  void UpdateSources();

  ///Sets ROI matched to source(s) boundaries, default padding 5 deg.
  std::vector<SkyPos> MatchROI(double padding = 5);

  ///Sets ROI via vector of >2 boundary SkyPos, or, for one point source, as
  ///vector of 2 SkyPos, where first is center and second holds radius as RA
  void SetROI(std::vector<SkyPos> ROI, bool fixedROI = true, bool galactic = false);

  
  ///Sets ROI via ra,dec and radius or long, lat and radius
  void SetROI(double ra, double dec, double radius, bool fixedROI, bool galactic = false);
  
  ///Sets ROI of a strip via rastart, rastop, decstart and decstop or in galactic coordinates
  void SetROI(double rastart, double rastop, double decstart, double decstop, bool fixedROI = false, bool galactic = false);

  ///Sets ROI via a healpix mask. Doesn't rely on SkyPos objects.
  void SetROI(std::string mask, double threshold = 0.5, bool fixedROI = true);
  
  ///Sets ROI via vector of 4 boundary SkyPos, or, for one point source, as
  ///vector of 2 SkyPos, where first is center and second holds radius as RA
  ///also, switch for GPD ROI (all of these defined in ROI.cc)
  //void SetROI(std::vector<SkyPos> ROI, const SkyMap<double> *eventMap,
  //            rangeset<int>& skymapPixels, bool GPD = false);
  //void SetROI(std::vector<SkyPos> ROI, bool GPD, bool fixedROI);

  ///Returns ROI as vector of boundary SkyPos, or, for one point source, as
  ///vector of 2 SkyPos, where first is center and second holds radius as RA
  std::vector<SkyPos> GetROISkyPosVector() { return roi_; };

  ///Returns SkyMapCollection pointer to data
  SkyMapCollection *GetData() { return data_; };

  ///Returns pointer to ModelInterface
  threeML::ModelInterface &GetModelInterface() const { return mi_; };

  ///Returns pointer to CalcBins vector
  CalcBinVector &GetCalcBins() { return calcBins_; };

  ///Returns pointer to PointSourceDetectorResponse for given source ID
  PointSourceDetectorResponsePtr GetPointSourceDetectorResponse(int sID);

  ///Returns pointer to ExtendedSourceDetectorResponse for given source ID
  ExtendedSourceDetectorResponsePtr GetExtendedSourceDetectorResponse(int sID);

  ///Returns pointer to InternalModel
  InternalModelPtr GetInternalModel() { return internal_; };

  ///Calculates and returns Log-Likelihood for simple model: data = BG
  double CalcBackgroundLogLikelihood(bool doIntFit = true);

  ///Calculates and returns Log-Likelihood (LL)
  double CalcLogLikelihood(bool doIntFit = true);

  ///Update sources (required if model or DR changed) and returns LL
  double CalcLogLikelihoodUpdateSources(bool doIntFit = true);

  ///Sets new model, calculates and returns LL(Model)
  double CalcLogLikelihood(threeML::ModelInterface &model, bool doIntFit = true);

  ///Calculates and returns LL(Model) - LL(BG)
  double CalcTestStatistic(bool doIntFit = true);

  ///Update sources (required if model or DR changed), returns LL(M)-LL(BG)
  double CalcTestStatisticUpdateSources(bool doIntFit = true);

  ///Sets new model, calculates and returns LL(Model) - LL(BG)
  double CalcTestStatistic(threeML::ModelInterface &model, bool doIntFit = true);

  ///A Gaussian approx. to the CommonNorm and Sigma for first guess
  void EstimateNormAndSigma(double &Norm, double &Sigma);

  ///Calculates and returns Log-Likelihood for simple model: data = BG
  double CalcTopHatBackgroundLogLikelihood
      (SkyPos center, std::vector<double> radius, bool doIntFit = true);

  ///Calculates and returns Log-Likelihood (LL)
  double CalcTopHatLogLikelihood
      (SkyPos center, std::vector<double> radius, bool doIntFit = true);

  ///Update sources (required if model or DR changed) and returns LL
  double CalcTopHatLogLikelihoodUpdateSources
      (SkyPos center, std::vector<double> radius, bool doIntFit = true);

  ///Sets new model, calculates and returns LL(Model)
  double CalcTopHatLogLikelihood(threeML::ModelInterface &model,
                                 SkyPos center, std::vector<double> radius, bool doIntFit = true);

  ///Calculates and returns LL(Model) - LL(BG)
  double CalcTopHatTestStatistic
      (SkyPos center, std::vector<double> radius, bool doIntFit = true);

  ///Update sources (required if model or DR changed), returns LL(M)-LL(BG)
  double CalcTopHatTestStatisticUpdateSources
      (SkyPos center, std::vector<double> radius, bool doIntFit = true);

  ///Sets new model, calculates and returns LL(Model) - LL(BG)
  double CalcTopHatTestStatistic(threeML::ModelInterface &model,
                                 SkyPos center, std::vector<double> radius, bool doIntFit = true);

  ///A Gaussian approx. to the CommonNorm and Sigma for first guess
  void EstimateTopHatNormAndSigma(double &Norm, double &Sigma,
                                  SkyPos center, std::vector<double> radius);

  ///Prints out info on Calculation bins (for touble shooting)
  void PrintCalcBinsInfo();

  ///Makes model maps in all CalcBins
  void MakeModelMap();

  ///Write model map to a maptree
  void WriteModelMap(std::string fileName, bool poisson = false);

  ///Write residual map to a maptree
  void WriteResidualMap(std::string fileName);

  ///----------------------------------------------------------------------
  /// All public methods below are direct links to methods of the
  /// InternalModel

  ///Reference toCommonNorm, multiplied to the flux of all sources
  double &CommonNorm() { return internal_->CommonNorm(); };

  ///Reference to CommonNormError
  double &CommonNormError() { return internal_->CommonNormError(); };

  ///Returns if CommonNorm is free in internal likelihood  fit
  bool IsCommonNormFree() const { return internal_->IsCommonNormFree(); };

  ///Switch CommonNorm internal likelihood fit on/off
  void SetCommonNormFree(bool cf = true) { internal_->SetCommonNormFree(cf); };

  void SetCommonNorm(double value);

  /*
  ///Background normalization, multiplied to all BG values (in Map/Model)
  double & BackgroundNorm()
    { return internal_->BackgroundNorm(); };

  ///BackgroundNormError
  double & BackgroundNormError()
    { return internal_->BackgroundNormError(); };
  */

  ////Returns if CommonNorm is free in internal likelihood  fit
  bool IsBackgroundNormFree() const { return internal_->IsBackgroundNormFree(); };

  ///Switch CommonNorm internal likelihood fit on/off
  void SetBackgroundNormFree(bool bf = true) { internal_->SetBackgroundNormFree(bf); };

  ///Uses BGMap from data in each bin directly, no BGModel
  void UseBackgroundFromData() { internal_->UseBackgroundFromData(); };

  ///Sets the BGModel via a TF2, only given par.IDs free after initial fit
  void SetBackgroundModel(TF2Ptr BGModel, std::vector<int> FreeBGParIDs) {
    internal_->SetBackgroundModel(BGModel,
                                  FreeBGParIDs);
  };

  ///Returns the BGModel TF2 poniter
  TF2Ptr GetBackgroundModel() const { return internal_->GetBackgroundModel(); };

  ///Returns the BGModel free parameter ID list
  std::vector<int> GetFreeBackgroundParameterIDList() const { return internal_->GetFreeBackgroundParameterIDList(); };

  ///Returns free parameter list
  const FreeParameterList &GetFreeParameterList() { return internal_->GetFreeParameterList(); };

  ///Clears free parameter list
  void ClearFreeParameterList() { internal_->ClearFreeParameterList(); };

  ///Adds new free parameter, indicate if DetectorResponse changes
  void AddFreeParameter(const char *func, int ParId, bool detResFree = false) {
    internal_->AddFreeParameter(func,
                                ParId,
                                detResFree);
  };

  ///Adds new free parameter, indicate if DetectorResponse changes
  void AddFreeParameter(TF1Ptr FuncPointer, int ParId, bool detResFree = false) {
    internal_->AddFreeParameter(FuncPointer,
                                ParId,
                                detResFree);
  };

  ///Sets MINUIT verbosity: -1 = no printing, 0 = a little, 1 = full
  void SetInternalFitVerbosity(int Verbosity = -1) { internal_->SetInternalFitVerbosity(Verbosity); };

  ///Returns MINUIT verbosity levl
  int GetInternalFitVerbosity() { return internal_->GetInternalFitVerbosity(); };

  ///Get the TopHat excess for all bins as expected from the current model
  std::vector<double> GetTopHatExpectedExcesses(double ra, double dec, double countradius);

  ///Get the TopHat excesses measured in all bins
  std::vector<double> GetTopHatExcesses(double ra, double dec, double countradius);

  ///Get the TopHat background in all bins
  std::vector<double> GetTopHatBackgrounds(double ra, double dec, double countradius);

  ///Get the pixel area of all bins
  std::vector<double> GetTopHatAreas(double ra, double dec, double countradius);

  //Get a vector of energy to cache flux for extended sources
  //Assuming it is the same for all sources and all bins
  std::vector<double> GetEnergies(bool reset = false);

  //Get a vector of positions to cache flux for each extended soure
  std::vector<std::pair<double, double> > GetPositions(int esId, bool reset=true);


  ///accessible for internal minimizer
  SkyPos topHatCenter;
  std::vector<double> topHatRadius;


 private:

  SkyMapCollection *data_;

  threeML::ModelInterface &mi_;

  double padding_;

  std::string detRes_;

  InternalModelPtr internal_;

  std::vector<SkyPos> roi_;

  bool fixedROI_;
  
  bool GPD_;

  PointSourceDetectorResponseVector pointSources_;
  ExtendedSourceDetectorResponseVector extendedSources_;

  CalcBinVector calcBins_;

  ///Minimizes -LL via free parameters in InternalModel
  //int InternalMinimize(int Verbosity=-1);

  std::vector<double> energies_;

};

extern LikeHAWC *gLikeHAWC;  //declaration of a global variable

#endif
