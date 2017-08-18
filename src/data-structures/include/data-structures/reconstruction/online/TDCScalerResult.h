/*!
 * @file TDCScalerResult.h 
 * @author Jim Braun
 * @date 19 Nov 2012 
 * @version $Id: TDCScalerResult.h 14879 2013-04-27 16:21:17Z sybenzvi $
*/

#ifndef DATA_STRUCTURES_RECO_ONLINE_TDC_SCALER_RESULT_INCLUDED_H
#define DATA_STRUCTURES_RECO_ONLINE_TDC_SCALER_RESULT_INCLUDED_H

#include <data-structures/detector/Detector.h>
#include <data-structures/time/TimeStamp.h>
#include <data-structures/time/ModifiedJulianDate.h>
#include <data-structures/time/UTCDateTime.h>

#include <hawcnest/RegisterService.h>
#include <hawcnest/HAWCUnits.h>
#include <hawcnest/HAWCNest.h>
#include <hawcnest/processing/BaggableVector.h>

#include <map>
#include <stdint.h>

class TDCScalerResult : public Baggable {

  public:

    TDCScalerResult(double intTime,
                    const TimeStamp& ts,
                    double timeWindow) :
                             intTime_(intTime),
                             ts_(ts),
                             timeWindow_(timeWindow) { }

    unsigned GetGPSSecond() const {return ts_.GetGPSSecond();}
    unsigned GetGPSNanoSecond() const {return ts_.GetGPSNanoSecond();}
    unsigned GetMJD() const {
      ModifiedJulianDate mjd(ts_);
      return static_cast<unsigned>(mjd.GetDate() / HAWCUnits::day);
    }
    unsigned GetUTCSecond() const {
      UTCDateTime udt(ts_);
      return udt.GetSecond() + 60*udt.GetMinute() + 3600*udt.GetHour();
    }
    double GetIntegrationTime() const {return intTime_;}

    const TimeStamp& GetTimeStamp() const {return ts_;}

    double GetTimeWindow() const {return timeWindow_;}

    std::map<unsigned, uint64_t>::const_iterator
    ChannelsBegin() const {return map_.begin();}

    std::map<unsigned, uint64_t>::const_iterator
    ChannelsEnd() const {return map_.end();}

    size_t GetNChannels() const {return map_.size();}

    void AddHit(unsigned gridID) {
      std::map<unsigned, uint64_t>::iterator it = map_.find(gridID);
      if (it == map_.end()) {
        map_[gridID] = 1;
      } else {
        it->second++;
      }
    }


    class MultiplicityCountSet {

      public:

        MultiplicityCountSet() : counts_(ALLOC_MAX_MULTIPLICITY, 0) { }

        void AddCount(unsigned multiplicity) {
          if (multiplicity >= counts_.size()) {
            counts_.resize(multiplicity + 1, 0);
          }
          ++(counts_[multiplicity]);
        }

        unsigned GetMaxMultiplicity() {return counts_.size() - 1;}

        uint64_t GetCount(unsigned multiplicity) const {
          if (multiplicity >= counts_.size()) {
            return 0;
          }
          return counts_[multiplicity];
        }

      private:

        static const unsigned ALLOC_MAX_MULTIPLICITY = 10;
        std::vector<uint64_t> counts_;
    };

    typedef std::map<unsigned, MultiplicityCountSet> TankMap;

    TankMap::const_iterator
    TanksBegin() const {return multiplicityMap_.begin();}

    TankMap::const_iterator
    TanksEnd() const {return multiplicityMap_.end();}

    size_t GetNTanks() const {return multiplicityMap_.size();}

    void AddTank(unsigned tankID, unsigned multiplicity) {
      multiplicityMap_[tankID].AddCount(multiplicity);
    }

  private:

    double intTime_;
    TimeStamp ts_;
    double timeWindow_;

    std::map<unsigned, uint64_t> map_;

    TankMap multiplicityMap_;
};

SHARED_POINTER_TYPEDEFS(TDCScalerResult);

typedef BaggableVector<TDCScalerResult> TDCScalerResultVector;
SHARED_POINTER_TYPEDEFS(TDCScalerResultVector);

typedef BaggableVector<TDCScalerResultConstPtr> TDCScalerResultPtrVector;
SHARED_POINTER_TYPEDEFS(TDCScalerResultPtrVector);

#endif // DATA_STRUCTURES_RECO_ONLINE_TDC_SCALER_RESULT_INCLUDED_H
