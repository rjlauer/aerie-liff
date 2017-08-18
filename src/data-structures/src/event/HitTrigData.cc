/*!
 * @file HitTrigData.cc 
 * @brief Implementation of hit data from the detector after triggering.
 * @author Segev BenZvi 
 * @date 2 Sep 2011 
 * @version $Id: HitTrigData.cc 22636 2014-11-10 21:08:23Z joshwood $
 */

#include <data-structures/event/HitTrigData.h>

using namespace evt;

Edge::Edge(const HitTrigData& htd, const uint8_t edgeId) :
  edgeId_(edgeId)
{
  switch (edgeId_) {
    case 0:
    {
      edgeTm_ = htd.time_;
      break;
    }
    case 1:
    {
      edgeTm_ = htd.time_ + htd.time01_;
      break;
    }
    case 2:
    {
      edgeTm_ = htd.time_ + htd.hiTOT_ + htd.time01_;
      break;
    }
    case 3:
    {
      edgeTm_ = htd.time_ + htd.loTOT_;
      break;
    }
    default:
    {
      edgeTm_ = 0;
      break;
    }
  }
}

