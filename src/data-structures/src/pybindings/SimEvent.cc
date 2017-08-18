/*!
 * @file SimEvent.cc 
 * @brief Python bindings to simulated event structures.
 * @author Segev BenZvi 
 * @date 21 Dec 2011 
 * @version $Id: SimEvent.cc 26723 2015-08-19 18:13:28Z sybenzvi $
 */

#include <boost/python.hpp>

#include <data-structures/event/Prescale.h>
#include <data-structures/event/SimulationParameters.h>
#include <data-structures/event/SimParticle.h>
#include <data-structures/event/SimChannel.h>
#include <data-structures/event/SimTank.h>
#include <data-structures/event/SimEventHeader.h>
#include <data-structures/event/SimEvent.h>
#include <data-structures/event/G4SimHeader.h>

#include <hawcnest/impl/pybindings.h>

using namespace boost::python;
using namespace evt;

typedef return_value_policy<copy_const_reference> RVPCCR;

// -----------------------------------------------------------------------------
/// Expose Prescale to boost::python
void
pybind_simevt_Prescale()
{
  class_<Prescale, bases<Baggable>, boost::shared_ptr<Prescale> >
    ("Prescale",
     "Store spectrum and throwing geometry prescale for simulated events.")
    .def(init<const int&>())
    .def(init<const int&, const int&>())
    .add_property("max_window_hits",
                  &Prescale::GetMaxWindowHits, &Prescale::SetMaxWindowHits)

    // Conversion to integer
    .def(self_ns::int_(self))
    ;

  register_pointer_conversions<Prescale>();
}

// -----------------------------------------------------------------------------
/// Expose SimulationParameters to boost::python
void
pybind_simevt_SimulationParameters()
{
  class_<SimulationParameters, bases<Baggable>, boost::shared_ptr<SimulationParameters> >
    ("SimulationParameters",
     "Detector parameters used to generate events during simulation.")
    .add_property("jitter",
                  &SimulationParameters::GetJitter,
                  &SimulationParameters::SetJitter,
                  "PMT jitter [ns].")
    .add_property("tnoise",
                   &SimulationParameters::GetTNoise,
                   &SimulationParameters::SetTNoise,
                  "Timing noise on each simulated hit [ns].")
    .add_property("qerr",
                  &SimulationParameters::GetQErr,
                  &SimulationParameters::SetQErr,
                  "Fractional charge error.")
    .add_property("min_hits",
                  &SimulationParameters::GetMinHits,
                  &SimulationParameters::SetMinHits,
                  "Minimum number of hits needed to reconstruct event.")
    .add_property("prescale",
                  &SimulationParameters::GetPrescale,
                  &SimulationParameters::SetPrescale,
                  "Prescale factor of cut events.")
    .add_property("noise",
                  &SimulationParameters::GetNoise,
                  &SimulationParameters::SetNoise,
                  "Noise rate [Hz].")
    ;

  register_pointer_conversions<SimulationParameters>();
}

// -----------------------------------------------------------------------------
/// Expose SimParticle to boost::python
void
pybind_simevt_SimParticle()
{
  class_<SimParticle, bases<Baggable>, boost::shared_ptr<SimParticle> >
    ("SimParticle",
     "Energy and geometry properties of simulated ground-level particles.")
    .add_property("particle_id", &SimParticle::particleId_,
                  "Particle type using ParticleType CORSIKA codes.")
    .add_property("energy", &SimParticle::energy_,
                  "Particle energy.")
    .add_property("theta", &SimParticle::theta_,
                  "Particle local zenith angle (in Detector coordinates).")
    .add_property("phi", &SimParticle::phi_,
                  "Particle local azimuth angle (in Detector coordinates).")
    .add_property("x", &SimParticle::x_,
                  "Particle ground position (in detector coordinates).")
    .add_property("y", &SimParticle::y_,
                  "Particle ground position (in detector coordinates).")
    ;

  register_pointer_conversions<SimParticle>();
}

// -----------------------------------------------------------------------------
/// Expose PEHit to boost::python
void
pybind_simevt_PEHit()
{
  class_<PEHit, bases<Baggable>, boost::shared_ptr<PEHit> >
    ("PEHit",
     "Properties of a simulated photoelectron hit.")
    .add_property("amplitude", &PEHit::pulseAmplitude_,
                  "Photoelectron pulse amplitude.")
    .add_property("time", &PEHit::hitTime_,
                  "True PE hit time.")
    .add_property("cathode_position", &PEHit::cathodeRadius_,
                  "Detection position on photocathode (distance from center).")
    .def(self < self)
    ;

  register_pointer_conversions<PEHit>();
}

// -----------------------------------------------------------------------------
typedef SignalTrace::iterator (SignalTrace::*BinIterator)();

double
trace_get(const SignalTrace* t, const int i)
{
  return (*t)[i];
}

void
trace_set(SignalTrace& t, const int i, const double x)
{
  t[i] = x;
}

/// Expose SignalTrace to boost::python
void
pybind_simevt_SignalTrace()
{
  class_<SignalTrace, bases<Baggable>, boost::shared_ptr<SignalTrace> >
    ("SignalTrace",
     "An evenly-binned signal trace for storing data.")

    .add_property("nbins", &SignalTrace::GetSize,
                  "Number of bins in the trace.")
    .add_property("dt", &SignalTrace::GetDt, &SignalTrace::SetDt,
                  "Time binning of the signal trace.")
    .add_property("t0", &SignalTrace::GetT0, &SignalTrace::SetT0,
                  "Start time of the trace.")
    .add_property("t1", &SignalTrace::GetT1,
                  "End time of the trace.")

    .def("get_time", &SignalTrace::GetT,
         "Get the time corresponding to bin i.")

    .add_property("max", &SignalTrace::Maximum,
                  "Maximum value in the trace.")
    .add_property("maxBin", &SignalTrace::MaximumBin,
                  "Bin location of maximum value in the trace.")
    .add_property("min", &SignalTrace::Minimum,
                  "Minimum value in the trace.")
    .add_property("minBin", &SignalTrace::MinimumBin,
                  "Bin location of minimum value in the trace.")
    
    .add_property("integral", &SignalTrace::Integral,
                  "Bin-wise Riemann sum of the trace.")

    .def("__getitem__", &trace_get,
         "Retrieve a trace value by bin index.")
    .def("__setitem__", &trace_set,
         "Set a trace value by accessing its bin index.")

    .def("__len__", &SignalTrace::GetSize,
         "Number of bins in the trace.")
//    .def("__max__", &SignalTrace::Maximum,
//         "Maximum value in the trace.")
//    .def("__min__", &SignalTrace::Minimum,
//         "Minimum value in the trace.")
//    .def("__iter__",
//         range<return_value_policy<copy_non_const_reference> >(
//             BinIterator(&SignalTrace::begin),
//             BinIterator(&SignalTrace::end)),
//         "Trace bin iterator, e.g., 'for s in trace: ...'")
    ;

  register_pointer_conversions<SignalTrace>();
}

// -----------------------------------------------------------------------------
/// Typedef required to choose the read-only PE iterators in SimChannel
typedef SimChannel::ConstPEIterator (SimChannel::*SimChannelPEIterator)() const;

// Pointer to GetTrace function
const SignalTrace& (SimChannel::*getTrace)() const =
  &SimChannel::GetTrace;

// Pointer to GetLogicPulse function
const DigitalLogic::Pulse& (SimChannel::*getPulse)() const =
  &SimChannel::GetLogicPulse;

/// Expose SimChannel to boost::python
void
pybind_simevt_SimChannel()
{
  class_<SimChannel, bases<Baggable>, boost::shared_ptr<SimChannel> >
    ("SimChannel",
     "Simulated hits in a given Channel.")
    .add_property("npe", &SimChannel::GetNPEs,
                  "Number of PEs in this SimChannel for this SimEvent.")
    .add_property("pmt_time", &SimChannel::pmtTime_,
                  "SimChannel hit time.")
    .add_property("channel_id", &SimChannel::channelId_,
                  "Global ID of the Channel.")
    .add_property("tank_id", &SimChannel::tankId_,
                  "ID of the parent Tank of this Channel.")
    .add_property("tank_channel_id", &SimChannel::tankChannelId_,
                  "ID of the Channel inside the Tank.")

    .add_property("signal_trace",
                  make_function(getTrace,
                       return_value_policy<copy_const_reference>()),
                  "Access the signal trace object.")

    .add_property("logic_pulse",
                  make_function(getPulse,
                       return_value_policy<copy_const_reference>()),
                  "Access the logic pulse object.")

    .def("__len__", &SimChannel::GetNPEs,
         "Number of PEs in this SimChannel for this SimEvent.")
    .def("__iter__",
         range<return_value_policy<copy_const_reference> >(
             SimChannelPEIterator(&SimChannel::PEsBegin),
             SimChannelPEIterator(&SimChannel::PEsEnd)),
         "PE iterator, e.g., 'for pe in simChannel: ...'")
    ;

  register_pointer_conversions<SimChannel>();
}

// -----------------------------------------------------------------------------
/// Typedef required to choose the read-only Channel iterators in SimTank
typedef SimTank::ConstChannelIterator (SimTank::*SimTankChannelIterator)() const;

/// Typedef required to choose the read-only Particle iterators in SimTank
typedef SimTank::ConstParticleIterator (SimTank::*SimTankParticleIterator)() const;

/// Expose SimTank to boost::python
void
pybind_simevt_SimTank()
{
  class_<SimTank, bases<Baggable>, boost::shared_ptr<SimTank> >
    ("SimTank",
     "Simulated hits in a given Channel.")
    .add_property("tank_id", &SimTank::GetId,
                  "ID of the Tank.")
    .def("channels",
         range<return_value_policy<copy_const_reference> >(
             SimTankChannelIterator(&SimTank::ChannelsBegin),
             SimTankChannelIterator(&SimTank::ChannelsEnd)),
         "Channel generator, e.g., 'for ch in simTank.channels(): ...'")
    .def("particles",
         range<return_value_policy<copy_const_reference> >(
             SimTankParticleIterator(&SimTank::ParticlesBegin),
             SimTankParticleIterator(&SimTank::ParticlesEnd)),
         "Particle generator, e.g., 'for pcl in simTank.particles(): ...'")
    ;

  register_pointer_conversions<SimTank>();
}

// -----------------------------------------------------------------------------
/// Expose SimEventHeader to boost::python
void
pybind_simevt_SimEventHeader()
{
  // Declare class scope so that nested ShowerComponent class shows up inside
  // the SimEventHeader class.
  scope simevent_header_class(
    class_<SimEventHeader, bases<Baggable>, boost::shared_ptr<SimEventHeader> >
      ("SimEventHeader",
       "Primary shower data for a simulated event.")
      .add_property("particle_id", &SimEventHeader::particleId_,
                    "Primary particle type (ParticleType enum).")
      .add_property("energy", &SimEventHeader::energy_,
                    "Primary particle energy (in base HAWC units).")
      .add_property("theta", &SimEventHeader::theta_,
                    "Primary particle zenith angle (in base HAWC units).")
      .add_property("phi", &SimEventHeader::phi_,
                    "Primary particle azimuth angle (in base HAWC units).")
      .add_property("xcore", &SimEventHeader::xcore_,
                    "Core location (x-direction) in simulated coordinates..")
      .add_property("ycore", &SimEventHeader::ycore_,
                    "Core location (y-direction) in simulated coordinates..")
      .add_property("xcoreDet", &SimEventHeader::xcoreDet_,
                    "Core location (x-direction) in detector coordinates.")
      .add_property("ycoreDet", &SimEventHeader::ycoreDet_,
                    "Core location (y-direction) in detector coordinates.")
      .add_property("xrange", &SimEventHeader::xrange_,
                    "Range of generated core positions (x-direction).")
      .add_property("yrange", &SimEventHeader::yrange_,
                    "Range of generated core positions (y-direction).")
      .add_property("nmax", &SimEventHeader::nmax_,
                    "Normalization of Gaisser-Hillas longitudinal profile fit.")
      .add_property("xmax", &SimEventHeader::xmax_,
                    "Maximum of Gaisser-Hillas longitudinal profile fit.")
      .add_property("x0", &SimEventHeader::x0_,
                    "First interaction of Gaisser-Hillas profile fit.")
      .add_property("a", &SimEventHeader::a_,
                    "Interaction length (constant term) of profile fit.")
      .add_property("b", &SimEventHeader::b_,
                    "Interaction length (linear term) of profile fit.")
      .add_property("c", &SimEventHeader::c_,
                    "Interaction length (quadratic term) of profile fit.")
      .add_property("chi2ndf", &SimEventHeader::chi2ndf_,
                    "Chi^2/NDF of Gaisser-Hillas longitudinal profile fit.")
      .add_property("age", &SimEventHeader::age_,
                    "Shower age s(X) = 3/(1 + 2*Xmax/X).")
      .add_property("nground", &SimEventHeader::nGround_,
                    "Number of particles at ground level.")
      .add_property("npond", &SimEventHeader::nPond_,
                    "Number of particles hitting the 'pond' area.")
      .add_property("rWgt", &SimEventHeader::rWgt_,
                    "HAWCSim radial throwing weight.")
      .add_property("wgtSim", &SimEventHeader::wgtSim_,
                    "Combined HAWCSim angular and energy throwing weight.")
      .add_property("toffset", &SimEventHeader::tOffset_,
                    "Shower time offset.")
      .add_property("npmts", &SimEventHeader::nPMTs_,
                    "Number of PMTs with a PE.")
      .add_property("nparticles", &SimEventHeader::nPcls_,
                    "Number of particles at ground level.")
      .add_property("em_shower", &SimEventHeader::emShower_,
                    "Properties of the shower electromagnetic component.")
      .add_property("muon_shower", &SimEventHeader::muonShower_,
                    "Properties of the shower muonic component.")
      .add_property("hadron_shower", &SimEventHeader::hadronShower_,
                    "Properties of the shower hadronic component.")
      .def(self_ns::str(self))
  );

  // Nested ShowerComponent class is in SimEventHeader scope
  class_<SimEventHeader::ShowerComponent>
    ("ShowerComponent",
     "Energy and number of particles in one component of the air shower.")
    .add_property("nparticles", &SimEventHeader::ShowerComponent::nPcls_,
                  "Number of particles in the shower component.")
    .add_property("energy", &SimEventHeader::ShowerComponent::energy_,
                  "Energy in the shower component.")
    ;

  register_pointer_conversions<SimEventHeader>();
}

// -----------------------------------------------------------------------------
/// Typedef required to choose the read-only Tank iterators in SimEvent
typedef SimEvent::ConstTankIterator (SimEvent::*SimEventTankIterator)() const;

/// Typedef required to choose the read-only Channel iterators in SimEvent
typedef SimEvent::ConstChannelIterator (SimEvent::*SimEventChannelIterator)() const;

/// Typedef required to choose the read-only Particle iterators in SimEvent
typedef SimEvent::ConstParticleIterator (SimEvent::*SimEventParticleIterator)() const;

/// Expose SimEvent to boost::python
void
pybind_simevt_SimEvent()
{
  class_<SimEvent, bases<Baggable>, boost::shared_ptr<SimEvent> >
    ("SimEvent",
     "Simulated event data: triggered Channels and Tanks, plus particles.")

    // Header
    .add_property("header",
                  make_function(&SimEvent::GetEventHeader,
                                return_value_policy<copy_const_reference>()),
                  "Access to information in the SimEvent header.")

    // Tanks
    .add_property("ntanks", &SimEvent::GetNSimTanks,
                  "Number of triggered Tanks in the SimEvent.")
    .def("tanks",
         range<return_value_policy<copy_const_reference> >(
             SimEventTankIterator(&SimEvent::TanksBegin),
             SimEventTankIterator(&SimEvent::TanksEnd)),
         "Tank generator, e.g., 'for tk in simEvent.tanks(): ...'")

    // Channels
    .add_property("nchannels", &SimEvent::GetNSimChannels,
                  "Number of triggered Channels in the SimEvent.")
    .def("channels",
         range<return_value_policy<copy_const_reference> >(
             SimEventChannelIterator(&SimEvent::ChannelsBegin),
             SimEventChannelIterator(&SimEvent::ChannelsEnd)),
         "Channel generator, e.g., 'for ch in simEvent.channels(): ...'")

    // Particles
    .add_property("nparticles", &SimEvent::GetNParticles,
                  "Number of particles in the SimEvent.")
    .def("particles",
         range<return_value_policy<copy_const_reference> >(
             SimEventParticleIterator(&SimEvent::ParticlesBegin),
             SimEventParticleIterator(&SimEvent::ParticlesEnd)),
         "Particle generator, e.g., 'for pcl in simEvent.particles(): ...'")
    ;

  register_pointer_conversions<SimEvent>();
}

// -----------------------------------------------------------------------------
/// Expose G4SimHeader to boost::python
void
pybind_simevt_G4SimHeader()
{
  // Declare class scope so that nested classes show up inside G4SimHeader.
  scope g4sim_header_class(
    class_<G4SimHeader, bases<Baggable>, boost::shared_ptr<G4SimHeader> >
      ("G4SimHeader",
       "Metadata for a CORSIKA + GEANT4 simulation.")
      .add_property("energy", &G4SimHeader::energy_,
                    "Simulated energy spectrum.")
      .add_property("zenith_angle", &G4SimHeader::zenithAngle_,
                    "Simulated zenith angle range.")
      .add_property("core_position", &G4SimHeader::corePos_,
                    "Simulated core position limits.")
      .add_property("generator_flags", &G4SimHeader::genFlags_,
                    "Simulated core position limits.")
      .add_property("magnetic_field", &G4SimHeader::B_,
                    "Local geomagnetic field settings.")
      .add_property("detector_type", &G4SimHeader::detectorType_,
                    "Detector type flag.")
      .add_property("particle_id", &G4SimHeader::primPartID_,
                    "CORSIKA ID of the primary particle type.")
      .add_property("dynamic_core", &G4SimHeader::dynamicCore_,
                    "Dynamic core flag.")
      .add_property("survey_file", &G4SimHeader::tankSurveyFile_,
                    "Survey file used in simulation.")
      .add_property("current_file", &G4SimHeader::tankSurveyFile_,
                    "Current input file used in the processing loop.")
      .add_property("geant_version", &G4SimHeader::geantVersion_,
                    "Version of GEANT used in simulation production.")
      .add_property("g4sim_version", &G4SimHeader::g4simVersion_,
                    "Version of g4sim/hawcsim used in simulation production.")
      .def(self_ns::str(self))
  );

  // Nested ShowerComponent classes are in G4SimHeader scope
  class_<G4SimHeader::Energy>
    ("Energy",
     "Energy spectrum used in CORSIKA simulation.")
    .add_property("sp_index", &G4SimHeader::Energy::spectralIndex_,
                  "Index of the simulated energy spectrum.")
    .add_property("min_energy", &G4SimHeader::Energy::min_,
                  "Minimum simulated energy.")
    .add_property("max_energy", &G4SimHeader::Energy::max_,
                  "Minimum simulated energy.")
    ;

  class_<G4SimHeader::ZenithAngle>
    ("ZenithAngle",
     "Zenith angle range used in CORSIKA simulation.")
    .add_property("min_zenith", &G4SimHeader::ZenithAngle::min_,
                  "Minimum simulated energy.")
    .add_property("max_zenith", &G4SimHeader::ZenithAngle::max_,
                  "Minimum simulated energy.")
    ;

  class_<G4SimHeader::CorePosition>
    ("CorePosition",
     "Core position limits used in CORSIKA simulation.")
    .add_property("xmax", &G4SimHeader::CorePosition::xMax_,
                  "Maximum x position with respect to detector center.")
    .add_property("ymax", &G4SimHeader::CorePosition::yMax_,
                  "Maximum y position with respect to detector center.")
    ;

  class_<G4SimHeader::GeneratorFlags>
    ("GeneratorFlags",
     "CORSIKA low and high-energy hadronic interaction models.")
    .add_property("GHEISHA", &G4SimHeader::GeneratorFlags::GHEISHA_,
                  "GHEISHA model flag.")
    .add_property("VENUS", &G4SimHeader::GeneratorFlags::VENUS_,
                  "VENUS model flag.")
    ;

  class_<G4SimHeader::MagneticField>
    ("MagneticField",
     "Local geomagnetic field used in CORSIKA simulation.")
    .add_property("x", &G4SimHeader::MagneticField::x_,
                  "Geomagnetic field x-component.")
    .add_property("z", &G4SimHeader::MagneticField::z_,
                  "Geomagnetic field z-component.")
    ;

  register_pointer_conversions<G4SimHeader>();
}

