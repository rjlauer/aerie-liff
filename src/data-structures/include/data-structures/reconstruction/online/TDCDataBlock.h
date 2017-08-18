/*!
 * @file TDCDataBlock.h 
 * @brief Representation of one block of TDC data.
 * @author Jim Braun
 * @version $Id: TDCDataBlock.h 17766 2013-11-01 04:23:51Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_RECO_ONLINE_TDC_DATA_BLOCK_H_INCLUDED
#define DATA_STRUCTURES_RECO_ONLINE_TDC_DATA_BLOCK_H_INCLUDED

#include <data-structures/reconstruction/online/HAWCTDCEvent.h>
#include <data-structures/reconstruction/online/HAWCTDCDAQ.h>
#include <data-structures/time/TimeStamp.h>

#include <hawcnest/HAWCNest.h>

#include <boost/iterator/indirect_iterator.hpp>

#include <stdint.h>
#include <vector>
#include <ctime>

/*!
 * @class TDCDataBlock
 * @author Jim Braun
 * @date 31 Jul 2012
 * @ingroup online_reconstruction
 * @brief Representation of one block of TDC data read from the TDC DAQ
 */

class TDCDataBlock {

  public:

    virtual ~TDCDataBlock() { }

    TDCDataBlock() : id_(0xFFFFFFFF),
                     startEventNumber_(0xFFFFFFFF),
                     stopEventNumber_(0xFFFFFFFF),
                     size_(0),
                     complete_(false) { }


    uint32_t GetID() const {return id_;}
    uint32_t GetStartEventNumber() const {return startEventNumber_;}
    uint32_t GetStopEventNumber() const {return stopEventNumber_;}
    uint32_t GetSize() const {return size_;}
    const TimeStamp& GetTimeStamp() const {return timeStamp_;}
    TimeStamp& GetTimeStamp() {return timeStamp_;}

    void SetID(const uint32_t id) {id_ = id;}

    void SetStartEventNumber(const uint32_t startEventNumber) {
      startEventNumber_ = startEventNumber;
    }

    void SetStopEventNumber(const uint32_t stopEventNumber) {
      stopEventNumber_ = stopEventNumber;
    }

    void SetSize(const uint32_t size) {size_ = size;}

    void SetTimeStamp(const uint64_t daqTimeStamp) {
      timeStamp_ = ConvertDAQTimeStamp(daqTimeStamp);
    }

    void SetTimeStamp(const TimeStamp& timeStamp) {timeStamp_ = timeStamp;}

    void AddEvent(const HAWCTDCEvent& e) {
      events_.push_back(boost::make_shared<HAWCTDCEvent>(e));
    }

    void AddEvent(const HAWCTDCEventPtr& e) {
      events_.push_back(e);
    }

    unsigned GetNEvents() const {return events_.size();}

    typedef boost::indirect_iterator<
         std::vector<HAWCTDCEventPtr>::const_iterator> TDCEventConstIterator;
    typedef boost::indirect_iterator<
         std::vector<HAWCTDCEventPtr>::iterator> TDCEventIterator;

    TDCEventConstIterator EventsBegin() const {
      return boost::make_indirect_iterator(events_.begin());
    }

    TDCEventConstIterator EventsEnd() const {
      return boost::make_indirect_iterator(events_.end());
    }

    TDCEventIterator EventsBegin() {
      return boost::make_indirect_iterator(events_.begin());
    }

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

    HAWCTDCEvent& Front() {return *(events_.front());}
    const HAWCTDCEvent& Front() const {return *(events_.front());}

    HAWCTDCEvent& Back() {return *(events_.back());}
    const HAWCTDCEvent& Back() const {return *(events_.back());}

    bool IsComplete() const {return complete_;}
    void SetComplete(bool complete = true) {complete_ = complete;}

  private:

    uint32_t id_;
    uint32_t startEventNumber_;
    uint32_t stopEventNumber_;
    uint32_t size_;
    TimeStamp timeStamp_;

    bool complete_;

    std::vector<HAWCTDCEventPtr> events_;
};

SHARED_POINTER_TYPEDEFS(TDCDataBlock);

#endif // DATA_STRUCTURES_RECO_ONLINE_TDC_DATA_BLOCK_H_INCLUDED
