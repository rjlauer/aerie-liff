/*!
 * @file LikeHAWC.cc
 * @author Patrick Younk
 * @date 25 Jul 2014
 * @brief Implement a CalcBin collection to define a complete likelihood.
 * @version $Id: LikeHAWC.cc 40100 2017-08-09 00:33:33Z henrike $
 */


#include <liff/BinList.h>
#include <liff/LikeHAWC.h>
#include <liff/Minimize.h>
#include <vector>
//#include <liff/ROI.h>
#include <hawcnest/HAWCUnits.h>
#include <data-structures/geometry/R3Transform.h>
#include <data-structures/astronomy/EquPoint.h>
#include <data-structures/astronomy/GalPoint.h>
#include <data-structures/astronomy/AstroCoords.h>

using namespace std;
using namespace threeML;
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

/*****************************************************/

// This is a default empty ModelInterface instance, to be used for the constructors which only have the internal model.
// This is needed because mi_ is a reference to a ModelInterface instance, and must be initialized during construction.
// In the constructors where no ModelInterface is passed in, we need something to initialize mi_. Hence, we make this
// default empty model for these cases.

EmptyModelInterface NullModelInterface;


LikeHAWC::LikeHAWC()
    : data_(0),
      mi_(NullModelInterface),
      detRes_(""),
      fixedROI_(false) {

  //default Internal Model:
  internal_ = InternalModelPtr(new InternalModel());
  log_debug("Using default InternalModel with only BG norm fit.");

  gLikeHAWC = this;
}

LikeHAWC::~LikeHAWC()
{
  log_info("Destructing LikeHAWC instance.")

  // This avoid having a global pointer pointing to something that has been destructed

  gLikeHAWC = NULL;

}

LikeHAWC::LikeHAWC(SkyMapCollection *Data, const string& DetRes,
                   const BinList& binList)
    : data_(Data),
      mi_(NullModelInterface),
      detRes_(DetRes),
      fixedROI_(true) {

  //default Internal Model:
  internal_ = InternalModelPtr(new InternalModel());
  log_debug("Using default InternalModel with only BG norm fit.");

  //default ROI all-sky:
  roi_.push_back(SkyPos(180., 0.));
  roi_.push_back(SkyPos(360., 0.));

  SetupCalcBins(binList);

  if (gLikeHAWC) {
    log_warn("Global pointer 'gLikeHAWC' is already defined,"
                 << " changing it to new instance of 'LikeHAWC'.");
  }
  gLikeHAWC = this;
}

LikeHAWC::LikeHAWC(
    SkyMapCollection *Data,
    const string& DetRes,
    ModelInterface &model,
    double roiRA,
    double roiDec,
    double roiRadius,
    bool roiFixed,
    const BinList& binList)
    : data_(Data),
      mi_(model),
      detRes_(DetRes),
      fixedROI_(roiFixed) {
      //fixedROI_(false) {

  //default Internal Model:
  internal_ = InternalModelPtr(new InternalModel());
  ResetSources(model);
  SetROI(roiRA, roiDec, roiRadius, roiFixed);
  //SetROI(roiRA, roiDec, roiRadius, roiFixed=false);
  SetupCalcBins(binList);

  if (gLikeHAWC) {
    log_warn("Global pointer 'gLikeHAWC' is already defined,"
                 << " changing it to new instance of 'LikeHAWC'.");
  }
  gLikeHAWC = this;
}


//tried
LikeHAWC::LikeHAWC(
    SkyMapCollection *Data,
    const string& DetRes,
    ModelInterface &model,
    const BinList& binList)
    : data_(Data),
      mi_(model),
      detRes_(DetRes),
      fixedROI_(false) {

  //default Internal Model:
  internal_ = InternalModelPtr(new InternalModel());

  log_debug("Using default InternalModel.");

  ResetSources(model);
  SetupCalcBins(binList); //error here

  if (gLikeHAWC) {
    log_warn("Global pointer 'gLikeHAWC' is already defined,"
                 << " changing it to new instance of 'LikeHAWC'.");
  }
  gLikeHAWC = this;
}

//mi_(LikeHAWC::NullModelInterface)

LikeHAWC::LikeHAWC(
    SkyMapCollection *Data,
    const string& DetRes,
    InternalModelPtr &internal,
    const BinList& binList)
    : data_(Data),
      mi_(NullModelInterface),
      detRes_(DetRes),
      internal_(internal),
      fixedROI_(false) {

  //default ROI all-sky:
  roi_.push_back(SkyPos(180., 0.));
  roi_.push_back(SkyPos(360., 0.));

  SetupCalcBins(binList);

  if (gLikeHAWC) {
    log_warn("Global pointer 'gLikeHAWC' is already defined,"
                 << " changing it to new instance of 'LikeHAWC'.");
  }
  gLikeHAWC = this;
}

LikeHAWC::LikeHAWC(
    SkyMapCollection *Data,
    const string& DetRes,
    ModelInterface &model,
    InternalModelPtr &internal,
    const BinList& binList)
    : data_(Data),
      mi_(model),
      detRes_(DetRes),
      internal_(internal),
      fixedROI_(false) {

  ResetSources(model);
  SetupCalcBins(binList);

  if (gLikeHAWC) {
    log_warn("Global pointer 'gLikeHAWC' is already defined,"
                 << " changing it to new instance of 'LikeHAWC'.");
  }
  gLikeHAWC = this;
}

LikeHAWC::LikeHAWC(
    const string& mapTreeFile,
    const string& DetRes,
    ModelInterface &model,
    const BinList& binList,
    bool loadAllSky)
    : data_(0),
      mi_(model),
      detRes_(DetRes),
      fixedROI_(false) {

  //default Internal Model:
  internal_ = InternalModelPtr(new InternalModel());
  log_debug("Using default InternalModel.");

  ResetSources(model);

  vector<SkyPos> roi;
  if (loadAllSky) {
    // ROI all-sky:
    roi.push_back(SkyPos(180., 0.));
    roi.push_back(SkyPos(360., 0.));
    log_debug("Loading all pixels in " << mapTreeFile << " ...");
  }
  else {
    roi = roi_;
    log_debug("Loading pixels in source-dependent ROI from " <<
        mapTreeFile << " ...");
  }

  //Load data
  SetData(mapTreeFile, roi, binList);

  SetupCalcBins(binList);

  if (gLikeHAWC) {
    log_warn("Global pointer 'gLikeHAWC' is already defined,"
                 << " changing it to new instance of 'LikeHAWC'.");
  }
  gLikeHAWC = this;
}

LikeHAWC::LikeHAWC(
    const string& mapTreeFile,
    double nTransits,
    const string& DetRes,
    ModelInterface &model,
    const BinList& binList,
    bool loadAllSky)
    : mi_(model),
      detRes_(DetRes),
      fixedROI_(false) {

  log_debug("Trying again...");
  log_info(mi_.getNumberOfPointSources());
  log_debug("Was that so bad?");

  //default Internal Model:
  internal_ = InternalModelPtr(new InternalModel());
  log_debug("Using default InternalModel.");

  ResetSources(model);

  vector<SkyPos> roi;
  if (loadAllSky) {
    // ROI all-sky:
    roi.push_back(SkyPos(180., 0.));
    roi.push_back(SkyPos(360., 0.));
    log_debug("Loading all pixels in " << mapTreeFile << " ...");
  }
  else {
    roi = roi_;
    log_debug("Loading pixels in source-dependent ROI from " <<
        mapTreeFile << " ...");
  }

  //Load data
  SetData(mapTreeFile, nTransits, roi, binList);

  SetupCalcBins(binList);

  if (gLikeHAWC) {
    log_warn("Global pointer 'gLikeHAWC' is already defined,"
                 << " changing it to new instance of 'LikeHAWC'.");
  }
  gLikeHAWC = this;
}

//ROI.cc ver.
//LikeHAWC::LikeHAWC(
//    const string& mapTreeFile,
//    double nTransits,
//    const string& DetRes,
//    ModelInterface &model,
//    const BinList& binList,
//    const BinName& binID,
//    bool loadAllSky,
//    bool gpdon)
//    : mi_(model),
//      detRes_(DetRes),
//      fixedROI_(false) {
//
//  log_debug("Trying again...");
//  log_info(mi_.getNumberOfPointSources());
//  log_debug("Was that so bad?");
//
//  //default Internal Model:
//  internal_ = InternalModelPtr(new InternalModel());
//  log_debug("Using default InternalModel.");
//
//  ResetSources(model);
//
//  vector<SkyPos> roi;
//  if (loadAllSky) {
//    // ROI all-sky:
//    roi.push_back(SkyPos(180., 0.));
//    roi.push_back(SkyPos(360., 0.));
//    log_debug("Loading all pixels in " << mapTreeFile << " ...");
//  }
//  else {
//    //Set skyMaps
//    //SkyMap<double> *new_eventMap = data_->GetEventMap(binID);
//    //rangeset<int> new_skymapPixels = data_->GetPixels(binID);
//    
//    //SetROI(roi, new_eventMap, new_skymapPixels, gpdon);
//    SetROI(roi,gpdon,fixedROI_);
//    
//    roi = roi_;
//    log_debug("Loading pixels in source-dependent ROI from " <<
//        mapTreeFile << " ...");
//  }
//
//  //Load data
//  SetData(mapTreeFile, nTransits, roi, binList);
//
//  SetupCalcBins(binList);
//
//  if (gLikeHAWC) {
//    log_warn("Global pointer 'gLikeHAWC' is already defined,"
//                 << " changing it to new instance of 'LikeHAWC'.");
//  }
//  gLikeHAWC = this;
//}

LikeHAWC::LikeHAWC(
    const string& mapTreeFile,
    const string& DetRes,
    const BinList& binList)
    : mi_(NullModelInterface),
      detRes_(DetRes),
      fixedROI_(false) {

  //default Internal Model:
  internal_ = InternalModelPtr(new InternalModel());
  log_debug("Using default InternalModel.");

  // ROI all-sky:
  roi_.push_back(SkyPos(180., 0.));
  roi_.push_back(SkyPos(360., 0.));
  log_debug("Loading all pixels in " << mapTreeFile << " ...");

  //Load data
  SetData(mapTreeFile, roi_, binList);

  SetupCalcBins(binList);

  if (gLikeHAWC) {
    log_warn("Global pointer 'gLikeHAWC' is already defined,"
                 << " changing it to new instance of 'LikeHAWC'.");
  }
  gLikeHAWC = this;
}


LikeHAWC::LikeHAWC(
    const string& mapTreeFile,
    double nTransits,
    const string& DetRes,
    const BinList& binList)
    : mi_(NullModelInterface),
      detRes_(DetRes),
      fixedROI_(false) {

  //default Internal Model:
  internal_ = InternalModelPtr(new InternalModel());
  log_debug("Using default InternalModel.");

  // ROI all-sky:
  roi_.push_back(SkyPos(180., 0.));
  roi_.push_back(SkyPos(360., 0.));
  log_debug("Loading all pixels in " << mapTreeFile << " ...");

  //Load data
  SetData(mapTreeFile, nTransits, roi_, binList);

  SetupCalcBins(binList);

  if (gLikeHAWC) {
    log_warn("Global pointer 'gLikeHAWC' is already defined,"
                 << " changing it to new instance of 'LikeHAWC'.");
  }
  gLikeHAWC = this;
}


/*****************************************************/
void LikeHAWC::SetData(SkyMapCollection *Data) {
  data_ = Data;
  log_debug("Cleared CalcBins due to new data pointer.");
  calcBins_.clear();
}

/*****************************************************/
void LikeHAWC::SetData(const string& mapTreeFile,
                       vector<SkyPos> roi, const BinList& binList) {

  data_ = new SkyMapCollection();

  if (roi.size() == 2) {
    data_->SetDisc(roi[0], roi[1].RA() * degree);
  }
  else if (roi.size() == 4) {
    data_->SetPolygon(roi);
  }
  else {
    log_fatal("ROI has to have 2 entries (disc) or 4 entries (polygon)!");
  }

  data_->LoadMaps(mapTreeFile, binList);
  log_debug("Cleared CalcBins due to new data pointer.");
  calcBins_.clear();
}

/*****************************************************/
void LikeHAWC::SetData(const string& mapTreeFile, double nTransits,
                       vector<SkyPos> roi, const BinList& binList) {
  SetData(mapTreeFile, roi, binList);
  data_->SetTransits(nTransits);
}

/*****************************************************/
void LikeHAWC::SetDetectorResponse(const string& DetRes) {
  detRes_ = DetRes;
  log_debug("New DetectorResponse, clearing source lists...");
  ResetSources(mi_);
}

/*****************************************************/
void LikeHAWC::SetInternalModel(const InternalModelPtr internal) {
  internal_ = internal;
  log_debug("Clear CalcBins due to new internal model.");
  calcBins_.clear();
}

/*****************************************************/
void LikeHAWC::SetupCalcBins(const BinList& binList) {
  // Make a Calc Bin for each Data Bin

  log_debug("(Re-)setting CalBins...");

  calcBins_.clear();

  if (roi_.size() < 2) {
    log_fatal("Region of interest is not defined!");
  }

  // Will make a Calculation Bin for every NH Bin in the data set

  AnalysisBinMap nhBins = data_->GetBins();

  // Loop through the NH Bins
  for (unsigned j = 0, n = binList.GetNBins(); j < n; ++j) {
  
    const BinName& binID = binList[j];
    
    bool found = false;
    AnalysisBinMap::const_iterator bin = nhBins.begin();
    for (const AnalysisBinMap::const_iterator end = nhBins.end();
         bin                                     != end; ++bin) {
      found = binID == bin->first;
      if (found)
        break;
    }
    if (!found)
      continue;
    
    // Make an instance of a CalcBin
    log_debug("Make an instance of a CalcBin" << roi_.size());
    log_debug("ROI parameters:");
    log_debug(" - size: " << roi_.size());
    for (unsigned int i = 0; i < roi_.size(); ++i) {
      log_debug(" - roi_[" << i << "]: ");
      roi_.at(i).Dump();
    }

    CalcBinPtr cb = CalcBinPtr(
      new CalcBin(binID, data_, pointSources_, extendedSources_, internal_,
                  roi_)
    );

    calcBins_.push_back(cb);
  
  }

  log_debug("Number of Calc Bins Setup: " << calcBins_.size());

  if (calcBins_.empty()) log_fatal("No Calculations Bins Setup!");

}

/*****************************************************/
void LikeHAWC::ResetSources(ModelInterface &model, double padding) {
  log_debug("(Re-)setting sources...");

  mi_ = model;
  pointSources_.clear();
  extendedSources_.clear();
  //clear also the cached expected signal:
  for (unsigned k = 0; k < calcBins_.size(); ++k) {
    calcBins_[k]->expectedSignalHash_.clear();
    calcBins_[k]->expectedBGCorrectionHash_.clear();
    calcBins_[k]->topHatExcessHash_.clear();
    calcBins_[k]->topHatBackgroundHash_.clear();
    calcBins_[k]->topHatExpectedExcessHash_.clear();
  }
  //and reset CommonNorm:
  internal_->CommonNorm() = 1;
  internal_->CommonNormError() = 1;

  //Convolute sources with detRes_ and sort into lists for point/extended

  //first, the point sources:
  int nps = model.getNumberOfPointSources();
  for (int n = 0; n < nps; n++) {
    log_info("Creating PointSourceDetectorResponse for source ID "
                 << n << " based on " << detRes_);

    //GV: there are two reasons to use shared_ptr here: 1) since "model" is a *reference* to a ModelInterface
    // instance, the PointSourceDetectorResponse cannot be copied with the default copy constructor, and so
    // push_back() will fail. Hence we need a pointer so we can place it in the vector. 2) we need a way to free
    // the memory as soon as the class is destroyed, and using the shared_pointer makes this automatic (in other
    // words, this is the usual reason why you want to use a shared pointer instead of a raw one).

    PointSourceDetectorResponsePtr psdr =
        PointSourceDetectorResponsePtr(new PointSourceDetectorResponse(detRes_, model, n));

    pointSources_.push_back(psdr);

  }
  log_debug("Prepared " << nps << " point source(s).");

  //then, extended sources:
  int nex = model.getNumberOfExtendedSources();
  for (int n = 0; n < nex; n++) {

    log_info("Creating ExtendedSourceDetectorResponse for source ID "
                 << n << " based on " << detRes_);

    // See comment in the loop for point sources above this one

    ExtendedSourceDetectorResponsePtr esdr =
        ExtendedSourceDetectorResponsePtr(new ExtendedSourceDetectorResponse(detRes_, model, n));

    extendedSources_.push_back(esdr);

  }
  log_debug("Prepared " << nex << " extended source(s).");

  if ((extendedSources_.empty()) && (pointSources_.empty())) {
    log_warn("No sources defined via ModelInterface!");
  }
  if (!fixedROI_) {
    log_debug("Setting ROI");
    SetROI(MatchROI(padding)); //setting roi_
  }
}

/*****************************************************/
void LikeHAWC::UpdateSources() {
  int nps = (int) (pointSources_.size());
  int nex = (int) (extendedSources_.size());
  if ((mi_.getNumberOfPointSources() != nps) ||
      (mi_.getNumberOfExtendedSources() != nex)) {
    ResetSources(mi_);
  }
  else {
    bool detResFree = internal_->IsDetectorResponseFree();
    int moved = 0;
    //the point sources:
    for (int n = 0; n < nps; n++) {
      log_debug("Updating source " << n);
      moved += pointSources_[n]->SetModel(mi_, detResFree);
    }
    log_debug("Updated " << nps << " point source(s).");
    //then, extended sources:
    for (int n = 0; n < nex; n++) {
      extendedSources_[n]->SetModel(mi_, true);
    }
    log_debug("Updated " << nex << " extended source(s).");

    if (!fixedROI_ && ((moved > 0) || (nex > 0))) {
      //setting new roi, includes clearing the cached expected signal
      SetROI(MatchROI(padding_));
    }
    else {
      //only clearing the cached expected signal
      for (unsigned k = 0; k < calcBins_.size(); ++k) {
        calcBins_[k]->expectedSignalHash_.clear();
        calcBins_[k]->expectedBGCorrectionHash_.clear();
        calcBins_[k]->topHatExcessHash_.clear();
        calcBins_[k]->topHatBackgroundHash_.clear();
        calcBins_[k]->topHatExpectedExcessHash_.clear();
      }
    }
  }
}

/*****************************************************/

// Calculate ROI from source boundaries + padding
vector<SkyPos> LikeHAWC::MatchROI(double padding) {

  vector<SkyPos> ROI;
  if ((pointSources_.empty()) && (extendedSources_.empty())) {
    log_warn("No sources defined via ModelInterface, "
                 << "setting ROI to whole sky!");
    ROI.push_back(SkyPos(180, 0));
    //second SkyPos only holds radius in RA-parameter:
    ROI.push_back(SkyPos(360, 0));
  }
  else {
    double minra = 360.;
    double maxra = 0.;
    double mindec = 90.;
    double maxdec = -90.;

    for (PointSourceDetectorResponseVector::iterator ps = pointSources_.begin();
         ps != pointSources_.end(); ps++) {

      double ra = (*ps)->GetRA();
      minra = min(minra, ra);
      maxra = max(maxra, ra);
      double dec = (*ps)->GetDec();
      mindec = min(mindec, dec);
      maxdec = max(maxdec, dec);
      //cout << "RA=" << ra << ", dec=" << dec << endl;
      //cout << "  minRA=" << minra << ", maxRA=" << maxra << endl;
      //cout << "  mindec=" << mindec << ", maxdec=" << maxdec << endl;
    }
    log_debug("ROI from point source list: minra=" << minra << " , maxra=" << maxra
                  << " , mindec=" << mindec << " , maxdec=" << maxdec);

    for (ExtendedSourceDetectorResponseVector::iterator es = extendedSources_.begin();
         es != extendedSources_.end(); es++) {
      minra = min(minra, (*es)->GetMinRA());
      maxra = max(maxra, (*es)->GetMaxRA());
      mindec = min(mindec, (*es)->GetMinDec());
      maxdec = max(maxdec, (*es)->GetMaxDec());
    }
    log_debug("ROI from extended source list: minra=" << minra << " , maxra=" <<
        maxra << " , mindec=" << mindec << " , maxdec=" << maxdec);

    if ((minra == maxra) && (mindec == maxdec)) {
      ROI.push_back(SkyPos(minra, mindec));
      //second SkyPos only holds radius in RA-parameter:
      ROI.push_back(SkyPos(padding, 0));
      log_debug("Matched ROI as disc with radius " << padding <<
          " deg. around RA=" << minra << " , Dec=" << mindec << " .");
    }
    else {
//      cout << " -minRA=" << minra << ", maxRA=" << maxra << endl;
//      cout << " -mindec=" << mindec << ", maxdec=" << maxdec << endl;

      minra = minra < padding ? fmod(minra + 360. - padding, 360.) : fmod(minra - padding, 360.);
      maxra = maxra < padding ? fmod(maxra + 360. + padding, 360.) : fmod(maxra + padding, 360.);

      mindec = max(-90., mindec - padding);
      maxdec = min(90., maxdec + padding);

//      minra = min(minra-padding,0.);
//      maxra = max(maxra+padding,359.99999);
//      mindec = min(mindec-padding,-90.);
//      maxdec = max(maxdec+padding,90.);
//
//      cout << " +minRA=" << minra << ", maxRA=" << maxra << endl;
//      cout << " +mindec=" << mindec << ", maxdec=" << maxdec << endl;

      ROI.push_back(SkyPos(minra, mindec));
      ROI.push_back(SkyPos(maxra, mindec));
      ROI.push_back(SkyPos(maxra, maxdec));
      ROI.push_back(SkyPos(minra, maxdec));
      log_debug("ROI: minra=" << minra << " , maxra=" <<
          maxra << " , mindec=" << mindec << " , maxdec=" << maxdec);
    }
  }
  padding_ = padding; //remember for future UpdateSources calls
  return ROI;
}

/*****************************************************/

void LikeHAWC::SetROI(vector<SkyPos> ROI, bool fixedROI, bool galactic) {

  fixedROI_ = fixedROI;

  roi_.clear();
  if (ROI.size() == 2) {
    log_debug("ROI from SkyPos-vector, using first SkyPos entry ("
                  << ROI[0].RA() << "," << ROI[0].Dec() << ") as center and"
                  << " RA from second entry (" << ROI[1].RA() << " deg.) as radius.");
    if (galactic) {
      log_debug("Coverting from Galactic coordinate to Equatorial coordinate");
      EquPoint cel;
      GalPoint gal;
      gal.SetBL(ROI[0].Dec() * HAWCUnits::degree, ROI[0].RA() * HAWCUnits::degree);
      gal2equ(gal, cel);
    
      SkyPos pos(cel.GetRA() / HAWCUnits::degree, cel.GetDec() / HAWCUnits::degree);
      roi_.push_back(pos);
      roi_.push_back(ROI[1]);
      log_debug("(" << roi_[0].RA() << "," << roi_[0].Dec() << ") as center and"
                    << " RA from second entry (" << roi_[1].RA() << " deg.) as radius.");
    } else {
      roi_ = ROI;
    }

  } else if (ROI.size() > 2) {
    log_debug("ROI from SkyPos polygon with " << ROI.size() << " boundary points: ");
    for (unsigned int i = 0; i < ROI.size(); i++) {
      log_debug("(" << ROI[i].RA() << "," << ROI[i].Dec() << ")");
    }
    if (galactic) {
      log_debug("Coverting from Galactic coordinate to Equatorial coordinate");
      for (unsigned int i = 0; i < ROI.size(); i++) {
        EquPoint cel;
        GalPoint gal;
        gal.SetBL(ROI[i].Dec() * HAWCUnits::degree, ROI[i].RA() * HAWCUnits::degree);
        gal2equ(gal, cel);
        SkyPos pos(cel.GetRA() / HAWCUnits::degree, cel.GetDec() / HAWCUnits::degree);
        roi_.push_back(pos);
      }
      for (unsigned int i = 0; i < roi_.size(); i++) {
        log_debug("(" << roi_[i].RA() << "," << roi_[i].Dec() << ")");
      }

    } else {
      roi_ = ROI;
    }

  } else {
    log_fatal("ROI has to have 2 entries (disc) or more entries (polygon)!");
  }

  //clear the cached expected signal:
  for (unsigned k = 0; k < calcBins_.size(); ++k) {
    calcBins_[k]->expectedSignalHash_.clear();
    calcBins_[k]->expectedBGCorrectionHash_.clear();
    calcBins_[k]->topHatExcessHash_.clear();
    calcBins_[k]->topHatBackgroundHash_.clear();
    calcBins_[k]->topHatExpectedExcessHash_.clear();
  }

  //better always reset CalcBin ROI
  log_debug("Resetting ROI in all CalcBins.");
  for (CalcBinVector::iterator it = calcBins_.begin();
       it != calcBins_.end(); ++it) {
    log_debug("Calling SetROIPixels()");
    (*it)->SetROIPixels(roi_);
  }
}

/*****************************************************/
//Option to input coordinates in Galactic added
void LikeHAWC::SetROI(double ra, double dec, double radius, bool fixedROI, bool galactic) {

  vector<SkyPos> roi;

  SkyPos pos(ra, dec);
  roi.push_back(pos);

  //The RA of this skypos will be used as radius
  //(don't know why it has been made like that...)
  SkyPos rad(radius, 0);

  roi.push_back(rad);

  this->SetROI(roi, fixedROI, galactic);
}

/*****************************************************/
//Strip ROI
void LikeHAWC::SetROI(double rastart, double rastop, double decstart, double decstop, bool fixedROI, bool galactic) {

  vector<SkyPos> roi;

  SkyPos pos1(rastart, decstart);
  SkyPos pos2(rastop, decstart);
  SkyPos pos3(rastop, decstop);
  SkyPos pos4(rastart, decstop);
 
  roi.push_back(pos1);
  roi.push_back(pos2);
  roi.push_back(pos3);
  roi.push_back(pos4);

  /*
  vector<double> raRange;
  vector<double> decRange;
  
  double d_ra = fabs(rastop - rastart);
  double raRatio = d_ra / ceil(d_ra);
  
  for (int i = 0; i <= ceil(d_ra); i++) {
    double c = rastart + (raRatio * i);
    raRange.push_back(c);
  }
  
  vector<double> raRangeRev(raRange);
  reverse(raRangeRev.begin(), raRangeRev.end());
  
  double d_dec = fabs(decstop - decstart);
  double decRatio = d_dec / ceil(d_dec);
  
  for (int i = 0; i <= ceil(d_dec); i++) {
    double c = decstart + (decRatio * i);
    decRange.push_back(c);
  }
  
  vector<double> decRangeRev(decRange);
  reverse(decRangeRev.begin(), decRangeRev.end());

  for (unsigned int i = 0; i < raRange.size()-1; i++) {
    SkyPos pos1(raRange[i], decstart);
    roi.push_back(pos1);
  }

  for (unsigned int i = 0; i < decRange.size()-1; i++) {
    SkyPos pos2(rastop, decRange[i]);
    roi.push_back(pos2);
  }
  
  for (unsigned int i = 0; i < raRangeRev.size()-1; i++) {
    SkyPos pos3(raRangeRev[i], decstop);
    roi.push_back(pos3);
  }

  for (unsigned int i = 0; i < decRangeRev.size()-1; i++) {
    SkyPos pos4(rastart, decRangeRev[i]);
    roi.push_back(pos4);
  }
  */
  this->SetROI(roi, fixedROI, galactic);
}

/*****************************************************/
void LikeHAWC::SetROI(string mask, double threshold, bool fixedROI) {
  
  fixedROI_=fixedROI;
  
  log_debug("Setting ROI through a fits files with pixels equal/greater than " << threshold << " to select any arbitrary region");
  if(mask.empty()) 
    log_fatal("Need to supply a fits map with masked region for analysis");
  //clear the cached expected signal:
  for (unsigned k = 0; k < calcBins_.size(); ++k) {
    calcBins_[k]->expectedSignalHash_.clear();
    calcBins_[k]->expectedBGCorrectionHash_.clear();
    calcBins_[k]->topHatExcessHash_.clear();
    calcBins_[k]->topHatBackgroundHash_.clear();
    calcBins_[k]->topHatExpectedExcessHash_.clear();
  }
  //better always reset CalcBin ROI
  log_debug("Resetting ROI in all CalcBins.");
  for (CalcBinVector::iterator it = calcBins_.begin();
      it != calcBins_.end(); it++) {
    log_debug("Calling SetROIPixels()");
    (*it)->SetROIPixels(mask, threshold);
  }

}

/*****************************************************/

PointSourceDetectorResponsePtr
LikeHAWC::GetPointSourceDetectorResponse(int sID) {
  if ((sID < 0) || (sID >= (int) pointSources_.size())) {
    log_fatal("Point source ID " << sID << " does not exist in model.");
  }
  return pointSources_[sID];
}

/*****************************************************/

ExtendedSourceDetectorResponsePtr
LikeHAWC::GetExtendedSourceDetectorResponse(int sID) {
  if ((sID < 0) || (sID >= (int) extendedSources_.size())) {
    log_fatal("Extended source ID " << sID << " does not exist in model.");
  }
  return extendedSources_[sID];
}

/*****************************************************/

double LikeHAWC::CalcBackgroundLogLikelihood(bool doIntFit) {
  if (doIntFit) {
    if (InternalBGMinimize() != 0) {
      log_warn("BG-only fit failed, returning LL(BG)=2");
      return 2;
    }
  }
  double LL = 0;
  for (unsigned k = 0; k < calcBins_.size(); ++k) {
    LL += calcBins_[k]->CalcBackgroundLogLikelihood();
  }
  log_debug("LL(Bg) =       " << LL)
  return LL;
  return LL;
}

/*****************************************************/

double LikeHAWC::CalcLogLikelihood(bool doIntFit) {
  if (doIntFit) {
    if (InternalMinimize() != 0) {
      log_warn("Source fit failed, returning LL(Model)=2");
      return 2;
    }
  }
  double LL = 0;
  for (unsigned k = 0; k < calcBins_.size(); ++k) {
    LL += calcBins_[k]->CalcLogLikelihood();
  }
  log_debug("LL(Bg+Model) = " << LL)
  return LL;
}

/*****************************************************/

double LikeHAWC::CalcLogLikelihoodUpdateSources(bool doIntFit) {
  UpdateSources();
  if (doIntFit) {
    if (InternalMinimize() != 0) {
      log_warn("Source fit failed, returning LL(Model)=2");
      return 2;
    }
  }
  double LL = 0;
  for (unsigned k = 0; k < calcBins_.size(); ++k) {
    LL += calcBins_[k]->CalcLogLikelihood();
  }
  log_debug("LL(Bg+Model) = " << LL)
  return LL;
}

/*****************************************************/

double LikeHAWC::CalcLogLikelihood(
    ModelInterface &model, bool doIntFit) {
  ResetSources(model);
  if (doIntFit) {
    if (InternalMinimize() != 0) {
      log_warn("Source fit failed, returning LL(Model)=2");
      return 2;
    }
  }
  double LL = 0;
  for (unsigned k = 0; k < calcBins_.size(); ++k) {
    LL += calcBins_[k]->CalcLogLikelihood();
  }
  log_debug("LL(Bg+Model) = " << LL)
  return LL;
}

/*****************************************************/

double LikeHAWC::CalcTestStatistic(bool doIntFit) {
  double bgLL = CalcBackgroundLogLikelihood(doIntFit);
  double modelLL = CalcLogLikelihood(doIntFit);
  log_debug("bgLL " << bgLL << " modelLL " << modelLL);
  //calculate LL(Model) last, so that BGNorm, CommonNorm, and
  //other InternalModel fit parameters will have values from
  //Model-Maximization
  if ((bgLL == 2) || (modelLL == 2)) {
    log_warn("TS calculation failed, returning TS=0");
    return 0;
  }
  else {
    double ts = 2 * (modelLL - bgLL);
    log_debug("ts " << ts);
    return ts;
  }
}

/*****************************************************/

double LikeHAWC::CalcTestStatisticUpdateSources(bool doIntFit) {
  double bgLL = CalcBackgroundLogLikelihood(doIntFit);
  double modelLL = CalcLogLikelihoodUpdateSources(doIntFit);
  //calculate LL(Model) last, so that BGNorm, CommonNorm, and
  //other InternalModel fit parameters will have values from
  //Model-Maximization
  if ((bgLL == 2) || (modelLL == 2)) {
    log_warn("TS calculation failed, returning TS=0");
    return 0;
  }
  else return 2 * (modelLL - bgLL);
}

/*****************************************************/

double LikeHAWC::CalcTestStatistic(ModelInterface &model, bool doIntFit) {
  double bgLL = CalcBackgroundLogLikelihood(doIntFit);
  double modelLL = CalcLogLikelihood(model, doIntFit);
  //calculate LL(Model) last, so that BGNorm, CommonNorm, and
  //other InternalModel fit parameters will have values from
  //Model-Maximization
  if ((bgLL == 2) || (modelLL == 2)) {
    log_warn("TS calculation failed, returning TS=0");
    return 0;
  }
  else return 2 * (modelLL - bgLL);
}

/*****************************************************/

void LikeHAWC::EstimateNormAndSigma(double &Norm, double &Sigma) {

  log_debug("Estimate CommonNorm and Sigma via Gaussian weights...");
  log_trace("Set CommonNorm=1 before Gaussian weight calculation.");
  internal_->CommonNorm() = 1; //This affects SumExpWeighted
  double SumExpWeighted = 0;
  double SumSignalWeighted = 0;
  double SumBGWeighted = 0;

  for (unsigned k = 0; k < calcBins_.size(); ++k) {

    double sumExpWeighted = 0;;
    double sumSignalWeighted = 0;
    double sumBGWeighted = 0;

    calcBins_[k]->CalcWeights(sumExpWeighted, sumSignalWeighted, sumBGWeighted);

    SumExpWeighted += sumExpWeighted;
    SumSignalWeighted += sumSignalWeighted;
    SumBGWeighted += sumBGWeighted;
  }

  // That's crappy and we know it, but this function is kind of pointless so we
  // don't care
  if (SumExpWeighted == 0) SumExpWeighted = 0.01;
  if (SumBGWeighted == 0) SumBGWeighted = 0.01;

  Norm = SumSignalWeighted / SumExpWeighted;
  Sigma = SumSignalWeighted / sqrt(SumBGWeighted);
}

/*****************************************************/

double LikeHAWC::CalcTopHatBackgroundLogLikelihood(
    SkyPos center, vector<double> radius, bool doIntFit) {
  if (radius.size() != calcBins_.size()) {
    log_fatal("Number of radius values does not match number of analysis bins"
                  << " defined in LikeHAWC object.");
  }
  topHatCenter = center;
  topHatRadius = radius;
  if (doIntFit) {
    if (InternalTopHatBGMinimize() != 0) {
      log_warn("BG-only fit failed, returning LL(BG)=2");
      return 2;
    }
  }
  double LL = 0;
  for (unsigned k = 0; k < calcBins_.size(); ++k) {
    LL += calcBins_[k]->CalcTopHatBackgroundLogLikelihood(center, radius[k]);
  }
  log_debug("LL(Bg) =       " << LL)
  return LL;
}

/*****************************************************/

double LikeHAWC::CalcTopHatLogLikelihood(
    SkyPos center, vector<double> radius, bool doIntFit) {
  if (radius.size() != calcBins_.size()) {
    log_fatal("Number of radius values does not match number of analysis bins"
                  << " defined in LikeHAWC object.");
  }
  topHatCenter = center;
  topHatRadius = radius;
  if (doIntFit) {
    if (InternalTopHatMinimize() != 0) {
      log_warn("Source fit failed, returning LL(Model)=2");
      return 2;
    }
  }
  double LL = 0;
  for (unsigned k = 0; k < calcBins_.size(); ++k) {
    LL += calcBins_[k]->CalcTopHatLogLikelihood(center, radius[k]);
  }
  log_debug("LL(Bg+Model) = " << LL)
  return LL;
}

/*****************************************************/

double LikeHAWC::CalcTopHatLogLikelihoodUpdateSources(
    SkyPos center, vector<double> radius, bool doIntFit) {
  if (radius.size() != calcBins_.size()) {
    log_fatal("Number of radius values does not match number of analysis bins"
                  << " defined in LikeHAWC object.");
  }
  topHatCenter = center;
  topHatRadius = radius;
  UpdateSources();
  if (doIntFit) {
    if (InternalTopHatMinimize() != 0) {
      log_warn("Source fit failed, returning LL(Model)=2");
      return 2;
    }
  }
  double LL = 0;
  for (unsigned k = 0; k < calcBins_.size(); ++k) {
    LL += calcBins_[k]->CalcTopHatLogLikelihood(center, radius[k]);
  }
  log_debug("LL(Bg+Model) = " << LL)
  return LL;
}

/*****************************************************/

double LikeHAWC::CalcTopHatLogLikelihood(
    ModelInterface &model, SkyPos center,
    vector<double> radius, bool doIntFit) {
  if (radius.size() != calcBins_.size()) {
    log_fatal("Number of radius values does not match number of analysis bins"
                  << " defined in LikeHAWC object.");
  }
  topHatCenter = center;
  topHatRadius = radius;
  ResetSources(model);
  if (doIntFit) {
    if (InternalTopHatMinimize() != 0) {
      log_warn("Source fit failed, returning LL(Model)=2");
      return 2;
    }
  }
  double LL = 0;
  for (unsigned k = 0; k < calcBins_.size(); ++k) {
    LL += calcBins_[k]->CalcTopHatLogLikelihood(center, radius[k]);
  }
  log_debug("LL(Bg+Model) = " << LL)
  return LL;
}

/*****************************************************/

double LikeHAWC::CalcTopHatTestStatistic(
    SkyPos center, vector<double> radius, bool doIntFit) {
  if (radius.size() != calcBins_.size()) {
    log_fatal("Number of radius values does not match number of analysis bins"
                  << " defined in LikeHAWC object.");
  }
  topHatCenter = center;
  topHatRadius = radius;
  double bgLL = CalcTopHatBackgroundLogLikelihood(center, radius, doIntFit);
  double modelLL = CalcTopHatLogLikelihood(center, radius, doIntFit);
  //calculate LL(Model) last, so that BGNorm, CommonNorm, and
  //other InternalModel fit parameters will have values from
  //Model-Maximization
  if ((bgLL == 2) || (modelLL == 2)) {
    log_warn("TS calculation failed, returning TS=0");
    return 0;
  }
  else return 2 * (modelLL - bgLL);
}

/*****************************************************/

double LikeHAWC::CalcTopHatTestStatisticUpdateSources(
    SkyPos center, vector<double> radius, bool doIntFit) {
  if (radius.size() != calcBins_.size()) {
    log_fatal("Number of radius values does not match number of analysis bins"
                  << " defined in LikeHAWC object.");
  }
  topHatCenter = center;
  topHatRadius = radius;
  double bgLL = CalcTopHatBackgroundLogLikelihood(center, radius, doIntFit);
  double modelLL = CalcTopHatLogLikelihoodUpdateSources(
      center, radius, doIntFit);
  //calculate LL(Model) last, so that BGNorm, CommonNorm, and
  //other InternalModel fit parameters will have values from
  //Model-Maximization
  if ((bgLL == 2) || (modelLL == 2)) {
    log_warn("TS calculation failed, returning TS=0");
    return 0;
  }
  else return 2 * (modelLL - bgLL);
}

/*****************************************************/

double LikeHAWC::CalcTopHatTestStatistic(
    ModelInterface &model, SkyPos center,
    vector<double> radius, bool doIntFit) {
  if (radius.size() != calcBins_.size()) {
    log_fatal("Number of radius values does not match number of analysis bins"
                  << " defined in LikeHAWC object.");
  }
  topHatCenter = center;
  topHatRadius = radius;
  double bgLL = CalcTopHatBackgroundLogLikelihood(center, radius, doIntFit);
  double modelLL = CalcTopHatLogLikelihood(model, center, radius, doIntFit);
  //calculate LL(Model) last, so that BGNorm, CommonNorm, and
  //other InternalModel fit parameters will have values from
  //Model-Maximization
  if ((bgLL == 2) || (modelLL == 2)) {
    log_warn("TS calculation failed, returning TS=0");
    return 0;
  }
  else return 2 * (modelLL - bgLL);
}

/*****************************************************/
void LikeHAWC::SetCommonNorm(double value) {

  internal_->CommonNorm() = value;

}

/*****************************************************/

void LikeHAWC::EstimateTopHatNormAndSigma(double &Norm, double &Sigma,
                                          SkyPos center, vector<double> radius) {

  if (radius.size() != calcBins_.size()) {
    log_fatal("Number of radius values does not match number of analysis bins"
                  << " defined in LikeHAWC object.");
  }
  log_debug("Estimate CommonNorm and Sigma via Gaussian weights...");
  log_trace("Set CommonNorm=1 before Gaussian weight calculation.");
  internal_->CommonNorm() = 1; //This affects SumExpWeighted
  double SumExpWeighted = 0;
  double SumSignalWeighted = 0;
  double SumBGWeighted = 0;

  for (unsigned k = 0; k < calcBins_.size(); ++k) {

    double sumExpWeighted = 0;
    double sumSignalWeighted = 0;
    double sumBGWeighted = 0;

    calcBins_[k]->CalcTopHatWeights(
        sumExpWeighted, sumSignalWeighted, sumBGWeighted, center, radius[k]);

    SumExpWeighted += sumExpWeighted;
    SumSignalWeighted += sumSignalWeighted;
    SumBGWeighted += sumBGWeighted;
  }

  // That's crappy and we know it, but this function is kind of pointless so we
  // don't care
  if (SumExpWeighted == 0) SumExpWeighted = 0.01;
  if (SumBGWeighted == 0) SumBGWeighted = 0.01;

  Norm = SumSignalWeighted / SumExpWeighted;
  Sigma = SumSignalWeighted / sqrt(SumBGWeighted);
}

/*****************************************************/

void LikeHAWC::MakeModelMap() {
  log_info("Making model Map")
  for (unsigned k = 0; k < calcBins_.size(); ++k) {
    calcBins_[k]->MakeModelMap();
  }
}

/*****************************************************/
void LikeHAWC::WriteModelMap(string fileName, bool poisson) {
  MakeModelMap();
  data_->WriteModelMapTree(fileName, poisson);
}

/*****************************************************/
void LikeHAWC::WriteResidualMap(string fileName) {
  MakeModelMap();
  data_->WriteResidualMapTree(fileName);
}

/*****************************************************/
void LikeHAWC::PrintCalcBinsInfo() {

  for (unsigned k = 0; k < calcBins_.size(); ++k) {
    calcBins_[k]->PrintInfo();
  }
}


/*****************************************************/

///Get the TopHat excess for all bins as expected from the current model
vector<double> LikeHAWC::GetTopHatExpectedExcesses(double ra, double dec, double countradius) {

  unsigned long nbins = calcBins_.size();

  vector<double> results;

  SkyPos SourcePosition(ra, dec);

  for (unsigned long k = 0; k < nbins; ++k) {

    results.push_back(calcBins_[k]->GetTopHatExpectedExcess(SourcePosition, countradius));

  }

  return results;

}

///Get the TopHat excesses measured in all bins
vector<double> LikeHAWC::GetTopHatExcesses(double ra, double dec, double countradius) {

  unsigned long nbins = calcBins_.size();

  vector<double> results;

  SkyPos SourcePosition(ra, dec);

  for (unsigned long k = 0; k < nbins; ++k) {

    results.push_back(calcBins_[k]->GetTopHatExcess(SourcePosition, countradius));

  }

  return results;

}

///Get the TopHat background in all bins
vector<double> LikeHAWC::GetTopHatBackgrounds(double ra, double dec, double countradius) {

  unsigned long nbins = calcBins_.size();

  vector<double> results;

  SkyPos SourcePosition(ra, dec);

  for (unsigned long k = 0; k < nbins; ++k) {

    results.push_back(calcBins_[k]->GetTopHatBackground(SourcePosition, countradius));

  }

  return results;

}

///Get the TopHat background in all bins
vector<double> LikeHAWC::GetTopHatAreas(double ra, double dec, double countradius) {

  unsigned long nbins = calcBins_.size();

  vector<double> results;

  SkyPos SourcePosition(ra, dec);

  for (unsigned long k = 0; k < nbins; ++k) {

    results.push_back(calcBins_[k]->GetTopHatArea(SourcePosition, countradius));

  }

  return results;

}

vector<double> LikeHAWC::GetEnergies(bool reset) {
  if (energies_.empty()) {
    if (detRes_=="") {
      log_fatal("Detector response file is not defined. Cannot get energy list.")
    }

    DetectorResponse dr = DetectorResponse(detRes_);
    AnalysisBinMap nhbmap = dr.GetAnalysisBinMap();
    AnalysisBinMap::iterator nh = nhbmap.begin();
    ResponseBinPtr rb = dr.GetBin(0, nh->first);
    energies_ = rb->GetLogEnBins();
    //convert to MeV
    for (vector<double>::iterator i = energies_.begin();
         i != energies_.end(); i++) {
      *i = pow(10., *i + 6.);
    }
  }
  return energies_;
}

vector<pair<double, double> > LikeHAWC::GetPositions(int esId, bool reset) {
  if (!data_) log_fatal("Data must be loaded before calling GetPositions.");
  return GetExtendedSourceDetectorResponse(esId)->GetPositions(data_->GetFirstEventMap()->Nside(), reset);
} 
