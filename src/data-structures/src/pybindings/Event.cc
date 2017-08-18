/*!
 * @file Event.cc 
 * @brief Python bindings to event-based data structures.
 * @author Segev BenZvi 
 * @date 21 Dec 2011 
 * @version $Id: Event.cc 37981 2017-03-03 15:07:44Z zhampel $
 */

#include <boost/python.hpp>

#include <data-structures/event/HitFlags.h>
#include <data-structures/event/HitTrigData.h>
#include <data-structures/event/HitCalData.h>
#include <data-structures/event/Hit.h>
#include <data-structures/event/EventFlags.h>
#include <data-structures/event/ChannelEvent.h>
#include <data-structures/event/TankEvent.h>
#include <data-structures/event/Event.h>
#include <data-structures/event/EventList.h>

#include <hawcnest/impl/pybindings.h>

using namespace boost::python;
using namespace evt;

typedef return_value_policy<copy_const_reference> RVPCCR;

// -----------------------------------------------------------------------------
/// Structure to expose trigger flags to python
struct __hitflags__ {

  static const uint16_t hitflags_INVALID_TOT() { return INVALID_TOT; }
  static const uint16_t hitflags_AFTERPULSE_VETO() { return AFTERPULSE_VETO; }
  static const uint16_t hitflags_AMBIGUOUS_EDGE_RESULT() { return AMBIGUOUS_EDGE_RESULT; }

};

/// Define python bindings for the trigger flags
void
pybind_evt_HitFlags()
{
  class_<__hitflags__, boost::noncopyable>
    ("HitFlags",
     "Flags to classify non-physics Hits (Invalid TOT, Afterpulse, Ambiguous, etc.).")
    .add_static_property("INVALID_TOT",
                         &__hitflags__::hitflags_INVALID_TOT)
    .add_static_property("AFTERPULSE_VETO",
                         &__hitflags__::hitflags_AFTERPULSE_VETO)
    .add_static_property("AMBIGUOUS_EDGE_RESULT",
                         &__hitflags__::hitflags_AMBIGUOUS_EDGE_RESULT)
    ;
}

// -----------------------------------------------------------------------------
/// Expose HitTrigData to boost::python
void
pybind_evt_HitTrigData()
{
  class_<HitTrigData, bases<Baggable>, boost::shared_ptr<HitTrigData> >
    ("HitTrigData",
     "Stores TDC edges from a 'raw' hit.")
    .def_readonly("time", &HitTrigData::time_,
                  "Time of leading edge relative to event time.")
    .def_readonly("time01", &HitTrigData::time01_,
                  "Time difference between leading edge and next edge.")
    .def_readonly("loTOT", &HitTrigData::loTOT_,
                  "Time above low discriminator threshold (=t01 for 2-edge).")
    .def_readonly("hiTOT", &HitTrigData::hiTOT_,
                  "Time above high discriminator threshold (=0 for 2-edge).")
    .def_readonly("flags", &HitTrigData::flags_,
                  "Flags characterizing the hit (e.g., for afterpulses).")
    .def_readonly("trigger_flags", &HitTrigData::triggerFlags_,
                  "Flags indicating presence in the trigger.")
    .def("is_four_edge", &HitTrigData::IsFourEdge,
         "Return true if this hit is a 4-edge event.")
    ;

  register_pointer_conversions<HitTrigData>();
}

// -----------------------------------------------------------------------------
/// Expose Edge data to boost::python
void
pybind_evt_Edge()
{
  class_<Edge, boost::shared_ptr<Edge> >
    ("Edge",
     "Stores TDC edges from a 'raw' hit.")
    .def(init<const HitTrigData&, const uint8_t>())
    .def("is_leading", &Edge::IsLeading, "True if this is a leading edge.")
    .def("is_trailing", &Edge::IsTrailing, "True if this is a trailing edge.")
    .def("time", &Edge::GetTime, "Edge time within hit, in clock steps.")
    .def(self == self)
    .def(self != self)
    ;
}

// -----------------------------------------------------------------------------
/// Expose HitCalData to boost::python
void
pybind_evt_HitCalData()
{
  class_<HitCalData, bases<Baggable>, boost::shared_ptr<HitCalData> >
    ("HitCalData",
     "Stores data from a calibrated hit after edge finding.")
    .def_readonly("PEs", &HitCalData::PEs_,
                  "Estimated photoelectron count in the hit.")
    .def_readonly("time", &HitCalData::time_,
                  "Time of the hit relative to the event time.")
    .def_readonly("is_max_charge", &HitCalData::maxCharge_,
                  "True if hiTOT exceeded ChargeCal Range.")
    ;

  register_pointer_conversions<HitCalData>();
}

// -----------------------------------------------------------------------------
/// Typedef required to choose the read-only Edge iterators in Hit
typedef Hit::ConstEdgeIterator (Hit::*HitEdgeIter)() const;

/// Expose Hit to boost::python
void
pybind_evt_Hit()
{
  class_<Hit, bases<Baggable>, boost::shared_ptr<Hit> >
    ("Hit",
     "Event data from a PMT pulse on a given Channel.")
    .def_readonly("trigger_data", &Hit::triggerData_,
                  "TDC edges from a 'raw' hit (HitTrigData).")
    .def_readonly("calib_data", &Hit::calibData_,
                  "Calibrated PEs in the hit (HitCaldata).")
    .def_readonly("channel_id", &Hit::channelId_,
                  "Global channel ID.")
    .def_readonly("tank_id", &Hit::tankId_,
                  "Parent tank ID.")
    .def_readonly("tank_channel_id", &Hit::tankChannelId_,
                  "Channel ID within a tank.")
    .def("__iter__",
         boost::python::range<return_value_policy<copy_const_reference> >(
             HitEdgeIter(&Hit::EdgesBegin),
             HitEdgeIter(&Hit::EdgesEnd)),
         "Edge iterator, e.g., 'for edge in hit: ...'")
    ;

  register_pointer_conversions<Hit>();
}

// -----------------------------------------------------------------------------
/// Typedef required to choose the read-only Hit iterators in ChannelEvent
typedef ChannelEvent::ConstHitIterator (ChannelEvent::*ChannelEventHitIterator)() const;

/// Typedef required to choose the read-only Edge iterators in ChannelEvent
typedef ChannelEvent::ConstEdgeIterator (ChannelEvent::*ChannelEventEdgeIterator)() const;

/// Expose ChannelEvent to boost::python
void
pybind_evt_ChannelEvent()
{
  class_<ChannelEvent, bases<Baggable>, boost::shared_ptr<ChannelEvent> >
    ("ChannelEvent",
     "Event data from a single channel (raw trigger and calibrated).")
    .def(init<int, int, int>())
    .add_property("channel_id", &ChannelEvent::GetChannelId,
                  "Global ID of the channel.")
    .add_property("tank_id", &ChannelEvent::GetTankId,
                  "ID of the parent tank of this channel.")
    .add_property("tank_channel_id", &ChannelEvent::GetTankChannelId,
                  "ID of the channel inside the tank.")
    .def("hits",
         boost::python::range<return_value_policy<copy_const_reference> >(
             ChannelEventHitIterator(&ChannelEvent::HitsBegin),
             ChannelEventHitIterator(&ChannelEvent::HitsEnd)),
         "Hit generator, e.g., 'for hit in chEvt.hits(): ...'")
    .def("edges",
         boost::python::range<return_value_policy<copy_const_reference> >(
             ChannelEventEdgeIterator(&ChannelEvent::EdgesBegin),
             ChannelEventEdgeIterator(&ChannelEvent::EdgesEnd)),
         "Edge generator, e.g., 'for edge in chEvt.edges(): ...'")
    ;

  register_pointer_conversions<ChannelEvent>();
}

// -----------------------------------------------------------------------------
/// Typedef required to choose the read-only Channel iterators in TankEvent
typedef TankEvent::ConstChannelIterator (TankEvent::*TankEventChannelIterator)() const;

/// Typedef required to choose the read-only Hit iterators in TankEvent
typedef TankEvent::ConstHitIterator (TankEvent::*TankEventHitIterator)() const;

/// Expose TankEvent to boost::python
void
pybind_evt_TankEvent()
{
  class_<TankEvent, bases<Baggable>, boost::shared_ptr<TankEvent> >
    ("TankEvent",
     "Container for Channels with event data.")
    .def(init<int>())
    .add_property("tank_id", &TankEvent::GetTankId,
                  "ID of the Tank.")
    .add_property("nhits", &TankEvent::GetNHits,
                  "Number of hits in the data from this Tank.")
    .add_property("nchannels", &TankEvent::GetNChannels,
                  "Number of Channels from this Tank with data.")
    .def("hits",
         boost::python::range<return_value_policy<copy_const_reference> >(
             TankEventHitIterator(&TankEvent::HitsBegin),
             TankEventHitIterator(&TankEvent::HitsEnd)),
         "Hit generator, e.g., 'for hit in chEvt.hits(): ...'")
    .def("channels",
         boost::python::range<return_value_policy<copy_const_reference> >(
             TankEventChannelIterator(&TankEvent::ChannelsBegin),
             TankEventChannelIterator(&TankEvent::ChannelsEnd)),
         "Channel generator, e.g., 'for edge in chEvt.channels(): ...'")
    ;

  register_pointer_conversions<TankEvent>();
}

// -----------------------------------------------------------------------------
/// Structure to expose EventFlags and TriggerFlags to python
struct __eventflags__ {

  // EventFlags
  static const uint16_t eventflags_BAD_EVENT()
  { return BAD_EVENT; }

  static const uint16_t eventflags_CALIBRATION_EVENT()
  { return CALIBRATION_EVENT; }

  static const uint16_t eventflags_TDC_CHIP_L1_BUFFER_ERROR()
  { return TDC_CHIP_L1_BUFFER_ERROR; }

  static const uint16_t eventflags_TDC_CHIP_FIFO_ERROR()
  { return TDC_CHIP_FIFO_ERROR; }

  static const uint16_t eventflags_TDC_CHIP_HITS_LOST()
  { return TDC_CHIP_HITS_LOST; }

  static const uint16_t eventflags_TDC_CHIP_ERROR()
  { return TDC_CHIP_ERROR; }

  static const uint16_t eventflags_TDC_OUTPUT_BUFFER_OVERFLOW()
  { return TDC_OUTPUT_BUFFER_OVERFLOW; }

  // TriggerFlags
  static const uint16_t eventflags_SIMPLE_MULTIPLICITY_TRIGGER()
  { return SIMPLE_MULTIPLICITY_TRIGGER; }

  static const uint16_t eventflags_TANK_MULTIPLICITY_TRIGGER()
  { return TANK_MULTIPLICITY_TRIGGER; }

  static const uint16_t eventflags_MIN_BIAS_TRIGGER()
  { return MIN_BIAS_TRIGGER; }

  static const uint16_t eventflags_MUON_TRIGGER()
  { return MUON_TRIGGER; }

};

void
pybind_evt_EventFlags()
{
  class_<__eventflags__, boost::noncopyable>
    ("EventFlags",
     "Flags to classify non-physics events, DAQ error states, and triggers.")
    // EventFlags
    .add_static_property("BAD_EVENT",
                         &__eventflags__::eventflags_BAD_EVENT)
    .add_static_property("CALIBRATION_EVENT",
                         &__eventflags__::eventflags_CALIBRATION_EVENT)
    .add_static_property("TDC_CHIP_L1_BUFFER_ERROR",
                         &__eventflags__::eventflags_TDC_CHIP_L1_BUFFER_ERROR)
    .add_static_property("TDC_CHIP_FIFO_ERROR",
                         &__eventflags__::eventflags_TDC_CHIP_FIFO_ERROR)
    .add_static_property("TDC_CHIP_HITS_LOST",
                         &__eventflags__::eventflags_TDC_CHIP_HITS_LOST)
    .add_static_property("TDC_CHIP_ERROR",
                         &__eventflags__::eventflags_TDC_CHIP_ERROR)
    .add_static_property("TDC_OUTPUT_BUFFER_OVERFLOW",
                         &__eventflags__::eventflags_TDC_OUTPUT_BUFFER_OVERFLOW)

    // TriggerFlags
    .add_static_property("SIMPLE_MULTIPLICITY_TRIGGER",
                         &__eventflags__::eventflags_SIMPLE_MULTIPLICITY_TRIGGER)
    .add_static_property("TANK_MULTIPLICITY_TRIGGER",
                         &__eventflags__::eventflags_TANK_MULTIPLICITY_TRIGGER)
    .add_static_property("MIN_BIAS_TRIGGER",
                         &__eventflags__::eventflags_MIN_BIAS_TRIGGER)
    .add_static_property("MUON_TRIGGER",
                         &__eventflags__::eventflags_MUON_TRIGGER)
    ;
}

// -----------------------------------------------------------------------------
/// Typedef required to choose the read-only Tank iterators in Event
typedef Event::ConstTankIterator (Event::*EventTankIterator)() const;

/// Typedef required to choose the read-only Channel iterators in Event
typedef Event::ConstChannelIterator (Event::*EventChannelIterator)() const;

/// Typedef required to choose the read-only Hit iterators in Event
typedef Event::ConstHitIterator (Event::*EventHitIterator)() const;

/// Expose Event to boost::python
void
pybind_evt_Event()
{
  class_<Event, bases<Baggable>, boost::shared_ptr<Event> >
    ("Event",
     "Container for Channels with event triggered and calibrated data.")
    // Event properties
    .add_property("run_id", &Event::GetRunID, &Event::SetRunID,
                  "The global run ID of the Event.")
    .add_property("time_slice_id", &Event::GetTimeSliceID, &Event::SetTimeSliceID,
                  "The time slice ID of the Event.")
    .add_property("event_id", &Event::GetEventID, &Event::SetEventID,
                  "The Event ID within the run.")
    .add_property("event_flags", &Event::GetEventFlags, &Event::SetEventFlags,
                  "The event flags (a bitmask: see EventFlags).")
    .add_property("trigger_flags", &Event::GetTriggerFlags, &Event::SetTriggerFlags,
                  "The trigger flags (a bitmask: see EventFlags).")
    .add_property("time", &Event::GetTime, &Event::SetTime,
                  "The GPS TimeStamp of the Event.")

    // Tank, Channel, and Hit properties and generators
    .add_property("ntanks", &Event::GetNTanks,
                  "Number of Tanks with the data in the Event.")
    .add_property("nchannels", &Event::GetNChannels,
                  "Number of Channels with the data in the Event.")
    .add_property("nhits", &Event::GetNHits,
                  "Number of hits in the Event.")
    .def("tanks",
         boost::python::range<return_value_policy<copy_const_reference> >(
             EventTankIterator(&Event::TanksBegin),
             EventTankIterator(&Event::TanksEnd)),
         "Tank generator, e.g., 'for tk in event.tanks(): ...'")
    .def("channels",
         boost::python::range<return_value_policy<copy_const_reference> >(
             EventChannelIterator(&Event::ChannelsBegin),
             EventChannelIterator(&Event::ChannelsEnd)),
         "Channel generator, e.g., 'for ch in event.channels(): ...'")
    .def("hits",
         boost::python::range<return_value_policy<copy_const_reference> >(
             EventHitIterator(&Event::HitsBegin),
             EventHitIterator(&Event::HitsEnd)),
         "Hit generator, e.g., 'for hit in event.hits(): ...'")

    .def("has_channel", &Event::HasChannel,
         "True if a channel (accessed by channel Id) is part of this event.")
    .def("get_channel", &Event::GetChannel,
         return_value_policy<copy_const_reference>(),
         "Access a channel by its channel Id.")

    // Properties related to the trigger and event flags
    .add_property("is_bad", &Event::IsBadEvent,
                  "True if Event has been flagged as bad.")
    .add_property("is_calibration", &Event::IsCalibrationEvent,
                  "True if Event has been flagged as a calibration event.")
    .add_property("is_minbias_trigger", &Event::IsMinBiasTriggerEvent,
                  "True if Event has been flagged as a minimum-bias trigger.")
    .add_property("is_smt", &Event::IsSMTEvent,
                  "True if Event was flagged as a simple-multiplicity trigger.")
    .add_property("is_tank_trigger", &Event::IsTankTriggerEvent,
                  "True if Event was flagged as a tank-multiplicity trigger.")
    .add_property("is_muon_trigger", &Event::IsMuonTriggerEvent,
                  "True if Event was flagged as a muon trigger.")
    ;

  register_pointer_conversions<Event>();
}

// -----------------------------------------------------------------------------
/// Typedef required to choose the read-only Event iterators in EventList
typedef EventList::ConstEventIterator (EventList::*EventListIterator)() const;

/// Expose Event to boost::python
void
pybind_evt_EventList()
{
  class_<EventList, bases<Baggable>, boost::shared_ptr<EventList> >
    ("Event",
     "A container of multiple Events.")
    .def(init<int>())
    .def("__len__", &EventList::GetNEvents)
    .add_property("nevents", &EventList::GetNEvents,
                  "Number of Events in the list.")
    .def("__iter__",
         boost::python::range<return_value_policy<copy_const_reference> >(
             EventListIterator(&EventList::EventsBegin),
             EventListIterator(&EventList::EventsEnd)),
         "Event iterator, 'for e in eventList: ...'")
    ;

  register_pointer_conversions<EventList>();
}

