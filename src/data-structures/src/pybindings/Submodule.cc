/*!
 * @file Submodule.cc 
 * @brief Submodule for linking bindings in data-structures to python.
 * @author Segev BenZvi 
 * @date 17 Dec 2011 
 * @version $Id: Submodule.cc 37960 2017-03-02 02:05:17Z zhampel $
 */

#include <boost/python.hpp>

#include <hawcnest/LoadProject.h>

using namespace boost::python;

void pybind_geometry_R3Vector();
void pybind_geometry_R3Transform();
void pybind_geometry_Rotate();
void pybind_geometry_Scale();
void pybind_geometry_Translate();
void pybind_geometry_Vector();
void pybind_geometry_AxialVector();
void pybind_geometry_Point();
void pybind_geometry_S2Point();
void pybind_geometry_UTMPoint();
void pybind_geometry_LatLonAlt();
void pybind_geometry_LatDistCalc();

void pybind_astronomy_EquPoint();
void pybind_astronomy_GalPoint();
void pybind_astronomy_EclPoint();
void pybind_astronomy_HorPoint();
void pybind_astronomy_AstroCoords();

void pybind_detector_PMT();
void pybind_detector_Channel();
void pybind_detector_Tank();
void pybind_detector_Detector();

void pybind_evt_HitFlags();
void pybind_evt_HitTrigData();
void pybind_evt_Edge();
void pybind_evt_HitCalData();
void pybind_evt_Hit();
void pybind_evt_ChannelEvent();
void pybind_evt_TankEvent();
void pybind_evt_EventFlags();
void pybind_evt_Event();
void pybind_evt_EventList();

void pybind_math_PowerLaw();
void pybind_math_BrokenPowerLaw();
void pybind_math_CutoffPowerLaw();
void pybind_math_DoubleBrokenPowerLaw();
void pybind_math_LogParabola();
void pybind_math_DigitalLogicEdge();
void pybind_math_DigitalLogicPulse();
void pybind_math_SplineTable();
void pybind_math_PeriodicFunction();
void pybind_math_PnPoly();
void pybind_math_ConvexHull();

void pybind_scaler_ScalerChannel();
void pybind_scaler_ScalerEvent();

void pybind_simevt_Prescale();
void pybind_simevt_SimulationParameters();
void pybind_simevt_SimParticle();
void pybind_simevt_PEHit();
void pybind_simevt_SignalTrace();
void pybind_simevt_SimChannel();
void pybind_simevt_SimTank();
void pybind_simevt_SimEventHeader();
void pybind_simevt_SimEvent();
void pybind_simevt_G4SimHeader();

void pybind_reco_RecoResult();
void pybind_reco_CoreFitResult();
void pybind_reco_SFCoreFitResult();
void pybind_reco_GaussCoreFitResult();
void pybind_reco_NKGCoreFitResult();
void pybind_reco_CompactnessResult();
void pybind_reco_PropagationPlane();
void pybind_reco_AngleFitResult();
void pybind_reco_GaussPlaneFitResult();
void pybind_reco_EnergyResult();
void pybind_reco_SimpleEnergyResult();

void pybind_physics_PhysicsConstants();
void pybind_physics_ParticleType();

void pybind_time_TimeInterval();
void pybind_time_TimeStamp();
void pybind_time_TimeRange();
void pybind_time_UTCDate();
void pybind_time_UTCDateTime();
void pybind_time_ModifiedJulianDate();

BOOST_PYTHON_MODULE(data_structures)
{
  // Shower user-defined and python docstrings; suppress C++ signatures
  docstring_options docOpts;
  docOpts.enable_all();
  docOpts.disable_cpp_signatures();

  load_project("data-structures", false);

  pybind_geometry_R3Vector();
  pybind_geometry_R3Transform();
  pybind_geometry_Rotate();
  pybind_geometry_Scale();
  pybind_geometry_Translate();
  pybind_geometry_Vector();
  pybind_geometry_AxialVector();
  pybind_geometry_Point();
  pybind_geometry_S2Point();
  pybind_geometry_UTMPoint();
  pybind_geometry_LatLonAlt();
  pybind_geometry_LatDistCalc();

  pybind_astronomy_EquPoint();
  pybind_astronomy_GalPoint();
  pybind_astronomy_EclPoint();
  pybind_astronomy_HorPoint();
  pybind_astronomy_AstroCoords();

  pybind_detector_PMT();
  pybind_detector_Channel();
  pybind_detector_Tank();
  pybind_detector_Detector();

  pybind_evt_HitFlags();
  pybind_evt_HitTrigData();
  pybind_evt_Edge();
  pybind_evt_HitCalData();
  pybind_evt_Hit();
  pybind_evt_ChannelEvent();
  pybind_evt_TankEvent();
  pybind_evt_EventFlags();
  pybind_evt_Event();
  pybind_evt_EventList();

  pybind_math_PowerLaw();
  pybind_math_BrokenPowerLaw();
  pybind_math_CutoffPowerLaw();
  pybind_math_DoubleBrokenPowerLaw();
  pybind_math_LogParabola();
  pybind_math_DigitalLogicEdge();
  pybind_math_DigitalLogicPulse();
  pybind_math_SplineTable();
  pybind_math_PeriodicFunction();
  pybind_math_PnPoly();
  pybind_math_ConvexHull();

  pybind_scaler_ScalerChannel();
  pybind_scaler_ScalerEvent();

  pybind_simevt_Prescale();
  pybind_simevt_SimulationParameters();
  pybind_simevt_SimParticle();
  pybind_simevt_PEHit();
  pybind_simevt_SignalTrace();
  pybind_simevt_SimChannel();
  pybind_simevt_SimTank();
  pybind_simevt_SimEventHeader();
  pybind_simevt_SimEvent();
  pybind_simevt_G4SimHeader();

  pybind_reco_RecoResult();
  pybind_reco_CoreFitResult();
  pybind_reco_SFCoreFitResult();
  pybind_reco_GaussCoreFitResult();
  pybind_reco_NKGCoreFitResult();
  pybind_reco_CompactnessResult();
  pybind_reco_PropagationPlane();
  pybind_reco_AngleFitResult();
  pybind_reco_GaussPlaneFitResult();
  pybind_reco_EnergyResult();
  pybind_reco_SimpleEnergyResult();

  pybind_physics_PhysicsConstants();
  pybind_physics_ParticleType();

  pybind_time_TimeInterval();
  pybind_time_TimeStamp();
  pybind_time_TimeRange();
  pybind_time_UTCDate();
  pybind_time_UTCDateTime();
  pybind_time_ModifiedJulianDate();
}

