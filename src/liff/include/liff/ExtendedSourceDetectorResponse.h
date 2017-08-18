/*!
 * @file ExtendedSourceDetectorResponse.h
 * @author Robert Lauer
 * @date 4 February 2015
 * @brief definition of SourceDetectorResponse class
 * @version $Id: ExtendedSourceDetectorResponse.h 36120 2016-12-02 10:39:12Z samm $
 */

#ifndef EXTENDED_SOURCE_DETECTOR_RESPONSE_H_INCLUDED
#define EXTENDED_SOURCE_DETECTOR_RESPONSE_H_INCLUDED

#include <hawcnest/Logging.h>
#include <hawcnest/HAWCUnits.h>

#include <liff/BinList.h>
#include <liff/DetectorResponse.h>
// #include <liff/FixedExtendedSource.h>
#include <liff/ModelInterface.h>
#include <liff/skymaps/SkyMap.h>

// #include <threeML/ModelInterface.h>

#include <TF1.h>

#include <map>
#include <utility>
#include <math.h>
#include <sys/stat.h>

#include <alm.h>
#include <xcomplex.h>
#include <healpix_map.h>
#include <alm_healpix_tools.h>
#include <alm_powspec_tools.h>

#include <fftw3.h>

/*!
 * @class ExtendedSourceDetectorResponse
 * @author Robert Lauer
 * @date 4 Feb 2014
 * @ingroup
 * @brief class for response objects describing one extended source
 */


class ExtendedSourceDetectorResponse {

 public:

  // GV: I assume the following constructor is useless, because what is the point of building an object like this
  // without model?

  /*
  ExtendedSourceDetectorResponse()
    : sourceId_(-1),
      numRegions_(0),
      minra_(-1000),
      maxra_(-1000),
      mindec_(-1000),
      maxdec_(-1000),
      fftwFP_(0),
      fftwBP_(0),
      fftwIn_(0),
      fftwOut_(0),
      gridRA_(0),
      gridDec_(0)
  {} */

  ExtendedSourceDetectorResponse(std::string dr,
                                 threeML::ModelInterface &mi,
                                 const int sourceId)
      : sourceId_(sourceId),
        mi_(mi),
        nside_(0),
        fftwFP_(0),
        fftwBP_(0),
        fftwIn_(0),
        fftwOut_(0),
        gridRA_(0),
        gridDec_(0) {
    dr_ = DetectorResponse(dr);
    SetModel(mi);
  }

  ~ExtendedSourceDetectorResponse() {
    if (fftwFP_) fftw_destroy_plan(fftwFP_);
    if (fftwBP_) fftw_destroy_plan(fftwBP_);
    if (fftwIn_) delete[] fftwIn_;
    if (fftwOut_) delete[] fftwOut_;
  }

  void SetModel(threeML::ModelInterface &mi, bool reconvolute = false);

  int GetSourceID() const { return sourceId_; }

  int GetNumRegions() const { return numRegions_; }

  double GetMinRA() const { return minra_; }

  double GetMaxRA() const { return maxra_; }

  double GetMinDec() const { return mindec_; }

  double GetMaxDec() const { return maxdec_; }

  double GetDecRegionLowerEdge(const int regionId) const;

  double GetDecRegionUpperEdge(const int regionId) const;

  TF1Ptr GetPsfFunction(const BinName& nhbin, const double ra,
                        const double dec);

  void ConvolutePSF(const BinName& nhbin, rangeset<int> &roiPix);

  void RescaleCounts();

  double GetExtendedSourceConvolutedSignal(const BinName& nhbin,
                                           const int nside,
                                           rangeset<int> &roiPix, const int hp);

  double GetExpectedSignal(const BinName& nhbin,
                           const double ra, const double dec);

  std::pair<TH1D, TH1D> CalculatePixelatedFTPsf(const BinName& nhbin,
                                                const double ra,
                                                const double dec,
                                                const double dGrid,
                                                const int nGrid);

  //Get a vector of positions to cache flux for each extended source
  std::vector<std::pair<double, double> > GetPositions(int nside, bool reset = true);

  //ResponseBin* GetBin(const int nhbin){return dr_.GetBin(decBinId_,nhbin);};

 private:
 
  typedef std::pair<BinName, int> BinPair;
  typedef std::map<BinName, SkyMap<double> > MapMap;

  int sourceId_;
  int numRegions_;
  std::vector<int> decBinId_;
  double minra_;
  double maxra_;
  double mindec_;
  double maxdec_;
  std::vector<double> decLowerEdge_;
  std::vector<double> decUpperEdge_;
  DetectorResponse dr_;
  threeML::ModelInterface &mi_;
  void checkRegionId(int regionId) const;
  int nside_;
  MapMap convolutedExpectedSignalMap_; //nhbin
  std::map<BinName, std::pair<SkyPos, double> > prevCount_;
  fftw_plan fftwFP_;
  fftw_plan fftwBP_;
  double *fftwIn_;
  fftw_complex *fftwOut_;
  int gridRA_;
  int gridDec_;
  std::map<BinPair, std::pair<TH1D, TH1D> > pixelatedFTPsf_;
  std::vector<std::pair<double, double> > positions_;
  rangeset<int> healpixIds_;
};

SHARED_POINTER_TYPEDEFS(ExtendedSourceDetectorResponse);

typedef std::vector<ExtendedSourceDetectorResponsePtr> ExtendedSourceDetectorResponseVector;

#endif // EXTENDED_SOURCE_DETECTOR_RESPONSE_H_INCLUDED
