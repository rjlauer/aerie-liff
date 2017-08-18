/*!
 * @file PointSourceDetectorResponse.h
 * @author Robert Lauer
 * @date 4 February 2015
 * @brief definition of SourceDetectorResponse class
 * @version $Id: PointSourceDetectorResponse.h 36120 2016-12-02 10:39:12Z samm $
 */

#ifndef POINT_SOURCE_DETECTOR_RESPONSE_H_INCLUDED
#define POINT_SOURCE_DETECTOR_RESPONSE_H_INCLUDED

#include <hawcnest/Logging.h>

#include <liff/BinList.h>
#include <liff/DetectorResponse.h>
#include <liff/ModelInterface.h>

//#include <threeML/ModelInterface.h>

#include <TF1.h>
#include <TMath.h>

#include <map>
#include <math.h>
#include <sys/stat.h>
#include <hawcnest/PointerTypedefs.h>

/*!
 * @class PointSourceDetectorResponse
 * @author Robert Lauer
 * @date 4 Feb 2014
 * @ingroup
 * @brief class for response objects describing one extended source
 */

class PointSourceDetectorResponse {

  public:
  /*
      PointSourceDetectorResponse()
        : sourceId_(-1),
          decBinId1_(-1),
          decBinId2_(-1),
          ra_(-1000),
          dec_(-1000)
      {} 
  */
    PointSourceDetectorResponse(std::string dr,
                                threeML::ModelInterface &mi,
                                const int sourceId)
        : sourceId_(sourceId),
          decBinId1_(-1),
          decBinId2_(-1),
          ra_(-1000),
          dec_(-1000),
          mi_(mi) {
      dr_ = DetectorResponse(dr);
      SetModel(mi);
    }

    //returns 1 if the position has changed, zero otherwise
    int SetModel(threeML::ModelInterface &mi, bool detResFree = false);

    void SetSkyPos(SkyPos pos);

    void ReweightEnergies();

    int GetSourceID() const { return sourceId_; }

    double GetRA() const { return ra_; }

    double GetDec() const { return dec_; }

    ///////////////////////#warning Fix and test me: SkyPos GetSkyPos() const { return skypos_; }
    SkyPos GetSkyPos() const { return SkyPos(ra_, dec_); }

    double GetDecRegionLowerEdge() const { return decLowerEdge_; };

    double GetDecRegionUpperEdge() const { return decUpperEdge_; };

    TF1Ptr GetPsfFunction(const BinName& nhbin);

    double GetExpectedSignal(const BinName& nhbin);

    ResponseBinPtr GetBin(const BinName& nhbin) { return dr_.GetBin(decBinId1_, nhbin); };

    DetectorResponse *GetDetectorResponse() { return &dr_; };

    double GetSmearedSignal(double distance, double pixelArea,
                            const BinName& nhbin);

    TH1D CalculatePixelatedPsf(double pixelArea, const BinName& nhbin);

    bool IsPSFDeltaFunction(double pixelArea, const BinName& nhbin);

    bool IsPSFDoubleGaussian(const BinName& nhbin);

    // The following three members functions are actually for extended searches, but this class is much better suited for them that ExtendedSourceDetectorResponse
    // In the future they should go into their own class, something like DiscSourceDetectorResponse, which would have the same members as this one but reaplacing the PSF by the DSF
    // ExtendedSourceDetectorResponse class is too general for this, and the idea behind its implementation does not match this idea
  
    //TF1 wrapper of ComputeDSF ("Disc spread function"). Only works when the PSF is a double gaussian.
    TF1Ptr GetDsfFunction(const double discR, const BinName& nhbin);

    //Returns a histogram whose bin values correspond to the fraction of the "Disc Spread Function" contained in a pixel at the distance of the bin center
    TH1D CalculatePixelatedDsf(const double discR ,const double pixelArea,
                               const BinName& nhbin);  
  
  private:
  
    typedef std::pair<BinName, int> BinPair;
  
    //Computes the "Disc Spread Function", to be wrapped by a TF1. Parameters are the same as for the double gaussian plus [4] = disc radius
    double ComputeDSF(double *radius, double *parameters);

    int sourceId_;
    int decBinId1_;
    int decBinId2_;
    double w1_;
    double w2_;
    double ra_;
    double dec_;
    double decLowerEdge_;
    double decUpperEdge_;
    DetectorResponse dr_;
    threeML::ModelInterface &mi_;
    std::map<BinPair, TH1D> pixelatedPsf_;
    std::map<BinName, bool> deltaFunctionPSF_;
    SkyPos skypos_;
};

// The following creates the typedef PointSourceDetectorResponsePtr (and PointSourceDetectorResponsePtrConst)

SHARED_POINTER_TYPEDEFS(PointSourceDetectorResponse);

typedef std::vector<PointSourceDetectorResponsePtr> PointSourceDetectorResponseVector;


#endif // POINT_SOURCE_DETECTOR_RESPONSE_H_INCLUDED
