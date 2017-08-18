/*!
 * @file DetectorResponse.h
 * @author Robert Lauer
 * @date 25 Jul 2014
 * @brief Expected counts/histograms for 1 transit.
 * @version $Id: DetectorResponse.h 36120 2016-12-02 10:39:12Z samm $
 */

#ifndef DETECTOR_RESPONSE_H_INCLUDED
#define DETECTOR_RESPONSE_H_INCLUDED

#include <TFile.h>
#include <TTree.h>
#include <TDirectory.h>

#include <liff/BinDefinitions.h>
#include <liff/BinList.h>
#include <liff/InternalModel.h>
#include <liff/LogLogSpectrum.h>
//#include <liff/Models.h>
#include <liff/ResponseBin.h>

#include <hawcnest/Logging.h>

#include <map>
#include <math.h>
#include <sys/stat.h>

SHARED_POINTER_TYPEDEFS(TFile);

/*!
 * @class DetectorResponse
 * @author Robert Lauer
 * @date 25 Jul 2014
 * @ingroup
 * @brief Simulation derived response histograms/functions for 1 transit
 */

// ResponseBinMap indexed with <decbin, nhbin>
typedef std::pair<int, BinName> BinPair;
typedef std::map<BinPair, ResponseBinPtr> ResponseBinMap;

class DetectorResponse {

 public:

  DetectorResponse() {
    //necessary to make root Directory (with object names)
    //available for functions like TH1D.Project
    currentDir_ = gDirectory;

    //Simple Crab spectrum as basic initialization
    simSpectrum_ = LogLogSpectrumPtr(new LogLogSpectrum("LogLogSpectrum"));
    simSpectrum_->SimplePowerLaw(3.5e-11, 2.63);
    simNorm_ = simSpectrum_->GetNorm();
    spectrum_ = LogLogSpectrumPtr(new LogLogSpectrum(*simSpectrum_));
    norm_ = simNorm_;
  }

  DetectorResponse(const std::string filename) {
    //necessary to make root Directory (with object names)
    //available for functions like TH1D.Project
    currentDir_ = gDirectory;

    // This will avoid storing objects in the directory, so they are destroyed when they go out of scope
    TH1::AddDirectory(kFALSE);

    struct stat buf;
    if (stat(filename.c_str(), &buf) == 0) {
      Read(filename);
    }
    else {
      log_fatal("DetectorResponse file " << filename << " does not exist!");
    }
  }

  /// Read  in response histograms
  void Read(const std::string filename);

  /// Write response histograms to file
  void Write(const std::string filename);

  /// Make empty histograms from cuts/bin structure and dec-center list
  void ResetBins(const std::string cutFile, std::vector<double> decCenters);

  /// Return Dec bin index for declination
  int GetDecBinIndex(const double dec) const;

  /// Get Bin pointer
  ResponseBinPtr GetBin(const int decbin, const BinName& nhbin);

  /// Return expected number of gamma-ray events in all nHit bins
  double GetTotalExpectedSignal(const int decbin);

  /// Return expected number of background events in all nHit bins
  double GetTotalExpectedBackground(const int decbin);

  /// Get log-energies vector:
  std::vector<double> GetLogEnBins() {
    if (responseBins_.empty()) log_fatal("No response bins defined!");
    return responseBins_.begin()->second->GetLogEnBins();
  }

  /// Reweight based on fluxes for log-energy bins
  void ReweightEnergies(std::vector<double> fluxes);

  /// Return LogLogSpectrum object
  LogLogSpectrumPtr GetLogLogSpectrum() const {
    if (!spectrum_) {
      log_fatal("No spectrum defined!")
    }
    return spectrum_;
  }

  /// Return normalization for simulated response
  double GetSimDiffFlux1TeV() const {
    return simNorm_;
  };

  /// Return simulated spectrum
  LogLogSpectrumConstPtr const GetSimSpectrum() const {
    if (!simSpectrum_) {
      log_fatal("No simulated spectrum defined!")
    }
    return simSpectrum_;
  }

  /// Make signal response histogram for given parameter name
  /// from weighted SWEETS-root
  void FillSignalHistFromSWEETS
      (TTree *events, TH1DPtr hist, const std::string parameter, const TCut cuts = TCut("1"));

  /// Make background response histogram for given parameter name
  /// from weighted SWEETS-root
  void FillBackgroundHistFromSWEETS
      (TTree *events, TH1DPtr hist, const std::string parameter, const TCut cuts = TCut("1"));

  /// Make detector response histograms for all bins from weighted SWEETS-root
  void MakeAllHistFromSWEETS(const std::string sweetspath, const Func1Ptr spectrum);

  int ListDecBins() const { return BinDefinitions::PrintDecBins(decBins_); }

  int ListAnalysisBins() const { return BinDefinitions::PrintAnalysisBins(analysisBins_); }

  DecBinMap GetDecBinMap() { return decBins_; };

  const AnalysisBinMap& GetAnalysisBinMap() { return analysisBins_; };

  TDirectory *GetCurrentTDirectory() { return currentDir_; };

 private:

   /// Check all the wanted SWEETS files are present. log_fatal if not.
   void CheckSWEETSFiles(const std::string sweetspath);

  ///Find weighted gamma SWEETS-root file for given dec and set simSpectrum & simNorm
  TFilePtr OpenSWEETS(const std::string sweetspath, const int dec);

  DecBinMap decBins_;

  AnalysisBinMap analysisBins_;

  void ClearResponseBinMap(ResponseBinMap &binmap);

  ResponseBinMap responseBins_;

  //model spectrum to be tested in fit
  LogLogSpectrumPtr spectrum_;

  //signal normalization, differential flux at 1 TeV
  double norm_;

  //simulated reference spectrum, sored in simEnSig histograms
  LogLogSpectrumPtr simSpectrum_;

  //simulated normalization, differential flux at 1 TeV
  double simNorm_;

  TDirectory *currentDir_;

  //Added by GV
  double getValueFromSpectrum(double logE);
  std::map<double, double> spModelHash_;

};

#endif // DETECTOR_RESPONSE_H_INCLUDED
