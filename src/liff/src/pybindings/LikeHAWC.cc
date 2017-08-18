#include <boost/python.hpp>

#include <liff/LikeHAWC.h>
#include <liff/ModelInterface.h>

using namespace std;
using namespace boost::python;

// Define function pointers and wrappers for LikeHAWC-3ML interface.
namespace {

double (LikeHAWC::*CalcLogLikelihood_a)(bool) =
    &LikeHAWC::CalcLogLikelihoodUpdateSources;

vector<SkyPos> (LikeHAWC::*MatchROI_a)(double) =
    &LikeHAWC::MatchROI;

void
MatchROI_b(LikeHAWC &self, double radius) {
  self.MatchROI(radius);
}

void (LikeHAWC::*SetBackgroundNormFree_a)(bool) =
    &LikeHAWC::SetBackgroundNormFree;

void (LikeHAWC::*SetROI_a)(float ra, float dec, float radius, bool fixedROI) =
    &LikeHAWC::SetROI;

//void (LikeHAWC::*SetROI_b)(std::vector<SkyPos> ROI, bool GPD, bool fixedROI) =
//    &LikeHAWC::SetROI;

void (LikeHAWC::*SetROI_b)(double rastart, double rastop, double decstart, double decstop, bool fixedROI, bool galactic) =
    &LikeHAWC::SetROI;

void (LikeHAWC::*ResetSources_a)(const ModelInterface *, double) =
    &LikeHAWC::ResetSources;

void (LikeHAWC::*AddFreeParam)(const char *, int, bool) =
    &LikeHAWC::AddFreeParameter;
}

void
pybind_liff_LikeHAWC() {
  class_<LikeHAWC>(
      "LikeHAWC",
      "Define binned likelihood for a maximum likelihood analysis.")

      .enable_pickling()

      .def(init<string, string, const threeML::ModelInterface *,
                int, int, bool>(
          args("mtfile", "detres", "model",
               "bstart", "bstop", "allSky"),
          "Initialize likelihood analysis object\nArgs:\n"
              "    mtfile: map tree file name for data analysis\n"
              "    detres: detector response file name\n"
              "     model: ModelInterface object for likelihood analysis\n"
              "    bstart: analysis bin start [0..9]\n"
              "     bstop: analysis bin stop [0..9]\n"
              "    allSky: if true, load full sky for likelihood analysis\n"))

      .def(init<string, double, string, const threeML::ModelInterface *,
                int, int, bool>(
          args("mtfile", "ntrans", "detres", "model",
               "bstart", "bstop", "allSky"),
          "Initialize likelihood analysis object\nArgs:\n"
              "    mtfile: map tree file name for data analysis\n"
              "    ntrans: number of transits in data file\n"
              "    detres: detector response file name\n"
              "     model: ModelInterface object for likelihood analysis\n"
              "    bstart: analysis bin start [0..9]\n"
              "     bstop: analysis bin stop [0..9]\n"
              "    allSky: if true, load full sky for likelihood analysis\n"))
      
      .def(init<string, string, const threeML::ModelInterface*,
                boost::python::list, bool>(
          args("mtfile", "detres", "model", "binlist", "allSky"),
          "Initialize likelihood analysis object\nArgs:\n"
              "    mtfile: map tree file name for data analysis\n"
              "    detres: detector response file name\n"
              "     model: ModelInterface object for likelihood analysis\n"
              "   binlist: list of bin names\n"
              "    allSky: if true, load full sky for likelihood analysis\n"))

      .def(init<string, double, string, const threeML::ModelInterface*,
                boost::python::list, bool>(
          args("mtfile", "ntrans", "detres", "model", "binlist", "allSky"),
          "Initialize likelihood analysis object\nArgs:\n"
              "    mtfile: map tree file name for data analysis\n"
              "    ntrans: number of transits in data file\n"
              "    detres: detector response file name\n"
              "     model: ModelInterface object for likelihood analysis\n"
              "   binlist: list of bin names\n"
              "    allSky: if true, load full sky for likelihood analysis\n"))
  
      .def(init<string, double, string, const threeML::ModelInterface *,
                int, int, bool, bool>(
          args("mtfile", "ntrans", "detres", "model",
               "bstart", "bstop", "allSky", "gpdon"),
          "Initialize likelihood analysis object\nArgs:\n"
              "    mtfile: map tree file name for data analysis\n"
              "    ntrans: number of transits in data file\n"
              "    detres: detector response file name\n"
              "     model: ModelInterface object for likelihood analysis\n"
              "    bstart: analysis bin start [0..9]\n"
              "     bstop: analysis bin stop [0..9]\n"
              "    allSky: if true, load full sky for likelihood analysis\n"
              "     gpdon: if true, gpd ROI is used\n"))

      .def("GetEnergies",
           &LikeHAWC::GetEnergies,
           args("reset"),
           "Get a vector of energy for extended sources flux caching.\n"
               "If argument is false, do not re-compute the energy vector.")

      .def("GetPositions",
           &LikeHAWC::GetPositions,
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
           args("RA", "Dec", "radius", "fixedROI"),
           "Set circular region of interest around point RA,Dec.")
  
//      .def("SetROI",
//           SetROI_b,
//           args("ROI", "GPD", "fixedROI"),
//           "Set any ROI desired using vector/switch")
      .def("SetROI",
           SetROI_b,
           args("rastart", "rastop", "decstart", "decstop", "fixedROI", "galactic"),
           "Set strip ROI with 4 points in either celestial/galactic coordinates")

      .def("ResetSources",
           ResetSources_a,
           "Define source list from ModelInterface pointer.");
}

