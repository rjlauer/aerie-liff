/*!
 * @file MergedTDCEvent.h 
 * @brief A vector of TDCEvent objects from one TDC hardware trigger.
 * @author Jim Braun
 * @version $Id: MergedTDCEvent.h 17766 2013-11-01 04:23:51Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_RECO_ONLINE_MERGED_TDC_EVENT_H_INCLUDED
#define DATA_STRUCTURES_RECO_ONLINE_MERGED_TDC_EVENT_H_INCLUDED

#include <data-structures/reconstruction/online/TDCIdentifier.h>
#include <data-structures/reconstruction/online/HAWCTDCEvent.h>

#include <hawcnest/HAWCNest.h>
#include <hawcnest/processing/BaggableVector.h>

#include <boost/iterator/indirect_iterator.hpp>

#include <stdint.h>
#include <vector>
#include <utility>
#include <algorithm>

/*!
 * @class MergedTDCEvent
 * @author Jim Braun
 * @date 31 Jul 2012
 * @ingroup online_reconstruction
 * @brief Representation of a vector of TDCEvent objects that correspond to
 * the same TDC hardware trigger.
 */

class MergedTDCEvent {

  public:

    virtual ~MergedTDCEvent() { }

    MergedTDCEvent() : timeFrameSequenceID_(0xFFFFFFFF),
                       gtcEdgeTime_(0),
                       gtcErrors_(0),
                       isActive_(false) { }

    uint32_t GetTimeFrameSequenceID() const {return timeFrameSequenceID_;}

    TimeStamp GetMatchWindowStartTimeStamp() const {
      return matchWindowStartTimeStamp_;
    }

    TimeStamp GetTriggerTimeStamp() const {
      return triggerTimeStamp_;
    }

    uint64_t GetGTCErrors() const {return gtcErrors_;}
    bool HasGTCErrors() const {return gtcErrors_;}

    bool HasGTCTimeStamp() const {
      return matchWindowStartTimeStamp_ != TimeStamp();
    }

    bool IsActive() const {return isActive_;}

    void SetTimeFrameSequenceID(const uint32_t timeFrameSequenceID) {
      timeFrameSequenceID_ = timeFrameSequenceID;
    }

    /*!
     *  Set the GTC match window start timestamp.  This procedure will
     *  require a const_cast in an AERIE loop if the GTC code is in a
     *  separate AERIE module.
     */
    void SetMatchWindowStartTimeStamp(const TimeStamp& ts) {
      matchWindowStartTimeStamp_ = ts;
    }

    /*!
     *  Set the GTC trigger timestamp.  This procedure will
     *  require a const_cast in an AERIE loop if the GTC code is in a
     *  separate AERIE module.
     */
    void SetTriggerTimeStamp(const TimeStamp& ts) {
      triggerTimeStamp_ = ts;
    }

    uint32_t GetGTCEdgeTime() const {return gtcEdgeTime_;}

    /*!
     *  Set the raw time of the GTC leading edge.  This procedure will
     *  require a const_cast in an AERIE loop if the GTC code is in a
     *  separate AERIE module.
     */
    void SetGTCEdgeTime(uint32_t edgeTime) {gtcEdgeTime_ = edgeTime;}

    /*!
     *  Set the GTC errors.  This procedure will require a
     *  const_cast in an AERIE loop if the GTC code is in a
     *  separate AERIE module.
     */
    void SetGTCErrors(const uint64_t gtcErrors) {
      gtcErrors_ = gtcErrors;
    }

    void SetActive(const bool active = true) {isActive_ = active;}

    unsigned GetNEvents() const {return events_.size();}

    /// Add data from a TDC
    void AddEvent(const HAWCTDCEvent& e) {
      events_.push_back(boost::make_shared<HAWCTDCEvent>(e));
    }

    /// Add data from a TDC wrapped in a shared_ptr
    void AddEvent(const HAWCTDCEventPtr& e) {
      events_.push_back(e);
    }

    typedef boost::indirect_iterator<
         std::vector<HAWCTDCEventPtr>::const_iterator> TDCEventConstIterator;
    typedef boost::indirect_iterator<
         std::vector<HAWCTDCEventPtr>::iterator> TDCEventIterator;

    /// Iterator to data from the the first TDC
    TDCEventConstIterator EventsBegin() const {
      return boost::make_indirect_iterator(events_.begin());
    }

    /// Iterator to the end of the TDC list
    TDCEventConstIterator EventsEnd() const {
      return boost::make_indirect_iterator(events_.end());
    }

    /// Iterator to data from the the first TDC
    TDCEventIterator EventsBegin() {
      return boost::make_indirect_iterator(events_.begin());
    }

    /// Iterator to the end of the TDC list
    TDCEventIterator EventsEnd() {
      return boost::make_indirect_iterator(events_.end());
    }

    typedef std::vector<HAWCTDCEventPtr>::const_iterator
                                                   TDCEventPtrConstIterator;
    typedef std::vector<HAWCTDCEventPtr>::iterator TDCEventPtrIterator;

    TDCEventPtrConstIterator EventPtrsBegin() const {return events_.begin();}
    TDCEventPtrConstIterator EventPtrsEnd() const {return events_.end();}

    TDCEventPtrIterator EventPtrsBegin() {return events_.begin();}
    TDCEventPtrIterator EventPtrsEnd() {return events_.end();}

    /// Reference to the first TDC contained in the event
    HAWCTDCEvent& Front() {return *(events_.front());}
    const HAWCTDCEvent& Front() const {return *(events_.front());}

    /// Reference to the last TDC contained in the event
    HAWCTDCEvent& Back() {return *(events_.back());}
    const HAWCTDCEvent& Back() const {return *(events_.back());}

    /*!
     * Get data from a specific TDC by TDCIdentifier.  Throw exception
     * if identifier is not found
     */
    const HAWCTDCEvent& GetTDCEvent(const TDCIdentifier& id) const {
      TDCEventConstIterator it =
          find_if(EventsBegin(), EventsEnd(), TDCIdentifierMatch(id));
      if (it == EventsEnd()) {
        log_fatal("No TDC event from SBC: " <<
            id.GetSBCID() << ", Geo: " << id.GetTDCGeoID());
      }
      return *it;
    }

    /*!
     * Does the event contain data from the
     * TDC identified by the given TDCIdentifier
     */
    bool HasTDCEvent(const TDCIdentifier& id) const {
      return find_if(EventsBegin(), EventsEnd(),
                     TDCIdentifierMatch(id)) != EventsEnd();
    }

  private:

    class TDCIdentifierMatch {

      public:

        TDCIdentifierMatch(const TDCIdentifier& id) : id_(id) { }

        bool operator()(const HAWCTDCEvent& event) const {
          return event.GetTDCIdentifier() == id_;
        }

      private:

        const TDCIdentifier& id_;
    };

    uint32_t timeFrameSequenceID_;

    TimeStamp matchWindowStartTimeStamp_;
    TimeStamp triggerTimeStamp_;
    uint32_t  gtcEdgeTime_;
    uint64_t gtcErrors_;

    bool isActive_;

    std::vector<HAWCTDCEventPtr> events_;
};

SHARED_POINTER_TYPEDEFS(MergedTDCEvent);

typedef BaggableVector<MergedTDCEvent> MergedTDCEventVector;
SHARED_POINTER_TYPEDEFS(MergedTDCEventVector);

#endif // DATA_STRUCTURES_RECO_ONLINE_MERGED_TDC_EVENT_H_INCLUDED
