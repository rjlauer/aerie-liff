/*!
 * @file BinDefinitions.cc
 * @author Patrick Younk
 * @date 25 Jul 2014
 * @brief Store analysis bin definitions (Nhit, declination bands, etc.).
 * @version $Id: BinDefinitions.cc 36120 2016-12-02 10:39:12Z samm $
 */

#include <cstdlib>
#include <string>

#include <liff/BinDefinitions.h>
#include <liff/BinList.h>

#include <hawcnest/Logging.h>

using namespace std;

namespace BinDefinitions {

void LoadDecBands(vector<double> decCenters, DecBinMap &decBins) {
  decBins.clear();
  DecBin dbn;
  dbn.lowerEdge_ = -30; //will be overridden
  for (unsigned int n = 0; n < decCenters.size(); ++n) {
    dbn.simDec_ = decCenters[n];
    decBins[n] = dbn;
    if (n > 0) {
      if (dbn.simDec_ <= decBins[n - 1].simDec_) {
        log_fatal("Declination band centers have to be in ascending order!");
      }
      decBins[n].lowerEdge_ = dbn.simDec_ - (dbn.simDec_ - decBins[n - 1].simDec_) / 2.;
      decBins[n - 1].upperEdge_ = decBins[n].lowerEdge_;
    }
  }
  int ds = decBins.size();
  if (ds > 1) {
    log_debug("Found " << decCenters.size() << " dec bins.")
    decBins[0].lowerEdge_ =
        decBins[0].simDec_ - (decBins[0].upperEdge_ - decBins[0].simDec_);
    decBins[ds - 1].upperEdge_ =
        decBins[ds - 1].simDec_ + (decBins[ds - 1].simDec_ - decBins[ds - 1].lowerEdge_);
  }
  else {
    decBins[0].lowerEdge_ = decBins[0].simDec_ - 25.;
    decBins[0].upperEdge_ = decBins[0].simDec_ + 25.;
    log_info("Only one declination band center defined (" << decBins[0].simDec_
                 << "), assuming +/- 25 deg. dec band.")
  }
}

void LoadBins(string cutFile, AnalysisBinMap &analysisBins) {
  analysisBins.clear();

  ifstream in(cutFile.c_str());
  if (!in.is_open()) {
    log_fatal("Could not open cut definitions file " << cutFile);
  }
  string line;
  BinName name;
  while (getline(in, line)) {
    log_trace("Reading line: " << line);

    stringstream ls(line);

    if (ls.peek() == '#') continue;

    else if (ls.peek() == '\n') continue;

    else if (ls.peek() == EOF) continue;

    ls >> name;
    ls.ignore(2048, '"');
    string cuts;
    getline(ls, cuts, '"');
    if (!ls)
      log_fatal("Failed to parse line \"" << line << "\" in cuts file.")
    AnalysisBin nhn;
    nhn.cuts_ = cuts.c_str();
    analysisBins.insert(AnalysisBinMap::value_type(name, nhn));
      
  }
  in.close();
}

int PrintDecBins(const DecBinMap &decBins) {
  if (decBins.empty()) {
    cout << endl << "No declination bands defined." << endl;
    return 0;
  }
  else {
    cout << endl << "declination bands:" << endl;
    cout << " bin#   lower edge   upper edge" << endl;
    DecBinMap::const_iterator db;
    for (db = decBins.begin(); db != decBins.end(); ++db) {
      cout << Form("  %2d      %5.1f        %5.1f",
                   db->first, db->second.lowerEdge_, db->second.upperEdge_)
          << endl;
    }
    cout << endl;
    return (db->first + 1);
  }
}


int PrintAnalysisBins(const AnalysisBinMap &analysisBins) {
  if (analysisBins.empty()) {
    cout << endl << "No nHit bins defined." << endl;
    return 0;
  }

  AnalysisBinMap::const_iterator nhb;
  
  int maxWidth = 0;
  for (nhb = analysisBins.begin(); nhb != analysisBins.end(); ++nhb) {
    const int width = nhb->first.size();
    if (width > maxWidth)
      maxWidth = width;
  }
  
  static const string binHeading = "bin";
  static const string separator  = "   ";
  cout << endl
       << "nHit bins:" << endl
       << " " << binHeading;
  for (int i = 0; i < maxWidth - int(binHeading.size()); ++i)
    cout << " ";
  cout << separator << "cuts" << endl;
  int ret = 0;
  for (nhb = analysisBins.begin(); nhb != analysisBins.end(); ++nhb, ++ret)
    cout << " " << nhb->first << separator << nhb->second.cuts_.GetTitle()
         << endl;
  cout << endl;
  
  return ret;
}

} // end namespace BinDefinitions
