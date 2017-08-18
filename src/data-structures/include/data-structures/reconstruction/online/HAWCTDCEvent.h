/*!
 * @file HAWCTDCEvent.h 
 * @brief An event from the TDC.
 * @author Jim Braun
 * @version $Id: HAWCTDCEvent.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_RECO_ONLINE_HAWC_TDC_EVENT_H_INCLUDED
#define DATA_STRUCTURES_RECO_ONLINE_HAWC_TDC_EVENT_H_INCLUDED

#include <data-structures/hardware/caen/TDCEvent.h>
#include <data-structures/reconstruction/online/TDCIdentifier.h>
#include <data-structures/reconstruction/online/HAWCTDCDAQ.h>
#include <data-structures/time/TimeStamp.h>

#include <hawcnest/HAWCNest.h>

/**
 * @class HAWCTDCEvent
 * @author Jim Braun
 * @date 2 Aug. 2012
 * @ingroup online_reconstruction
 * @brief caen::TDCEvent including time stamp and identifier
 */
class HAWCTDCEvent : public caen::TDCEvent {

  public:

    virtual ~HAWCTDCEvent() { }

    HAWCTDCEvent() : complete_(false) { }

    TDCIdentifier GetTDCIdentifier() const {return tdcIdentifier_;}
    uint16_t GetSBCID() const {return tdcIdentifier_.GetSBCID();}
    uint16_t GetTDCGeoID() const {return tdcIdentifier_.GetTDCGeoID();}

    void SetTDCIdentifier(const TDCIdentifier id) {tdcIdentifier_ = id;}
    void SetSBCID(const uint16_t sbcID) {tdcIdentifier_.SetSBCID(sbcID);}

    void SetTDCGeoID(const uint16_t tdcGeoID) {
      tdcIdentifier_.SetTDCGeoID(tdcGeoID);
    }

    const TimeStamp& GetCoarseTimeStamp() const {return coarseTimeStamp_;}
    TimeStamp& GetCoarseTimeStamp() {return coarseTimeStamp_;}

    void SetCoarseTimeStamp(const uint64_t daqTimeStamp) {
      coarseTimeStamp_ = ConvertDAQTimeStamp(daqTimeStamp);
    }

    void SetCoarseTimeStamp(const TimeStamp& timeStamp) {
      coarseTimeStamp_ = timeStamp;
    }

    bool IsComplete() const {return complete_;}
    void SetComplete(bool complete = true) {complete_ = complete;}

  private:

    TimeStamp coarseTimeStamp_;
    TDCIdentifier tdcIdentifier_;
    bool complete_;
};

SHARED_POINTER_TYPEDEFS(HAWCTDCEvent);

#endif // DATA_STRUCTURES_RECO_ONLINE_HAWC_TDC_EVENT_H_INCLUDED
