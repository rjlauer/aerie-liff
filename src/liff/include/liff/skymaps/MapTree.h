/*!
 * @file MapTree.h
 * @author Robert Lauer
 * @date 10 Sep 2014
 * @brief I/O class for reading/writing HEALPix maps to ROOT TTree disk objects.
 * @version $Id: MapTree.h 31968 2016-05-11 23:32:54Z criviere $
 */

#ifndef MAP_TREE_H
#define MAP_TREE_H

#include <TFile.h>
#include <TTree.h>
#include <TParameter.h>

#include <healpix_map.h>

#include <hawcnest/Logging.h>

#include <sys/stat.h>
#include <limits>

/// A class to store Healpix_Map data in a root TTree
class MapTree {

 public:

  MapTree()
      : fname_(""),
        file_(0),
        tree_(0) { }

  /// Constructs MapTree that by connecting to a Healpix_Map
  template<typename T>
  MapTree(Healpix_Map<T> &map)
      : fname_(""),
        file_(0) { SetMap(map); }

  /// Constructs MapTree by opening a TTree in a TFile
  MapTree(std::string filename, std::string treename)
      : file_(0) {
    if (!OpenFile(filename)) {
      log_fatal("TFile " << filename << " does not exist.")
    }
    if (!OpenTree(treename)) {
      log_fatal("TTree " << treename << " does not exist in " << filename << ".")
    }
  }

  /// Copy contents from Healpix_Map
  void SetMap(Healpix_Map<double> &map) {
    map_ = map;
    tree_ = 0;
  }

  /// Copy contents from Healpix_Map and convert to doubles
  void SetMap(Healpix_Map<float> &map) {
    map_.SetNside(map.Nside(), map.Scheme());
    for (int p = 0; p < Npix(); p++) {
      map_[p] = (double) map[p];
    }
    tree_ = 0;
  }

  /// Copy contents from Healpix_Map and convert to doubles
  void SetMap(Healpix_Map<int> &map) {
    map_.SetNside(map.Nside(), map.Scheme());
    for (int p = 0; p < Npix(); p++) {
      map_[p] = (double) map[p];
    }
    tree_ = 0;
  }

  /// Open TFile; return true if TFile exists, false if not
  bool OpenFile(std::string filename) {
    log_debug("Trying to open file \""<<filename<<"\"...");
    if ((file_!=0) && (fname_ == filename) && (file_->IsOpen())) {
      log_debug("... already open!");
      return true;
    }
    CloseFile();
    struct stat buf;
    if (stat(filename.c_str(), &buf) == 0) {
      // the file exists
      file_ = new TFile(filename.c_str());
      if (file_->IsZombie()) {
        log_fatal("Could not open file \"" << filename << "\".");
      }
      fname_ = filename;
        log_debug("... succeeded.");
      return true;
    } else {
      // the file does not exist
      log_debug("... failed.");
      return false;
    }
  }

  /// Create new TFile
  void CreateFile(std::string filename) {
    CloseFile();
    file_ = new TFile(filename.c_str(), "RECREATE");
    if (!file_->IsWritable()) {
      log_fatal("Could not create file \"" << filename << "\".");
    }
    fname_ = filename;
    log_trace("Created new TFile \"" << filename << "\".");
  }

  /// Open TTree; return true if TTree exists, false if not
  bool OpenTree(std::string treename) {
    if (file_==0 || !file_->IsOpen()) {
      log_fatal("No TFile open, do OpenFile(name) first.");
    }
    tree_ = 0;
    map_.Set(0, string2HealpixScheme("RING")); //clear map
    file_->GetObject(treename.c_str(), tree_);
    if (tree_) {
      TParameter<int> *nside = (TParameter<int> *) tree_->GetUserInfo()->FindObject("Nside");
      TParameter<int> *si = (TParameter<int> *) tree_->GetUserInfo()->FindObject("Scheme");
      Healpix_Ordering_Scheme scheme = (Healpix_Ordering_Scheme) si->GetVal();
      map_.SetNside(nside->GetVal(), scheme);
      tree_->SetBranchAddress("count", &count_);
      return true;
    }
    log_warn("Cannot open MapTree " << treename << " in file " << file_->GetName())
    return false;
  }

  /// Create new TTree
  void CreateTree(std::string treename) {
    if (!file_->IsOpen()) {
      log_fatal("No TFile open, do OpenFile(name) first.");
    }
    tree_ = new TTree(treename.c_str(), treename.c_str());
    log_trace("Created new TTree " << treename << " in TFile " << file_->GetName())
    tree_->Branch("count", &count_, "count/D");
  }

  /// Returns nside of the stored healpix data
  int Nside() const { return map_.Nside(); }

  /// Returns number of pixels stored in healpix data
  int Npix() const { return map_.Npix(); }

  /// Returns order of the stored healpix data
  int Order() const { return map_.Order(); }

  /// Returns scheme of the stored healpix data
  Healpix_Ordering_Scheme Scheme() const { return map_.Scheme(); }

  /// Returns value stored in pixel number p
  double GetPixel(int p) const {
    tree_->GetEntry(p);
    return count_;
  }

  /// Returns full Healpix_map, cast required
  template<typename T>
  Healpix_Map<T> GetMap() {
    Healpix_Map<T> map(Nside(), Scheme(), SET_NSIDE);
    for (int p = 0; p < Npix(); p++) {
      map[p] = (T) GetPixel(p);
    }
    return map;
  }

  /// Write Healpix_Map data as TTree into a TFile
  void Write(std::string filename, std::string treename) {
    if (Npix() == 0) {
      log_fatal("No Healpix_Map data defined. Use SetMap first.")
    }
    if (!OpenFile(filename)) {
      CreateFile(filename);
    }
    else {
      file_->ReOpen("UPDATE");
      tree_ = (TTree *) file_->Get(treename.c_str());
      if (tree_) {
        CloseFile();
        log_fatal("TTree " << treename << " already exists in TFile "
                      << filename << " .");
      }
    }
    size_t slash = treename.find_last_of("/");
    if (slash != std::string::npos) {
      std::string dir = treename.substr(0, slash);
      if (!file_->GetDirectory(dir.c_str())) file_->mkdir(dir.c_str());
      file_->cd(dir.c_str());
      treename.erase(0, slash + 1);
    }
    CreateTree(treename);
    for (int i = 0; i < Npix(); i++) {
      count_ = map_[i];
      tree_->Fill();
    }

    TParameter<int> nside("Nside", map_.Nside());
    TParameter<int> scheme("Scheme", map_.Scheme());
    tree_->GetUserInfo()->Add(&nside);
    tree_->GetUserInfo()->Add(&scheme);

    tree_->Write();
    tree_->GetUserInfo()->Clear();
    file_->cd();
  }

  /// Closes TFile
  void CloseFile() {
    if (file_) {
      if (file_->IsOpen()) {
        file_->Close();
      }
      //delete file_;
      fname_ = "";
    }
  }

 private:

  std::string fname_;
  TFile *file_;
  TTree *tree_;
  Healpix_Map<double> map_;
  double count_;

};
#endif
