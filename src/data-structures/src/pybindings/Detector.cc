/*!
 * @file Detector.cc 
 * @brief Python bindings to classes in the detector structures.
 * @author Segev BenZvi 
 * @date 17 Dec 2011 
 * @version $Id: Detector.cc 32626 2016-06-09 16:25:15Z sybenzvi $
 */

#include <boost/python.hpp>

#include <data-structures/detector/PMT.h>
#include <data-structures/detector/Channel.h>
#include <data-structures/detector/Tank.h>
#include <data-structures/detector/Detector.h>

#include <hawcnest/impl/pybindings.h>

using namespace boost::python;
using namespace det;

// -----------------------------------------------------------------------------
/// Expose det::PMT to boost::python
void
pybind_detector_PMT()
{
  class_<PMT, boost::shared_ptr<PMT> >
    ("PMT",
     "Photomultiplier tube (PMT) physical properties")
    .add_property("pmt_id",
                  make_function(&PMT::GetPMTId, 
                      return_value_policy<copy_const_reference>()),
                  "The unique ID number of the PMT.")
    .add_property("pmt_type",
                  make_function(&PMT::GetPMTType,
                      return_value_policy<copy_const_reference>()),
                  "Type of PMT (R5912, R7081HQE, R12199, ...).")
    .add_property("diameter",
                  &PMT::GetDiameter,
                  "Diameter of the glass envelope of the PMT.")
    .add_property("cathode_diameter",
                  &PMT::GetCathodeDiameter,
                  "Effective diameter of the photocathode.")
    .add_property("base_diameter",
                  &PMT::GetBaseDiameter,
                  "Diameter of the watertight base.")
    .add_property("base_height",
                  &PMT::GetBaseHeight,
                  "Height of the watertight base.")
    .add_property("gain",
                  &PMT::GetPMTGain,
                  "Gain = PE/charge ratio after the last dynode.")
    ;

  enum_<PMTType>("PMTType")
    .value("R5912", R5912)
    .value("R7081HQE", R7081HQE)
    .value("R12199", R12199)
    .value("UNKNOWN_PMT", UNKNOWN_PMT)
    .export_values()
    ;
}

// -----------------------------------------------------------------------------
/// Expose det::Channel to boost::python
void
pybind_detector_Channel()
{
  class_<Channel, boost::shared_ptr<Channel> >
    ("Channel",
     "Storage for detector channel geometry")
    .def(init<const IdType&, const IdType&, const IdType&, const Point&>())
    .add_property("name",
                  make_function(&Channel::GetName,
                      return_value_policy<copy_const_reference>()),
                  "The name of the Channel, e.g. E18A")
    .add_property("channel_id",
                  make_function(&Channel::GetChannelId, 
                      return_value_policy<copy_const_reference>()),
                  "The unique global ID number of the Channel.")
    .add_property("tank_id",
                  make_function(&Channel::GetTankId, 
                      return_value_policy<copy_const_reference>()),
                  "The ID of the Tank for this Channel.")
    .add_property("tank_channel_id",
                  make_function(&Channel::GetTankChannelId, 
                      return_value_policy<copy_const_reference>()),
                  "The ID of the Channel within the Tank.")
    .add_property("pmt",
                  make_function(&Channel::GetPMT,
                      return_value_policy<copy_const_reference>()),
                  "The physical PMT connected to this channel.")
    .add_property("position",
                  make_function(&Channel::GetPosition, 
                      return_value_policy<copy_const_reference>()),
                  "The position of the Channel in Detector coordinates.")
    .def(self < self)
    ;
}

// -----------------------------------------------------------------------------
/// Typedef required to choose the read-only Channel iterators in Tank
typedef Tank::ConstChannelIterator (Tank::*TankChannelIter)() const;

/// Expose det::Tank to boost::python
void
pybind_detector_Tank()
{
  class_<Tank, boost::shared_ptr<Tank> >
    ("Tank",
     "Storage for detector channel geometry")
    .def(init<const IdType&, const Point&>())
    .add_property("tank_id",
                  make_function(&Tank::GetTankId, 
                      return_value_policy<copy_const_reference>()),
                  "The unique global ID number of the Tank.")
    .add_property("tank_type",
                  make_function(&Tank::GetTankType,
                      return_value_policy<copy_const_reference>()),
                  "The type of water tank (HAWC_WCD, ...).")
    .add_property("nchannels",
                  &Tank::GetNChannels, 
                  "The number of Channel slots inside the Tank.")
    .add_property("name",
                  make_function(&Tank::GetName,
                      return_value_policy<copy_const_reference>()),
                  "The name of the Tank, e.g. E18")
    .add_property("position",
                  make_function(&Tank::GetPosition, 
                      return_value_policy<copy_const_reference>()),
                  "The position of the Tank in Detector coordinates.")
    .add_property("diameter",
                  &Tank::GetDiameter, 
                  "The diameter of the Tank.")
    .add_property("height",
                  &Tank::GetHeight, 
                  "The height of the Tank.")
    .add_property("water_height",
                  &Tank::GetWaterHeight, 
                  "The height of the water inside the Tank.")
    .def("__len__",
         &Tank::GetNChannels, 
         "The number of Channel slots inside the Tank.")
    .def("__iter__",
         range<return_value_policy<copy_const_reference> >(
             TankChannelIter(&Tank::ChannelsBegin),
             TankChannelIter(&Tank::ChannelsEnd)),
         "Channel iterator, e.g., 'for ch in tank: ...'")
    .def(self < self)
    ;

  enum_<TankType>("TankType")
    .value("HAWC_WCD", HAWC_WCD)
    .value("HAWC_OR", HAWC_OR)
    .value("UNKNOWN_TANK", UNKNOWN_TANK)
    .export_values()
    ;
}

// -----------------------------------------------------------------------------
/// Typedef required to choose the read-only Tank iterators in Detector
typedef Detector::ConstTankIterator (Detector::*DetTankIter)() const;

/// Typedef required to choose the read-only Channel iterators in Detector
typedef Detector::ConstChannelIterator (Detector::*DetChannelIter)() const;

// Overload the GetTank function for different argument signatures
const Tank& (Detector::*gt1)(const IdType&) const = &Detector::GetTank;
const Tank& (Detector::*gt2)(const std::string&) const = &Detector::GetTank;

// Overload the HasTank function for different argument signatures
bool (Detector::*ht1)(const IdType&) const = &Detector::HasTank;
bool (Detector::*ht2)(const std::string&) const = &Detector::HasTank;

// Overload the GetChannel function for different argument signatures
const Channel& (Detector::*gc1)(const IdType&) const = &Detector::GetChannel;
const Channel& (Detector::*gc2)(const std::string&) const =
                                                &Detector::GetChannel;

// Overload the HasChannel function for different argument signatures
bool (Detector::*hc1)(const IdType&) const = &Detector::HasChannel;
bool (Detector::*hc2)(const std::string&) const = &Detector::HasChannel;

/// Expose the read-only parts of det::Detector to boost::python
void
pybind_detector_Detector()
{
  class_<Detector, boost::shared_ptr<Detector> >
    ("Detector",
     "In-memory representation of the physical HAWC detector.")

    .add_property("ntanks",
                  &Detector::GetNTanks,
                  "The number of Tanks in the Detector.")
    .def("get_tank", gt1,
         return_value_policy<copy_const_reference>(),
         "Access a Tank object by ID.")
    .def("get_tank", gt2,
         return_value_policy<copy_const_reference>(),
         "Access a Tank object by name.")
    .def("has_tank", ht1,
         "Return true if Detector has a Tank ID.")
    .def("has_tank", ht2,
         "Return true if Detector has a Tank name.")
    .def("get_array_center_x",&Detector::GetArrayCenterX,
         "Return the array center X.")
    .def("get_array_center_y",&Detector::GetArrayCenterY,
         "Return the array center Y.")
    .def("get_array_center_z",&Detector::GetArrayCenterZ,
         "Return the array center Z.")
    .add_property("nchannels",
                  &Detector::GetNChannels,
                  "The number of Channels in the Detector.")
    .def("get_channel", gc1,
         return_value_policy<copy_const_reference>(),
         "Access a Channel object by ID.")
    .def("get_channel", gc2,
         return_value_policy<copy_const_reference>(),
         "Access a Channel object by name.")
    .def("has_channel", hc1,
         "Return true if Detector has Channel ID.")
    .def("has_channel", hc2,
         "Return true if Detector has Channel name.")

    .def("__len__",
         &Detector::GetNTanks,
         "The number of Tanks in the Detector.")
    .def("__iter__",
         range<return_value_policy<copy_const_reference> >(
             DetTankIter(&Detector::TanksBegin),
             DetTankIter(&Detector::TanksEnd)),
         "Tank iterator, e.g., 'for tk in detector:'")

    .def("tanks",
         range<return_value_policy<copy_const_reference> >(
             DetTankIter(&Detector::TanksBegin),
             DetTankIter(&Detector::TanksEnd)),
         "Tank iterator, e.g., 'for tk in detector.tanks():'")

    .def("channels",
         range<return_value_policy<copy_const_reference> >(
             DetChannelIter(&Detector::ChannelsBegin),
             DetChannelIter(&Detector::ChannelsEnd)),
         "Channel generator, e.g., 'for ch in detector.channels(): ...'")

    .add_property("name",
                  make_function(&Detector::GetName, 
                      return_value_policy<copy_const_reference>()),
                  "The name of the Detector in the survey.")
    .add_property("version",
                  make_function(&Detector::GetVersion, 
                      return_value_policy<copy_const_reference>()),
                  "The version of the Detector survey.")
    .add_property("lat_lon_alt",
                  make_function(&Detector::GetLatitudeLongitudeHeight,
                       return_value_policy<copy_const_reference>()),
                  "The latitude, longitude, and altitude of the Detector.")
    ;
}

