/*!
 * @file BinList.cc
 * @author Sam Marinelli
 * @date 15 Sep 2016
 * @brief List of analysis bins.
 * @version $Id$
 */

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <hawcnest/CommandLineConfigurator.h>
#include <hawcnest/Logging.h>
#include <liff/BinList.h>

#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

#include <TFile.h>
#include <TObjArray.h>
#include <TTree.h>

using namespace std;

using namespace boost;

BinList::BinList(const BinIndex binStart, const BinIndex binStop) {
  
  if (binStart > binStop)
    return;

  name_.reserve(binStop - binStart + 1);
  for (BinIndex i = binStart; i <= binStop; ++i)
    name_.push_back(BinIndexToName(i));

}

BinList::BinList(const vector<BinIndex>& index) : name_(index.size()) {

  for (unsigned i = 0, nBins = index.size(); i < nBins; ++i)
    name_[i] = BinIndexToName(index[i]);

}

BinList::BinList(const string& mapFileName) {

  TFile mapFile(mapFileName.c_str());
  if (!mapFile.IsOpen())
    log_fatal("Could not open map-tree file " << mapFileName << ".")
  
  TTree* const binInfo = (TTree*)mapFile.Get("BinInfo");
  if (!binInfo)
    log_fatal("Could not find tree BinInfo in map-tree file " << mapFileName
              << ".")

  const TObjArray* const branch = binInfo->GetListOfBranches();
  const bool hasName            = branch->FindObject("name");

  BinName name;
  const BinName* namePtr = &name;
  BinIndex id;
  if (hasName)
    binInfo->SetBranchAddress("name", &namePtr);
  else if (branch->FindObject("id"))
    binInfo->SetBranchAddress("id", &id);
  else
    log_fatal("BinInfo tree in map-tree file " << mapFileName << " does not "
              "have name or ID branch.")

  const unsigned nBins = binInfo->GetEntries();
  name_.reserve(nBins);

  for (unsigned i = 0; i < nBins; ++i) {
    binInfo->GetEntry(i);
    name_.push_back(hasName ? name : BinIndexToName(id));
  }

}

bool BinList::Contains(const BinName& name) const {

  for (unsigned i = 0, nBins = name_.size(); i < nBins; ++i)
    if (name_[i] == name)
      return true;
  
  return false;

}

BinName BinIndexToName(const BinIndex index) {

  ostringstream name;
  name << index;
  return name.str();

}

string PadBinName(const BinName& name, const unsigned nDigits) {

  ostringstream stream;

  try {
    stream << setfill('0') << setw(nDigits) << lexical_cast<BinIndex>(name);
  } catch (const bad_lexical_cast&) {
    return name;
  }

  return stream.str();

}

BinListPtr ParseBinOptions(const CommandLineConfigurator& cl) {

  if (cl.HasFlag("bin-name"))
    return boost::make_shared<BinList>(
      cl.GetArgument<vector<BinName> >("bin-name")
    );
  
  return GetBinInterval(cl);

}

BinListPtr ParseBinOptions(const CommandLineConfigurator& cl,
                           const string& mapFileName) {

  const BinListPtr available = boost::make_shared<BinList>(mapFileName);
  
  if (cl.HasFlag("load-all-bins"))
    return *available && *GetBinInterval(cl);

  if (cl.HasFlag("load-all-bins-in-file"))
    return available;

  return ParseBinOptions(cl);

}

BinListPtr operator&&(const BinList& first, const BinList& second) {

  const BinListPtr ret = boost::make_shared<BinList>();
  
  for (unsigned i = 0, nBins = first.GetNBins(); i < nBins; ++i)
    if (second.Contains(first[i]))
      ret->Append(first[i]);

  return ret;

}
