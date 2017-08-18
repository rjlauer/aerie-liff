/*!
 * @file DetectorResponse.cc
 * @author Robert Lauer
 * @date 25 Jul 2014
 * @brief Expected counts/histograms for 1 transit.
 * @version $Id: DetectorResponse.cc 36445 2016-12-19 16:27:10Z criviere $
 */

#include <TObjArray.h>
#include <TVectorD.h>

#include <dirent.h>

#include <fstream>
#include <iomanip>
#include <sstream>

#include <boost/algorithm/string.hpp>

#include <liff/BinList.h>
#include <liff/DetectorResponse.h>

using namespace std;

void DetectorResponse::Read(string filename) {

  TFile infile(filename.c_str());
  currentDir_->cd();

  //test if first PSF histogram is there
  if (!(TTree *) infile.FindObjectAny("DecBins")) {
    log_fatal("Found no 'DecBins' tree in file " << filename);
  }

  //spectrum and norm
  LogLogSpectrum *llspectrum = 0;
  infile.GetObject("LogLogSpectrum", llspectrum);
  simSpectrum_ = LogLogSpectrumPtr((LogLogSpectrum *) llspectrum->Clone());
  simNorm_ = simSpectrum_->GetNorm();
  //Initialize spectrum_ from simSpectrum_
  spectrum_ = LogLogSpectrumPtr(new LogLogSpectrum(*simSpectrum_));
  norm_ = simNorm_;

  TTree *decBinTree = (TTree *) infile.Get("DecBins");
  DecBin dbin;
  double decwidth = 0;
  int nd = decBinTree->GetEntries();
  TBranch *cbr = (TBranch *) decBinTree->GetListOfBranches()->FindObject("center");
  if (cbr) {
    log_debug("DetectorResponse file stores dec band in old style with center/width.");
    decBinTree->SetBranchAddress("center", &dbin.simDec_);
    decBinTree->SetBranchAddress("width", &decwidth);
  }
  else {
    decBinTree->SetBranchAddress("simdec", &dbin.simDec_);
    decBinTree->SetBranchAddress("lowerEdge", &dbin.lowerEdge_);
    decBinTree->SetBranchAddress("upperEdge", &dbin.upperEdge_);
  }

  string analysisBinTreeName = "AnalysisBins";
  if (! infile.GetListOfKeys()->Contains(analysisBinTreeName.c_str())) {
    // New name not found, try with the old one
    analysisBinTreeName = "NHitBins";
    if (! infile.GetListOfKeys()->Contains(analysisBinTreeName.c_str())) {
      log_fatal("Neither AnalysisBins or NHitBins tree not found in detector "
                "response file");
    }
  }
  TTree *analysisBinTree = (TTree *) infile.Get(analysisBinTreeName.c_str());
  AnalysisBin nbin;
  int nn = analysisBinTree->GetEntries();
  //analysisBinTree->SetBranchAddress("min",  &nbin.min);
  //analysisBinTree->SetBranchAddress("max",  &nbin.max);
  TCut *cuts = &nbin.cuts_;
  analysisBinTree->SetBranchAddress("cuts", &cuts);
  
  const TObjArray* const branch = analysisBinTree->GetListOfBranches();
  const bool hasName            = branch->FindObject("name");

  BinName nhname;
  const BinName* namePtr = &nhname;
  int nhid = -1;
  if (hasName)
    analysisBinTree->SetBranchAddress("name", &namePtr);
  else if (branch->FindObject("id"))
    analysisBinTree->SetBranchAddress("id", &nhid);
  else
    log_fatal("Cannot find branch 'name' or 'id' in analysis-bin tree.")
  
  decBins_.clear();
  analysisBins_.clear();
  ClearResponseBinMap(responseBins_);
  spModelHash_.clear();

  for (int d = 0; d < nd; ++d) {
    decBinTree->GetEntry(d);
    if (cbr) {
      dbin.lowerEdge_ = dbin.simDec_ - decwidth / 2;
      dbin.upperEdge_ = dbin.simDec_ + decwidth / 2;
    }
    decBins_[d] = dbin;

    for (int n = 0; n < nn; ++n) {
      analysisBinTree->GetEntry(n); //replaces nhid with value, if id branch exists
      
      if (!hasName)
        nhname = BinIndexToName(nhid);
      
      if (!d) {
        analysisBins_[nhname] = nbin;
      }
      
      const string dir = Form(
        "dec_%02d/nh_%s/", d, PadBinName(nhname).c_str()
      );
      const string suffix = Form(
        "_dec%d_nh%s", d, nhname.c_str()
      );
      
      ResponseBinPtr bin = GetBin(d, nhname);
      TH1D *histpointer = 0;

      infile.GetObject((dir + "PSF" + suffix).c_str(), histpointer);
      bin->simPsfHist_ = TH1DPtr((TH1D *) histpointer->Clone());
      delete histpointer;

      infile.GetObject((dir + "EnSig" + suffix).c_str(), histpointer);
      bin->simEnSigHist_ = TH1DPtr((TH1D *) histpointer->Clone());
      delete histpointer;

      bin->simFluxes_.clear();
      bin->logEnBins_.clear();
      for (int b = 1; b <= bin->simEnSigHist_->GetNbinsX(); ++b) {
        const double logen = bin->simEnSigHist_->GetBinCenter(b);
        (bin->simFluxes_).push_back(pow(10., getValueFromSpectrum(logen)));
        (bin->logEnBins_).push_back(logen);
      }
      bin->sigExp_ = bin->simEnSigHist_->Integral(); //exp. signal events

      infile.GetObject((dir + "EnBg" + suffix).c_str(), histpointer);
      bin->simEnBgHist_ = TH1DPtr((TH1D *) histpointer->Clone());
      delete histpointer;

      bin->bgExp_ = bin->simEnBgHist_->Integral(); //exp. bkg events

      TF1 *funcpointer = 0;

      infile.GetObject((dir + "PSF" + suffix + "_fit").c_str(), funcpointer);
      bin->simPsfFunc_ = TF1Ptr((TF1 *) funcpointer->Clone());
      delete funcpointer;

      infile.GetObject((dir + "EnSig" + suffix + "_fit").c_str(), funcpointer);
      bin->simEnSigFunc_ = TF1Ptr((TF1 *) funcpointer->Clone());
      delete funcpointer;

      infile.GetObject((dir + "EnBg" + suffix + "_fit").c_str(), funcpointer);
      bin->simEnBgFunc_ = TF1Ptr((TF1 *) funcpointer->Clone());
      delete funcpointer;
    }
  }

  infile.Close();

  log_debug("Loaded response histograms from :  " << filename << endl);
  //<<"  with spectrum: "<<simSpectrum_->PrintOut()<<endl
  //<<"  and differential flux at 1-TeV: "<<simNorm_);
}


void DetectorResponse::Write(string filename) {
  TFile *outfile = new TFile(filename.c_str(), "RECREATE");

  spectrum_->Write("LogLogSpectrum");

  DecBin dbin;
  TTree *decBinTree = new TTree("DecBins", "DecBins");
  decBinTree->Branch("simdec", &dbin.simDec_, "simdec/D");
  decBinTree->Branch("lowerEdge", &dbin.lowerEdge_, "lowerEdge/D");
  decBinTree->Branch("upperEdge", &dbin.upperEdge_, "upperEdge/D");

  AnalysisBin nbin;
  TTree *analysisBinTree = new TTree("AnalysisBins", "AnalysisBins");
  //analysisBinTree->Branch("min",  &nbin.min,  "min/I");
  //analysisBinTree->Branch("max",  &nbin.max,  "max/I");
  analysisBinTree->Branch("cuts", "TCut", &nbin.cuts_);

  BinName nhname;
  BinName* namePtr = &nhname;
  analysisBinTree->Branch("name", &namePtr);

  DecBinMap::const_iterator db;
  for (db = decBins_.begin(); db != decBins_.end(); ++db) {
    int d = db->first;
    dbin = db->second;
    decBinTree->Fill();

    AnalysisBinMap::const_iterator nhb;
    for (nhb = analysisBins_.begin(); nhb != analysisBins_.end(); ++nhb) {
      nhname = nhb->first;
      if (db->first == 0) {
        nbin = nhb->second;
        analysisBinTree->Fill();
      }

      const string dir = Form("dec_%02d/nh_%s", d, PadBinName(nhname).c_str());
      outfile->mkdir(dir.c_str());
      outfile->cd(dir.c_str());

      ResponseBinPtr bin = GetBin(d, nhname);
      bin->GetPsfHist()->Write();
      bin->GetEnSigHist()->Write();
      bin->GetEnBgHist()->Write();

      bin->GetPsfFunction()->Write();
      bin->GetEnSigFunction()->Write();
      bin->GetEnBgFunction()->Write();

    }
  }

  outfile->cd();
  decBinTree->Write();
  analysisBinTree->Write();

  outfile->Close();
  log_info("Created new response output file " << filename);
  currentDir_->cd();
}


void DetectorResponse::ResetBins
    (const string cutFile, vector<double> decCenters) {
  ClearResponseBinMap(responseBins_);
  spModelHash_.clear();

  BinDefinitions::LoadDecBands(decCenters, decBins_);
  if (decBins_.empty()) {
    log_fatal("No dec bands defined!");
  }
  else {
    log_info("Preparing DetectorResponse for " << decBins_.size()
                 << " dec bands:");
    ListDecBins();
  }

  BinDefinitions::LoadBins(cutFile, analysisBins_);
  if (analysisBins_.empty()) {
    log_fatal("No analysis bins defined!");
  }
  else {
    log_info("Preparing DetectorResponse for " << analysisBins_.size()
                 << " analysis bins:");
    ListAnalysisBins();
  }

  //log energy bins:
  int nebins = 140;
  double nemin = -3.;
  double nemax = 4;
  double logen = nemin;
  vector<double> logenbins;
  vector<double> simfluxes;
  for (int i = 0; i <= nebins; i++) {
    logen = nemin + i * (nemax - nemin) / ((double) nebins);
    logenbins.push_back(logen);
    simfluxes.push_back(pow(10., getValueFromSpectrum(logen)));
  }

  DecBinMap::const_iterator db;
  for (db = decBins_.begin(); db != decBins_.end(); ++db) {
    int d = db->first;
    AnalysisBinMap::const_iterator nhb;
    for (nhb = analysisBins_.begin(); nhb != analysisBins_.end(); ++nhb) {
      const BinName& n = nhb->first;
      ResponseBinPtr bin = GetBin(d, n);
      
      const string suffix = Form("_dec%d_nh%s", d, n.c_str());
      
      string name = "simPSF" + suffix;
      bin->simPsfHist_ = TH1DPtr(
        new TH1D(name.c_str(), name.c_str(), 200, 0., 10.)
      );
      bin->simPsfHist_->Sumw2();
      
      name = "simPSF" + suffix + "_fit";
      bin->simPsfFunc_ = TF1Ptr(new TF1(name.c_str(), "gaus", 0., 10.));

      name = "simEnSig" + suffix;
      bin->simEnSigHist_ = TH1DPtr(
        new TH1D(name.c_str(), name.c_str(), nebins, &logenbins[0])
      );
      bin->simEnSigHist_->Sumw2();
      bin->logEnBins_ = logenbins;
      bin->simFluxes_ = simfluxes;

      name = "simEnSig" + suffix + "_fit";
      bin->simEnSigFunc_ = TF1Ptr(new TF1(name.c_str(), "gaus", nemin, nemax));

      name = "simEnBg" + suffix;
      bin->simEnBgHist_ = TH1DPtr(
        new TH1D(name.c_str(), name.c_str(), nebins, &logenbins[0])
      );
      bin->simEnBgHist_->Sumw2();
      
      name = "simEnBg" + suffix + "_fit";
      bin->simEnBgFunc_ = TF1Ptr(new TF1(name.c_str(), "gaus", nemin, nemax));

    }
  }
}


int DetectorResponse::GetDecBinIndex(const double dec) const {
  DecBinMap::const_iterator db;
  for (db = decBins_.begin(); db != decBins_.end(); ++db) {
    if ((dec >= db->second.lowerEdge_)
        && (dec < db->second.upperEdge_)) {
      break;
    }
  }
  if (db == decBins_.end()) {
    log_fatal("Declination " << dec << " degrees outside defined dec-bins.");
  }
  return db->first;
}


ResponseBinPtr DetectorResponse::GetBin
    (const int decbin, const BinName& nhbin) {

  //Just try to access the requested bin. This is the fastest solution for the
  //normal case when we have already a bin. If not, then we build it.

  try {

    return responseBins_.at(BinPair(decbin,nhbin));

  } catch (...) {

    if ((decbin < 0) || (decbin > (decBins_.rbegin()->first))) {

      log_fatal("Dec bin index " << decbin << " does not exist!");

    } else if (analysisBins_.find(nhbin) == analysisBins_.end()) {

      log_fatal("nHit bin index " << nhbin << " does not exist!");

    } else {

      responseBins_[BinPair(decbin,nhbin)] = ResponseBinPtr(new ResponseBin(decbin, nhbin));

    }

    return responseBins_.at(BinPair(decbin,nhbin));

  }
}


double DetectorResponse::GetTotalExpectedSignal(const int decbin) {
  double sum = 0.;
  AnalysisBinMap::const_iterator nhb;
  for (nhb = analysisBins_.begin(); nhb != analysisBins_.end(); ++nhb)
    sum += GetBin(decbin, nhb->first)->GetExpectedSignal();
  return sum;
}


double DetectorResponse::GetTotalExpectedBackground(const int decbin) {
  double sum = 0.;
  AnalysisBinMap::const_iterator nhb;
  for (nhb = analysisBins_.begin(); nhb != analysisBins_.end(); ++nhb)
    sum += GetBin(decbin, nhb->first)->GetExpectedBackground();
  return sum;
}


void  DetectorResponse::ReweightEnergies(vector<double> fluxes) {
  DecBinMap::const_iterator db;
  for (db = decBins_.begin(); db != decBins_.end(); ++db) {
    int d = db->first;
    AnalysisBinMap::const_iterator nhb;
    for (nhb = analysisBins_.begin(); nhb != analysisBins_.end(); ++nhb) {
      ResponseBinPtr bin = GetBin(d, nhb->first);
      bin->ReweightEnergies(fluxes);
    }
  }
  //spectrum_ and norm_ invalid now:
  spectrum_.reset();
  norm_ = 0;
}


//The following method is a memorization trick: each time
//it is called, it memorizes in a map the value of the function
//at that particular logE value. The next time the method is called
//for the same logE value, it will return the memorized value. This
//avoids unnecessary repeated calls to the spectral function, which
//might be expensive to compute.
double
DetectorResponse::getValueFromSpectrum(double logE) {
  if (spModelHash_.count(logE) > 0)
    return spModelHash_[logE];

  double val = spectrum_->Eval(logE);
  spModelHash_[logE] = val;
  return val;
}


void DetectorResponse::FillSignalHistFromSWEETS
    (TTree *events, TH1DPtr hist, const string parameter, const TCut cuts) {
  currentDir_->cd(); //set root dir to the one where histograms are named
  TCut selection = cuts * Form("(mc.corsikaParticleId==1)*(sweets.TWgt)"
                                   "*(rec.angleFitStatus==0)");
  log_debug("Project SWEETS data into " << hist->GetName());
  events->Project(hist->GetName(), parameter.c_str(), selection);
}


void DetectorResponse::FillBackgroundHistFromSWEETS
    (TTree *events, TH1DPtr hist, const string parameter, const TCut cuts) {
  currentDir_->cd(); //set root dir to the one where histograms are named
  TCut selection = cuts * Form("(mc.corsikaParticleId!=1)*(sweets.TWgt)"
                                   "*(rec.angleFitStatus==0)");
  log_debug("Project SWEETS data into " << hist->GetName());
  events->Project(hist->GetName(), parameter.c_str(), selection);
}


void DetectorResponse::CheckSWEETSFiles(const string sweetspath) {
  if (decBins_.empty()) {
    log_fatal("No bins defined, run ResetBins(binlist)!");
  }
  DecBinMap::const_iterator db;
  for (db = decBins_.begin(); db != decBins_.end(); ++db) {
    log_info("Try to open SWEETS ROOT file for dec "
                 << (int) db->second.simDec_ << " in directory " << sweetspath);
    TFilePtr sweets = OpenSWEETS(sweetspath, (int) db->second.simDec_);
    log_info("SWEETS file successfuly opened");
    sweets->Close();
  }
  log_info("All SWEETS files are present.")
}


void DetectorResponse::MakeAllHistFromSWEETS(const string sweetspath, const Func1Ptr spectrum) {
  this->CheckSWEETSFiles(sweetspath);
  DecBinMap::const_iterator db;
  const double pi = acos(-1);
  for (db = decBins_.begin(); db != decBins_.end(); db++) {
    int d = db->first;
    log_info("Try to open SWEETS ROOT file for dec "
                 << (int) db->second.simDec_ << " in directory " << sweetspath);
    TFilePtr sweets = OpenSWEETS(sweetspath, (int) db->second.simDec_); //this also sets new simSpectrum_
    log_info("SWEETS file successfuly opened");
    //if a spectrum Func1Ptr is provided, rescale weights accordingly:
    TCut spectrumweight = "1.";
    if (spectrum) {
      //from sweets, set inside OpenSWEETS():
      double sweets_n = simSpectrum_->GetNorm();
      double sweets_i = simSpectrum_->GetIndex();
      double sweets_c = simSpectrum_->GetCutoff();
      if (sweets_c<=0) sweets_c = 1e10; 
      //chosen in function call in MakeDetectorResponse:
      double mdr_n = spectrum->GetParameter(0);
      double mdr_i = spectrum->GetParameter(1);
      double mdr_c = spectrum->GetParameter(2);
      spectrumweight = 
        Form("%e/%e*pow(10,(mc.logEnergy-3)*(-%f+%f))*exp(-pow(10,mc.logEnergy-3)*(1/%e-1/%e))",
            mdr_n,sweets_n,mdr_i,sweets_i,mdr_c,sweets_c); 
      log_info("reweighting each event based on flux ratio for input over sweets spectrum:"
                << spectrumweight );
      //store new simSpectrum_:
      simSpectrum_->CutOffPowerLaw(mdr_n,mdr_i,mdr_c);
    }
    //reset stored spectral information to new simSpectrum
    spectrum_ = LogLogSpectrumPtr(new LogLogSpectrum(*simSpectrum_));
    
    AnalysisBinMap::const_iterator nhb;
    for (nhb = analysisBins_.begin(); nhb != analysisBins_.end(); ++nhb) {
      ResponseBinPtr bin = GetBin(d, nhb->first);
      TCut cuts = nhb->second.cuts_;
      TTree *events = 0;
      sweets->GetObject("XCDF", events);
      if (!events) {
        log_fatal("Object 'XCDF' not found in ROOT file.");
      }
      // First, cut input tree for speedup
      // but ROOT is evil, so need a writable TFile opened
      TFile *tempfile = new TFile("temporary_sweets_in_bin.root", "recreate");
      TTree *events_in_bin = events->CopyTree(cuts.GetTitle());
      
     
      //PSF
      FillSignalHistFromSWEETS(events_in_bin, bin->simPsfHist_,
                               Form("mc.delAngle*180./%f", pi),spectrumweight);
      double integ = bin->simPsfHist_->Integral();
      bin->simPsfHist_->Scale(1 / integ); //normalize to 1
      bin->GetPsfHist(true); //reset
      //Energy Signal, in TeV instead of GeV
      FillSignalHistFromSWEETS(events_in_bin, bin->simEnSigHist_,
                               "mc.logEnergy-3.",spectrumweight);
      bin->GetEnSigHist(true); //reset
      integ = bin->simEnSigHist_->Integral();
      bin->sigExp_ = integ;
      //Energy Background, in TeV instead of GeV
      FillBackgroundHistFromSWEETS(events_in_bin, bin->simEnBgHist_,
                                   "mc.logEnergy-3.");
      //keep normalized to per steradian
      bin->GetEnBgHist(true); //reset
      integ = bin->simEnBgHist_->Integral();
      bin->bgExp_ = integ;
      tempfile->Close();
    }
    sweets->Close();
  }
}


TFilePtr DetectorResponse::OpenSWEETS
    (const string sweetspath, const int dec) {
  string sweetsfile;
  vector<string> part;
  double si = 0;
  double cutoff = 0;
  DIR *pDIR;
  struct dirent *entry;
  // old file name format (all in top directory)
  log_trace("Trying to read SWEETS files with old MC file structure");
  pDIR = opendir(sweetspath.c_str());
  if (pDIR != NULL) {
    while ((entry = readdir(pDIR))) {
      string fname(entry->d_name);
      log_trace("File candidate: " << fname);
      string name = fname.substr(0, fname.find_last_of("."));
      boost::split(part, name, boost::is_any_of("_"));
      string sfx = fname.substr(fname.find_last_of(".") + 1);
      log_trace("File candidate part.size, sfx: " << part.size() << " " << sfx << " ");
      if (sfx != "root") continue;
      // old format
      if (part.size() >= 6) {
        if ((part[0] == "sweets") && (part[1] == "transit")) {
          if (atoi(part[5].c_str()) == dec) {
            sweetsfile = Form("%s/%s", sweetspath.c_str(), fname.c_str());
            si = atof(part[2].c_str());
            simNorm_ = atof(part[3].c_str());
            cutoff = atof(part[4].c_str());
            break;
          }
        }
      }
      // New format, but all in the same directory
      if (part.size() >= 7) {
        if ((part[5] == "combined") && (part[6] == "rec")) {
          if (atoi(part[3].c_str()) == dec) {
            sweetsfile = Form("%s/%s", sweetspath.c_str(), fname.c_str());
            si = atof(part[0].c_str());
            simNorm_ = atof(part[1].c_str());
            cutoff = atof(part[2].c_str());
            break;
          }
        }
      }
    }
  }
  // new MC file structure
  if (sweetsfile.empty()) {
    log_trace("Trying to read SWEETS files with new MC file structure");
    pDIR = opendir(sweetspath.c_str());
    if (pDIR != NULL) {
      while ((entry = readdir(pDIR))) {
        string name(entry->d_name);
        boost::split(part, name, boost::is_any_of("_"));
        if (part.size() < 5) continue;
        else if (atoi(part[3].c_str()) == dec) {
          sweetsfile = Form("%s/%s/succeeded/%s_combined_rec.root", sweetspath.c_str(), name.c_str(), name.c_str());
          si = atof(part[0].c_str());
          simNorm_ = atof(part[1].c_str());
          cutoff = atof(part[2].c_str());
          break;
        }
      }
    }
  }
  if (sweetsfile.empty()) {
    log_fatal("found no SWEETS ROOT file for dec=" << dec << " in directory " << sweetspath);
  } else {
    log_info("found SWEETS ROOT file " << sweetsfile);
  }

  simSpectrum_ = LogLogSpectrumPtr(new LogLogSpectrum("LogLogSpectrum"));
  if ((cutoff == 0) || (cutoff >= 100000)) {
    simSpectrum_->SimplePowerLaw(simNorm_, si);
  } else {
    simSpectrum_->CutOffPowerLaw(simNorm_, si, cutoff);
  }

  log_info("Loading histograms from SWEETS file:" << endl
               << "  " << sweetsfile << endl << "  with spectrum: ");
  simSpectrum_->PrintOut();

  TFilePtr openfile = TFilePtr(new TFile(sweetsfile.c_str()));
  if ((!openfile->IsOpen()) || openfile->IsZombie()) {
    log_fatal("Could not open file " << sweetsfile);
  }

  return openfile;
}


void DetectorResponse::ClearResponseBinMap(ResponseBinMap &binmap) {
  binmap.clear();
}
