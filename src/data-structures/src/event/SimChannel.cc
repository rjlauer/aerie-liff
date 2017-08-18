/*!
 * @file SimChannel.cc 
 * @brief Storage of channel-level data from simulated events.
 * @author Segev BenZvi 
 * @date 27 Apr 2010 
 * @version $Id: SimChannel.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <data-structures/event/SimChannel.h>

SimChannel::SimChannel()
{
  photoElectrons_.reserve(500);
}

