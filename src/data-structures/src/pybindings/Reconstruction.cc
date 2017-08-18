/*!
 * @file Reconstruction.cc 
 * @brief Python bindings to the reconstruction results.
 * @author Segev BenZvi 
 * @date 31 Dec 2011 
 * @version $Id: Reconstruction.cc 37959 2017-03-02 01:30:31Z zhampel $
 */

#include <boost/python.hpp>

#include <data-structures/reconstruction/Reco.h>
#include <data-structures/reconstruction/RecoResult.h>

#include <data-structures/reconstruction/core-fitter/CoreFitResult.h>
#include <data-structures/reconstruction/core-fitter/SFCoreFitResult.h>
#include <data-structures/reconstruction/core-fitter/GaussCoreFitResult.h>
#include <data-structures/reconstruction/core-fitter/NKGCoreFitResult.h>

#include <data-structures/reconstruction/gamma-filter/CompactnessResult.h>

#include <data-structures/reconstruction/track-fitter/PropagationPlane.h>
#include <data-structures/reconstruction/track-fitter/AngleFitResult.h>
#include <data-structures/reconstruction/track-fitter/GaussPlaneFitResult.h>

#include <data-structures/reconstruction/energy-estimator/EnergyResult.h>
#include <data-structures/reconstruction/energy-estimator/SimpleEnergyResult.h>

#include <hawcnest/impl/pybindings.h>

using namespace boost::python;

/// Expose the RecoResult base class and status enums to python
void
pybind_reco_RecoResult()
{
  scope in_RecoResult =
  class_<RecoResult, bases<Baggable>, boost::shared_ptr<RecoResult> >
    ("RecoResult",
     "Interface for storing algorithm status for reconstruction modules.")
    .add_property("nfit", &RecoResult::GetNFit, &RecoResult::SetNFit,
                  "Number of PMTs used in fit.")
    .add_property("status", &RecoResult::GetStatus, &RecoResult::SetStatus,
                  "Status of the reconstruction result.")
    ;

  register_pointer_conversions<RecoResult>();

  enum_<RecoStatus>
    ("RecoStatus",
     "Common status values returned by reconstruction modules.")
    .value("RECO_SUCCESS", RECO_SUCCESS)
    .value("RECO_FAIL", RECO_FAIL)
    .value("RECO_UNKNOWN", RECO_UNKNOWN)
    .value("RECO_NO_CONVERGENCE", RECO_NO_CONVERGENCE)
    .export_values()
    ;

  enum_<RecoAction>
    ("RecoAction",
     "Action of reconstruction module on shower summary.")
    .value("RECO_ACTIVE", RECO_ACTIVE)
    .value("RECO_INACTIVE", RECO_INACTIVE)
    .value("RECO_ACTIVE_IF_SUCCESS", RECO_ACTIVE_IF_SUCCESS)
    .export_values()
    ;
}

/// Expose the CoreFitResult class to python
void
pybind_reco_CoreFitResult()
{
  class_<CoreFitResult, bases<RecoResult>, boost::shared_ptr<CoreFitResult> >
    ("CoreFitResult",
     "Storage for the output of core fitters.")
    .add_property("position",
                  make_function(&CoreFitResult::GetPosition,
                                return_value_policy<copy_const_reference>()),
                  &CoreFitResult::SetPosition,
                  "Point position of the core.")
    .add_property("x",
                  &CoreFitResult::GetX,
                  "X-coordinate of the core.")
    .add_property("y",
                  &CoreFitResult::GetY,
                  "Y-coordinate of the core.")
    .add_property("z",
                  &CoreFitResult::GetZ,
                  "Z-coordinate of the core.")
    .add_property("xErr",
                  &CoreFitResult::GetXUncertainty,
                  &CoreFitResult::SetXUncertainty,
                  "One-sigma uncertainty in the x-coordinate of the core.")
    .add_property("yErr",
                  &CoreFitResult::GetYUncertainty,
                  &CoreFitResult::SetYUncertainty,
                  "One-sigma uncertainty in the y-coordinate of the core.")
    .add_property("zErr",
                  &CoreFitResult::GetZUncertainty,
                  &CoreFitResult::SetZUncertainty,
                  "One-sigma uncertainty in the z-coordinate of the core.")
    .add_property("xyErr",
                  &CoreFitResult::GetXYUncertainty,
                  "One-sigma uncertainty of core position in xy plane.")
    .add_property("amplitude",
                  &CoreFitResult::GetAmplitude,
                  &CoreFitResult::SetAmplitude,
                  "Reconstructed amplitude of the shower.")
    .add_property("amplitudeErr",
                  &CoreFitResult::GetAmplitudeUncertainty,
                  &CoreFitResult::SetAmplitudeUncertainty,
                  "One-sigma uncertainty in the reconstructed amplitude.")
    .add_property("sigma",
                  &CoreFitResult::GetSigma,
                  &CoreFitResult::SetSigma,
                  "Reconstructed width of the core.")
    .add_property("sigmaErr",
                  &CoreFitResult::GetSigmaUncertainty,
                  &CoreFitResult::SetSigmaUncertainty,
                  "One-sigma uncertainty in the core width.")
    .add_property("uncertainties_calculated",
                  &CoreFitResult::UncertaintiesCalculated,
                  "Indicates if uncertainties have been calculated.")
    .add_property("chi2",
                  &CoreFitResult::GetChiSq, &CoreFitResult::SetChiSq,
                  "Chi-square figure of merit for the fit.")
    .add_property("ndof",
                  &CoreFitResult::GetNdof, &CoreFitResult::SetNdof,
                  "Number of degrees of freedom of the fit.")
    ;

  register_pointer_conversions<CoreFitResult>();
}

/// Expose the CoreFitResult class to python
void
pybind_reco_SFCoreFitResult()
{
  class_<SFCoreFitResult, bases<CoreFitResult>,
         boost::shared_ptr<SFCoreFitResult> >
    ("SFCoreFitResult",
     "Fit result from the SF core fitter.")
    ;

  register_pointer_conversions<SFCoreFitResult>();
}

void
pybind_reco_GaussCoreFitResult()
{
  class_<GaussCoreFitResult, bases<CoreFitResult>,
         boost::shared_ptr<GaussCoreFitResult> >
    ("GaussCoreFitResult",
     "Fit result from the Gaussian core fitter.")
    ;

  register_pointer_conversions<GaussCoreFitResult>();
}

void
pybind_reco_NKGCoreFitResult()
{
  class_<NKGCoreFitResult, bases<CoreFitResult>,
         boost::shared_ptr<NKGCoreFitResult> >
    ("NKGCoreFitResult",
     "Fit result from the NKG core fitter.")
    ;

  register_pointer_conversions<NKGCoreFitResult>();
}

/// Expose the CompactnessResult class to python
void
pybind_reco_CompactnessResult()
{
  class_<CompactnessResult, bases<RecoResult>,
         boost::shared_ptr<CompactnessResult> >
    ("CompactnessResult",
     "Compactness of a shower, given a particular search radius.")
    .add_property("radius",
                  &CompactnessResult::GetRadius,
                  &CompactnessResult::SetRadius,
                  "Radius of the compactness calculation.")
    .add_property("maxPE",
                  &CompactnessResult::GetMaxPE,
                  &CompactnessResult::SetMaxPE,
                  "Maximum PE cluster outside the search radius.")
    .add_property("compactness",
                  &CompactnessResult::GetCompactness,
                  &CompactnessResult::SetCompactness,
                  "The compactness of the shower.")
    .add_property("maxPEChannelId",
                  &CompactnessResult::GetMaxPEChannelID,
                  "Channel ID of the maximum PE hit.")
    .add_property("nChExcluded",
                  &CompactnessResult::GetNchExcluded,
                  "Number of channels excluded after radial cut.")
    ;

  register_pointer_conversions<CompactnessResult>();
}

// Pointers to member functions of PropagationPlane
namespace {

  void (PropagationPlane::*sa1)(const Vector&) =
      &PropagationPlane::SetAxis;

  void (PropagationPlane::*tr1)(const Vector&) =
      &PropagationPlane::Translate;
  void (PropagationPlane::*tr2)(const double, const double, const double) =
      &PropagationPlane::Translate;

  double (PropagationPlane::*at1)(const Point&) const =
      &PropagationPlane::ArrivalTime;
  double (PropagationPlane::*at2)(const double, const double, const double) const=
      &PropagationPlane::ArrivalTime;
}

/// Expose the PropagationPlane class to python
void
pybind_reco_PropagationPlane()
{
  class_<PropagationPlane, boost::shared_ptr<PropagationPlane> >
    ("PropagationPlane",
     "Description of a plane propagating at the speed of light.")
    .add_property("axis",
                  make_function(&PropagationPlane::GetAxis,
                                return_value_policy<copy_const_reference>()),
                  sa1,
                  "The axis of the propagating plane.")
    .add_property("reference_point",
                  make_function(&PropagationPlane::GetReferencePoint,
                                return_value_policy<copy_const_reference>()),
                  "Reference point for the plane calculations (read-only).")
    .add_property("reference_time",
                  &PropagationPlane::GetReferenceTime,
                  &PropagationPlane::SetReferenceTime,
                  "The reference time t0 of the plane (can be arbitrary).")
    .def("angle", &PropagationPlane::Angle,
         "Calculate the angle between two propagation planes.")
    .def("translate", tr1,
         "Translate the plane by a Vector distance.")
    .def("translate", tr2,
         "Translate the plane by a vector given by x, y, and z distances.")
    .def("arrival_time", at1,
         "Calculate plane arrival at a Point w.r.t. the start of the event.")
    .def("arrival_time", at2,
         "Calculate plane arrival at a Point w.r.t. the start of the event.")
    ;
}

/// Expose the AngleFitResult class to python
void
pybind_reco_AngleFitResult()
{
  class_<AngleFitResult, bases<RecoResult>, boost::shared_ptr<AngleFitResult> >
    ("AngleFitResult",
     "A reconstructed shower axis and its uncertainties.")
    .add_property("propagation_plane",
                  make_function(&AngleFitResult::GetPropagationPlane,
                                return_value_policy<copy_const_reference>()),
                  &AngleFitResult::SetPropagationPlane,
                  "Propagation plane of the shower.")
    .add_property("reference_time",
                  &AngleFitResult::GetReferenceTime,
                  "Time shower plane crosses detector at the core position.")
    .add_property("axis",
                  make_function(&AngleFitResult::GetAxis,
                                return_value_policy<copy_const_reference>()),
                  "Reconstructed shower axis.")
    .add_property("uncertainties_calculated",
                  &AngleFitResult::UncertaintiesCalculated,
                  "Indicates if uncertainties have been calculated.")
    .add_property("t0Err",
                  &AngleFitResult::GetTimeUncertainty,
                  &AngleFitResult::SetTimeUncertainty,
                  "One-sigma Gaussian uncertainty in t0.")
    .add_property("angleErr",
                  &AngleFitResult::GetAngleUncertainty,
                  &AngleFitResult::SetAngleUncertainty,
                  "One-sigma polar Gaussian uncertainty in angle.")
    .add_property("chi2",
                  &AngleFitResult::GetChiSq,
                  &AngleFitResult::SetChiSq,
                  "Figure of merit, expressed as chi2 = -2 log(likelihood).")
    .add_property("ndof",
                  &AngleFitResult::GetNdof,
                  &AngleFitResult::SetNdof,
                  "Number of degrees of freedom in the fit.")
    ;

  register_pointer_conversions<AngleFitResult>();
}

/// Expose the GaussPlaneFitResult class to python
void
pybind_reco_GaussPlaneFitResult()
{
  class_<GaussPlaneFitResult, bases<AngleFitResult>,
         boost::shared_ptr<GaussPlaneFitResult> >
    ("GaussPlaneFitResult",
     "Result of Gaussian plane shower front fit.")
    ;

  register_pointer_conversions<GaussPlaneFitResult>();
}

/// Expose the EnergyResult class to python
void
pybind_reco_EnergyResult()
{
  class_<EnergyResult, bases<RecoResult>,
         boost::shared_ptr<EnergyResult> >
    ("EnergyResult",
     "Base class with common properties for any energy estimator .")
    .add_property("E_primary",
                  &EnergyResult::GetPrimaryEnergy,
                  &EnergyResult::SetPrimaryEnergy,
                  "Energy of the primary particle.")
    .add_property("E_ground",
                  &EnergyResult::GetGroundEnergy,
                  &EnergyResult::SetGroundEnergy,
                  "Energy of the shower at ground level.")
    ;

  register_pointer_conversions<EnergyResult>();
}

/// Expose the SimpleEnergyResult class to python
void
pybind_reco_SimpleEnergyResult()
{
  class_<SimpleEnergyResult, bases<EnergyResult>,
         boost::shared_ptr<SimpleEnergyResult> >
    ("SimpleEnergyResult",
     "A simple energy estimate for a shower reaching ground level.")
    .add_property("npe",
                  &SimpleEnergyResult::GetNPEs,
                  &SimpleEnergyResult::SetNPEs,
                  "Number of PEs.")
    ;

  register_pointer_conversions<SimpleEnergyResult>();
}

