/*!
 * @file Hit.h
 * @brief Pulse data from an Event.
 * @author Jim Braun
 * @ingroup event_data
 * @version $Id: Hit.h 24165 2015-02-19 20:28:36Z jbraun $
 */

#ifndef DATACLASSES_EVENT_HIT_H_INCLUDED
#define DATACLASSES_EVENT_HIT_H_INCLUDED

#include <data-structures/event/HitTrigData.h>
#include <data-structures/event/HitCalData.h>

#include <hawcnest/processing/Bag.h>

#include <vector>

namespace evt {

  /*!
   * @class Hit
   * @author Jim Braun
   * @ingroup event_data
   * @brief Event data from a pulse on a given Channel.
   */
  class Hit : public Baggable {

    public:

      typedef std::vector<Edge> EdgeList;
      typedef EdgeList::const_iterator ConstEdgeIterator;

      Hit() { }

      HitTrigData triggerData_;
      HitCalData calibData_;

      /// Include tankID for easy access to tank info when iterating over hits
      int channelId_;           ///< Global Channel ID [1..(Ntk x Nch)]
      int tankId_;              ///< Parent Tank ID [1..Ntk]
      int tankChannelId_;       ///< Channel ID in Tank [1..Nch]

      bool operator==(const Hit& hit) const {

        return (channelId_     == hit.channelId_ &&
                tankId_        == hit.tankId_ &&
                tankChannelId_ == hit.tankChannelId_ &&
                triggerData_   == hit.triggerData_ &&
                calibData_     == hit.calibData_);
      }

      bool operator<(const Hit& hit) const {
        return triggerData_ < hit.triggerData_;
      }

      ConstEdgeIterator EdgesBegin() const {
        SetEdges();
        return edges_.begin();
      }

      ConstEdgeIterator EdgesEnd() const {
        SetEdges();
        return edges_.end();
      }

    private:

      mutable EdgeList edges_;

      void SetEdges() const {
        if (edges_.empty()) {
          int idMax = triggerData_.IsFourEdge() ? 4 : 2;
          for (int i = 0; i < idMax; ++i)
            edges_.push_back(Edge(triggerData_, i));
        }
      }

  }; // Hit

  SHARED_POINTER_TYPEDEFS(Hit);

  // Define ordering based on calibrated time
  class HitCalibTimeCompare {

    public:
      bool operator()(const Hit& h1, const Hit& h2) {
        return h1.calibData_.time_ < h2.calibData_.time_;
      }
  };

  // Define ordering based on raw time
  class HitRawTimeCompare {

    public:
      bool operator()(const Hit& h1, const Hit& h2) {
        return h1.triggerData_.time_ < h2.triggerData_.time_;
      }
  };

  // Define ordering based on charge
  class HitChargeCompare {

    public:
      bool operator()(const Hit& h1, const Hit& h2) {
        return h1.calibData_.PEs_ < h2.calibData_.PEs_;
      }
  };

}

#endif // DATACLASSES_EVENT_HIT_H_INCLUDED

