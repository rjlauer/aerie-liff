/*!
 * @file RTFilterResult.h
 * @brief Results from radius-time filtering process
 * @author Jim Braun
 * @version $Id: RTFilterResult.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_RT_FILTER_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RT_FILTER_RESULT_H_INCLUDED

#include <data-structures/event/Hit.h>

#include <hawcnest/HAWCNest.h>

#include <vector>

/*!
 * @class RTFilterResult
 * @author Jim Braun
 * @date 2 Aug 2013
 * @ingroup reconstruction
 * @brief Results from radius-time filtering process
 */

class RTFilterResult : public Baggable {

  public:

    class HitResult {

      public:

        double time_;
        double charge_;
        unsigned channelId_;
        unsigned tankId_;
        double timeInterval_;
        double radius_;
        unsigned multiplicity_;
    };

    RTFilterResult() { }

    void AddHit(const evt::Hit& hit, double radius,
                         double timeInterval, unsigned mult) {
      hitResults_.push_back(HitResult());
      hitResults_.back().time_ = hit.calibData_.time_;
      hitResults_.back().charge_ = hit.calibData_.PEs_;
      hitResults_.back().channelId_ = hit.channelId_;
      hitResults_.back().tankId_ = hit.tankId_;
      hitResults_.back().timeInterval_ = timeInterval;
      hitResults_.back().radius_ = radius;
      hitResults_.back().multiplicity_ = mult;
    }

    double GetNHitResults() const {return hitResults_.size();}

    std::vector<HitResult>::iterator HitResultsBegin() {
      return hitResults_.begin();
    }

    std::vector<HitResult>::iterator HitResultsEnd() {
      return hitResults_.end();
    }

    std::vector<HitResult>::const_iterator HitResultsBegin() const {
      return hitResults_.begin();
    }

    std::vector<HitResult>::const_iterator HitResultsEnd() const {
      return hitResults_.end();
    }

  private:

    std::vector<HitResult> hitResults_;
};

SHARED_POINTER_TYPEDEFS(RTFilterResult);

#endif // DATA_STRUCTURES_RT_FILTER_RESULT_H_INCLUDED
