/*!
 * @file ChannelEvent.h
 * @brief Channel-based event data.
 * @author Segev BenZvi
 * @ingroup event_data
 * @date 6 Apr 2010
 * @version $Id: ChannelEvent.h 24278 2015-02-27 17:38:10Z jbraun $
 */

#ifndef DATACLASSES_EVENT_CHANNEL_H_INCLUDED
#define DATACLASSES_EVENT_CHANNEL_H_INCLUDED

#include <data-structures/event/Hit.h>
#include <data-structures/iterator/FlatIterator.h>

#include <hawcnest/processing/Bag.h>

#include <algorithm>

namespace evt {

  /*!
   * @class ChannelEvent
   * @author Segev BenZvi, Jim Braun
   * @ingroup event_data
   * @date 6 Apr 2010
   * @brief Event data from a single Channel; provides access to trigger
   *        and calibration data from the event
   */
  class ChannelEvent : public Baggable {

    public:

      ChannelEvent() : channelId_(0),
                       tankId_(0),
                       tankChannelId_(0),
                       hasL1Err_(false),
                       hasFIFOErr_(false) { }

      ChannelEvent(int channelId, int tankId, int tankChannelId) :
                       channelId_(channelId),
                       tankId_(tankId),
                       tankChannelId_(tankChannelId),
                       hasL1Err_(false),
                       hasFIFOErr_(false) { }

      int GetChannelId() const { return channelId_; }
      int GetTankId() const { return tankId_; }
      int GetTankChannelId() const { return tankChannelId_; }

      /// Return true if L1 overflow flag is set for this channel's TDC group
      bool HasL1Error() const { return hasL1Err_; }
      void SetL1Error() { hasL1Err_ = true; }

      /// Return true if FIFO overflow flag is set for this channel's TDC group
      bool HasFIFOError() const { return hasFIFOErr_; }
      void SetFIFOError() { hasFIFOErr_ = true; }

      typedef std::vector<Hit> HitList;
      typedef HitList::iterator HitIterator;
      typedef HitList::const_iterator ConstHitIterator;

      HitIterator HitsBegin() { return hits_.begin(); }
      HitIterator HitsEnd()   { return hits_.end(); }

      ConstHitIterator HitsBegin() const { return hits_.begin(); }
      ConstHitIterator HitsEnd() const   { return hits_.end(); }

      /// Does the channel have a hit matching Selection
      template <typename Selection>
      bool HasHit(const Selection& sel) const {
        return std::find_if(hits_.begin(), hits_.end(), sel) != hits_.end();
      }

      void AddHit(const Hit& hit)
      {
        HitIterator i = std::lower_bound(hits_.begin(), hits_.end(), hit);
        hits_.insert(i, hit);
      }

      /// Access policy to Edges via Hit objects
      class ConstEdgeAccessPolicy {
        public:
          static Hit::ConstEdgeIterator begin(const Hit& t)
          { return t.EdgesBegin(); }
          static Hit::ConstEdgeIterator end(const Hit& t)
          { return t.EdgesEnd(); }
      };

      typedef flat_iterator<ConstHitIterator,
                            Hit::ConstEdgeIterator,
                            ConstEdgeAccessPolicy> ConstEdgeIterator;

      /// Access to the start of the Edge list
      ConstEdgeIterator EdgesBegin() const
      { return ConstEdgeIterator(HitsBegin(), HitsEnd()); }

      /// Access to the end of the Edge list
      ConstEdgeIterator EdgesEnd() const
      { return ConstEdgeIterator(HitsEnd(), HitsEnd()); }

    private:

      int channelId_;      ///< Global Channel ID [1..999]
      int tankId_;         ///< Parent Tank ID [1..300]
      int tankChannelId_;  ///< Channel ID in Tank [1..3]

      bool hasL1Err_;      ///< TDC L1 error flag set for this channel's group
      bool hasFIFOErr_;    ///< TDC FIFO err flag set for this channel's group

      HitList hits_;

  };

  SHARED_POINTER_TYPEDEFS(ChannelEvent);

}

#endif // DATACLASSES_EVENT_CHANNEL_H_INCLUDED

