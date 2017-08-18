/*!
 * @file Channel.cc 
 * @brief Geometrical and electronic properties of a physical channel.
 * @author Segev BenZvi 
 * @date 19 Mar 2010 
 * @version $Id: Channel.cc 25716 2015-06-08 15:36:23Z srfontaine $
 */

#include <data-structures/detector/Channel.h>

#include <data-structures/physics/PhysicsConstants.h>

using namespace std;
using namespace det;
using namespace HAWCUnits;

Channel::Channel() :
  chId_(0),
  tankId_(0),
  tcId_(0),
  pmtType_(R5912),
  pmt_(R5912),
  tankType_(HAWC_WCD),
  position_(0, 0, 0),
  rotX_(0),
  rotY_(0),
  rotZ_(0),

  cab_("RG-59", 500*foot, 75*ohm, 21.1*pF/foot, 0.65*PhysicsConstants::c,
       3.4/*dB*/ / (100*foot), 100*MHz),

  // FEB properties.
  // Note: the threshold of the low-voltage channel has been set to 1/4 PE.
  //       In the simulation of a one-PE pulse propagating through 500 feet
  //       of RG-59 without any detector noise, the 1 PE peak is 28.4 mV.
  //       Hence, 1/4 PE = 7.1 mV.
  feb_(66*nF, 75*ohm, 7*nV / sqrt(Hz),
       AnalogCard(ABChannel(1210*ohm, 47*pF, 0.02381*ampere/volt, 7.1*mV),
                  ABChannel(680*ohm, 100*pF, .005882*ampere/volt, 80*mV))),

  name_(UNKNOWN_CHANNEL_NAME)
{
}

Channel::Channel(const IdType& chId, const IdType& tankId, const IdType& tcId,
                 const Point& pos,
                 const double rX, const double rY, const double rZ) :
  chId_(chId),
  tankId_(tankId),
  tcId_(tcId),
  pmtType_(R5912),
  pmt_(R5912),
  tankType_(HAWC_WCD),
  position_(pos),
  rotX_(rX),
  rotY_(rY),
  rotZ_(rZ),

  cab_("RG-59", 500*foot, 75*ohm, 21.1*pF/foot, 0.65*PhysicsConstants::c,
       3.4/*dB*/ / (100*foot), 100*MHz),

  // FEB properties.
  // Note: the threshold of the low-voltage channel has been set to 1/4 PE.
  //       In the simulation of a one-PE pulse propagating through 500 feet
  //       of RG-59 without any detector noise, the 1 PE peak is 28.4 mV.
  //       Hence, 1/4 PE = 7.1 mV.
  feb_(66*nF, 75*ohm, 7*nV / sqrt(Hz),
       AnalogCard(ABChannel(1210*ohm, 47*pF, 0.02381*ampere/volt, 7.1*mV),
                  ABChannel(680*ohm, 100*pF, .005882*ampere/volt, 80*mV))),

  name_(UNKNOWN_CHANNEL_NAME)
{
}

Channel::Channel(const IdType& chId, const IdType& tankId, const IdType& tcId,
                 const PMTType& pmtType, const std::string& chName,
                 const TankType& tankType, const Point& pos,
                 const double rX, const double rY, const double rZ) :
  chId_(chId),
  tankId_(tankId),
  tcId_(tcId),
  pmtType_(pmtType),
  pmt_(pmtType),
  tankType_(tankType),
  position_(pos),
  rotX_(rX),
  rotY_(rY),
  rotZ_(rZ),

  cab_("RG-59", 500*foot, 75*ohm, 21.1*pF/foot, 0.65*PhysicsConstants::c,
       3.4/*dB*/ / (100*foot), 100*MHz),

  // FEB properties.
  // Note: the threshold of the low-voltage channel has been set to 1/4 PE.
  //       In the simulation of a one-PE pulse propagating through 500 feet
  //       of RG-59 without any detector noise, the 1 PE peak is 28.4 mV.
  //       Hence, 1/4 PE = 7.1 mV.
  feb_(66*nF, 75*ohm, 7*nV / sqrt(Hz),
       AnalogCard(ABChannel(1210*ohm, 47*pF, 0.02381*ampere/volt, 7.1*mV),
                  ABChannel(680*ohm, 100*pF, .005882*ampere/volt, 80*mV))),

  name_(chName)
{
}


