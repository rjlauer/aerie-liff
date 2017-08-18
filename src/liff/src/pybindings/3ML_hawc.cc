#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>

#include <liff/BinList.h>
#include <liff/LikeHAWC.h>

using namespace std;
using namespace threeML;
using namespace boost::python;


class WrongModelInterfaceVersion: public exception {
  virtual const char *what() const throw() {
    return "Mismatch between the version of ModelInterface.h contained in LiFF "
        "and the version of ModelInterface.h used "
        "to build the model LiFF has received (most likely from 3ML)";
  }
} wrongModelInterfaceVersion;

// Define function pointers and wrappers for LikeHAWC-3ML interface.
namespace {

void MatchROI_b(LikeHAWC &self, double radius) {
  self.MatchROI(radius);
}

void (LikeHAWC::*SetBackgroundNormFree_a)(bool) = &LikeHAWC::SetBackgroundNormFree;

void (LikeHAWC::*SetROI_a)(double, double, double, bool, bool) =
    (void (LikeHAWC::*)(double, double, double, bool, bool)) &LikeHAWC::SetROI;
  
//void (LikeHAWC::*SetROI_b)(std::vector<SkyPos>, bool, bool) = (void (LikeHAWC::*)(std::vector<SkyPos>, bool, bool)) &LikeHAWC::SetROI;
void (LikeHAWC::*SetROI_b)(double, double, double, double, bool, bool) =
    (void (LikeHAWC::*)(double, double, double, double, bool, bool)) &LikeHAWC::SetROI;

//SetROI list of SkyPos
void SetROI_c_wrapper(LikeHAWC &self, boost::python::list ra, boost::python::list dec, bool fixedROI, bool galactic)
 
    {
 
        vector<double> raList = vector<double>(stl_input_iterator<double>(ra),
                                            stl_input_iterator<double>());
        vector<double> decList = vector<double>(stl_input_iterator<double>(dec),
                                             stl_input_iterator<double>());
        if (raList.size() != decList.size()) {
          log_fatal("RA and Dec lists have different sizes");
        }

        vector<SkyPos> posList;

        for (unsigned int i=0; i<raList.size(); i++) {
          SkyPos pos(raList[i], decList[i]);
          posList.push_back(pos);
        }

        self.SetROI(posList, fixedROI, galactic);

}

void (LikeHAWC::*SetROI_d)(string, double, bool) =
    (void (LikeHAWC::*)(string, double, bool)) &LikeHAWC::SetROI;

void (LikeHAWC::*ResetSources_a)(threeML::ModelInterface &, double) =
    &LikeHAWC::ResetSources;

void (LikeHAWC::*AddFreeParam)(const char *, int, bool) =
    &LikeHAWC::AddFreeParameter;

// THese functions are made to avoid having to wrap a vectors of pairs (which would be slow)
boost::python::list GetPositions_wrapper(LikeHAWC &self, int esid, bool reset)
 
    {
 
        boost::python::list result;
                
        // call original function
        vector<pair<double, double> > positions = self.GetPositions(esid, reset);
                
        // put all the pairs inside the python list
        vector<pair<double, double> >::iterator it;
        
        for (it = positions.begin(); it != positions.end(); ++it){
                        
            boost::python::tuple this_pair = boost::python::make_tuple(it->first, it->second);
            
            result.append(this_pair);
                
        }
        
        return result;
    }

}

// This function can be used to make sure that the class we receive from 3ML
// has the same interface version of the one we imported
void CheckCorrectInterfaceVersion(threeML::ModelInterface &model) {

  // Instance an empty model using the ModelInterface.h contained in LiFF

  threeML::EmptyModelInterface EmptyModel = EmptyModelInterface();

  if (EmptyModel.getInterfaceVersion() != model.getInterfaceVersion()) {

    throw wrongModelInterfaceVersion;

  }

}

// This function can be used for testing purposes

void describe_model(threeML::ModelInterface &model) {

  CheckCorrectInterfaceVersion(model);

  cerr << "Number of point sources in model pointer: " << model.getNumberOfPointSources() << endl;
  cerr << "Number of extended sources in model pointer: " << model.getNumberOfExtendedSources() << endl;

}

// This is a constructor which is going to be visible only to the python side (i.e. 3ML)
// It is used so that we can check that the model we are receiving has the interface expected by
// LiFF
boost::shared_ptr<LikeHAWC> LikeHAWC_from_pointer(string mapTreeFile,
                                string DetRes,
                                ModelInterface &model,
                                int NHBinStart,
                                int NHBinEnd, bool loadAllSky) {

  CheckCorrectInterfaceVersion(model);

  return boost::shared_ptr<LikeHAWC>( new LikeHAWC(mapTreeFile, DetRes, model, BinList(NHBinStart, NHBinEnd), loadAllSky) );

}

boost::shared_ptr<LikeHAWC> LikeHAWC_from_pointer_with_transits(string mapTreeFile,
                                              double nTransits,
                                              string DetRes,
                                              ModelInterface &model,
                                              int NHBinStart,
                                              int NHBinEnd, bool loadAllSky) {

  CheckCorrectInterfaceVersion(model);

  return boost::shared_ptr<LikeHAWC>( new LikeHAWC(mapTreeFile, nTransits, DetRes, model,
                                                   BinList(NHBinStart, NHBinEnd), loadAllSky));
}

boost::shared_ptr<LikeHAWC> LikeHAWC_from_pointer_using_BinList(
  const string mapTreeFile, const string detRes, ModelInterface& model,
  const boost::python::list binList, const bool loadAllSky) {
  
  CheckCorrectInterfaceVersion(model);
  
  return boost::shared_ptr<LikeHAWC>(
    new LikeHAWC(
      mapTreeFile, detRes, model,
      BinList(
        vector<string>(stl_input_iterator<string>(binList),
                       stl_input_iterator<string>())
      ), loadAllSky
    )
  );
  
}

boost::shared_ptr<LikeHAWC> LikeHAWC_from_pointer_with_transits_using_BinList(
  const string mapTreeFile, const double nTransits, const string detRes,
  ModelInterface& model, const boost::python::list binList,
  const bool loadAllSky) {
  
  CheckCorrectInterfaceVersion(model);
  
  return boost::shared_ptr<LikeHAWC>(
    new LikeHAWC(
      mapTreeFile, nTransits, detRes, model,
      BinList(
        vector<string>(stl_input_iterator<string>(binList),
                       stl_input_iterator<string>())
      ), loadAllSky
    )
  );
  
}

//boost::shared_ptr<LikeHAWC> LikeHAWC_from_pointer_with_transits_and_GPD_switch(string mapTreeFile,
//                                              double nTransits,
//                                              string DetRes,
//                                              ModelInterface &model,
//                                              int NHBinStart,
//                                              int NHBinEnd,
//                                              const BinName& BinID,
//                                              bool loadAllSky,
//                                              bool gpdon) {
//
//  CheckCorrectInterfaceVersion(model);
//
//  return boost::shared_ptr<LikeHAWC>(
//    new LikeHAWC(mapTreeFile, nTransits, DetRes, model,
//                 BinList(NHBinStart, NHBinEnd), BinID, loadAllSky, gpdon));
//}

void
pybind_liff_LikeHAWC_3ML() {

  def("describe_model", &describe_model);

  class_<LikeHAWC>(
      "LikeHAWC",
      "Define binned likelihood for a maximum likelihood analysis.")

      // Using make_constructor we can create a constructor specific for python
      // without having to implement it in the LikeHAWC class
      .def("__init__",
           make_constructor(&LikeHAWC_from_pointer),
           "Initialize likelihood analysis object\nArgs:\n"
               "    mtfile: map tree file name for data analysis\n"
               "    detres: detector response file name\n"
               "     model: ModelInterface object for likelihood analysis\n"
               "    bstart: analysis bin start [0..9]\n"
               "     bstop: analysis bin stop [0..9]\n"
               "    allSky: if true, load full sky for likelihood analysis\n")

      .enable_pickling()

      .def("__init__",
           make_constructor(&LikeHAWC_from_pointer_with_transits),
           "Initialize likelihood analysis object\nArgs:\n"
               "    mtfile: map tree file name for data analysis\n"
               "    ntrans: number of transits in data file\n"
               "    detres: detector response file name\n"
               "     model: ModelInterface object for likelihood analysis\n"
               "    bstart: analysis bin start [0..9]\n"
               "     bstop: analysis bin stop [0..9]\n"
               "    allSky: if true, load full sky for likelihood analysis\n")
               
      .def("__init__",
           make_constructor(&LikeHAWC_from_pointer_using_BinList),
           "Initialize likelihood analysis object\nArgs:\n"
               "    mtfile: map tree file namef or data analysis\n"
               "    detres: detector response file name\n"
               "     model: ModelInterface object for likelihood analysis\n"
               "   binlist: list of bin names\n"
               "    allSky: if true, load full sky for likelihood analysis\n")

      .def("__init__",
           make_constructor(
               &LikeHAWC_from_pointer_with_transits_using_BinList
           ),
           "Initialize likelihood analysis object\nArgs:\n"
               "    mtfile: map tree file namef or data analysis\n"
               "    ntrans: number of transits in data file\n"
               "    detres: detector response file name\n"
               "     model: ModelInterface object for likelihood analysis\n"
               "   binlist: list of bin names\n"
               "    allSky: if true, load full sky for likelihood analysis\n")
  
//      .def("__init__",
//           make_constructor(&LikeHAWC_from_pointer_with_transits_and_GPD_switch),
//           "Initialize likelihood analysis object\nArgs:\n"
//               "    mtfile: map tree file name for data analysis\n"
//               "    ntrans: number of transits in data file\n"
//               "    detres: detector response file name\n"
//               "     model: ModelInterface object for likelihood analysis\n"
//               "    bstart: analysis bin start [0..9]\n"
//               "     bstop: analysis bin stop [0..9]\n"
//               "    allSky: if true, load full sky for likelihood analysis\n"
//               "     gpdon: if true, gpd ROI is used\n")

      .def("GetEnergies",
           &LikeHAWC::GetEnergies,
           args("reset"),
           "Get a vector of energy for extended sources flux caching.\n"
               "If argument is false, do not re-compute the energy vector.")

      .def("GetPositions",
           &GetPositions_wrapper,
           args("esId", "reset"),
           "Get a vector of (RA, dec) pair for extended sources flux caching.\n"
               "If argument is false, do not re-compute the position vector.")

      .def("UpdateSources",
           &LikeHAWC::UpdateSources,
           "Update existing sources to propagate changes in ModelInterface.")

      .def("getLogLike",
           &LikeHAWC::CalcLogLikelihoodUpdateSources,
           args("doIntFit"),
           "Calculate and return log-likelihood (LL).\n"
               "If argument is false, do not perform a minimization.")

      .def("calcTS",
           &LikeHAWC::CalcTestStatisticUpdateSources,
           args("doIntFit"),
           "Calculate and return test statistic (TS).\n"
               "If argument is false, do not perform a minimization.")

      .def("AddFreeParameter",
           AddFreeParam,
           args("funcName", "parId", "detResFree"),
           "Free the i^th parameter of a model in fits.")

      .def("MatchROI", MatchROI_b,
           args("padding"),
           "Set region of interest (ROI) to src boundaries + padding [deg].")

      .def("SetBackgroundNormFree",
           SetBackgroundNormFree_a,
           args("bfree"),
           "Switch the common norm internal likelihood fit on/off.")

      .def("CommonNorm",
           &LikeHAWC::CommonNorm,
           "Return common norm factor multiplied to the flux of all sources.",
           return_value_policy<return_by_value>())

      .def("SetCommonNorm",
           &LikeHAWC::SetCommonNorm,
           args("norm"),
           "Set common norm factor for all sources.")

      .def("WriteModelMap",
           &LikeHAWC::WriteModelMap,
           args("fileName", "poisson"),
           "Write model map to a map tree.")

      .def("WriteResidualMap",
           &LikeHAWC::WriteResidualMap,
           args("fileName"),
           "Write residual map to a map tree.")

      .def("SetROI",
           SetROI_a,
           args("RA", "Dec", "radius", "fixedROI", "galactic"),
           "Set circular region of interest around point RA,Dec.")
  
      .def("SetROI",
           SetROI_b,
           args("rastart", "rastop", "decstart", "decstop", "fixedROI", "galactic"),
           "Set strip ROI with 4 points in either celestial/galactic coordinates")
  
      .def("SetROI",
           SetROI_c_wrapper,
           args("RA", "Dec", "fixedROI", "galactic"),
           "Set polygon ROI from lists of RA and Dec or GL and GB.")
  
      .def("SetROI",
           SetROI_d,
           args("maskFile", "threshold", "fixedROI"),
           "Set ROI from a healpix file in celestial coordinates. Pixels that are equal or greater than the threshold will be used.")

      .def("ResetSources",
           ResetSources_a,
           "Define source list from ModelInterface pointer.")

      .def("GetTopHatExpectedExcesses",
           &LikeHAWC::GetTopHatExpectedExcesses,
           args("RA", "Dec", "countradius"),
           "Get the excesses expected from the current model using the TopHat")

      .def("GetTopHatExcesses",
           &LikeHAWC::GetTopHatExcesses,
           args("RA", "Dec", "countradius"),
           "Get the excesses measured using the TopHat")

      .def("GetTopHatBackgrounds",
           &LikeHAWC::GetTopHatBackgrounds,
           args("RA", "Dec", "countradius"),
           "Get the background measured using the TopHat")

      .def("GetTopHatAreas",
           &LikeHAWC::GetTopHatAreas,
           args("RA", "Dec", "countradius"),
           "Get the area of the TopHat")
      
      .def("UpdateSources",
           &LikeHAWC::UpdateSources,
           "Update sources inside LikeHAWC")
       
       .def("CalcBackgroundLogLikelihood",
           &LikeHAWC::CalcBackgroundLogLikelihood,
            args("doIntFit"),
            "Calculate background likelihood");
}

