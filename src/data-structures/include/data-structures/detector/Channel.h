/*!
 * @file Channel.h
 * @brief Detector channel geometry.
 * @author Segev BenZvi
 * @version $Id: Channel.h 25716 2015-06-08 15:36:23Z srfontaine $
 */

#ifndef DATACLASSES_DETECTOR_CHANNEL_H_INCLUDED
#define DATACLASSES_DETECTOR_CHANNEL_H_INCLUDED

#include <data-structures/geometry/Point.h>

#include <data-structures/detector/PMT.h>
#include <data-structures/detector/CoaxialCable.h>
#include <data-structures/detector/FEBoard.h>

#include <string>

namespace det {

  /// Default name for a Channel object when name is not provided
  static const std::string UNKNOWN_CHANNEL_NAME = "Unknown";

  class Tank;

  /**
   * @class Channel
   * @author Segev BenZvi
   * @date 19 Mar 2010
   * @ingroup detector_props
   * @brief Storage for Channel geometry information
   */
  class Channel {

    public:

      Channel();

      /// Initialize with global channel ID, tank ID, slot ID within tank,
      /// and position
      Channel(const IdType& chId, const IdType& tankId, const IdType& tcId,
              const Point& pos,
              const double rX=0, const double rY=0, const double rZ=0);

      /// Initialize with global channel ID, tank ID, slot ID within tank,
      /// tank type, channel type and position
      Channel(const IdType& chId, const IdType& tankId, const IdType& tcId,
              const PMTType& pmtType, const std::string& chName,
              const TankType& tankType, const Point& pos,
              const double rX=0, const double rY=0, const double rZ=0);

      virtual ~Channel() { }

      /// Global channel ID (like Milagro "igrid")
      const IdType& GetChannelId() const { return chId_; }

      /// ID of the parent Tank
      const IdType& GetTankId() const { return tankId_; }

      /// Channel number within a single Tank
      const IdType& GetTankChannelId() const { return tcId_; }

      /// Type number of the channel as in hawcsim survey file
      const PMTType& GetChannelType() const { return pmtType_; }

      /// Type number of the tank as in hawcsim survey file
      const TankType GetTankType() const { return tankType_; }

      /// Global position of the Channel (not with respect to Tank)
      const Point& GetPosition() const { return position_; }

      /// Get rotation angle of channel slot in x, y, z
      double GetRotationX() const { return rotX_; }
      double GetRotationY() const { return rotY_; }
      double GetRotationZ() const { return rotZ_; }

      /// PMT plugged into this channel (todo: as a function of TimeStamp?)
      const PMT& GetPMT() const { return pmt_; }

      /// Coaxial cable plugged into this channel (todo: specify length)
      const CoaxialCable& GetCable() const { return cab_; }

      /// Front-end board plugged into this channel (todo: specify length)
      const FEBoard& GetFEBoard() const { return feb_; }

      /// Comparison, used to store Channels in a sorted container
      bool operator<(const Channel& c) const { 
        if (tankId_ == c.tankId_)
          return chId_ < c.chId_;
        return tankId_ < c.tankId_;
      }

      /// Get the name of the channel, e.g. E18A
      const std::string& GetName() const { return name_; }

      /// Does the Channel have an assigned name?
      bool HasName() const { return name_.compare(UNKNOWN_CHANNEL_NAME); }

    private:

      IdType chId_;              ///< Global channel ID at the Detector level
      IdType tankId_;            ///< ID of the parent Tank
      IdType tcId_;              ///< Channel ID at the Tank level
      PMTType pmtType_;          ///< Type, as in 4th col. of hawcsim survey
      PMT   pmt_;                ///< PMT plugged into this channel
      TankType tankType_;        ///< Type, as in 4th col. of hawcsim survey
      Point position_;           ///< Global position in Detector coords

      double rotX_;              ///< Rotation of PMT about X-axis
      double rotY_;              ///< Rotation of PMT about Y-axis
      double rotZ_;              ///< Rotation of PMT about Z-axis

      CoaxialCable cab_;         ///< Cable taking PMT output to counting house
      FEBoard feb_;              ///< Front-end board

      std::string name_;      ///< Name of channel, e.g. E18A

    friend class Tank;

  };

  SHARED_POINTER_TYPEDEFS(Channel);

}

#endif // DATACLASSES_DETECTOR_CHANNEL_H_INCLUDED

