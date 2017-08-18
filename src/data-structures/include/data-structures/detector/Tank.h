/*!
 * @file Tank.h
 * @brief Geometrical properties of the water Cherenkov detectors.
 * @author Segev BenZvi
 * @version $Id: Tank.h 18303 2013-12-21 00:55:00Z sybenzvi $
 */

#ifndef DATACLASSES_DETECTOR_TANK_H_INCLUDED
#define DATACLASSES_DETECTOR_TANK_H_INCLUDED

#include <data-structures/detector/Channel.h>

#include <vector>
#include <string>

namespace det {

  /// Default name for a Tank object when name is not provided
  static const std::string UNKNOWN_TANK_NAME = "Unknown";

  class Detector;

  /**
   * @class Tank
   * @author S.Y. BenZvi
   * @date 19 Mar 2010
   * @ingroup detector_props
   * @brief Physical representation of Tanks
   * @todo Create a Bladder class to store dynamic tank data (e.g.,
   *       calibrations and water levels)?
   */
  class Tank {

    public:

      typedef std::vector<Channel>::iterator ChannelIterator;
      typedef std::vector<Channel>::const_iterator ConstChannelIterator;

      Tank();

      Tank(const IdType& tankId, const Point& pos);

      Tank(const IdType& tankId,
           const TankType& tankType,
           std::string tankName,
           const Point& pos);

      virtual ~Tank() { }

      /// Get global tank ID
      const IdType& GetTankId() const { return tankId_; }

      /// Get global tank ID
      const TankType& GetTankType() const { return tankType_; }

      /// Read-write access to the start of the Channel list
      ChannelIterator ChannelsBegin() { return chBegin_; }

      /// Read-write access to the end of the Channel list
      ChannelIterator ChannelsEnd() { return chEnd_; }

      /// Read-only access to the start of the Channel list
      ConstChannelIterator ChannelsBegin() const { return chBegin_; }

      /// Read-only access to the end of the Channel list
      ConstChannelIterator ChannelsEnd() const { return chEnd_; }

      /// Get the number of Channels in the tank
      size_t GetNChannels() const;

      /// Return the tank position in detector coordinates
      const Point& GetPosition() const { return position_; }

      /// Tank diameter
      float GetDiameter() const { return diameter_; }

      /// Tank height
      float GetHeight() const { return height_; }

      /// Height of water within tank
      float GetWaterHeight() const { return waterHeight_; }

      /// Charge * chargeFactor_ is the equivalent charge that would be
      /// observed by a PMT in a standard HAWC tank given a Cherenkov
      /// spectrum.  So smaller chargeFactor_ --> more sensitive tank.
      /// NOTE:  This factor depends on the physical process we're
      /// considering, e.g. muons vs EM air showers.  Assume for here
      /// that it is based on EM air showers.
      double GetChargeFactor() const { return chargeFactor_; }

      /// Comparison; used to store Tanks in a sorted list
      bool operator<(const Tank& tank) const { return tankId_ < tank.tankId_; }

      /// Get the name of the tank, e.g. E18
      const std::string& GetName() const { return name_; }

      /// Does the Tank have an assigned name?
      bool HasName() const { return name_.compare(UNKNOWN_TANK_NAME); }

    private:

      IdType   tankId_;           ///< Global Tank ID
      TankType tankType_;         ///< Tank type (standard, outrigger, etc.)
      Point    position_;         ///< Tank position in detector coordinates
      float    diameter_;         ///< Tank diameter
      float    height_;           ///< Tank height
      float    waterHeight_;      ///< Water height (not static; doesn't belong?)

      ChannelIterator chBegin_; ///< Iterator to beginning of Channel range
      ChannelIterator chEnd_;   ///< Iterator beyond end of Channel range

      /// Charge * chargeFactor_ is the equivalent charge that would be
      /// observed by a PMT in a standard HAWC tank given a Cherenkov
      /// spectrum.  So smaller chargeFactor_ --> more sensitive tank.
      /// NOTE:  This factor depends on the physical process we're
      /// considering, e.g. muons vs EM air showers.  Assume for here
      /// that it is based on EM air showers.
      double chargeFactor_;

      std::string name_;        ///< Name of the tank, e.g. E18

    friend class Detector;

  };

  SHARED_POINTER_TYPEDEFS(Tank);

}

#endif // DATACLASSES_DETECTOR_TANK_H_INCLUDED

