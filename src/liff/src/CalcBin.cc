/*!
 * @file CalcBin.cc
 * @author Patrick Younk
 * @date 25 Jul 2014
 * @brief Calculate expected counts and likelihood in skymap bins.
 * @version $Id: CalcBin.cc 40122 2017-08-10 16:19:48Z henrike $
 */

#include <liff/BinList.h>
#include <liff/CalcBin.h>
#include <liff/SkyMapCollection.h>

#include <liff/ROI.h>

#include <hawcnest/HAWCUnits.h>

#include <data-structures/geometry/R3Transform.h>
#include <data-structures/astronomy/EquPoint.h>
#include <data-structures/astronomy/GalPoint.h>
#include <data-structures/astronomy/AstroCoords.h>

using namespace std;
using namespace HAWCUnits;

// Define rotation matrix for galactic-equatorial transformations at the level
// of this C++ translation unit. This is a hack to get the code independent of
// astro-service so that it can work with python bindings in 3ML. Will think of
// a better approach in the future.
namespace {

  // Galactic north pole in J2000.0 equatorial coordinates
  const EquPoint gnpJ2000(HrMinSec(12*hour, 51*minute, 26.28*second),
                          DegMinSec(27*degree, 7*arcminute, 41.70*arcsecond));

  // Galactic center in J2000.0 equatorial coordinates
  const EquPoint gcnJ2000(HrMinSec(17*hour, 45*minute, 37.2*second),
                         -DegMinSec(28*degree, 56*arcminute, 10.22*arcsecond));

  // Equatorial to galactic rotation matrix
  const Rotate e2gMtx(gcnJ2000.GetPoint(), gnpJ2000.GetPoint(),
                      XAXIS, ZAXIS);

  // Galactic to equatorial rotation matrix
  const Rotate g2eMtx(XAXIS, ZAXIS,
                      gcnJ2000.GetPoint(), gnpJ2000.GetPoint());

  void equ2gal(const EquPoint& equ, GalPoint& gal) {
    gal.SetPoint(e2gMtx * equ.GetPoint());
  }

  void gal2equ(const GalPoint& gal, EquPoint& equ) {
    equ.SetPoint(g2eMtx * gal.GetPoint());
  }

}

bool notSequential(int a, int b) { return (a + 1) != b; }
bool GPD_ = false; /// will be moved later...

/*****************************************************/
CalcBin::CalcBin(const BinName& binID,
                 SkyMapCollection *skyMaps,
                 PointSourceDetectorResponseVector &pointSources,
                 ExtendedSourceDetectorResponseVector &extendedSources,
                 InternalModelPtr internalModel,
                 vector<SkyPos> roi
                 )
    : binID_(binID), pointSources_(pointSources), extendedSources_(extendedSources) {

  //Set skyMaps
  eventMap_ = skyMaps->GetEventMap(binID_);
  backgroundMap_ = skyMaps->GetBackgroundMap(binID_);
  skyMapPixels_ = skyMaps->GetPixels(binID_);
  //Set Model
  modelMap_ = skyMaps->GetModelMap(binID_);
  //Calculate pixel area
  nside_ = eventMap_->Nside();
  int nside_off = backgroundMap_->Nside();
  if (nside_ != nside_off) {
    log_fatal("Bin " << binID_ << "<<: Healpix NSide for background map (" <<
        nside_off << ") does not match NSide for data map (" << nside_
                  << ").");
  }
  pixelArea_ = 4 * pi / degree / degree / 12. / nside_ / nside_;
  //DirectIntegration duration
  diDuration_ = skyMaps->GetIntegrationDuration(binID_);
  //Number of transits
  numTransits_ = skyMaps->GetTransits();
  pointSources_ = pointSources;
  extendedSources_ = extendedSources;
  SetROIPixels(roi);

  imb_ = InternalModelBin(binID_, internalModel, backgroundMap_,
                          skyMaps->GetSkyPosVector(binID_), true);
  //If internalModel points to a BG model, the default is to fit the BGModel once
  //(via ROOT) to the OFF-data within the loaded SkyMapCollection region,
  //with all parameters in internalModel:BGModel left free
  //If last argument is "false", this is not done.

  //minimum ON count to remove zeros for 1/sqrt(BG) in Gaussian weights
  //and keep Minimizer away from negative/zero expEvt during LL-calculation
  minOnCount_ = numeric_limits<double>::min();
  log_trace("  Minimum ON-count value set to:  " << minOnCount_);
}

/*****************************************************/
//new version of SetROIPixels to go with ROI.h and ROI.cc
//and with galactic plane diffuse model roi added
//commented out for now.

void CalcBin::SetROIPixels(vector<SkyPos> roi) {

  if (!eventMap_) {
    log_fatal("No data-map defined for CalcBin with ID " << binID_);
  }
  roiPix_.clear();
  //galactic plane diffuse model
  if (GPD_) {
    log_debug("Setting ROI GPD");
    GPDROI my_roi;
    my_roi.CalcROI(eventMap_, roi, skyMapPixels_);
    roiPix_ = my_roi.GetPixelList();
  }
  else {
    if (roi.size() == 2) {
      //disc
      log_debug("Setting ROI disk (" << roi[0].RA() << ", " << roi[0].Dec()
                << ") " << roi[1].RA());
      DiscROI my_roi;
      my_roi.CalcROI(eventMap_, roi, skyMapPixels_);
      roiPix_ = my_roi.GetPixelList();
    }
    else {
      //polygon
      log_debug("Setting ROI polygon");
      PolygonROI my_roi;
      my_roi.CalcROI(eventMap_, roi, skyMapPixels_);
      roiPix_ = my_roi.GetPixelList();
    }
  }

  log_debug("Number of ROI pixels: " << roiPix_.nval());
  
  //check if SkyMapCollection region contains all of ROI:
  if (!skyMapPixels_.contains(roiPix_)) {
    log_debug("Pixels in SkyMapCollection for bin " << binID_ << " : " << skyMapPixels_.nval());
    log_debug("Pixels in ROI for bin " << binID_ << " :              " << roiPix_.nval());
    int firstROIPix = roiPix_.ivbegin(0);
    log_debug("first ROI pixel " << firstROIPix << 
              " " << SkyPos(eventMap_->pix2ang(firstROIPix)).RA() <<
              " " << SkyPos(eventMap_->pix2ang(firstROIPix)).Dec());
    log_trace("skyMapPixels_ pixels " << skyMapPixels_);
    log_trace("roiPix_ pixels " << roiPix_);
    rangeset<int> missingPix = roiPix_;
#if HEALPIX_VERSION < 330
    missingPix.subtract(skyMapPixels_);
#else
    missingPix = missingPix.op_andnot(skyMapPixels_);
#endif
    log_trace("missingPix pixels " << missingPix);
    int firstMissingPix = missingPix.ivbegin(0);
    int lastMissingPix = missingPix.ivbegin(missingPix.size()-1) + 
                         missingPix.ivlen(missingPix.size()-1) - 1;
    log_debug("first missing pixel " << firstMissingPix << 
              " " << SkyPos(eventMap_->pix2ang(firstMissingPix)).RA() <<
              " " << SkyPos(eventMap_->pix2ang(firstMissingPix)).Dec());
    log_debug("last missing pixel " << lastMissingPix << 
              " " << SkyPos(eventMap_->pix2ang(lastMissingPix)).RA() <<
              " " << SkyPos(eventMap_->pix2ang(lastMissingPix)).Dec());
    log_fatal("The SkyMap region loaded from data does not (fully) contain " <<
        "the region-of-interest (probably defined to include all sources)");
  }
}

//void CalcBin::SetROIPixels(vector<SkyPos> roi) {
//
//  if (!eventMap_) {
//    log_fatal("No data-map defined for CalcBin with ID " << binID_);
//  }
//  roiPix_.clear();
//  if (roi.size() == 2) {
//    //one point source with disc-like ROI:
//    log_debug("Setting ROI disk (" << roi[0].RA() << ", " << roi[0].Dec()
//              << ") " << roi[1].RA());
//    eventMap_->query_disc(roi[0].GetPointing(), roi[1].RA() * degree, roiPix_);
//  }
//  else {
//    //polygon
//    log_debug("Setting ROI polygon");
//    vector<pointing> pol;
//    for (vector<SkyPos>::iterator p = roi.begin();
//         p != roi.end(); p++) {
//      pol.push_back(p->GetPointing());
//    }
//    eventMap_->query_polygon(pol, roiPix_);
//  }
//
//  //check if SkyMapCollection region contains all of ROI:
//  if (!skyMapPixels_.contains(roiPix_)) {
//    log_debug("Pixels in SkyMapCollection for bin " << binID_ << " : " << skyMapPixels_.nval());
//    log_debug("Pixels in ROI for bin " << binID_ << " :              " << roiPix_.nval());
//    int firstROIPix = roiPix_.ivbegin(0);
//    log_debug("first ROI pixel " << firstROIPix << 
//              " " << SkyPos(eventMap_->pix2ang(firstROIPix)).RA() <<
//              " " << SkyPos(eventMap_->pix2ang(firstROIPix)).Dec());
//    log_trace("skyMapPixels_ pixels " << skyMapPixels_);
//    log_trace("roiPix_ pixels " << roiPix_);
//    rangeset<int> missingPix = roiPix_;
//    missingPix.subtract(skyMapPixels_);
//    log_trace("missingPix pixels " << missingPix);
//    int firstMissingPix = missingPix.ivbegin(0);
//    int lastMissingPix = missingPix.ivbegin(missingPix.size()-1) + 
//                         missingPix.ivlen(missingPix.size()-1) - 1;
//    log_debug("first missing pixel " << firstMissingPix << 
//              " " << SkyPos(eventMap_->pix2ang(firstMissingPix)).RA() <<
//              " " << SkyPos(eventMap_->pix2ang(firstMissingPix)).Dec());
//    log_debug("last missing pixel " << lastMissingPix << 
//              " " << SkyPos(eventMap_->pix2ang(lastMissingPix)).RA() <<
//              " " << SkyPos(eventMap_->pix2ang(lastMissingPix)).Dec());
//    log_fatal("The SkyMap region loaded from data does not (fully) contain " <<
//        "the region-of-interest (probably defined to include all sources)");
//  }
//}

/*****************************************************/
void CalcBin::SetROIPixels(string mask, double threshold) {

  log_debug("Setting ROI from healpix map");
  if (!eventMap_) {
    log_fatal("No data-map defined for CalcBin with ID " << binID_);
  }
  roiPix_.clear();

  /*
  //with map-maker dependency, using RoIMask class:
  RoIMask m(eventMap_->Nside());
  m.addThresholdMap(mask,threshold,true); //Masked map with ones and zeros. 
  vector<int> pixels = m.getMaskedPixels();

  instead, we now have a function in liff/Util.h that does the same:
  */
  //with reimplemented RoIMask::getMaskedPixels in liff/Util.h, avoiding dependency
  vector<int> pixels = maskPixels(mask,threshold,true,eventMap_->Nside());
  
  //Convert pixels into rangesets
  int v1=0;
  int nr=0;
  vector<int>::iterator it = pixels.begin();
  vector<int>::iterator end = pixels.end();
  log_debug("Initial pixel: "<<*it);
  log_debug("Final pixel: "<<*--end);
  while (it != end)
  {
    v1 = *it;
    it = adjacent_find(it,end,notSequential);
    if (it != end)
    {
      log_debug("Ranges: "<<v1<<" "<<*it);
      roiPix_.add(v1,*it+1);
      ++it;
      nr++;
    }
  }
  log_debug("Ranges: "<<v1<<" "<<*(pixels.end()-1));
  roiPix_.add(v1,*(pixels.end()-1)+1);
  nr++;
  log_debug("N Ranges: "<<nr);
  log_debug("Finishing getting the rangeset")  
  log_debug("Number of ROI pixels: " << roiPix_.nval());

  //check if SkyMapCollection region contains all of ROI:
  if (!skyMapPixels_.contains(roiPix_)) {
    log_debug("Pixels in SkyMapCollection for bin " << binID_ << " : " << skyMapPixels_.nval());
    log_debug("Pixels in ROI for bin " << binID_ << " :              " << roiPix_.nval());
    int firstROIPix = roiPix_.ivbegin(0);
    log_debug("first ROI pixel " << firstROIPix << 
              " " << SkyPos(eventMap_->pix2ang(firstROIPix)).RA() <<
              " " << SkyPos(eventMap_->pix2ang(firstROIPix)).Dec());
    log_trace("skyMapPixels_ pixels " << skyMapPixels_);
    log_trace("roiPix_ pixels " << roiPix_);
    rangeset<int> missingPix = roiPix_;
#if HEALPIX_VERSION < 330
    missingPix.subtract(skyMapPixels_);
#else
    missingPix = missingPix.op_andnot(skyMapPixels_);
#endif
    log_trace("missingPix pixels " << missingPix);
    int firstMissingPix = missingPix.ivbegin(0);
    int lastMissingPix = missingPix.ivbegin(missingPix.size()-1) + 
                         missingPix.ivlen(missingPix.size()-1) - 1;
    log_debug("first missing pixel " << firstMissingPix << 
              " " << SkyPos(eventMap_->pix2ang(firstMissingPix)).RA() <<
              " " << SkyPos(eventMap_->pix2ang(firstMissingPix)).Dec());
    log_debug("last missing pixel " << lastMissingPix << 
              " " << SkyPos(eventMap_->pix2ang(lastMissingPix)).RA() <<
              " " << SkyPos(eventMap_->pix2ang(lastMissingPix)).Dec());
    log_fatal("The SkyMap region loaded from data does not (fully) contain " <<
        "the region-of-interest (probably defined to include all sources)");
  }
}


/*****************************************************/
//Here we need something nice to convolute PSF and extended source counts
/*
double CalcBin::GetExtendedSourceConvolutedSignal
  (int sID, int hp) {
  SkyPos pixelCenter(ON->pix2ang(hp));
  vector<int> listpix;
  ON->query_disc_inclusive(pixelCenter.GetPointing(),3.*degree,listpix);
  double ra = pixelCenter.RA();
  double dec = pixelCenter.Dec();
  double expectedSignalPerSr=0;//(*extendedSources_)[sID].GetExpectedSignal(binID_,ra,dec);
  for (vector<int>::iterator p = listpix.begin();
       p != listpix.end(); p++) {
    SkyPos Pixel(ON->pix2ang(*p));
    double distance = pixelCenter.Angle(Pixel);
    expectedSignalPerSr+=(*extendedSources_)[sID].GetExpectedSignal(binID_,Pixel.RA(),Pixel.Dec()) * 
                          (*extendedSources_)[sID].GetPSF(distance,pixelArea_,binID_,Pixel.RA(),Pixel.Dec());

  }
  double expectedSignalPerPixel = expectedSignalPerSr;// *pixelArea_*degree*degree;
  return expectedSignalPerPixel;
}
*/


/*****************************************************/
// the int-argument is the healpix pixel-ID for the scheme and 
// NSide set by the data and background maps for this bin.
double CalcBin::GetPerPixelExpectedExcess(int hp) {

  /*
  if (expectedSignalHash_.count(hp)>0) {
    return (imb_.CommonNorm())*expectedSignalHash_[hp];
  }*/

  SkyPos *pixelCenter;

  try {

    pixelCenter = &pixelCenterHash_.at(hp);

  } catch (...) {

    pixelCenterHash_[hp] = SkyPos(eventMap_->pix2ang(hp));

    pixelCenter = &pixelCenterHash_.at(hp);

  }

  double counts = 0.;
  log_trace("Source list sizes: "<<pointSources_.size()<<" and "<<extendedSources_.size());
  for (unsigned s = 0; s < pointSources_.size(); s++) {
    PointSourceDetectorResponsePtr ps = pointSources_[s];
    double distance = pixelCenter->Angle(ps->GetSkyPos());
    counts += ps->GetSmearedSignal(distance, pixelArea_, binID_);
  }
  for (unsigned s = 0; s < extendedSources_.size(); s++) {
    counts += (extendedSources_)[s]->GetExtendedSourceConvolutedSignal(binID_, nside_, roiPix_, hp);
  }

  if (GPD_) {
    //const AstroService& astroX = GetService<AstroService>("astroX");
    EquPoint cel;
    GalPoint gal;
    double RA = SkyPos(eventMap_->pix2ang(hp)).RA();
    double dec = SkyPos(eventMap_->pix2ang(hp)).Dec();
    cel.SetRADec(RA*HAWCUnits::degree, dec*HAWCUnits::degree);
    equ2gal(cel, gal);
    //astroX.Equ2Gal(cel, gal);
    double result = (imb_.CommonNorm()) * counts * numTransits_ * exp(-(abs(gal.GetB()/HAWCUnits::degree)*abs(gal.GetB()/HAWCUnits::degree)) / 0.5);
    log_trace("CalcBin " << binID_ << ": Expected excess = " << result);
    return result;
  }

  else {
    double result = (imb_.CommonNorm()) * counts * numTransits_;
    log_trace("CalcBin " << binID_ << ": Expected excess = " << result);
  //expectedSignalHash_[hp] = counts*numTransits_;
    return result;
  }
}

/*****************************************************/
// the expected BG correction in the DirectIntegration declination band
// (the HEALPix ring) of a given HEALpix pixel ID, i.e. 
// alpha_Li&Ma * N_exp(DIring)
double CalcBin::GetPerPixelExpectedBackgroundCorrection(int hp) {

  return 0.;

  /* Commented out by Colas, 2015-01-28
  // We need to rethink this function:
  //  - I don't think it is correct as is. Or only if the source is exactly in
        this pixel.
  //  - We need to deal with ROI masking
  // So in the meantime, the returned 0 is only correct in the regions masked by
  // ROI. Elsewhere, the background will be slightly underestimated (biasing
  // significance and flux).

  int ringnumber = OFF->pix2ring(hp);

  try
  {

    return (imb_.BackgroundNorm())*(imb_.CommonNorm())
            *expectedBGCorrectionHash_.at(ringnumber);

  } catch (...)
  {

    if (imb_.CommonNorm()==0.) return 0;

    int startpix = 0;
    int ringpix = 0;
    double theta = 0.;
    bool shifted = true;
    OFF->get_ring_info2(ringnumber, startpix, ringpix, theta, shifted);
    log_trace("ROI:                         "<<roiPix_.nval()<<" pixels.");
    rangeset<int> diring;
    OFF->query_strip(theta-sqrt(pixelArea_)/2.*degree,
                     theta+sqrt(pixelArea_)/2.*degree,false,diring);
    log_trace("Healpix dec ring:            "<<diring.nval()<<" pixels.");
    diring.intersect(roiPix_);
    log_trace("ROI / dec-ring intersection: "<<diring.nval()<<" pixels.");
    double sig = 0.;
    for (unsigned k = 0; k < diring.size(); ++k) {
      for (int j = diring.ivbegin(k); j < diring.ivend(k); ++j) {
        sig += GetPerPixelExpectedExcess(j);
      }
    }
    log_trace("CalcBin ID "<<binID_<<" , healpix dec ring "<<ringnumber
              <<" ; declination "<<90.-theta/degree<<" deg.:");
    log_trace("  Expected excess counts in DI integration:"<<sig);
    //calculate alpha_Li&Ma for a round bin:
    double alpha = sqrt(pixelArea_/2.)/(diDuration_*15.*cos(90.*degree-theta));
    log_trace("  Pixel area over DI region (Li&Ma alpha): "<<alpha);

    double bgcorr = alpha*sig;

    expectedBGCorrectionHash_[ringnumber] = bgcorr/(imb_.CommonNorm());
    return (imb_.BackgroundNorm())*bgcorr;

  } //end of catch

  //if (expectedBGCorrectionHash_.count(ringnumber)>0) {
  //  return (imb_.BackgroundNorm())*(imb_.CommonNorm())
  //          *expectedBGCorrectionHash_[ringnumber];
  //}
  //else if (imb_.CommonNorm()==0.) {
  //  return 0.;
  //}
  */
}


/*****************************************************/
//
double CalcBin::GetTopHatExpectedExcess(SkyPos center, double radius) {

  rangeset<int> myROI;
  eventMap_->query_disc(center.GetPointing(), radius * degree, myROI);
  double expectedSig = 0;
  //fast short-cut for single point source and no PSF:
  if ((pointSources_.size() == 1) && (extendedSources_.size() == 0)) {
    PointSourceDetectorResponsePtr ps = pointSources_[0];
    log_trace(" total exp. sig: " << (ps->GetExpectedSignal(binID_)));
    log_trace("   *Transits:    " << (ps->GetExpectedSignal(binID_)) * numTransits_);
    log_trace("   * CN:         " << (ps->GetExpectedSignal(binID_)) * (imb_.CommonNorm()) * numTransits_);
    if (ps->IsPSFDeltaFunction(pixelArea_, binID_)) {
      if (myROI.contains(eventMap_->ang2pix(ps->GetSkyPos().GetPointing()))) {
        return (ps->GetExpectedSignal(binID_)) * (imb_.CommonNorm()) * numTransits_;
      }
      else {
        return 0.;
      }
    }
  }
  //Loop through myROI
  for (unsigned k = 0; k < myROI.size(); ++k) {
    for (int j = myROI.ivbegin(k); j < myROI.ivend(k); ++j) {
      expectedSig += GetPerPixelExpectedExcess(j);
    }
  }
  log_trace(" pixel-summed exp. signal: " << expectedSig / numTransits_ / (imb_.CommonNorm()));
  log_trace("   *Transits:              " << expectedSig / (imb_.CommonNorm()));
  log_trace("   *CN:                    " << expectedSig);
  return expectedSig;
}

/*****************************************************/
//Excess relative to BGModel stored in InternalModel!
double CalcBin::GetTopHatExcess(SkyPos center, double radius) {

  rangeset<int> myROI;
  eventMap_->query_disc(center.GetPointing(), radius * degree, myROI);

  double evt = 0;
  double bg = 0;
  //Loop through myROI
  for (unsigned k = 0; k < myROI.size(); ++k) {
    for (int j = myROI.ivbegin(k); j < myROI.ivend(k); ++j) {
      evt += (*eventMap_)[j];
      bg += (imb_.BG(j));
    }
  }

  /*
   * Commented out by Robert on 5/10/16
   * To be consistent with per-pixel calculation and avoid double-counting when
   * using ROI-masked maps, this correction should not be applied in this way
   * anymore
  //calculate alpha_Li&Ma for a round bin:
  double alpha = pi * radius / (2. * diDuration_ * 15. * cos(center.Dec() * degree));
  //correct OFF counts, assuming that excess in this bin is due to a source:
  bg -= alpha * (Sig - bg);
  */

  return evt - bg;
}

/*****************************************************/
//Background counts from BGModel stored in InternalModel!
double CalcBin::GetTopHatBackground(SkyPos center, double radius) {

  rangeset<int> myROI;
  eventMap_->query_disc(center.GetPointing(), radius * degree, myROI);

  double bg = 0;
  double evt = 0;
  //Loop through myROI
  for (unsigned k = 0; k < myROI.size(); ++k) {
    for (int j = myROI.ivbegin(k); j < myROI.ivend(k); ++j) {
      bg += imb_.BG(j);
      evt += (*eventMap_)[j];
    }
  }

  /*
   * Commented out by Robert on 5/10/16
   * To be consistent with per-pixel calculation and avoid double-counting when
   * using ROI-masked maps, this correction should not be applied in this way
   * anymore
  //calculate alpha_Li&Ma for a round bin:
  double alpha = pi * radius / (2. * diDuration_ * 15. * cos(center.Dec() * degree));
  //correct OFF counts, assuming that excess in this bin is due to a source:
  bg -= alpha * (evt - bg);
  */

  return bg;
}

/*****************************************************/


/*****************************************************/
//Return the area (in deg^2) of a circular ROI with a given position/radius.
//Area is calculated as pixel area * number of pixels in ROI.
double CalcBin::GetTopHatArea(SkyPos center, double radius) {

  rangeset<int> myROI;
  eventMap_->query_disc(center.GetPointing(), radius * degree, myROI);
  
  int nPixInROI = myROI.nval();
  
  return pixelArea_ * nPixInROI;
}

/*****************************************************/

double CalcBin::CalcLogLikelihood() {

  double logLike = 0;

  //Loop through ROI
  for (unsigned k = 0; k < roiPix_.size(); ++k) {
    for (int j = roiPix_.ivbegin(k); j < roiPix_.ivend(k); ++j) {

      double evtVal = (*eventMap_)[j];

      //Check on On Value:
      //it might be the healpix default "empty" value
      if (evtVal < -1.e30) {
        log_trace("Healpix undefined pixel value, changed to 0.");
        evtVal = 0;
      }
        //or it might be negativ in case of residual maps
      else if (evtVal < 0) {
        //log_warn("Negative on count " << evtVal << ", changed to 0.");
        evtVal = 0;
        // log_fatal("On Count is < 0.");
      }

      //correct BG based on expected signal contribution in DI dec ring
      double bgVal = imb_.BG(j);

      //CommonNorm() is used inside the following method:

      double expExcess = GetPerPixelExpectedExcess(j);

      double corrBG = GetPerPixelExpectedBackgroundCorrection(j);

      double expEvt = expExcess + bgVal - corrBG;

      if (bgVal == 0) {
        //ignore BG=0
        log_trace("Bin " << binID_ << "; pixel " << j << " : OFF=" << bgVal
                      << " , ignoring via LL+=0");
      }
      else if (bgVal < 0) {
        log_trace("Bin " << binID_ << "; pixel " << j << " : OFF=" << bgVal
                      << " is negative, set LL+=-1e30");
        logLike += -1.e30;
      }
      else {
        if (expEvt < minOnCount_) {
          log_trace("Negative or zero expected On counts = " << expEvt
                        << ", changed to minimum double value: " << minOnCount_);
          expEvt = minOnCount_;
        }
        //The following is the logarithm of the factorial of N:
        // log(N!) = lgamma(N+1)
        double logCounts = lgamma(evtVal + 1);
        logLike += evtVal * log(expEvt) - expEvt - logCounts;
      }

      log_trace("Data value: " << evtVal);
      log_trace("BG value: " << bgVal);
      log_trace("Expected excess: " << expExcess);
      log_trace("Expected counts: " << expEvt);
    }
  }

  log_trace("CalcBin " << binID_ << ": LL(Model+BG) = " << logLike);
  return logLike;
}

/*****************************************************/
double CalcBin::CalcBackgroundLogLikelihood() {

  double logLike = 0;

  //Loop through ROI
  for (unsigned k = 0; k < roiPix_.size(); ++k) {
    for (int j = roiPix_.ivbegin(k); j < roiPix_.ivend(k); ++j) {

      double evtVal = (*eventMap_)[j];

      //Check on On Value:
      //it might be the healpix default "empty" value
      if (evtVal < -1.e30) {
        log_trace("Healpix undefined pixel value, changed to 0.");
        evtVal = 0;
      }
        //or it might be negative in case of residual maps
      else if (evtVal < 0) {
        //log_warn("Negative on count " << evtVal << ", changed to 0.");
        evtVal = 0;
        //   log_fatal("On Count is < 0.");
      }

      double bgVal = imb_.BG(j);

      if (bgVal == 0) {
        //ignore BG=0
        log_trace("Bin " << binID_ << "; pixel " << j << " : OFF=" << bgVal
                      << " , ignoring via LL+=0");
      }
      else if (bgVal < 0) {
        log_trace("Bin " << binID_ << "; pixel " << j << " : OFF=" << bgVal
                      << " is negative, set LL+=-1e30");
        logLike += -1.e30;
      }
      else {
        //The following is the logarithm of the factorial of N:
        // log(N!) = lgamma(N+1)
        double logCounts = lgamma(evtVal + 1);
        logLike += evtVal * log(bgVal) - bgVal - logCounts;
      }

      log_trace("Data value: " << evtVal);
      log_trace("BG value: " << bgVal);
    }
  }

  log_trace("CalcBin " << binID_ << ": LL(BG) = " << logLike);
  return logLike;
}

/*****************************************************/
void CalcBin::CalcWeights(double &sumExpWeighted, double &sumSignalWeighted,
                          double &sumBGWeighted) {

  sumExpWeighted = 0.;
  sumSignalWeighted = 0.;

  //Loop through ROI

  for (unsigned k = 0; k < roiPix_.size(); ++k) {
    for (int j = roiPix_.ivbegin(k); j < roiPix_.ivend(k); ++j) {

      double evtVal = (*eventMap_)[j];

      double bgVal = imb_.BG(j);

      if (bgVal <= 0) {
        log_trace("Negative or zero BG value " << bgVal
                      << ", ignoring it via weight=0.");
      }
      else {

        //correct BG based on expected signal contribution in DI dec ring
        bgVal -= GetPerPixelExpectedBackgroundCorrection(j);

        double obsSignal = evtVal - bgVal;

        //*****Cache-ing the Distances in a vector may give 2x or more speed up

        double expExcess = GetPerPixelExpectedExcess(j);

        double weight = expExcess / bgVal;

        sumExpWeighted += weight * expExcess;
        sumSignalWeighted += weight * obsSignal;
        sumBGWeighted += weight * weight * bgVal;
      }
    }
  }
}

/*****************************************************/

double CalcBin::CalcTopHatLogLikelihood(SkyPos center, double radius) {

  double logLike = 0;

  double excess = 0;
  double bg_val = 0;
  double on_val = 0;
  double sig_val = 0;
  double exp_val = 0;

  //Excess
  if (topHatExcessHash_.count(radius) > 0) {
    excess = topHatExcessHash_[radius];
  }
  else {
    excess = GetTopHatExcess(center, radius);
    topHatExcessHash_[radius] = excess;
  }
  //BG
  if (topHatBackgroundHash_.count(radius) > 0) {
    bg_val = topHatBackgroundHash_[radius];
  }
  else {
    bg_val = GetTopHatBackground(center, radius);
    topHatBackgroundHash_[radius] = bg_val;
  }
  //On value
  on_val = excess + bg_val;

  //expected signal
  if (topHatExpectedExcessHash_.count(radius) > 0) {
    sig_val = (imb_.CommonNorm()) * topHatExpectedExcessHash_[radius];
  }
  else if (imb_.CommonNorm() != 0.) {
    sig_val = GetTopHatExpectedExcess(center, radius);
    topHatExpectedExcessHash_[radius] = sig_val / (imb_.CommonNorm());
  }
  else {
    sig_val = 0.;
  }
  exp_val = sig_val + bg_val;

  //Check on On Value:
  //it might be affected by the healpix default "empty" value
  if (on_val < -1.e30) {
    log_trace("Top hat on_val < -1.e30, probably including undefined pixels."
                  << " Changed to 0.");
    on_val = 0;
  }
    //or it might be negativ in case of residual maps
  else if (on_val < 0) {
    log_warn("Negative Top hat on_val " << on_val << ", changed to 0.");
    on_val = 0;
  }

  if (bg_val == 0) {
    //ignore BG=0
    log_trace("Bin " << binID_ << ": OFF=" << bg_val << " , ignoring via LL=0");
  }
  else if (bg_val < 0) {
    log_trace("Bin " << binID_ << ": OFF=" << bg_val << " is negative, set LL=-1e30");
    logLike = -1.e30;
  }
  else {
    if (exp_val < minOnCount_) {
      log_trace("Negative or zero expected On counts = " << exp_val
                    << ", changed to minimum double value: " << minOnCount_);
      exp_val = minOnCount_;
    }
    //The following is the logarithm of the factorial of N:
    // log(N!) = lgamma(N+1)
    double logCounts = lgamma(on_val + 1);
    logLike = on_val * log(exp_val) - exp_val - logCounts;
  }

  log_trace("Data value: " << on_val);
  log_trace("BG value: " << bg_val);
  log_trace("Expected excess: " << sig_val);
  log_trace("Expected counts: " << exp_val);

  log_trace("CalcBin " << binID_ << ": LL(Model+BG) = " << logLike);
  return logLike;
}

/*****************************************************/

double CalcBin::CalcTopHatBackgroundLogLikelihood
    (SkyPos center, double radius) {

  double logLike = 0;

  double excess = 0;
  double bg_val = 0;
  double on_val = 0;

  //Excess
  if (topHatExcessHash_.count(radius) > 0) {
    excess = topHatExcessHash_[radius];
  }
  else {
    excess = GetTopHatExcess(center, radius);
    topHatExcessHash_[radius] = excess;
  }
  //BG
  if (topHatBackgroundHash_.count(radius) > 0) {
    bg_val = topHatBackgroundHash_[radius];
  }
  else {
    bg_val = GetTopHatBackground(center, radius);
    topHatBackgroundHash_[radius] = bg_val;
  }
  //On value
  on_val = excess + bg_val;

  //Check on On Value:
  //it might be affected by the healpix default "empty" value
  if (on_val < -1.e30) {
    log_trace("Top hat on_val < -1.e30, probably including undefined pixels."
                  << " Changed to 0.");
    on_val = 0;
  }
    //or it might be negativ in case of residual maps
  else if (on_val < 0) {
    log_warn("Negative Top hat on_val " << on_val << ", changed to 0.");
    on_val = 0;
  }

  if (bg_val == 0) {
    //ignore BG=0
    log_trace("Bin " << binID_ << ": OFF=" << bg_val << " , ignoring via LL=0");
  }
  else if (bg_val < 0) {
    log_trace("Bin " << binID_ << ": OFF=" << bg_val << " is negative, set LL=-1e30");
    logLike = -1.e30;
  }
  else {
    //The following is the logarithm of the factorial of N:
    // log(N!) = lgamma(N+1)
    double logCounts = lgamma(on_val + 1);
    logLike = on_val * log(bg_val) - bg_val - logCounts;
  }

  log_trace("Data value: " << on_val);
  log_trace("BG value: " << bg_val);

  log_trace("CalcBin " << binID_ << ": LL(Model+BG) = " << logLike);
  return logLike;
}


/*****************************************************/
void CalcBin::CalcTopHatWeights(double &sumExpWeighted,
                                double &sumSignalWeighted, double &sumBGWeighted,
                                SkyPos center, double radius) {

  sumExpWeighted = 0.;
  sumSignalWeighted = 0.;

  double excess = 0;
  double bg_val = 0;
  double on_val = 0;
  double sig_val = 0;

  //Excess
  if (topHatExcessHash_.count(radius) > 0) {
    excess = topHatExcessHash_[radius];
  }
  else {
    excess = GetTopHatExcess(center, radius);
    topHatExcessHash_[radius] = excess;
  }
  //BG
  if (topHatBackgroundHash_.count(radius) > 0) {
    bg_val = topHatBackgroundHash_[radius];
  }
  else {
    bg_val = GetTopHatBackground(center, radius);
    topHatBackgroundHash_[radius] = bg_val;
  }
  //On value
  on_val = excess + bg_val;

  //expected signal
  if (topHatExpectedExcessHash_.count(radius) > 0) {
    sig_val = (imb_.CommonNorm()) * topHatExpectedExcessHash_[radius];
  }
  else if (imb_.CommonNorm() != 0.) {
    sig_val = GetTopHatExpectedExcess(center, radius);
    topHatExpectedExcessHash_[radius] = sig_val / (imb_.CommonNorm());
  }
  else {
    sig_val = 0.;
  }

  if (bg_val <= 0) {
    log_trace("Negative or zero BG value " << bg_val
                  << ", ignoring it via weight=0.");
  }
  else {

    double weight = sig_val / bg_val;

    sumExpWeighted += weight * sig_val;
    sumSignalWeighted += weight * excess;
    sumBGWeighted += weight * weight * bg_val;
  }
}


/*****************************************************/

void CalcBin::MakeModelMap(bool add) {
  for (unsigned k = 0; k < roiPix_.size(); ++k) {
    for (int j = roiPix_.ivbegin(k); j < roiPix_.ivend(k); ++j) {
      if ((*backgroundMap_)[j] < 1e-30) {
        modelMap_->SetPixel(j, 0.);
      } else {
        if (add) {
          modelMap_->AddToPixel(j, GetPerPixelExpectedExcess(j));
        }
        else {
          modelMap_->SetPixel(j, GetPerPixelExpectedExcess(j));
        };
      }
    }
  }
}

/*****************************************************/

void CalcBin::PrintInfo() {

  cout << "****Calculation Bin Info************************" << endl;
  cout << "  Bin ID: " << binID_;
  cout << "  Num of Transits: " << numTransits_ << endl;
  cout << "  Pixel Area: " << pixelArea_ << " degrees squared" << endl;
  cout << endl << endl;

}
