/*!
 * @file SimTank.h
 * @brief Simulated event data from a single water Cherenkov detector.
 * @author Segev BenZvi
 * @ingroup event_data
 * @version $Id: SimTank.h 18879 2014-02-19 21:41:29Z sybenzvi $
 */

#ifndef DATACLASSES_EVENT_SIMTANK_H_INCLUDED
#define DATACLASSES_EVENT_SIMTANK_H_INCLUDED

#include <data-structures/event/SimChannel.h>
#include <data-structures/event/SimParticle.h>

#include <hawcnest/PointerTypedefs.h>

#include <vector>

/**
 * @class SimTank
 * @author Segev BenZvi
 * @ingroup event_data
 * @brief Container for Channels participating in an event
 * @todo This class may eventually contain summary data for all Channels in the
 *       Tank to aid in tank-level triggers and reconstructions
 */
class SimTank : public Baggable {

  public:

    typedef std::vector<SimChannel> SimChannelList;
    typedef SimChannelList::iterator ChannelIterator;
    typedef SimChannelList::const_iterator ConstChannelIterator;

    typedef std::vector<SimParticle> SimParticleList;
    typedef SimParticleList::iterator ParticleIterator;
    typedef SimParticleList::const_iterator ConstParticleIterator;

    SimTank();
    SimTank(int id);

    /// Get Tank ID
    int GetId() const { return ID_; }

    ChannelIterator ChannelsBegin() { return pmts_.begin(); }
    ChannelIterator ChannelsEnd() { return pmts_.end(); }
    ConstChannelIterator ChannelsBegin() const { return pmts_.begin(); }
    ConstChannelIterator ChannelsEnd() const { return pmts_.end(); }
    void AddSimChannel(const SimChannel& pmt) { pmts_.push_back(pmt); }

    ParticleIterator ParticlesBegin() { return pcls_.begin(); }
    ParticleIterator ParticlesEnd() { return pcls_.end(); }
    ConstParticleIterator ParticlesBegin() const { return pcls_.begin(); }
    ConstParticleIterator ParticlesEnd() const { return pcls_.end(); }
    void AddSimParticle(const SimParticle& pcl) { pcls_.push_back(pcl); }

  private:

    int ID_;
    SimChannelList pmts_;
    SimParticleList pcls_;

    friend class SimEvent;

};

SHARED_POINTER_TYPEDEFS(SimTank);

#endif // DATACLASSES_EVENT_SIMTANK_H_INCLUDED

