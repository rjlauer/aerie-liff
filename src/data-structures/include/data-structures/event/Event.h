/*!
 * @file Event.h
 * @brief Event data interface.
 * @author Segev BenZvi
 * @ingroup event_data
 * @version $Id: Event.h 24130 2015-02-18 15:44:35Z jbraun $
 */

#ifndef DATACLASSES_EVENT_EVENT_H_INCLUDED
#define DATACLASSES_EVENT_EVENT_H_INCLUDED

#include <data-structures/event/TankEvent.h>
#include <data-structures/event/ChannelEvent.h>
#include <data-structures/event/EventFlags.h>
#include <data-structures/event/GTCFlags.h>
#include <data-structures/time/TimeStamp.h>

#include <hawcnest/processing/Bag.h>

#include <stdint.h>

namespace evt {

  /*!
   * @class Event
   * @author Jim Braun
   * @author Segev BenZvi
   * @ingroup event_data
   * @brief Top-level structure for triggered and calibrated event data.
   * 
   * The Event class contains a nested hierarchy of TankEvent, ChannelEvent,
   * and Hit data.  Event --> TankEvent --> ChannelEvent --> Hit
   * Iterators are provided to make a "triple loop" over tanks, Channels, and
   * Hits.  Alternatively, a flat_iterator can be used to loop directly over
   * Hits from the top level.
   */
  class Event : public Baggable {

    public:

      Event() : eventID_(0),
                runID_(0),
                timeSliceID_(0),
                triggerFlags_(0x0),
                eventFlags_(0x0),
                gtcFlags_(0x0),
                laserTStart_(LASER_DATA_UNSET),
                laserTStop_(LASER_DATA_UNSET),
                laserLightToTanksStart_(LASER_DATA_UNSET),
                laserLightToTanksStop_(LASER_DATA_UNSET) {tanks_.reserve(350);}

      typedef std::vector<TankEvent> TankList;
      typedef TankList::iterator TankIterator;
      typedef TankList::const_iterator ConstTankIterator;

      /// Read-write access to the start of the triggered tank list
      TankIterator TanksBegin()
      { return tanks_.begin(); }

      /// Read-write access to the end of the triggered Tank list
      TankIterator TanksEnd()
      { return tanks_.end(); }

      /// Read-only access to the start of the triggered tank list
      ConstTankIterator TanksBegin() const
      { return tanks_.begin(); }

      /// Read-only access to the end of the triggered Tank list
      ConstTankIterator TanksEnd() const
      { return tanks_.end(); }

      /// Number of Tanks participating in the Event
      size_t GetNTanks() const
      { return tanks_.size(); }

      /// Access policy to Channels via Tank objects
      class ChannelAccessPolicy {
        public:
          static TankEvent::ChannelIterator begin(TankEvent& t)
          { return t.ChannelsBegin(); }
          static TankEvent::ChannelIterator end(TankEvent& t)
          { return t.ChannelsEnd(); }
      };

      typedef flat_iterator<TankIterator,
                            TankEvent::ChannelIterator,
                            ChannelAccessPolicy> ChannelIterator;

      /// Read-write access to the start of the triggered Channel list
      ChannelIterator ChannelsBegin()
      { return ChannelIterator(TanksBegin(), TanksEnd()); }

      /// Read-write access to the end of the triggered Channel list
      ChannelIterator ChannelsEnd()
      { return ChannelIterator(TanksEnd(), TanksEnd()); }

      /// Number of ChannelEvents
      size_t GetNChannels() const
      { return distance(ChannelsBegin(), ChannelsEnd()); }

      /// Access policy to constant Channels, via Tank objects
      class ConstChannelAccessPolicy {
        public:
          static TankEvent::ConstChannelIterator begin(const TankEvent& t)
          { return t.ChannelsBegin(); }
          static TankEvent::ConstChannelIterator end(const TankEvent& t)
          { return t.ChannelsEnd(); }
      };

      typedef flat_iterator<ConstTankIterator,
                            TankEvent::ConstChannelIterator,
                            ConstChannelAccessPolicy> ConstChannelIterator;

      /// Read-only access to the start of the triggered Channel list
      ConstChannelIterator ChannelsBegin() const
      { return ConstChannelIterator(TanksBegin(), TanksEnd()); }

      /// Read-only access to the end of the triggered Channel list
      ConstChannelIterator ChannelsEnd() const
      { return ConstChannelIterator(TanksEnd(), TanksEnd()); }


      // Hit-level access using nested flat_iterators
      typedef flat_iterator<ChannelIterator,
                            ChannelEvent::HitIterator,
                            TankEvent::HitAccessPolicy> HitIterator;

      /// Read-write access to the start of the triggered Channel list
      HitIterator HitsBegin()
      { return HitIterator(ChannelsBegin(), ChannelsEnd()); }

      /// Read-write access to the end of the triggered Channel list
      HitIterator HitsEnd()
      { return HitIterator(ChannelsEnd(), ChannelsEnd()); }

      typedef flat_iterator<ConstChannelIterator,
                            ChannelEvent::ConstHitIterator,
                            TankEvent::ConstHitAccessPolicy> ConstHitIterator;

      /// Read-only access to the start of the Hit list
      ConstHitIterator HitsBegin() const
      { return ConstHitIterator(ChannelsBegin(), ChannelsEnd()); }

      /// Read-only access to the end of the Hit list
      ConstHitIterator HitsEnd() const
      { return ConstHitIterator(ChannelsEnd(), ChannelsEnd()); }

      /// Number of Hits
      size_t GetNHits() const
      { return distance(HitsBegin(), HitsEnd()); }

      /// Add a Hit to the event (inserts into the proper Tank & Channel)
      void AddHit(const Hit& hit);

      /// Add a Channel to the event (inserts into the proper Tank)
      void AddChannel(const ChannelEvent& channel);

      /// Add a tank to the list of tanks
      void AddTank(const TankEvent& tank) { tanks_.push_back(tank); }

      /// Check for the presence of a tank in the tank list by ID
      bool HasTank(const int tankId) const;

      /// Access evt::TankEvent object by its tank ID
      const TankEvent& GetTank(const int tankId) const;
      
      /// Check for the presence of a Channel in the event by ID
      bool HasChannel(const int channelId) const;

      /// Access evt::ChannelEvent object by its channel ID
      const ChannelEvent& GetChannel(const int channelId) const;

      /// The event global run ID
      int GetRunID() const {return runID_;}
      void SetRunID(const int runID) {runID_ = runID;}

      /*
       * The event time slice ID.  Unique event identification:
       * RunID --> TimeSliceID --> EventID
       */
      int GetTimeSliceID() const {return timeSliceID_;}
      void SetTimeSliceID(const int timeSliceID) {timeSliceID_ = timeSliceID;}

      /// The event flags
      uint16_t GetEventFlags() const {return eventFlags_;}
      void SetEventFlags(const uint16_t flags) {eventFlags_ = flags;}

      /// The trigger flags
      uint16_t GetTriggerFlags() const {return triggerFlags_;}
      void SetTriggerFlags(const uint16_t flags) {triggerFlags_ = flags;}

      /// The GTC flags
      uint64_t GetGTCFlags() const {return gtcFlags_;}
      void SetGTCFlags(const uint64_t gtcFlags) {gtcFlags_ = gtcFlags;}

      /// The event ID within the run
      int GetEventID() const {return eventID_;}
      void SetEventID(const int eventID) {eventID_ = eventID;}

      /// Event time stamp, corresponding to the start of the trigger window
      TimeStamp GetTime() const {return time_;}
      void SetTime(const TimeStamp time) {time_ = time;}

      /// Parse info from event flags
      bool IsBadEvent() const {return eventFlags_ & BAD_EVENT;}
      bool IsCalibrationEvent() const {return eventFlags_ & CALIBRATION_EVENT;}
      bool HasLightInDetector() const {return eventFlags_ & LIGHT_IN_DETECTOR;}
      bool IsMinBiasTriggerEvent() const {
        return triggerFlags_ & MIN_BIAS_TRIGGER;
      }
      bool IsSMTEvent() const {
        return triggerFlags_ & SIMPLE_MULTIPLICITY_TRIGGER;
      }
      bool IsTankTriggerEvent() const {
        return triggerFlags_ & TANK_MULTIPLICITY_TRIGGER;
      }
      bool IsMuonTriggerEvent() const {
        return triggerFlags_ & MUON_TRIGGER;
      }
      bool IsFPGATriggerEvent() const {
        return triggerFlags_ & FPGA_TRIGGER;
      }
      bool IsCalibrationTriggerEvent() const {
        return triggerFlags_ & CALIBRATION_TRIGGER;
      }
      bool IsCleanCalibrationEvent() const {
        return triggerFlags_ == CALIBRATION_TRIGGER;
      }
      bool IsTimeFromGTC() {
        return gtcFlags_ & evt::GPS_GTC_TIME_IS_USED;
      }

      /// Set event flags info
      void SetBadEvent() {eventFlags_ |= BAD_EVENT;}
      void SetCalibrationEvent() {eventFlags_ |= CALIBRATION_EVENT;}
      void SetLightInDetector() {eventFlags_ |= LIGHT_IN_DETECTOR;}
      void SetMinBiasTriggerEvent() {triggerFlags_ |= MIN_BIAS_TRIGGER;}
      void SetSMTEvent() {triggerFlags_ |= SIMPLE_MULTIPLICITY_TRIGGER;}
      void SetTankTriggerEvent() {triggerFlags_ |= TANK_MULTIPLICITY_TRIGGER;}
      void SetMuonTriggerEvent() {triggerFlags_ |= MUON_TRIGGER;}

      bool HasCalibrationData() {
        return laserTStart_ != LASER_DATA_UNSET &&
               laserTStop_ != LASER_DATA_UNSET;
      }

      /// Set the Laser TStart signal, in TDC units offset from the trigger
      void SetLaserTStart(const int32_t tStart) {laserTStart_ = tStart;}

      /// Get the Laser TStart signal, in TDC units offset from the trigger
      int32_t GetLaserTStart() const {return laserTStart_;}

      /// Set the Laser TStop signal, in TDC units offset from the trigger
      void SetLaserTStop(const int32_t tStop) {laserTStop_ = tStop;}

      /// Get the Laser TStop signal, in TDC units offset from the trigger
      int32_t GetLaserTStop() const {return laserTStop_;}

      /// Set the Laser LTT start signal, in TDC units offset from the trigger
      void SetLaserLightToTanksStart(const int32_t lttStart) {
        laserLightToTanksStart_ = lttStart;
      }

      /// Get the Laser LTT start signal, in TDC units offset from the trigger
      int32_t GetLaserLightToTanksStart() const {
        return laserLightToTanksStart_;
      }

      /// Set the Laser LTT stop signal, in TDC units offset from the trigger
      void SetLaserLightToTanksStop(const int32_t lttStop) {
        laserLightToTanksStop_ = lttStop;
      }

      /// Get the Laser LTT stop signal, in TDC units offset from the trigger
      int32_t GetLaserLightToTanksStop() const {return laserLightToTanksStop_;}

    private:

      TankList tanks_;

      TimeStamp time_;
      int eventID_;
      int runID_;
      int timeSliceID_;
      uint16_t triggerFlags_;
      uint16_t eventFlags_;
      uint64_t gtcFlags_;

      int32_t laserTStart_;
      int32_t laserTStop_;
      int32_t laserLightToTanksStart_;
      int32_t laserLightToTanksStop_;

      static const int32_t LASER_DATA_UNSET = -100000;
  };

  SHARED_POINTER_TYPEDEFS(Event);

}

#endif // DATACLASSES_EVENT_EVENT_H_INCLUDED
