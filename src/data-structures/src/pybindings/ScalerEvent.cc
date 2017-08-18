/*!
 * @file ScalerEvent.cc 
 * @brief Python bindings to simulated event structures.
 * @author Segev BenZvi 
 * @date 21 Dec 2011 
 * @version $Id: ScalerEvent.cc 16411 2013-07-25 22:13:45Z sybenzvi $
 */

#include <boost/python.hpp>

#include <data-structures/event/ScalerEvent.h>

#include <hawcnest/impl/pybindings.h>

using namespace boost::python;

typedef return_value_policy<copy_const_reference> RVPCCR;

// -----------------------------------------------------------------------------
/// Expose ScalerChannel to boost::python
void
pybind_scaler_ScalerChannel()
{
  class_<ScalerChannel, bases<Baggable>, boost::shared_ptr<ScalerChannel> >
    ("ScalerChannel",
     "Scaler count data in a physical channel.")
    .add_property("channelId", &ScalerChannel::GetChannelId)
    .add_property("tankId", &ScalerChannel::GetTankId)
    .add_property("tankChannelId", &ScalerChannel::GetTankChannelId)
    .add_property("count", &ScalerChannel::GetCount)
    ;
}

// -----------------------------------------------------------------------------
/// Typedef required to choose the read-only Channel iterators in ScalerEvent
typedef ScalerEvent::ConstChannelIterator (ScalerEvent::*ScalerChannelIterator)() const;

/// Expose ScalerEvent to boost::python
void
pybind_scaler_ScalerEvent()
{
  class_<ScalerEvent, bases<Baggable>, boost::shared_ptr<ScalerEvent> >
    ("ScalerEvent",
      "Storage of scaler data from physical detector channels.")
    .def("channels",
         range<return_value_policy<copy_const_reference> >(
             ScalerChannelIterator(&ScalerEvent::ChannelsBegin),
             ScalerChannelIterator(&ScalerEvent::ChannelsEnd)),
         "Channel generator, e.g., 'for ch in scalerEvent.channels(): ...'")

    .add_property("nchannels",
                  &ScalerEvent::GetNScalerChannels)
    .add_property("startTime", 
                  make_function(&ScalerEvent::GetStartTime, RVPCCR()),
                  "Starting TimeStamp of the measurement.")
    .add_property("timeWidth",
                  make_function(&ScalerEvent::GetTimeWidth, RVPCCR()),
                  "Duration of the measurement.")
    ;
}

