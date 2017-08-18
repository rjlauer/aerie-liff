/*!
 * @file BinList.h
 * @author Sam Marinelli
 * @date 15 Sep 2016
 * @brief List of analysis bins.
 * @version $Id$
 */

#ifndef LIFF_BIN_LIST_H_INCLUDED
#define LIFF_BIN_LIST_H_INCLUDED

#include <string>
#include <vector>

#include <boost/make_shared.hpp>

#include <hawcnest/CommandLineConfigurator.h>
#include <hawcnest/PointerTypedefs.h>

// Types.
typedef int         BinIndex;
typedef std::string BinName;

class BinList {
  
  public:
  
    /// Default constructor (no bins).
    inline BinList() {}
  
    /// Constructor taking min and max bin indices.
    BinList(BinIndex binStart, BinIndex binStop);
    
    /// Constructor taking list of indices.
    BinList(const std::vector<BinIndex>& index);
    
    /// Constructor taking list of names.
    inline BinList(const std::vector<BinName>& name) : name_(name) {}
    
    /// Constructor taking map-tree file.
    BinList(const std::string& mapFileName);
    
    /// Number of bins.
    inline unsigned GetNBins() const {
      return name_.size();
    }
    
    /// Get ith bin name.
    inline const BinName& operator[](const unsigned bin) const {
      return name_.at(bin);
    }
    
    /// Check whether a bin is in the list.
    bool Contains(const BinName& name) const;
    
    /// Append bin to end of list.
    inline void Append(const BinName& name) {
      name_.push_back(name);
    }
    
  protected:
  
    // Data.
    std::vector<BinName> name_;
    
};

SHARED_POINTER_TYPEDEFS(BinList)
  
/// Convert integer bin index to string.
BinName BinIndexToName(BinIndex index);

/// Pad bin name with zeros if numeric.
std::string PadBinName(const BinName& name, unsigned nDigits = 2);

/// Set up command-line arguments specifying analysis bins, omitting options
/// that require reading from the map tree.
template<typename Parser>
void AddBinOptionsNoMapTree(Parser& cl) {

  cl.template AddOption<BinIndex>(
    "binStart,f",
    0,
    "Lowest bin number in range."
  );
  cl.template AddOption<BinIndex>(
    "binStop,t",
    9,
    "Highest bin number in range."
  );
  cl.template AddMultiOption<std::vector<BinName> >(
    "bin-name",
    "Names of bins. E.g., \"--binName 0 1 2 3 6 foo bar\". Disregards binStart "
    "and binStop."
  );

}

/// Set up command-line arguments specifying analysis bins.
template<typename Parser>
void AddBinOptions(Parser& cl) {

  AddBinOptionsNoMapTree(cl);
  
  cl.AddFlag(
    "load-all-bins",
    "Load all bins between binStart and binStop that are present in the "
    "map-tree file."
  );
  cl.AddFlag(
    "load-all-bins-in-file",
    "Load all bins that are present in the map-tree file, disregarding "
    "binStart and binStop."
  );

}

/// Make BinList containing interval from binStart to binStop.
inline BinListPtr GetBinInterval(const CommandLineConfigurator& cl) {
  return boost::make_shared<BinList>(
    cl.GetArgument<BinIndex>("binStart"),
    cl.GetArgument<BinIndex>("binStop")
  );
}

/// Make BinList using whatever command-line arguments were provided. Does not
/// read from map tree.
BinListPtr ParseBinOptions(const CommandLineConfigurator& cl);

/// Make BinList using whatever command-line arguments were provided.
BinListPtr ParseBinOptions(const CommandLineConfigurator& cl,
                           const std::string& mapFileName);

/// Intersection of two lists.
BinListPtr operator&&(const BinList& first, const BinList& second);

#endif
