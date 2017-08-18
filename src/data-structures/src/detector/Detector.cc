/*!
 * @file Detector.cc
 * @brief Front-end interface to the detector geometry and status.
 * @author Segev BenZvi
 * @date 19 Mar 2010
 * @version $Id: Detector.cc 27080 2015-10-02 17:36:35Z jbraun $
 */

#include <data-structures/detector/Detector.h>

#include <algorithm>
#include <sstream>
#include <stdexcept>

using namespace det;
using namespace std;

Detector::Detector() :
  chAccessArray_(maxChannelID_+1, (const Channel*)NULL),
  tkAccessArray_(maxTankID_+1, (const Tank*)NULL),
  isInitialized_(false),
  name_("HAWC"),
  arrayCenter_(0.,0.,0.)
{
}

Detector::Detector(const Detector& det) :
                      channels_(det.channels_),
                      chAccessArray_(maxChannelID_+1, (const Channel*)NULL),
                      tanks_(det.tanks_),
                      tkAccessArray_(maxTankID_+1, (const Tank*)NULL),
                      isInitialized_(false),
                      name_(det.name_),
                      sversion_(det.sversion_),
                      position_(det.position_),
                      arrayCenter_(det.arrayCenter_),
                      tankNameLookupMap_(det.tankNameLookupMap_),
                      channelNameLookupMap_(det.channelNameLookupMap_)
{
  if (det.isInitialized_) {
    InitializeDetector();
  }
}

Detector& Detector::operator=(Detector det) {

  // Copy and swap
  swap(channels_, det.channels_);
  swap(chAccessArray_, det.chAccessArray_);
  swap(tanks_, det.tanks_);
  swap(tkAccessArray_, det.tkAccessArray_);
  swap(isInitialized_, det.isInitialized_);
  swap(name_, det.name_);
  swap(sversion_, det.sversion_);
  swap(position_, det.position_);
  swap(arrayCenter_, det.arrayCenter_);
  swap(tankNameLookupMap_, det.tankNameLookupMap_);
  swap(channelNameLookupMap_, det.channelNameLookupMap_);
  return *this;
}

void
Detector::AddTank(const Tank& tank)
{
  if (isInitialized_)
    isInitialized_ = false;
  if (tank.GetTankId()>maxTankID_)
    log_fatal("Trying to add Tank ID great than allowed maximum: " <<
              maxTankID_);
  tanks_.push_back(tank);
  if (tank.HasName()) {
    if (tankNameLookupMap_.find(tank.GetName()) !=
                                   tankNameLookupMap_.end()) {
      log_fatal("Attempted to add tank with " <<
                           "duplicate name: " << tank.GetName());
    }
    tankNameLookupMap_[tank.GetName()] = tank.GetTankId();
  }
}

const Tank&
Detector::GetTank(const IdType& tankId)
  const
{

  if (tankId < maxTankID_ && tkAccessArray_[tankId])
    return *tkAccessArray_[tankId];

  log_fatal_nothrow("Unrecognized Tank ID: " << tankId);
  throw out_of_range("Unrecognized Tank ID");
}

const Tank&
Detector::GetTank(const std::string& tankName)
  const
{
  std::map<std::string, IdType>::const_iterator it =
                          tankNameLookupMap_.find(tankName);
  if (it == tankNameLookupMap_.end()) {
    log_fatal_nothrow("Unrecognized Tank name: " << tankName);
    throw out_of_range("Unrecognized Tank name");
  }
  return GetTank(it->second);
}

bool
Detector::HasTank(const std::string& tankName)
  const
{
  return (tankNameLookupMap_.find(tankName) != tankNameLookupMap_.end());
}

void
Detector::AddChannel(const Channel& ch)
{
  if (isInitialized_)
    isInitialized_ = false;
  if (ch.GetChannelId()>maxChannelID_)
  log_fatal("Trying to add Channel ID great than allowed maximum: " <<
            maxTankID_);
  channels_.push_back(ch);
  if (ch.HasName()) {
    if (channelNameLookupMap_.find(ch.GetName()) !=
                                   channelNameLookupMap_.end()) {
      log_fatal("Attempted to add channel with " <<
                                   "duplicate name: " << ch.GetName());
    }
    channelNameLookupMap_[ch.GetName()] = ch.GetChannelId();
  }
}

const Channel&
Detector::GetChannel(const IdType& chId)
  const
{
  if (chId < maxChannelID_ && chAccessArray_[chId])
    return *chAccessArray_[chId];

  log_fatal_nothrow("Unrecognized Channel ID: " << chId);
  throw out_of_range("Unrecognized Channel ID");
}

const Channel&
Detector::GetChannel(const std::string& chName)
  const
{
  std::map<std::string, IdType>::const_iterator it =
                          channelNameLookupMap_.find(chName);
  if (it == channelNameLookupMap_.end()) {
    log_fatal_nothrow("Unrecognized Channel name: " << chName);
    throw out_of_range("Unrecognized Channel name");
  }
  return GetChannel(it->second);
}

bool
Detector::HasChannel(const std::string& chName)
  const
{
  return (channelNameLookupMap_.find(chName) != channelNameLookupMap_.end());
}

void
Detector::InitializeDetector()
{
  if (!isInitialized_) {
    // Sort Channels and Tanks and fill the sparse array of Channel pointers
    sort(channels_.begin(), channels_.end());
    sort(tanks_.begin(), tanks_.end());
    ChannelIterator iC;
    for (iC = channels_.begin(); iC != channels_.end(); ++iC)
      chAccessArray_[iC->GetChannelId()] = &(*iC);

    // Assign the Channel iterator range to the internal state of each Tank
    iC = channels_.begin();
    bool incr = true;
    for (TankIterator iT = tanks_.begin(); iT != tanks_.end(); ++iT) {
      tkAccessArray_[iT->GetTankId()] = &(*iT);

      while (iC->GetTankId() < iT->GetTankId())
        ++iC;
      iT->chBegin_ = iC;

      do {
        iT->chEnd_ = ++iC;
        if (iC != channels_.end())
          incr = (iC->GetTankId() == iT->GetTankId());
        else
          incr = false;
      } while (incr);
    }
    isInitialized_ = true;
  }
}

