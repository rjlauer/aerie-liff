/*!
 * @file SimEvent.cc 
 * @brief Top-level access to data from simulated events.
 * @author Segev BenZvi 
 * @date 27 Apr 2010 
 * @version $Id: SimEvent.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <data-structures/event/SimEvent.h>
#include <stdexcept>

using namespace std;

class SimTankIdMatch : public binary_function<SimTank, int, bool> {
  public:
    bool operator()(const SimTank& t, const int id) const {
      return t.GetId() == id;
    }
};

SimEvent::SimEvent()
{
  tanks_.reserve(300);
}

bool
SimEvent::HasSimTank(const int tankId)
  const
{
  return find_if(TanksBegin(), TanksEnd(), bind2nd(SimTankIdMatch(), tankId))
    != TanksEnd();
}

//SimTank&
//SimEvent::GetSimTank(const int tankId)
//{
//  TankIterator iTank;
//  if ((iTank = 
//      find_if(TanksBegin(), TanksEnd(), bind2nd(SimTankIdMatch(), tankId)))
//      != TanksEnd())
//  {
//    return *iTank;
//  }
//
//  throw out_of_range("Unrecognized tank id");
//}

size_t
SimEvent::GetNSimChannels()
  const
{
  return distance(ChannelsBegin(), ChannelsEnd());
}

void
SimEvent::AddSimChannel(const SimChannel& pmt)
{
  TankIterator iTank;
  if ((iTank = find_if(TanksBegin(), TanksEnd(),
                 bind2nd(SimTankIdMatch(), pmt.tankId_))) != TanksEnd()) {
    iTank->AddSimChannel(pmt);
  }
  else {
    SimTank tank(pmt.tankId_);
    tank.AddSimChannel(pmt);
    AddSimTank(tank);
  }
}

size_t
SimEvent::GetNParticles()
  const
{
  return distance(ParticlesBegin(), ParticlesEnd());
}

void
SimEvent::AddSimParticle(const int tankId, const SimParticle& pcl)
{
  TankIterator iTank;
  if ((iTank = find_if(TanksBegin(), TanksEnd(),
                 bind2nd(SimTankIdMatch(), tankId))) != TanksEnd()) {
    iTank->AddSimParticle(pcl);
  }
  else {
    SimTank tank(tankId);
    tank.AddSimParticle(pcl);
    AddSimTank(tank);
  }
}

