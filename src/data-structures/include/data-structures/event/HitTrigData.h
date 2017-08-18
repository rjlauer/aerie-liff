/*!
 * @file HitTrigData.h
 * @brief "Raw" pulse data in a Hit.
 * @author Segev BenZvi
 * @ingroup event_data
 * @version $Id: HitTrigData.h 23409 2014-12-19 22:35:38Z joshwood $
 */

#ifndef DATACLASSES_EVENT_HITTRIGDATA_H_INCLUDED
#define DATACLASSES_EVENT_HITTRIGDATA_H_INCLUDED

#include <hawcnest/processing/Bag.h>
#include <data-structures/event/HitFlags.h>

namespace evt {

  /*!
   * @class HitTrigData
   * @author Segev BenZvi, Jim Braun
   * @date 6 Apr 2010
   * @ingroup event_data
   * @brief Storage for TDC edges from a "raw" hit.  Integer
   *        values are the resolution of the TDC clock.
   */
  class HitTrigData : public Baggable {

    public:

      HitTrigData() : time_(0x7FFFFFFF),
                      time01_(0),
                      loTOT_(0),
                      hiTOT_(0),
                      flags_(0x0),
                      triggerFlags_(0x0) { }

      int32_t  time_;         ///< Time of leading edge, relative to event time
      uint16_t time01_;       ///< Time difference between leading and next edge
      uint16_t loTOT_;        ///< Time above low disc.  Same as t01 for 2-edge
      uint16_t hiTOT_;        ///< Time above high discriminator; 0 for 2-edge
      uint16_t flags_;        ///< Flags marking the raw hit (e.g. afterpulsing)
      uint16_t triggerFlags_; ///< Flags indicating presence in the trigger

      bool IsFourEdge() const
      { return hiTOT_ > 0; }

      int GetTime() const
      { return time_; }

      uint16_t GetTime01() const
      { return time01_; }

      uint16_t GetLoTOT() const
      { return loTOT_; }

      uint16_t GetHiTOT() const
      { return hiTOT_; }

      uint16_t GetFlags() const
      { return flags_; }

      uint16_t GetTriggerFlags() const
      { return triggerFlags_; }

      bool IsInvalidTOT() const
      { return flags_ & INVALID_TOT; }

      bool IsAfterpulseVeto() const
      { return flags_ & AFTERPULSE_VETO; }

      bool IsAmbiguous() const
      { return flags_ & AMBIGUOUS_EDGE_RESULT; }

      bool IsSixEdge() const
      { return flags_ & SIX_EDGE; }

      bool IsPEPromoted() const 
      { return flags_ & PE_PROMOTED; }

      bool IsChannelKilled() const
      { return flags_ & KILL_CHANNEL; }

      bool IsSimulatedNoise() const 
      { return flags_ & SIMULATED_NOISE; }

      bool IsHitKilled() const
      { return flags_ & evt::KILL_HIT; }

      bool IsOutsideTriggerCut() const 
      { return flags_ & evt::OUTSIDE_TRIGGER_CUT; }

      bool HasPriorHit() const 
      { return flags_ & evt::PRIOR_HIT; }

      bool operator==(const HitTrigData& hitTrigData) const {
        return (time_         == hitTrigData.time_ &&
                time01_       == hitTrigData.time01_ &&
                loTOT_        == hitTrigData.loTOT_ &&
                hiTOT_        == hitTrigData.hiTOT_ &&
                flags_        == hitTrigData.flags_ &&
                triggerFlags_ == hitTrigData.triggerFlags_);
      }

      bool operator<(const HitTrigData& hitTrigData) const
      { return time_ < hitTrigData.time_; }

  };

  SHARED_POINTER_TYPEDEFS(HitTrigData);

  /*!
   * @class Edge
   * @author Segev BenZvi
   * @date 2 Sep 2011
   * @ingroup event_data
   * @brief Simple class for representing a single edge transition in a "raw"
   *        TDC hit.
   */
  class Edge {

    public:

      Edge() : edgeTm_(0), edgeId_(0) { }

      Edge(const HitTrigData& td, const uint8_t edgeId);

      void SetEdgeId(const uint8_t id) { edgeId_ = id; }

      int32_t GetTime() const { return edgeTm_; }
      bool IsLeading() const { return !(edgeId_ % 2); }
      bool IsTrailing() const { return edgeId_ % 2; }

      bool operator==(const Edge& e) const
      { return (edgeTm_ == e.edgeTm_) && (edgeId_ == e.edgeId_); }

      bool operator!=(const Edge& e) const
      { return !(*this == e); }

    private:

      int32_t edgeTm_;
      uint8_t edgeId_;

  }; 

  SHARED_POINTER_TYPEDEFS(Edge);

}

#endif // DATACLASSES_EVENT_HITTRIGDATA_H_INCLUDED

