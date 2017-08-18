/*!
 * @file ScalerEvent.cc 
 * @brief Top-level access to data from the scaler system.
 * @author Segev BenZvi 
 * @date 24 Jul 2013
 * @version $Id: ScalerEvent.cc 16416 2013-07-26 02:47:51Z sybenzvi $
 */

#include <data-structures/event/ScalerEvent.h>

#include <hawcnest/HAWCUnits.h>

using namespace std;

class ScalerChannelIdMatch : public binary_function<ScalerChannel, int, bool> {
  public:
    bool operator()(const ScalerChannel& c, const int id) const {
      return c.GetChannelId() == id;
    }
};

ScalerEvent::ScalerEvent() :
  startTime_(0),
  timeWidth_(10*HAWCUnits::millisecond)
{
  channels_.reserve(1200);
}

ScalerEvent&
ScalerEvent::operator+=(const ScalerEvent& s)
{
  for (ConstChannelIterator iC = s.ChannelsBegin(); iC != s.ChannelsEnd(); ++iC)
  {
    if (HasChannel(iC->GetChannelId())) {
      ScalerChannel& ch = GetChannel(iC->GetChannelId());
      ch.SetCount(ch.GetCount() + iC->GetCount());
    }
    else
      AddScalerChannel(*iC);
  }

  return *this;
}

bool
ScalerEvent::HasChannel(const int chId)
  const
{
  return find_if(ChannelsBegin(), ChannelsEnd(),
                 bind2nd(ScalerChannelIdMatch(), chId)) != ChannelsEnd();
}

void
ScalerEvent::AddScalerChannel(const ScalerChannel& c)
{
  if (HasChannel(c.GetChannelId())) {
    ScalerChannel& sc = GetChannel(c.GetChannelId());
    sc.SetCount(c.GetCount() + sc.GetCount());
  }
  else
    channels_.push_back(c);
}

const ScalerChannel&
ScalerEvent::GetChannel(const int chId)
  const
{
  ConstChannelIterator iCh = find_if(ChannelsBegin(), ChannelsEnd(),
                                     bind2nd(ScalerChannelIdMatch(), chId));
  if (iCh == ChannelsEnd())
    log_fatal("Could not find scaler data for channel " << chId);

  return *iCh;
}

ScalerChannel&
ScalerEvent::GetChannel(const int chId)
{
  ChannelIterator iCh = find_if(ChannelsBegin(), ChannelsEnd(),
                                bind2nd(ScalerChannelIdMatch(), chId));
  if (iCh == ChannelsEnd())
    log_fatal("Could not find scaler data for channel " << chId);

  return *iCh;
}

