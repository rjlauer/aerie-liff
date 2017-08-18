/*!
 * @file SimEvent.h
 * @brief Simulated event data access classes.
 * @author Segev BenZvi
 * @ingroup event_data
 * @version $Id: SimEvent.h 18879 2014-02-19 21:41:29Z sybenzvi $
 */

#ifndef DATACLASSES_EVENT_SIMEVENT_H_INCLUDED
#define DATACLASSES_EVENT_SIMEVENT_H_INCLUDED

#include <data-structures/iterator/FlatIterator.h>
#include <data-structures/event/SimTank.h>
#include <data-structures/event/SimEventHeader.h>

#include <hawcnest/PointerTypedefs.h>

/*!
 * @class SimEvent
 * @author Segev BenZvi
 * @ingroup event_data
 * @brief Storage of simulated event data in a hierarchical structure.
 *
 * Like the Event class, this structure stores data in a double hierarchy of
 * SimTank and SimChannel objects.  A flat_iterator over Channels is provided for
 * fast tube-wise access.  Each SimTank also stores a SimParticle list, and a
 * direct iterator over ground particles is also provided.
 */
class SimEvent : public Baggable {

  public:

    typedef std::vector<SimTank> TankList;
    typedef TankList::iterator TankIterator;
    typedef TankList::const_iterator ConstTankIterator;

    SimEvent();

    /// Read the event header
    const SimEventHeader& GetEventHeader() const { return header_; }

    /// Set the event header
    void SetEventHeader(const SimEventHeader& sh) { header_ = sh; }

    /// Read-write access to the start of the triggered tank list
    TankIterator TanksBegin() { return tanks_.begin(); }

    /// Read-write access to the end of the triggered Tank list
    TankIterator TanksEnd() { return tanks_.end(); }

    /// Read-only access to the start of the triggered tank list
    ConstTankIterator TanksBegin() const { return tanks_.begin(); }

    /// Read-only access to the end of the triggered Tank list
    ConstTankIterator TanksEnd() const { return tanks_.end(); }

    /// Add a tank with simulated data to the list of tanks
    void AddSimTank(const SimTank& tank) { tanks_.push_back(tank); }

    /// Check for the presence of a tank in the tank list by ID
    bool HasSimTank(const int tankId) const;

//    /// Access a tank by reference
//    SimTank& GetSimTank(const int tankId);

    /// Number of tanks participating in the simulated event
    size_t GetNSimTanks() const { return tanks_.size(); }

    /// Access policy to SimChannels via Tank objects
    class ChannelAccessPolicy {
      public:
        static SimTank::ChannelIterator begin(SimTank& t)
        { return t.ChannelsBegin(); }
        static SimTank::ChannelIterator end(SimTank& t)
        { return t.ChannelsEnd(); }
    };

    typedef flat_iterator<TankIterator, SimTank::ChannelIterator,
      ChannelAccessPolicy> ChannelIterator;

    /// Read-write access to the start of the triggered Channel list
    ChannelIterator ChannelsBegin()
    { return ChannelIterator(TanksBegin(), TanksEnd()); }

    /// Read-write access to the end of the triggered Channel list
    ChannelIterator ChannelsEnd()
    { return ChannelIterator(TanksEnd(), TanksEnd()); }

    /// Access policy to constant Channels, via Tank objects
    class ConstChannelAccessPolicy {
      public:
        static SimTank::ConstChannelIterator begin(const SimTank& t)
        { return t.ChannelsBegin(); }
        static SimTank::ConstChannelIterator end(const SimTank& t)
        { return t.ChannelsEnd(); }
    };

    typedef flat_iterator<ConstTankIterator, SimTank::ConstChannelIterator,
      ConstChannelAccessPolicy> ConstChannelIterator;

    /// Read-only access to the start of the triggered Channel list
    ConstChannelIterator ChannelsBegin() const
    { return ConstChannelIterator(TanksBegin(), TanksEnd()); }

    /// Read-only access to the end of the triggered Channel list
    ConstChannelIterator ChannelsEnd() const
    { return ConstChannelIterator(TanksEnd(), TanksEnd()); }

    /// Number of Channels participating in the simulated event
    size_t GetNSimChannels() const;

    /// Add a Channel to the event (inserts into the proper SimTank)
    void AddSimChannel(const SimChannel& pmt);

    /// Get the total number of PEs in the SimEvent
    size_t GetNSimPE() const {
      size_t npe = 0;
      for (ConstChannelIterator it = ChannelsBegin();
                                it != ChannelsEnd(); ++it) {
        npe += it->GetNPEs();
      }
      return npe;
    }

    /// Access policy to SimParticles via Tank objects
    class ParticleAccessPolicy {
      public:
        static SimTank::ParticleIterator begin(SimTank& t)
        { return t.ParticlesBegin(); }
        static SimTank::ParticleIterator end(SimTank& t)
        { return t.ParticlesEnd(); }
    };

    typedef flat_iterator<TankIterator, SimTank::ParticleIterator,
      ParticleAccessPolicy> ParticleIterator;

    /// Read-write access to the start of the Particle list
    ParticleIterator ParticlesBegin()
    { return ParticleIterator(TanksBegin(), TanksEnd()); }

    /// Read-write access to the end of the Particle list
    ParticleIterator ParticlesEnd()
    { return ParticleIterator(TanksEnd(), TanksEnd()); }

    /// Access policy to constant Particles, via Tank objects
    class ConstParticleAccessPolicy {
      public:
        static SimTank::ConstParticleIterator begin(const SimTank& t)
        { return t.ParticlesBegin(); }
        static SimTank::ConstParticleIterator end(const SimTank& t)
        { return t.ParticlesEnd(); }
    };

    typedef flat_iterator<ConstTankIterator, SimTank::ConstParticleIterator,
      ConstParticleAccessPolicy> ConstParticleIterator;

    /// Read-only access to the start of the Particle list
    ConstParticleIterator ParticlesBegin() const
    { return ConstParticleIterator(TanksBegin(), TanksEnd()); }

    /// Read-only access to the end of the Particle list
    ConstParticleIterator ParticlesEnd() const
    { return ConstParticleIterator(TanksEnd(), TanksEnd()); }

    /// Number of ground particles produced by the event
    size_t GetNParticles() const;

    /// Add a simulated particle to the event (inserts into proper SimTank)
    void AddSimParticle(const int tankId, const SimParticle& pcl);

  private:

    TankList tanks_;
    SimEventHeader header_;

};

SHARED_POINTER_TYPEDEFS(SimEvent);

#endif // DATACLASSES_EVENT_SIMEVENT_H_INCLUDED

