/*!
 * @file TankEvent.h
 * @brief Tank-level data from an Event.
 * @author Segev BenZvi
 * @ingroup event_data
 * @version $Id: TankEvent.h 24278 2015-02-27 17:38:10Z jbraun $
 */

#ifndef DATACLASSES_EVENT_TANK_H_INCLUDED
#define DATACLASSES_EVENT_TANK_H_INCLUDED

#include <data-structures/iterator/FlatIterator.h>
#include <data-structures/event/ChannelEvent.h>

#include <hawcnest/processing/Bag.h>

#include <vector>

namespace evt {

/**
 * @class TankEvent
 * @author Segev BenZvi, Jim Braun
 * @ingroup event_data
 * @brief Container for Channels participating in an event
 * @todo This class may eventually contain summary data for all Channels in the
 *       tank to aid in tank-level triggers and reconstructions
 */

  class TankEvent : public Baggable {

    public:

      TankEvent() : tankId_(0) { }
      TankEvent(int tankId) : tankId_(tankId) { }

      int GetTankId() const {return tankId_;}

      typedef std::vector<ChannelEvent> ChannelEventList;
      typedef ChannelEventList::iterator ChannelIterator;
      typedef ChannelEventList::const_iterator ConstChannelIterator;

      ChannelIterator ChannelsBegin()
      { return channels_.begin(); }

      ChannelIterator ChannelsEnd()
      { return channels_.end(); }

      ConstChannelIterator ChannelsBegin() const
      { return channels_.begin(); }

      ConstChannelIterator ChannelsEnd() const
      { return channels_.end(); }

      /// Access policy to Hits via Channel objects
      class HitAccessPolicy {
        public:
          static ChannelEvent::HitIterator begin(ChannelEvent& ch)
          { return ch.HitsBegin(); }
          static ChannelEvent::HitIterator end(ChannelEvent& ch)
          { return ch.HitsEnd(); }
      };

      typedef flat_iterator<ChannelIterator,
                            ChannelEvent::HitIterator,
                            HitAccessPolicy> HitIterator;

      /// Read-write access to the start of the triggered Channel list
      HitIterator HitsBegin()
      { return HitIterator(ChannelsBegin(), ChannelsEnd()); }

      /// Read-write access to the end of the triggered Channel list
      HitIterator HitsEnd()
      { return HitIterator(ChannelsEnd(), ChannelsEnd()); }

      /// Check for the presence of a Channel in the TankEvent by ID
      bool HasChannel(const int channelId) const;

      /// Number of ChannelEvents
      size_t GetNChannels() const
      { return channels_.size(); }

      /// Number of Hits
      size_t GetNHits() const
      { return distance(HitsBegin(), HitsEnd()); }

      /// Access policy to constant Channels, via Tank objects
      class ConstHitAccessPolicy {
        public:
          static ChannelEvent::ConstHitIterator begin(const ChannelEvent& ch)
          { return ch.HitsBegin(); }
          static ChannelEvent::ConstHitIterator end(const ChannelEvent& ch)
          { return ch.HitsEnd(); }
      };

      typedef flat_iterator<ConstChannelIterator,
                            ChannelEvent::ConstHitIterator,
                            ConstHitAccessPolicy> ConstHitIterator;

      /// Read-only access to the start of the triggered Channel list
      ConstHitIterator HitsBegin() const
      { return ConstHitIterator(ChannelsBegin(), ChannelsEnd()); }

      /// Read-only access to the end of the triggered Channel list
      ConstHitIterator HitsEnd() const
      { return ConstHitIterator(ChannelsEnd(), ChannelsEnd()); }

      void AddChannel(const ChannelEvent& channel)
      { channels_.push_back(channel); }

      void AddHit(const Hit& hit);

    private:

      int tankId_;
      ChannelEventList channels_;
  };

  SHARED_POINTER_TYPEDEFS(TankEvent);

}

#endif // DATACLASSES_EVENT_TANK_H_INCLUDED

