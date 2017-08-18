/*!
 * @file TankEvent.cc 
 * @brief Event data from real events, at the tank level.
 * @author Jim Braun 
 * @date 18 Jul 2011 
 * @version $Id: TankEvent.cc 24135 2015-02-18 17:36:55Z jbraun $
 */

#include <data-structures/event/TankEvent.h>
#include <data-structures/event/ChannelEvent.h>
#include <data-structures/event/Hit.h>

using namespace evt;

class ChannelIdMatch : public std::binary_function<ChannelEvent, int, bool> {
  public:
    bool operator()(const ChannelEvent& p, const int id) const {
      return p.GetChannelId() == id;
    }
};

bool TankEvent::HasChannel(const int channelId) const {
  return find_if(ChannelsBegin(), ChannelsEnd(), bind2nd(ChannelIdMatch(), channelId))
    != ChannelsEnd();
}

void TankEvent::AddHit(const Hit& hit) {

  ChannelIterator iCh;
  if ((iCh = find_if(ChannelsBegin(), ChannelsEnd(),
                bind2nd(ChannelIdMatch(), hit.channelId_))) != ChannelsEnd()) {
    iCh->AddHit(hit);
  }
  else {
    channels_.push_back(ChannelEvent(hit.channelId_,
                                     hit.tankId_,
                                     hit.tankChannelId_));
    channels_.back().AddHit(hit);
  }
}
