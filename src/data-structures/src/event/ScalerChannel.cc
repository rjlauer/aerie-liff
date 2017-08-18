/*!
 * @file ScalerChannel.cc
 * @brief Scaler count data in a physical channel.
 * @author Segev BenZvi
 * @version $Id: ScalerChannel.cc 16416 2013-07-26 02:47:51Z sybenzvi $
 */

#include <data-structures/event/ScalerChannel.h>

using namespace std;

ScalerChannel::ScalerChannel() :
  channelId_(0),
  tankId_(0),
  tankChannelId_(0),
  count_(0)
{ }

ScalerChannel::ScalerChannel(int chId, int tkId, int tkChId, double count) :
  channelId_(chId),
  tankId_(tkId),
  tankChannelId_(tkChId),
  count_(count)
{ }

