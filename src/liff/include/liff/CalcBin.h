/*!
 * @file CalcBin.h
 * @author Patrick Younk
 * @date 25 Jul 2014
 * @brief Calculate expected counts and likelihood in skymap bins.
 * @version $Id: CalcBin.h 40100 2017-08-09 00:33:33Z henrike $
 */

#ifndef CALC_BIN_H
#define CALC_BIN_H

#include <liff/BinList.h>
#include <liff/SkyMapCollection.h>
#include <liff/PointSourceDetectorResponse.h>
#include <liff/ExtendedSourceDetectorResponse.h>
#include <liff/InternalModelBin.h>


/*!
 * @class CalcBin
 * @author Patrick Younk
 * @date 25 Jul 2014
 * @ingroup 
 * @brief Calculates Expected Counts in Sky Bins and Likelihood Values
 */

class CalcBin {

 public:

  friend class LikeHAWC;

  ///CalcBin constructor
  CalcBin(const BinName& binID,
          SkyMapCollection *skyMaps,
          PointSourceDetectorResponseVector &pointSources,
          ExtendedSourceDetectorResponseVector &extendedSources,
          InternalModelPtr internalModel,
          std::vector<SkyPos> roi
          );

  ///Pointer to the ON data, set it to double in case of residual maps
  SkyMap<double> *eventMap_;

  ///Pointer to the OFF data
  SkyMap<double> *backgroundMap_;

  ///Pointer to the MODEL
  SkyMap<double> *modelMap_;

  ///Sets pixels based on an ROI (disc or polygon, see LikeHAWC::MatchROI)
  void SetROIPixels(std::vector<SkyPos> roi);

  ///Sets pixels based on an ROI with healpix map
  void SetROIPixels(std::string, double threshold);

  ///Returns the ROI as a rangeset
  rangeset<int> &GetROIPixels() { return roiPix_; };

  ///Returns bin ID
  const BinName& GetBinID() { return binID_; };

  ///Returns number of expected counts for a given healpix pixel ID
  double GetPerPixelExpectedExcess(int hp);

  ///Returns correction of BG due to expected signal for given healpix pixID
  double GetPerPixelExpectedBackgroundCorrection(int hp);

  ///Returns the signal summed over all pixels within a given radius
  double GetTopHatExpectedExcess(SkyPos center, double radius = 2.);

  ///Return the total signal summed over all pixels within a given radius
  double GetTopHatExcess(SkyPos center, double radius = 2.);

  ///Returns the OFF counts summed over all pixels within a given radius
  double GetTopHatBackground(SkyPos center, double radius = 2.);

  ///Returns the total area of all pixels within a given radius
  double GetTopHatArea(SkyPos center, double radius = 2.);

  ///Returns the Log Likelihood
  double CalcLogLikelihood();

  ///Returns Log Likelihood for BG-only
  double CalcBackgroundLogLikelihood();

  ///These numbers are used for Gaussian Approximations
  void CalcWeights(double &sumExpWeighted, double &sumSignalWeighted,
                   double &sumBGWeighted);

  ///Returns the TopHat Log Likelihood
  double CalcTopHatLogLikelihood(SkyPos center, double radius);

  ///Returns TopHat Log Likelihood for BG-only
  double CalcTopHatBackgroundLogLikelihood(SkyPos center, double radius);

  ///These numbers are used for TopHat Gaussian Approximations
  void CalcTopHatWeights(double &sumExpWeighted, double &sumSignalWeighted,
                         double &sumBGWeighted, SkyPos center, double radius);

  ///Returns pointer to InternalModelBin
  InternalModelBin &GetInternalModelBin() { return imb_; };

  ///Prints info to cout
  void PrintInfo();

  ///Make model map. If add, add to the current model map instead
  void MakeModelMap(bool add=false);

 private:

  BinName binID_;
  int nside_;
  double pixelArea_;
  double diDuration_;
  double numTransits_;
  PointSourceDetectorResponseVector &pointSources_;
  ExtendedSourceDetectorResponseVector &extendedSources_;
  InternalModelBin imb_;
  rangeset<int> roiPix_;
  rangeset<int> skyMapPixels_;

  double minOnCount_; //minimum ON count, to avoid log(negative/0)

  std::map<int, double> expectedSignalHash_;

  std::map<int, double> expectedBGCorrectionHash_;

  std::map<double, double> topHatExcessHash_;

  std::map<double, double> topHatBackgroundHash_;

  std::map<double, double> topHatExpectedExcessHash_;

  std::map<int, SkyPos> pixelCenterHash_;

  //double GetPointSourceConvolutedSignal(int sID, double distance);

  //double GetExtendedSourceConvolutedSignal(int sID, int hp);

  /*
  ///Caches the Log Likelihood for the model: data = OFF
  void CacheBGLogLikelihood();
  */

};
SHARED_POINTER_TYPEDEFS(CalcBin);

typedef std::vector<CalcBinPtr> CalcBinVector;

#endif
