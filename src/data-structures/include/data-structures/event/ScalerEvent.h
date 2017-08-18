/*!
 * @file ScalerEvent.h
 * @brief Scaler event data access classes.
 * @author Segev BenZvi
 * @ingroup event_data
 * @version $Id: ScalerEvent.h 18879 2014-02-19 21:41:29Z sybenzvi $
 */

#ifndef DATACLASSES_EVENT_SCALEREVENT_H_INCLUDED
#define DATACLASSES_EVENT_SCALEREVENT_H_INCLUDED

#include <data-structures/event/ScalerChannel.h>
#include <data-structures/time/TimeStamp.h>

#include <hawcnest/PointerTypedefs.h>

/*!
 * @class ScalerEvent
 * @author Segev BenZvi
 * @date 24 Jul 2013
 * @ingroup event_data
 * @brief Storage of scaler data from physical detector channels.
 *
 * A scaler event records the number of scaler counts above individual the
 * electronics low ToT threshold for physical channels in the detector during
 * some time window.
 */
class ScalerEvent : public Baggable {

  public:

    typedef std::vector<ScalerChannel> ChannelList;
    typedef ChannelList::iterator ChannelIterator;
    typedef ChannelList::const_iterator ConstChannelIterator;

    ScalerEvent();

    /// Read-write access to the start of the scaler channel list
    ChannelIterator ChannelsBegin() { return channels_.begin(); }

    /// Read-write access to the end of the scaler channel list
    ChannelIterator ChannelsEnd() { return channels_.end(); }

    /// Read-only access to the start of the scaler channel list
    ConstChannelIterator ChannelsBegin() const { return channels_.begin(); }

    /// Read-only access to the end of the scaler channel list
    ConstChannelIterator ChannelsEnd() const { return channels_.end(); }

    /// Add a tank with simulated data to the list of tanks
    void AddScalerChannel(const ScalerChannel& c);

    /// Number of tanks participating in the simulated event
    size_t GetNScalerChannels() const { return channels_.size(); }
      
    /// Check for the presence of a Channel in the event by ID
    bool HasChannel(const int channelId) const;

    /// Access scaler data by global channel ID
    const ScalerChannel& GetChannel(const int channelId) const;

    ScalerChannel& GetChannel(const int channelId);

    /// Get the start time of the event window
    const TimeStamp& GetStartTime() const { return startTime_; }

    void SetStartTime(const TimeStamp& t) { startTime_ = t; }

    /// Get the width the event window
    const TimeInterval& GetTimeWidth() const { return timeWidth_; }

    void SetTimeWidth(const TimeInterval& w) { timeWidth_ = w; }

    /// Combine this event with another one
    ScalerEvent& operator+=(const ScalerEvent& s);

  private:

    TimeStamp startTime_;     ///< Start time of the event record
    TimeInterval timeWidth_;  ///< Width of the record readout window

    ChannelList channels_;

};

SHARED_POINTER_TYPEDEFS(ScalerEvent);

#endif // DATACLASSES_EVENT_SCALEREVENT_H_INCLUDED

