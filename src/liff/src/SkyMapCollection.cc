/*!
 * @file SkyMapCollection.cc
 * @author Robert Lauer
 * @date 10 Sep 2014
 * @brief A collection of SkyMap objects.
 * @version $Id: SkyMapCollection.cc 36273 2016-12-10 02:00:03Z samm $
 */

#include <cstdlib>
#include <limits>
#include <string>
#include <sstream>

#include <data-structures/time/UTCDateTime.h>
#include <hawcnest/HAWCUnits.h>
#include <liff/BinList.h>
#include <liff/SkyMapCollection.h>

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

#include <dirent.h>

#include <TObjArray.h>
#include <TTree.h>

using namespace std;
using namespace HAWCUnits;

const double SkyMapCollection::defaultTransits_ = 0.;
const SkyPos SkyMapCollection::defaultCenter_(180., 0.);
const double SkyMapCollection::defaultRadius_ = 360.;
const double SkyMapCollection::defaultMinDec_ =
  numeric_limits<double>::quiet_NaN();
const double SkyMapCollection::defaultMaxDec_ =
  numeric_limits<double>::quiet_NaN();

const double SkyMapCollection::dontSetTransits_ = -1.;

void SkyMapCollection::SetBins(const BinList& binList) {

  analysisBins_.clear();
  
  const unsigned nBins = binList.GetNBins();
  
  ostringstream message;
  message << "Preparing to load maps for these bins:";
  for (unsigned i = 0; i < nBins; ++i)
    message << " " << binList[i];
  message << ".";
  log_debug(message.str())
  
  for (unsigned i = 0; i < nBins; ++i)
    analysisBins_[binList[i]];

}

void SkyMapCollection::SetTransits(double t) {
  transits_ = t;
  log_info("Setting transit signal fraction to " << transits_);
}

void SkyMapCollection::SetDir(string directory) {
  dir_ = directory;
}

void SkyMapCollection::SetDisc(SkyPos position, double radius) {
  polygon_.clear();
  center_ = position;
  radius_ = radius;
}

void SkyMapCollection::SetDecBand(double dec1, double dec2) {
  polygon_.clear();
  radius_ = -1.;
  minDec_ = min(dec1, dec2);
  maxDec_ = max(dec1, dec2);
}

void  SkyMapCollection::SetPolygon(const vector<SkyPos>& polygon) {
  center_ = polygon[0];
  radius_ = 0.;
  polygon_.clear();
  for (unsigned i = 0; i < polygon.size(); i++) {
    polygon_.push_back(polygon[i].GetPointing());
  }
}

rangeset<int> SkyMapCollection::GetPixels(const BinName& nhbin) {
  rangeset<int> pixset;
  if (radius_ > 0.) {
    GetEventMap(nhbin)->query_disc(center_.GetPointing(), radius_, pixset);
  }
  else if (!polygon_.empty()) {
    GetEventMap(nhbin)->query_polygon(polygon_, pixset);
  }
  else if (minDec_ == minDec_ && maxDec_ == maxDec_) {
    double theta1 = HAWCUnits::halfpi - minDec_;
    double theta2 = HAWCUnits::halfpi - maxDec_;
    double thetaMin = min(theta1, theta2);
    double thetaMax = max(theta1, theta2);
    GetEventMap(nhbin)->query_strip(thetaMin, thetaMax, true, pixset);
  }
  else {
    log_fatal("No sky region information given, " <<
        "use SetDisc, SetPolygon or SetDecBand first.")
  }
  return pixset;
}

vector<SkyPos> SkyMapCollection::GetSkyPosVector(const BinName& nhbin) {
  vector<SkyPos> pos;
  if (radius_ > 0.) {
    pos.push_back(center_);
    //second SkyPos only holds radius in RA-parameter:
    pos.push_back(SkyPos(radius_, 0));
  }
  else if (!polygon_.empty()) {
    for (vector<pointing>::iterator it = polygon_.begin();
         it != polygon_.end(); it++) {
      pos.push_back(SkyPos(*it));
    }
  }
  else if (minDec_ == minDec_ && maxDec_ == maxDec_) {
    // Return a vector of size zero!
  }
  else {
    log_fatal("No sky region information given, " <<
        "use SetDisc or SetPolygon first.")
  }
  return pos;
}

vector<string> SkyMapCollection::FindSiderealDayMaps(UTCDate start, UTCDate stop, 
                                                  string prefix) const {

  if (dir_.empty()) log_fatal("No base directory defined, use SetDir(BASEDIR).")

  log_info("Loading maps for UTC dates " << start.GetYear() << "/"
               << start.GetMonth() << "/" << start.GetDay() << " to "
               << stop.GetYear() << "/" << stop.GetMonth() << "/" << stop.GetDay());

  double hour = HAWCUnits::hour;
  //always define UTC day at noon, to avoid leap-second trouble
  TimeStamp tstart = UTCDateTime(start).GetTimeStamp() + TimeInterval(12 * hour);
  TimeStamp tstop = UTCDateTime(stop).GetTimeStamp() + TimeInterval(12 * hour);
  if (tstart > tstop) log_fatal("Start date > stop date!")

  string ab;
  // This is assuming a 6-hour transit window!
  ab = ((center_.RA() >= 45.) && (center_.RA() < 315.)) ? "a" : "b";

  vector<string> maptrees;

  while (tstart <= tstop) {
    UTCDateTime date(tstart);
    string path = Form("%s/%04d/%02d/%02d", dir_.c_str(), date.GetYear(),
                       date.GetMonth(), date.GetDay());
    string mtname = Form("%s_%s_%04d-%02d-%02d", prefix.c_str(), ab.c_str(),
                         date.GetYear(), date.GetMonth(), date.GetDay());
    size_t strsize = mtname.size();
    DIR *pDIR;
    struct dirent *entry;
    pDIR = opendir(path.c_str());
    if (pDIR != NULL) {
      while ((entry = readdir(pDIR))) {
        string fname(entry->d_name);
        if (fname.size() < strsize) continue;
        string sfx = fname.substr(fname.find_last_of(".") + 1);
        if ((fname.substr(0, strsize).compare(mtname) == 0) && (sfx == "root")) {
          //need to implement sorting, otherwise might be switched
          maptrees.push_back(path + "/" + fname);
        }
      }
      closedir(pDIR);
    }
    double day = HAWCUnits::day;
    tstart += TimeInterval(day);
  }

  if (maptrees.size() == 0) {
    if (ab == "b") {
      // given the selection above, this RA is not covered in a-maps
      log_warn("No type 'b' maps found for these dates"
                   << " and type 'a' does not cover full 6-hour transit.");
      return maptrees;
    }
    else if ((center_.RA() >= 135.) && (center_.RA() < 225.)) {
      // this RA is not covered in b-maps
      log_warn("No type 'a' maps found for these dates"
                   << " and type 'b' does not cover full 6-hour transit.");
      return maptrees;
    }
    //try b-map instead, if 6-hour transit is also covered in those...
    ab = "b";
    //reset tstart/stop
    tstart = UTCDateTime(start).GetTimeStamp();
    tstop = UTCDateTime(stop).GetTimeStamp();
    while (tstart <= tstop) {
      UTCDateTime date(tstart);
      string path = Form("%s/%04d/%02d/%02d", dir_.c_str(), date.GetYear(),
                         date.GetMonth(), date.GetDay());
      string mtname = Form("%s_%s_%04d-%02d-%02d", prefix.c_str(), ab.c_str(),
                           date.GetYear(), date.GetMonth(), date.GetDay());
      size_t strsize = mtname.size();
      DIR *pDIR;
      struct dirent *entry;
      pDIR = opendir(path.c_str());
      if (pDIR != NULL) {
        while ((entry = readdir(pDIR))) {
          string fname(entry->d_name);
          if (fname.size() < strsize) continue;
          string sfx = fname.substr(fname.find_last_of(".") + 1);
          if ((fname.substr(0, strsize).compare(mtname) == 0) && (sfx == "root")) {
            //need to implement sorting, otherwise might be switched
            maptrees.push_back(path + "/" + fname);
          }
        }
        closedir(pDIR);
      }
      double day = HAWCUnits::day;
      tstart += TimeInterval(day);
    }
    if (maptrees.size() == 0) {
      log_warn("Neither type 'a' nor 'b' maps found for these dates.")
      return maptrees;
    }
  }

  return maptrees;
}

vector<string> SkyMapCollection::LoadSiderealDayMaps(UTCDate start,UTCDate stop,
                                                  string prefix) {
  transits_ = 0;

  vector<string> maptrees = FindSiderealDayMaps(start, stop,prefix);
  LoadMapList(maptrees);

  transits_ = (double) maptrees.size();
  log_info("Assuming one full transit per sidereal day, setting total "
               << "transit signal fraction to " << GetTransits() << " transits.");
  log_debug("This should be corrected based on actual transit coverage.");

  return maptrees;
}

void SkyMapCollection::LoadMapList(vector<string> maptrees) {

  if (maptrees.size() == 0) {
    log_warn("No maptrees listed for loading...")
  }

  eventMaps_.clear();
  modelMaps_.clear();
  backgroundMaps_.clear();
  binInfoMap_.clear();
  transits_ = 0;

  MapTree mt;
  vector<string>::iterator it = maptrees.begin();

  bool firstSD = true;
  map<BinName, double> avgDuration;

  for (; it != maptrees.end(); it++) {
    if (!mt.OpenFile(*it)) {
      log_warn("cannot open MapTree root file " << *it << " , skipping.")
    }
    else if (firstSD) {
      //mt.CloseFile(); //Open-cmd inside LoadMaps
      // Load first file, includes SetBins
      //log_info("  Loading "<<*it);
      LoadMaps(*it);
      firstSD = false;
      for (BinInfoMap::iterator b = binInfoMap_.begin();
           b != binInfoMap_.end(); ++b) {
        avgDuration[b->first] = b->second.duration;
      }
    }
    else {
      //all other SDs:
      log_info("  Loading " << *it);
      for (AnalysisBinMap::iterator nb = analysisBins_.begin();
           nb != analysisBins_.end(); ++nb) {
        const BinName& n     = nb->first;
        const string nHitDir = "nHit" + PadBinName(n) + "/";
        
        if (!mt.OpenTree((nHitDir + "data").c_str())) {
          if (!mt.OpenTree((nHitDir + "gh00_data").c_str())) {
            log_fatal("Does this MapTree follow the tree naming convention nHitXX/data ?");
          }
          log_info("Found maptree via old naming scheme (gh00).");
        }
        eventMaps_[n].AddMapTree(mt);

        if (!mt.OpenTree((nHitDir + "bkg").c_str())) {
          if (!mt.OpenTree((nHitDir + "gh00_bkg").c_str())) {
            log_fatal("Does this MapTree follow the tree naming convention nHitXX/bkg?");
          }
          log_info("Found maptree via old naming scheme (gh00).");
        }
        backgroundMaps_[n].AddMapTree(mt);
      }
      mt.CloseFile();
      // add BinInfo information
      LoadBinInfo(*it, false);
      for (BinInfoMap::iterator b = binInfoMap_.begin();
           b != binInfoMap_.end(); b++) {
        avgDuration[b->first] += b->second.duration;
      }
    }
  }
  for (BinInfoMap::iterator b = binInfoMap_.begin();
       b != binInfoMap_.end(); ++b) {
    b->second.duration = avgDuration[b->first] / ((double) maptrees.size());
  }

  transits_ = binInfoMap_.begin()->second.totalDuration / 24.;
  log_info("Setting transit signal fraction based on summed "
               << " BinInfo entries 'totalDuration' in days: " << transits_);
}

void SkyMapCollection::LoadMaps(const string& file, const BinList& binList,
                                const double transits) {

  eventMaps_.clear();
  modelMaps_.clear();
  backgroundMaps_.clear();
  log_info("Loading maps from file " << file);

  SetBins(binList);

  MapTree mt;
  if (!mt.OpenFile(file)) {
    log_fatal("Cannot open MapTree root file " << file)
  }

  for (AnalysisBinMap::iterator nb = analysisBins_.begin(); nb != analysisBins_.end(); ++nb) {
    const BinName& n = nb->first;
    log_debug("Loading bin " << n);

    // Define the list of pixels
    rangeset<int> pixset;
    const string nHitDir = "nHit" + PadBinName(n) + "/";
    if (!mt.OpenTree((nHitDir + "data").c_str())) {
      if (!mt.OpenTree((nHitDir + "gh00_data").c_str())) {
        log_fatal("Could not find bin " << n << " count map in map-tree file.");
      }
      log_info("Found maptree via old naming scheme (gh00).");
    }
    if (radius_ > 0.) {
      // Disc case
      log_debug("Loading from disk, center (" << center_.RA() << ", "
                << center_.Dec() << "), radius " << radius_/degree);
      mt.GetMap<double>().query_disc(center_.GetPointing(), radius_, pixset);
    }
    else if (!polygon_.empty()) {
      // Polygon case
      log_debug("Loading from polygon");
      mt.GetMap<double>().query_polygon(polygon_, pixset);
    }
    else if (minDec_ == minDec_ && maxDec_ == maxDec_) {
      // Declination band case
      double theta1 = HAWCUnits::halfpi - minDec_;
      double theta2 = HAWCUnits::halfpi - maxDec_;
      double thetaMin = min(theta1, theta2);
      double thetaMax = max(theta1, theta2);
      log_debug("Preparing pixel list to load data between declination "
                    << minDec_ / degree << " and " << maxDec_ / degree);
      mt.GetMap<double>().query_strip(thetaMin, thetaMax, true, pixset);
    }
    else {
      log_fatal("No sky region information given, " <<
          "use SetDisc, SetPolygon or SetDecBand first.")
    }

    // Load the data for the list of pixels
    eventMaps_[n] = SkyMap<double>(mt, pixset);
    modelMaps_[n] = SkyMap<double>(pixset, mt.Nside(), mt.Scheme());
    if (!mt.OpenTree((nHitDir + "/bkg").c_str())) {
      if (!mt.OpenTree((nHitDir + "/gh00_bkg").c_str())) {
        log_fatal(
          "Could not find bin " << n << " background map in map-tree file."
        );
      }
      log_info("Found maptree via old naming scheme (gh00).");
    }
    backgroundMaps_[n] = SkyMap<double>(mt, pixset);
  }
  mt.CloseFile();

  // load BinInfo information
  LoadBinInfo(file);
  double totDur = binInfoMap_.begin()->second.totalDuration;
  log_info("Reading transit signal fraction from bin "
               << binInfoMap_.begin()->first
               << " BinInfo entry 'totalDuration': " << totDur / 24. << " transits.");
  transits_ = totDur / 24.;
  
  // Override transit signal fraction read from bin info if provided.
  if (transits != dontSetTransits_)
    SetTransits(transits);
  
}

void SkyMapCollection::InitializeMaps(const BinList& binList, const int nside) {
  eventMaps_.clear();
  modelMaps_.clear();
  backgroundMaps_.clear();
  binInfoMap_.clear();
  SetBins(binList);
  transits_ = 1;
  Healpix_Ordering_Scheme scheme = RING;
  SkyMap<double> smap;
  Healpix_Map<double> empty_double(nside, scheme, SET_NSIDE);
  if (radius_ > 0.) {
    smap.DiscFromMap(empty_double, center_.GetPointing(), radius_);
  }
  else if (!polygon_.empty()) {
    rangeset<int> pixset;
    empty_double.query_polygon(polygon_, pixset);

    cout << "query_polygon-checked3" << endl;

    smap.SetFromMap(empty_double, pixset);
  }
  else {
    log_fatal("No sky region information given, " <<
        "use SetDisc or SetPolygon first.")
  }

  BinInfo emptyBinInfo;
  emptyBinInfo.duration = 0;
  emptyBinInfo.totalDuration = 0;
  emptyBinInfo.startMJD = 0;
  emptyBinInfo.stopMJD = 0;
  for (AnalysisBinMap::iterator nb = analysisBins_.begin(); nb != analysisBins_.end(); nb++) {
    const BinName& n = nb->first;
    eventMaps_[n] = smap;
    modelMaps_[n] = smap;
    backgroundMaps_[n] = smap;
    binInfoMap_[n] = emptyBinInfo;
  }
}

void SkyMapCollection::EmptyModelMaps() {
  log_debug("Empty model maps");
  for (MapMap::iterator it = modelMaps_.begin();
       it != modelMaps_.end(); ++it) {
    it->second.Empty();
  }
}

void SkyMapCollection::WriteMapTree(const string& filename, WriteType writeType,
                                    bool poisson) {
  if (poisson && (writeType != WRITE_MODEL) && (writeType != WRITE_INJECT)) {
    log_fatal("Poisson fluctuation only work with WRITE_MODEL or WRITE_INJECT "
              "cases.");
  }
  
  MapTree mt;
  mt.CreateFile(filename);
  mt.OpenFile(filename);

  BinName n;
  BinName* namePtr = &n;
  BinInfo bi;
  TTree binInfo("BinInfo", "BinInfo");
  binInfo.Branch("name", &namePtr);
  binInfo.Branch("duration", &bi.duration, "duration/D");
  binInfo.Branch("totalDuration", &bi.totalDuration, "totalDuration/D");
  binInfo.Branch("startMJD", &bi.startMJD, "startMJD/D");
  binInfo.Branch("stopMJD", &bi.stopMJD, "stopMJD/D");

  for (AnalysisBinMap::iterator nb = analysisBins_.begin();
       nb != analysisBins_.end(); ++nb) {
    n = nb->first;
    
    const string binDir       = "nHit" + PadBinName(n) + "/";
    const string dataTreeName = binDir + "data";
    const string bkgTreeName  = binDir + "bkg" ;
    
    // Write first column
    {
      SkyMap<double> newMap;
      switch (writeType) {
        case WRITE_STANDARD:
        case WRITE_RESIDUAL:
        {
          // In both these cases, just write event map in first column
          newMap = *this->GetEventMap(n);
          break;
        }
        case WRITE_MODEL:
        {
          // Write (background + model) map in first column
          newMap = *this->GetBackgroundMap(n);
          newMap.Add(*this->GetModelMap(n));
          if (poisson)
            newMap.PoissonFluctuate();
          break;
        }
        case WRITE_INJECT:
        {
          // Write (event + model) map in first column
          newMap = *this->GetModelMap(n);
          if (poisson) {
            // Fluctuate the injected signal only, not the original event map
            newMap.PoissonFluctuate();
          }
          newMap.Add(*this->GetEventMap(n));
          break;
        }
        default:
        {
          log_fatal("Unknown WriteType: "<<writeType);
        }
      }
      Healpix_Map<double> hmd;
      newMap.FillHealpixMap(hmd);
      mt.SetMap(hmd);
      mt.Write(filename, ("nHit" + PadBinName(n) + "/data").c_str());
    }

    // Write second column
    {
      SkyMap<double> newMap;
      switch (writeType) {
        case WRITE_STANDARD:
        case WRITE_MODEL:
        case WRITE_INJECT:
        {
          // In all 3 cases, just write background map in second column
          newMap = *this->GetBackgroundMap(n);
          break;
        }
        case WRITE_RESIDUAL:
        {
          // Write (background + model) map in second column
          newMap = *this->GetBackgroundMap(n);
          newMap.Add(*this->GetModelMap(n));
          break;
        }
        default:
        {
          log_fatal("Unknown WriteType: "<<writeType);
        }
      }
      Healpix_Map<double> hmd;
      newMap.FillHealpixMap(hmd);
      mt.SetMap(hmd);
      mt.Write(filename, ("nHit" + PadBinName(n) + "/bkg").c_str());
    }

    // Write bin info
    bi = binInfoMap_[n];
    binInfo.Fill();
  }
  binInfo.Write();
  mt.CloseFile();
  log_info("Wrote MapTree file '" << filename << "'.");
}

void SkyMapCollection::UpdateWithModel(WriteType writeType,
                                       bool poisson) {

  if (writeType==WRITE_STANDARD) {
    log_warn("Why is this function call? It should not do anything for the "
             "WRITE_STANDARD case.");
    return;
  }
  else if (writeType!=WRITE_MODEL && writeType!=WRITE_RESIDUAL
           && writeType!=WRITE_INJECT) {
    log_fatal("Unknown writeType: "<<writeType);
  }
  
  for (AnalysisBinMap::iterator nb = analysisBins_.begin();
       nb != analysisBins_.end(); nb++) {
    const BinName& n = nb->first;
    // Deal with first column
    switch (writeType) {
      case WRITE_MODEL:
      {
        // (background + model) map
        SkyMap<double> newMap;
        newMap = *this->GetBackgroundMap(n);
        newMap.Add(*this->GetModelMap(n));
        if (poisson) {
          newMap.PoissonFluctuate();
        }
        this->SetEventMap(n,newMap);
        break;
      }
      case WRITE_INJECT:
      {
        // (event + model) map
        SkyMap<double> newMap;
        newMap = *this->GetModelMap(n);
        if (poisson) {
          newMap.PoissonFluctuate();
        }
        newMap.Add(*this->GetEventMap(n));
        this->SetEventMap(n,newMap);
        break;
      }
      default:
      {
        log_debug("No need to update data map.");
      }
    }
    
    // Deal with the second column
    switch (writeType) {
      case WRITE_RESIDUAL:
      {
        // (background + model) map
        SkyMap<double> newMap;
        newMap = *this->GetBackgroundMap(n);
        newMap.Add(*this->GetModelMap(n));
        this->SetBackgroundMap(n,newMap);
        break;
      }
      default:
      {
        log_debug("No need to update background map.");
      }
    }
  }
  log_info("Updated maptree instance.");
}

SkyMap<double> *
SkyMapCollection::GetEventMap(const BinName& nhbin) {
  if (eventMaps_.find(nhbin) == eventMaps_.end()) {
    log_fatal("nHit bin index " << nhbin << " is outside bin range.");
  }
  return (&eventMaps_[nhbin]);
}

SkyMap<double> *
SkyMapCollection::GetFirstEventMap() {
  if (eventMaps_.begin() == eventMaps_.end()) {
    log_fatal("No nHit bin found");
  }
  return (&eventMaps_.begin()->second);
}

SkyMap<double> *
SkyMapCollection::GetModelMap(const BinName& nhbin) {
  if (modelMaps_.find(nhbin) == modelMaps_.end()) {
    log_fatal("nHit bin index " << nhbin << " is outside bin range.")
  }
  return (&modelMaps_[nhbin]);
}

SkyMap<double> *
SkyMapCollection::GetBackgroundMap(const BinName& nhbin) {
  if (backgroundMaps_.find(nhbin) == backgroundMaps_.end()) {
    log_fatal("nHit bin index " << nhbin << " is outside bin range.")
  }
  return (&backgroundMaps_[nhbin]);
}

void SkyMapCollection::LoadBinInfo(const string& mtFile, const bool reset) {
  log_debug("Loading duration information from MapTree file.");
  const boost::shared_ptr<TFile> infile =
    boost::make_shared<TFile>(mtFile.c_str());
  //test if BinInfo tree is there
  if (!infile->FindObjectAny("BinInfo")) {
    log_warn("MapTree file " << mtFile << " has no TTree 'BinInfo'.");
    if (reset) {
      log_warn(" Assuming 2h average integration duration for all bins.");
      log_warn(" Assuming 1 transit duration.");
      binInfoMap_.clear();
      BinInfo bi;
      bi.startMJD = 0;
      bi.stopMJD = 0;
      bi.nEvents = -1;
      bi.duration = 2;
      bi.totalDuration = 24;
      bi.maptype = "unknown";
      bi.maxDur = -1;
      bi.minDur = -1;
      bi.epoch = "unknown";
      for (AnalysisBinMap::iterator nb = analysisBins_.begin(); nb != analysisBins_.end(); ++nb) {
        binInfoMap_[nb->first] = bi;
      }
    }
    else {
      log_warn("No BinInfo information added!");
    }
  }
  else {
    TTree *binInfo = (TTree *) infile->Get("BinInfo");

    BinName nhname;
    BinName* namePtr = &nhname;
    BinIndex nhid;
    const TObjArray* const branch = binInfo->GetListOfBranches();
    const bool hasName = branch->FindObject("name");
    if (hasName)
      binInfo->SetBranchAddress("name", &namePtr);
    else if (branch->FindObject("id"))
      binInfo->SetBranchAddress("id", &nhid);
    else
      log_fatal("'BinInfo' in " << mtFile << " has no 'name' or 'id' branch!")

    BinInfo bi;
    //integration duration
    TBranch* idbr =
      (TBranch *) binInfo->GetListOfBranches()->FindObject("duration");
    if (!idbr) {
      log_warn("'BinInfo' in " << mtFile << " has no 'duration' branch!"
                   << " Assuming 2h average integration duration for all bins.");
    }
    else {
      binInfo->SetBranchAddress("duration", &bi.duration);
    }
    //total duration
    idbr = (TBranch *) binInfo->GetListOfBranches()->FindObject("totalDuration");
    if (!idbr) {
      log_warn("'BinInfo' in " << mtFile << " has no 'totalDuration' branch!"
                   << " Assuming 1 transit duration.");
    }
    else {
      binInfo->SetBranchAddress("totalDuration", &bi.totalDuration);
    }
    //start/stop MJD
    idbr = (TBranch *) binInfo->GetListOfBranches()->FindObject("startMJD");
    if (!idbr) {
      log_warn("'BinInfo' in " << mtFile << " has no 'startMJD' branch!");
    }
    else {
      binInfo->SetBranchAddress("startMJD", &bi.startMJD);
      binInfo->SetBranchAddress("stopMJD", &bi.stopMJD);
    }
    //nEvents
    idbr = (TBranch *) binInfo->GetListOfBranches()->FindObject("nEvents");
    if (!idbr) {
      log_warn("'BinInfo' in " << mtFile << " has no 'nEvents' branch!"
                   << " Setting to -1.");
    }
    else {
      binInfo->SetBranchAddress("nEvents", &bi.nEvents);
    }
    //maptype
    idbr = (TBranch *) binInfo->GetListOfBranches()->FindObject("maptype");
    string *maptype = new string("unknown");
    if (!idbr) {
      log_warn("'BinInfo' in " << mtFile << " has no 'maptype' branch!"
                   << " Setting to 'unknown'.");
    }
    else {
      binInfo->SetBranchAddress("maptype", &maptype);
    }
    //maxDur
    idbr = (TBranch *) binInfo->GetListOfBranches()->FindObject("maxDur");
    if (!idbr) {
      log_warn("'BinInfo' in " << mtFile << " has no 'maxDur' branch!"
                   << " Setting to -1.");
    }
    else {
      binInfo->SetBranchAddress("maxDur", &bi.maxDur);
    }
    //minDur
    idbr = (TBranch *) binInfo->GetListOfBranches()->FindObject("minDur");
    if (!idbr) {
      log_warn("'BinInfo' in " << mtFile << " has no 'minDur' branch!"
                   << " Setting to -1.");
    }
    else {
      binInfo->SetBranchAddress("minDur", &bi.minDur);
    }
    //epoch
    idbr = (TBranch *) binInfo->GetListOfBranches()->FindObject("epoch");
    string *epoch = new  string("unknown");
    if (!idbr) {
      log_warn("'BinInfo' in " << mtFile << " has no 'epoch' branch!"
                   << " Setting to 'unknown'.");
    }
    else {
      binInfo->SetBranchAddress("epoch", &epoch);
    }
    
    for (int n = 0, nn = binInfo->GetEntries(); n < nn; n++) {
      bi.startMJD = 0;
      bi.stopMJD = 0;
      bi.duration = 2;
      bi.totalDuration = 24;
      bi.nEvents = -1;
      bi.minDur = -1;
      bi.maxDur = -1;
      //fill with actual entries
      binInfo->GetEntry(n);
      bi.maptype = maptype->c_str();
      bi.epoch = epoch->c_str();
      
      if (!hasName)
        nhname = BinIndexToName(nhid);
      
      if (reset || (binInfoMap_.find(nhname) == binInfoMap_.end())) {
        binInfoMap_[nhname] = bi;
      }
      else {
        binInfoMap_[nhname] += bi;
      }
    }

    //When using strings, SetBranchAddress doesn't seem to work with anything else, including smart pointers (I think it worked at some point, but maybe it was just my imagination). 20 pesotes for the person who can fix this. 
    delete maptype;
    delete epoch;
    
  }
}

double SkyMapCollection::GetIntegrationDuration(const BinName& nhbin) {
  if (binInfoMap_.find(nhbin) == binInfoMap_.end()) {
    log_fatal("nHit bin index " << nhbin << " is outside bin range.")
  }
  return binInfoMap_[nhbin].duration;
}

double SkyMapCollection::GetTotalDuration(const BinName& nhbin) {
  if (binInfoMap_.find(nhbin) == binInfoMap_.end()) {
    log_fatal("nHit bin index " << nhbin << " is outside bin range.")
  }
  return binInfoMap_[nhbin].totalDuration;
}

double SkyMapCollection::GetStartMJD(const BinName& nhbin) {
  if (binInfoMap_.find(nhbin) == binInfoMap_.end()) {
    log_fatal("nHit bin index " << nhbin << " is outside bin range.")
  }
  return binInfoMap_[nhbin].startMJD;
}

double SkyMapCollection::GetStopMJD(const BinName& nhbin) {
  if (binInfoMap_.find(nhbin) == binInfoMap_.end()) {
    log_fatal("nHit bin index " << nhbin << " is outside bin range.")
  }
  return binInfoMap_[nhbin].stopMJD;
}

double SkyMapCollection::GetNEvents(const BinName& nhbin) {
  if (binInfoMap_.find(nhbin) == binInfoMap_.end()) {
    log_fatal("nHit bin index " << nhbin << " is outside bin range.")
  }
  return binInfoMap_[nhbin].nEvents;
}

double SkyMapCollection::GetMaxDur(const BinName& nhbin) {
  if (binInfoMap_.find(nhbin) == binInfoMap_.end()) {
    log_fatal("nHit bin index " << nhbin << " is outside bin range.")
  }
  return binInfoMap_[nhbin].maxDur;
}

double SkyMapCollection::GetMinDur(const BinName& nhbin) {
  if (binInfoMap_.find(nhbin) == binInfoMap_.end()) {
    log_fatal("nHit bin index " << nhbin << " is outside bin range.")
  }
  return binInfoMap_[nhbin].minDur;
}

string SkyMapCollection::GetMaptype(const BinName& nhbin) {
  if (binInfoMap_.find(nhbin) == binInfoMap_.end()) {
    log_fatal("nHit bin index " << nhbin << " is outside bin range.")
  }
  return binInfoMap_[nhbin].maptype;
}

string SkyMapCollection::GetEpoch(const BinName& nhbin) {
  if (binInfoMap_.find(nhbin) == binInfoMap_.end()) {
    log_fatal("nHit bin index " << nhbin << " is outside bin range.")
  }
  return binInfoMap_[nhbin].epoch;
}

void SkyMapCollection::GetBinInfo(const BinName& nhbin,
                                  double &startMJD,
                                  double &stopMJD,
                                  double &nEvents,
                                  double &totDur,
                                  double &duration,
                                  string &maptype,
                                  double &maxDur,
                                  double &minDur,
                                  string &epoch) {
  if (binInfoMap_.find(nhbin) == binInfoMap_.end()) {
    log_fatal("nHit bin index " << nhbin << " is outside bin range.")
  }

  startMJD = binInfoMap_[nhbin].startMJD; 
  stopMJD  = binInfoMap_[nhbin].stopMJD; 
  nEvents  = binInfoMap_[nhbin].nEvents; 
  totDur   = binInfoMap_[nhbin].totalDuration;  
  duration = binInfoMap_[nhbin].duration;
  maptype  = binInfoMap_[nhbin].maptype; 
  maxDur   = binInfoMap_[nhbin].maxDur;  
  minDur   = binInfoMap_[nhbin].minDur;  
  epoch    = binInfoMap_[nhbin].epoch;   

}

