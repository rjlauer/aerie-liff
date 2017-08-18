#include <liff/BinList.h>
#include <liff/InternalModelBin.h>
#include <liff/Func1.h>

#include <TH2.h>

#include <hawcnest/HAWCUnits.h>

using namespace std;
using namespace HAWCUnits;

/*****************************************************/

InternalModelBin::InternalModelBin()
    : bgMap_(0),
      pixelArea_(-1),
      backgroundNorm_(1),
      backgroundNormError_(0) {
}

/*****************************************************/

InternalModelBin::InternalModelBin(const BinName& binID,
                                   InternalModelPtr Internal,
                                   SkyMap<double> *BGMap,
                                   vector<SkyPos> ROI,
                                   bool fitBGModeltoMap)
    : binID_(binID),
      intModel_(Internal),
      bgMap_(BGMap),
      roiSkyPos_(ROI),
      backgroundNorm_(1),
      backgroundNormError_(0) {

  int nside_ = bgMap_->Nside();
  pixelArea_ = 4 * pi / degree / degree / 12. / nside_ / nside_;

  //copy BGModel:
  if (!(intModel_->GetBackgroundModel())) {
    SetBackgroundFromMap(bgMap_);
  }
  else {
    if (ROI.size() == 0) {
      log_fatal("Can't fit background with ROI vector of size zero.");
    }
    bgModelBin_ = TF2Ptr(new TF2(*(intModel_->GetBackgroundModel())));
    if (fitBGModeltoMap) FitBackgroundModelToMap(bgMap_);
    vector<int> freeBGParIDs = intModel_->GetFreeBackgroundParameterIDList();
    for (vector<int>::const_iterator p = freeBGParIDs.begin();
         p != freeBGParIDs.end(); ++p) {
      AddFreeBackgroundParameter(*p);
    }
  }
}

/*****************************************************/

void InternalModelBin::FitBackgroundModelToMap(SkyMap<double> *BGMap) {
  if (!bgModelBin_) log_fatal("No BackgroundModel defined in CalcBin "
                                  << binID_ << "!");
  bgHash_.clear();
  log_debug("CalcBin " << binID_ << ": Fitting BGModel to BG-Map, "
                << "all parameters free...")

  //1. Make TH2 for ROI
  double minra = -1000.;
  double maxra = -1000.;
  double mindec = -1000.;
  double maxdec = -1000.;
  vector<pointing> pol;
  if (roiSkyPos_.size() == 2) {
    //one point source with disc-like ROI--> square with width sqrt(radius/2)
    SkyPos center = roiSkyPos_[0];
    double width = roiSkyPos_[1].RA() / sqrt(2);
    minra = center.RA() - width;
    maxra = center.RA() + width;
    mindec = center.Dec() - width;
    maxdec = center.Dec() + width;
    SkyPos corner(minra, mindec);
    pol.push_back(corner.GetPointing());
    corner = SkyPos(maxra, mindec);
    pol.push_back(corner.GetPointing());
    corner = SkyPos(maxra, maxdec);
    pol.push_back(corner.GetPointing());
    corner = SkyPos(minra, maxdec);
    pol.push_back(corner.GetPointing());
  }
  else if (roiSkyPos_.size() == 4) {
    //polygon
    minra = roiSkyPos_[0].RA();
    maxra = roiSkyPos_[1].RA();
    mindec = roiSkyPos_[1].Dec();
    maxdec = roiSkyPos_[2].Dec();
    pol.push_back(roiSkyPos_[0].GetPointing());
    pol.push_back(roiSkyPos_[1].GetPointing());
    pol.push_back(roiSkyPos_[2].GetPointing());
    pol.push_back(roiSkyPos_[3].GetPointing());
  }
  else {
    log_fatal("ROI must be vector<SkyPos> with 2 or 4 elements.");
  }
  log_debug("  ROI : minra=" << minra << " , maxra=" << maxra
                << " , mindec=" << mindec << " , maxdec=" << maxdec);
  int nra = (int) ((maxra - minra) / sqrt(pixelArea_));
  int ndec = (int) ((maxdec - mindec) / sqrt(pixelArea_));
  TH2D roihist("roihist", "roihist", nra, minra, maxra, ndec, mindec, maxdec);
  log_debug("  Fitting BGModel in ROI with " << nra << "x" << ndec << " pixels.");
  rangeset<int> roipix;
  BGMap->query_polygon(pol, roipix);

  //2. Fill with values from BGMap
  double bgtotal = 0.;
  double areatotal = 0.;
  for (unsigned k = 0; k < roipix.size(); ++k) {
    for (int j = roipix.ivbegin(k); j < roipix.ivend(k); ++j) {
      SkyPos p = SkyPos(BGMap->pix2ang(j));
      roihist.Fill(p.RA(), p.Dec(), (*BGMap)[j] / pixelArea_);
      bgtotal += (*BGMap)[j];
      areatotal += pixelArea_;
      log_trace("  Filled pixel at RA=" << p.RA() << ", Dec=" << p.Dec() << " with "
                    << (*BGMap)[j] / pixelArea_);
    }
  }
  //roihist.Scale(bgtotal/areatotal/roihist.Integral());
  //3. Fit TF2 to TH2
  roihist.Fit(bgModelBin_.get(), "MNQ");

  /*
  log_debug("  BG sum over ROI:         "<<bgtotal);
  log_debug("  Integral over ROI-hist:  "<<roihist.Integral("width"));
  log_debug("  Integral of BGModel TF2: "
      <<bgModelBin_->Integral(minra,maxra,mindec,maxdec));
  double bgfsum = 0.;
  for (unsigned k = 0; k < roipix.size(); ++k) {
    for (int j = roipix.ivbegin(k); j < roipix.ivend(k); ++j) {
      SkyPos center = SkyPos(BGMap->pix2ang(j));
      bgfsum += bgModelBin_->Eval(center.RA(),center.Dec())*pixelArea_;
    }
  }
  log_debug("  TF2-Sum over HP pixels:  "<<bgfsum);

  log_debug("  Fit result for"<< bgModelBin_->GetTitle() << ":");
  for (int i = 0; i < bgModelBin_->GetNpar(); ++i) {
    log_debug("    (Par " << i << ") " <<  bgModelBin_->GetParName(i)
        << ": "<< bgModelBin_->GetParameter(i) << " +/- "
        <<  bgModelBin_->GetParError(i));
  }
  */

  BackgroundNorm() = 1; //reset to 1
}

/*****************************************************/
void InternalModelBin::SetBackgroundFromMap(SkyMap<double> *BGMap) {
  bgMap_ = BGMap;
  bgModelBin_.reset();
  freeBGParList_.clear();
  bgHash_.clear();
  log_debug("Use BG map from data as background in CalcBin " << binID_ << " .");
}

/*****************************************************/

double InternalModelBin::BG(int hp) {

  if (bgHash_.count(hp) > 0) {
    return bgHash_[hp] * BackgroundNorm();
  }
  else if (!bgMap_) {
    log_fatal("No BGMap from data defined for CalcBin " << binID_ << "!");
    return 0.; //to get rid of compiler warning
  }
  else if (!bgModelBin_) {
    double bgval = (*bgMap_)[hp];
    //cache without variable BackgroundNorm
    /*
    if (bgval==0.) {
      //90%-Poisson confidence limit for 0 counts in integration duration:
      double lim = -log(0.9);
      //get number of pixels in healpix dec-ring:
      int ringnumber = bgMap_->pix2ring(hp);
      int startpix = 0;
      int ringpix = 0;
      double theta = 0.;
      bool shifted = true;
      bgMap_->get_ring_info2(ringnumber, startpix, ringpix, theta, shifted);
      //duration should be flexible !!!!
      double duration = 2.; //hours
      //!!!!!!
      //calculate number of pixels over which direct-integration was done:
      double diPixels = duration*15*cos(theta-pi/2.)*((double)ringpix)/360;
      //upper BG-limit for this pixel:
      bgval = lim/diPixels;
      log_trace("Bin "<<binID_<<" ; pixel "<<hp<<" : "<<
        "value BG=0 from OFF-map replaced with 90% upper limit: "<<bgval
        <<" , assuming "<<duration<<" hour direct-integration duration");
    }
    */
    bgHash_[hp] = bgval;
    return bgval * BackgroundNorm();
  }
  else {
    SkyPos center(bgMap_->pix2ang(hp));
    //very simple, only value at pixel-center taken into account
    double bgval = bgModelBin_->Eval(center.RA(), center.Dec()) * pixelArea_;
    //cache without variable BackgroundNorm
    bgHash_[hp] = bgval;
    return bgval * BackgroundNorm();
  }
}

/*****************************************************/

void InternalModelBin::AddFreeBackgroundParameter(int ParId) {
  bgHash_.clear();
  FreeParameter FP;
  FP.FuncPointer = GetBackgroundModel();
  FP.ParId = ParId;
  freeBGParList_.push_back(FP);
}

