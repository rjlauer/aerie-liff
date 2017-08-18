/*!
 * @file BinDefinitions.h
 * @author Patrick Younk
 * @date 25 Jul 2014
 * @brief Store analysis bin definitions (Nhit, declination bands, etc.).
 * @version $Id: BinDefinitions.h 36120 2016-12-02 10:39:12Z samm $
 */

#ifndef BIN_DEFINITIONS_H_INCLUDED
#define BIN_DEFINITIONS_H_INCLUDED

#include <fstream>
#include <map>
#include <sstream>

#include <boost/algorithm/string.hpp>

#include <TCut.h>
#include <TVectorD.h>

#include <hawcnest/Logging.h>
#include <liff/BinList.h>

struct DecBin {
  double simDec_; //declination of the simulated response input
  double lowerEdge_;
  double upperEdge_;
};

typedef std::map<int, DecBin> DecBinMap;

struct AnalysisBin {
  TCut cuts_;
};

typedef std::map<BinName, AnalysisBin> AnalysisBinMap;

namespace BinDefinitions {

//function to convert a vector of dec-band centers into DecBin map
void LoadDecBands(std::vector<double> decCenters, DecBinMap &decBins);

//function to read the analysis bin definitions stored in a cutFile
void LoadBins(std::string cutFile, AnalysisBinMap &analysisBin);

//function for listing declination bins
int PrintDecBins(const DecBinMap &decBins);

//function for listing analysis bin definitions
int PrintAnalysisBins(const AnalysisBinMap &analysisBin);

}

#endif
