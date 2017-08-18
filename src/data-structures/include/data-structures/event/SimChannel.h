/*!
 * @file SimChannel.h
 * @brief Simulated hit data in a channel.
 * @author Segev BenZvi
 * @ingroup event_data
 * @version $Id: SimChannel.h 18879 2014-02-19 21:41:29Z sybenzvi $
 */

#ifndef DATACLASSES_EVENT_SIMCHANNEL_H_INCLUDED
#define DATACLASSES_EVENT_SIMCHANNEL_H_INCLUDED

#include <data-structures/time/TimeStamp.h>
#include <data-structures/math/Trace.h>
#include <data-structures/math/DigitalLogic.h>
#include <vector>

/*!
 * @class PEHit
 * @author Segev BenZvi
 * @date 14 Apr 2010
 * @ingroup event_data
 * @brief Properties of a single simulated photoelectron hit: position, time,
 *        amplitude, etc.
 */
class PEHit : public Baggable {

  public:

    /// Photoelectron pulse amplitude
    float pulseAmplitude_;

    /// True PE time
    float hitTime_;

    /// Detection position on photocathode (expressed as a radius)
    float cathodeRadius_;

    /// Detection phi on photocathode (expressed as a angle)
    float cathodePhi_;

    /// Photon energy
    float photonEnergy_;

    /// Type of particle responsible for photon which generated PE
    int partType_;

    /// Comparison operator for hit sorting
    bool operator<(const PEHit& pe) const { return hitTime_ < pe.hitTime_; }

};

SHARED_POINTER_TYPEDEFS(PEHit);


/*!
 * @class PEList
 * @author Segev BenZvi
 * @date 6 Feb 2012
 * @ingroup event_data
 * @brief A sortable list of simulated photoelectron hits
 */
class PEList : public Baggable {

  private:

    typedef std::vector<PEHit> HitList;

  public:

    typedef HitList::iterator PEIterator;
    typedef HitList::const_iterator ConstPEIterator;

    PEList() : isSorted_(false) { }

    PEIterator PEsBegin() { return hits_.begin(); }
    PEIterator PEsEnd() { return hits_.end(); }

    ConstPEIterator PEsBegin() const { return hits_.begin(); }
    ConstPEIterator PEsEnd() const { return hits_.end(); }

    const int GetNPEs() const { return hits_.size(); }

    void AddPE(const PEHit& h) {
      hits_.push_back(h);
      isSorted_ = false;
    }

    void Sort() {
      if (!isSorted_) {
        sort(hits_.begin(), hits_.end());
        isSorted_ = true;
      }
    }

  private:

    HitList hits_;
    bool isSorted_;

};

SHARED_POINTER_TYPEDEFS(PEList);


/*!
 * @typedef SignalTrace
 * @date 7 Feb 2012
 * @ingroup event_data
 * @brief An evenly-binned signal trace (double precision) with 8192 data
 *        points.
 *
 * Note: the number of data points in the trace is a power of 2, which is
 * convenient for FFT algorithms.
 */
typedef Trace<double, (1<<13)> SignalTrace;

SHARED_POINTER_TYPEDEFS(SignalTrace);


/*!
 * @class SimChannel
 * @author Segev BenZvi
 * @date 6 Apr 2010
 * @ingroup event_data
 * @brief Storage of simulated hits in an optical module
 */
class SimChannel : public Baggable {

  public:

    typedef std::vector<PEHit> PEList;
    typedef PEList::iterator PEIterator;
    typedef PEList::const_iterator ConstPEIterator;

    SimChannel();

    PEIterator PEsBegin() { return photoElectrons_.begin(); }
    PEIterator PEsEnd() { return photoElectrons_.end(); }
    ConstPEIterator PEsBegin() const { return photoElectrons_.begin(); }
    ConstPEIterator PEsEnd() const { return photoElectrons_.end(); }
    const size_t GetNPEs() const { return photoElectrons_.size(); }
    void AddPE(const PEHit& pe) { photoElectrons_.push_back(pe); }

    void SortPEs()
    { std::sort(photoElectrons_.begin(), photoElectrons_.end()); }

    SignalTrace& GetTrace() { return trace_; }
    const SignalTrace& GetTrace() const { return trace_; }

    DigitalLogic::Pulse& GetLogicPulse() { return logicPulse_; }
    const DigitalLogic::Pulse& GetLogicPulse() const { return logicPulse_; }
    
    int GetChannelId() const { return channelId_; }

    float pmtTime_;

    int channelId_;      ///< Global Channel ID at the Detector level
    int tankId_;         ///< Tank ID
    int tankChannelId_;  ///< Channel ID within Tank

  private:

    PEList photoElectrons_;
    SignalTrace trace_;
    DigitalLogic::Pulse logicPulse_;

};

SHARED_POINTER_TYPEDEFS(SimChannel);

#endif // DATACLASSES_EVENT_SIMCHANNEL_H_INCLUDED

