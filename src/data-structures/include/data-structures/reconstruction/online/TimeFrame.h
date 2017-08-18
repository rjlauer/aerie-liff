/*!
 * @file TimeFrame.h 
 * @brief DAQ time frame.
 * @author Jim Braun
 * @version $Id: TimeFrame.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_RECO_ONLINE_TIME_FRAME_H_INCLUDED
#define DATA_STRUCTURES_RECO_ONLINE_TIME_FRAME_H_INCLUDED

#include <data-structures/reconstruction/online/TDCDataBlock.h>
#include <data-structures/reconstruction/online/TDCIdentifier.h>
#include <data-structures/iterator/FlatIterator.h>

#include <hawcnest/HAWCNest.h>
#include <hawcnest/processing/BaggableVector.h>

#include <vector>

/**
 * @class TimeFrame
 * @author Jim Braun
 * @date 31 Jul 2012
 * @ingroup online_reconstruction
 * @brief Representation of DAQ time frame, including header information
 * and a vector of data blocks
 */
class TimeFrame {

  public:

    virtual ~TimeFrame() { }

    TimeFrame() : sequenceID_(0xFFFFFFFF),
                  version_(0xFFFF),
                  startEventNumber_(0xFFFFFFFF),
                  stopEventNumber_(0xFFFFFFFF),
                  size_(0),
                  isValid_(false),
                  isFirst_(false),
                  isLast_(false) { }

    uint16_t GetSBCID() const {return tdcIdentifier_.GetSBCID();}
    uint16_t GetTDCGeoID() const {return tdcIdentifier_.GetTDCGeoID();}
    TDCIdentifier GetTDCIdentifier() const {return tdcIdentifier_;}
    uint32_t GetSequenceID() const {return sequenceID_;}
    uint16_t GetVersion() const {return version_;}
    uint32_t GetStartEventNumber() const {return startEventNumber_;}
    uint32_t GetStopEventNumber() const {return stopEventNumber_;}
    uint32_t GetSize() const {return size_;}
    bool IsValid() const {return isValid_;}
    bool IsLast() const {return isLast_;}
    bool IsFirst() const {return isFirst_;}

    void SetSBCID(const uint16_t sbcID) {tdcIdentifier_.SetSBCID(sbcID);}

    void SetTDCGeoID(const uint16_t tdcGeoID) {
      tdcIdentifier_.SetTDCGeoID(tdcGeoID);
    }

    void SetTDCIdentifier(const TDCIdentifier id) {tdcIdentifier_ = id;}
    void SetSequenceID(const uint32_t sequenceID) {sequenceID_ = sequenceID;}
    void SetVersion(const uint16_t version) {version_ = version;}

    void SetStartEventNumber(const uint32_t startEventNumber) {
      startEventNumber_ = startEventNumber;
    }

    void SetStopEventNumber(const uint32_t stopEventNumber) {
      stopEventNumber_ = stopEventNumber;
    }

    void SetSize(const uint32_t size) {size_ = size;}

    void SetValid(bool valid) {isValid_ = valid;}
    void SetLast(bool last) {isLast_ = last;}
    void SetFirst(bool first) {isFirst_ = first;}

    void AddTDCDataBlock(const TDCDataBlock& b) {dataBlocks_.push_back(b);}
    unsigned GetNTDCDataBlocks() const {return dataBlocks_.size();}

    typedef std::vector<TDCDataBlock>::const_iterator
                                                TDCDataBlockConstIterator;
    typedef std::vector<TDCDataBlock>::iterator TDCDataBlockIterator;

    TDCDataBlockConstIterator TDCDataBlocksBegin() const {
      return dataBlocks_.begin();
    }

    TDCDataBlockConstIterator TDCDataBlocksEnd() const {
      return dataBlocks_.end();
    }

    TDCDataBlockIterator TDCDataBlocksBegin() {return dataBlocks_.begin();}
    TDCDataBlockIterator TDCDataBlocksEnd() {return dataBlocks_.end();}

    TDCDataBlock& Front() {return dataBlocks_.front();}
    const TDCDataBlock& Front() const {return dataBlocks_.front();}

    TDCDataBlock& Back() {return dataBlocks_.back();}
    const TDCDataBlock& Back() const {return dataBlocks_.back();}

    /// Access policy to const HAWCTDCEvents via const TDCDataBlock objects
    class ConstTDCEventAccessPolicy {
      public:

        static TDCDataBlock::TDCEventConstIterator
        begin(const TDCDataBlock& block) {
          return block.EventsBegin();
        }

        static TDCDataBlock::TDCEventConstIterator
        end(const TDCDataBlock& block) {
          return block.EventsEnd();
        }
    };

    typedef flat_iterator<TDCDataBlockConstIterator,
                          TDCDataBlock::TDCEventConstIterator,
                          ConstTDCEventAccessPolicy> TDCEventConstIterator;

    TDCEventConstIterator TDCEventsBegin() const {
      return TDCEventConstIterator(TDCDataBlocksBegin(), TDCDataBlocksEnd());
    }

    TDCEventConstIterator TDCEventsEnd() const {
      return TDCEventConstIterator(TDCDataBlocksEnd(), TDCDataBlocksEnd());
    }

    /// Access policy to HAWCTDCEvents via TDCDataBlock objects
    class TDCEventAccessPolicy {
      public:

        static TDCDataBlock::TDCEventIterator begin(TDCDataBlock& block) {
          return block.EventsBegin();
        }

        static TDCDataBlock::TDCEventIterator end(TDCDataBlock& block) {
          return block.EventsEnd();
        }
    };

    typedef flat_iterator<TDCDataBlockIterator,
                          TDCDataBlock::TDCEventIterator,
                          TDCEventAccessPolicy> TDCEventIterator;

    TDCEventIterator TDCEventsBegin() {
      return TDCEventIterator(TDCDataBlocksBegin(), TDCDataBlocksEnd());
    }

    TDCEventIterator TDCEventsEnd() {
      return TDCEventIterator(TDCDataBlocksEnd(), TDCDataBlocksEnd());
    }

    /// Access policy to const HAWCTDCEvents via const TDCDataBlock objects
    class ConstTDCEventPtrAccessPolicy {
      public:

        static TDCDataBlock::TDCEventPtrConstIterator
        begin(const TDCDataBlock& block) {
          return block.EventPtrsBegin();
        }

        static TDCDataBlock::TDCEventPtrConstIterator
        end(const TDCDataBlock& block) {
          return block.EventPtrsEnd();
        }
     };

    typedef flat_iterator<TDCDataBlockConstIterator,
                      TDCDataBlock::TDCEventPtrConstIterator,
                      ConstTDCEventPtrAccessPolicy> TDCEventPtrConstIterator;

    TDCEventPtrConstIterator TDCEventPtrsBegin() const {
      return TDCEventPtrConstIterator(TDCDataBlocksBegin(), TDCDataBlocksEnd());
    }

    TDCEventPtrConstIterator TDCEventPtrsEnd() const {
      return TDCEventPtrConstIterator(TDCDataBlocksEnd(), TDCDataBlocksEnd());
    }

    /// Access policy to HAWCTDCEvents via TDCDataBlock objects
    class TDCEventPtrAccessPolicy {
      public:

        static TDCDataBlock::TDCEventPtrIterator begin(TDCDataBlock& block) {
          return block.EventPtrsBegin();
        }

        static TDCDataBlock::TDCEventPtrIterator end(TDCDataBlock& block) {
          return block.EventPtrsEnd();
        }
    };

    typedef flat_iterator<TDCDataBlockIterator,
                          TDCDataBlock::TDCEventPtrIterator,
                          TDCEventPtrAccessPolicy> TDCEventPtrIterator;

    TDCEventPtrIterator TDCEventPtrsBegin() {
      return TDCEventPtrIterator(TDCDataBlocksBegin(), TDCDataBlocksEnd());
    }

    TDCEventPtrIterator TDCEventPtrsEnd() {
      return TDCEventPtrIterator(TDCDataBlocksEnd(), TDCDataBlocksEnd());
    }

  private:

    TDCIdentifier tdcIdentifier_;
    uint32_t sequenceID_;
    uint16_t version_;
    uint32_t startEventNumber_;
    uint32_t stopEventNumber_;
    uint32_t size_;
    bool     isValid_;
    bool     isFirst_;
    bool     isLast_;

    std::vector<TDCDataBlock> dataBlocks_;
};

SHARED_POINTER_TYPEDEFS(TimeFrame);

typedef BaggableVector<TimeFrame> TimeFrameVector;
SHARED_POINTER_TYPEDEFS(TimeFrameVector);

#endif // DATA_STRUCTURES_RECO_ONLINE_TIME_FRAME_H_INCLUDED
