/*!
 * @file Event.cc 
 * @brief Top-level access to data from real events.
 * @author Jim Braun 
 * @date 11 May 2010 
 * @version $Id: Event.cc 24135 2015-02-18 17:36:55Z jbraun $
 */

#include <data-structures/event/Event.h>
#include <data-structures/event/TankEvent.h>
#include <data-structures/event/ChannelEvent.h>
#include <data-structures/event/Hit.h>

using namespace evt;

class TankIdMatch : public std::binary_function<TankEvent, int, bool> {
  public:
    bool operator()(const TankEvent& t, const int id) const {
      return t.GetTankId() == id;
    }
};

class ChannelIdMatch : public std::binary_function<ChannelEvent, int, bool> {
  public:
    bool operator()(const ChannelEvent& p, const int id) const {
      return p.GetChannelId() == id;
    }
};

bool Event::HasTank(const int tankId) const {
  return find_if(TanksBegin(), TanksEnd(), bind2nd(TankIdMatch(), tankId))
    != TanksEnd();
}

const TankEvent&
Event::GetTank(const int tkId)
  const
{
  ConstTankIterator iTk =
    find_if(TanksBegin(), TanksEnd(), bind2nd(TankIdMatch(), tkId));
  if (iTk == TanksEnd())
    log_fatal("Tank " << tkId << " not found in event.");
  return *iTk;
}

bool Event::HasChannel(const int channelId) const {
  return find_if(ChannelsBegin(), ChannelsEnd(), bind2nd(ChannelIdMatch(), channelId))
    != ChannelsEnd();
}

const ChannelEvent&
Event::GetChannel(const int chId)
  const
{
  ConstChannelIterator iCh =
    find_if(ChannelsBegin(), ChannelsEnd(), bind2nd(ChannelIdMatch(), chId));
  if (iCh == ChannelsEnd())
    log_fatal("Channel " << chId << " not found in event.");
  return *iCh;
}

void Event::AddChannel(const ChannelEvent& ch) {

  TankIterator iTank;
  if ((iTank = find_if(TanksBegin(), TanksEnd(),
                 bind2nd(TankIdMatch(), ch.GetTankId()))) != TanksEnd()) {
    iTank->AddChannel(ch);
  }
  else {
    tanks_.push_back(TankEvent(ch.GetTankId()));
    tanks_.back().AddChannel(ch);
  }
}

void Event::AddHit(const Hit& hit) {

  TankIterator iTank;
  if ((iTank = find_if(TanksBegin(), TanksEnd(),
                bind2nd(TankIdMatch(), hit.tankId_))) != TanksEnd()) {
    iTank->AddHit(hit);
  }
  else {
    tanks_.push_back(TankEvent(hit.tankId_));
    tanks_.back().AddHit(hit);
  }
}
