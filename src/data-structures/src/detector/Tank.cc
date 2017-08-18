/*!
 * @file Tank.cc 
 * @brief Geometrical properties of a physical water tank.
 * @author Segev BenZvi 
 * @date 19 Mar 2010 
 * @version $Id: Tank.cc 18303 2013-12-21 00:55:00Z sybenzvi $
 */

#include <data-structures/detector/Tank.h>
#include <hawcnest/HAWCUnits.h>

using namespace det;
using namespace HAWCUnits;

Tank::Tank() :
  tankId_(0),
  tankType_(HAWC_WCD),
  position_(0, 0, 0),
  diameter_(7.3 * meter),
  height_(5 * meter),
  waterHeight_(4.35 * meter),
  chargeFactor_(1.),
  name_(UNKNOWN_TANK_NAME)
{
}

Tank::Tank(const IdType& tankId, const Point& pos) :
  tankId_(tankId),
  tankType_(HAWC_WCD),
  position_(pos),
  diameter_(7.3 * meter),
  height_(5 * meter),
  waterHeight_(4.35 * meter),
  chargeFactor_(1.),
  name_(UNKNOWN_TANK_NAME)
{
}

Tank::Tank(const IdType& tankId,
           const TankType& tankType,
           std::string tankName,
           const Point& pos) :

  tankId_(tankId),
  tankType_(tankType),
  position_(pos),
  diameter_(7.3 * meter),
  height_(5 * meter),
  waterHeight_(4.35 * meter),
  chargeFactor_(1.),
  name_(tankName)
{
}

size_t
Tank::GetNChannels()
  const
{
  return distance(chBegin_, chEnd_);
}

