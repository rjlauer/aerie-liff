/*!
 * @file Detector.h
 * @brief Detector geometry interface.
 * @author Segev BenZvi
 * @version $Id: Detector.h 27080 2015-10-02 17:36:35Z jbraun $
 */

#ifndef DATACLASSES_DETECTOR_DETECTOR_H_INCLUDED
#define DATACLASSES_DETECTOR_DETECTOR_H_INCLUDED

#include <data-structures/detector/Tank.h>
#include <data-structures/geometry/LatLonAlt.h>
#include <data-structures/geometry/Vector.h>

#include <map>

namespace det {

  /*!
   * @class Detector
   * @author Segev BenZvi
   * @date 19 Mar 2010
   * @ingroup detector_props
   * @brief Top-level interface for the representation of the physial detector.
   *
   * The Detector class stores the physical properties of the HAWC detector,
   * such as its geodetic latitude, longitude, and altitude, as well as its
   * hardware components.
   *
   * Like the real detector, this object presents a hierarchical interface of
   * hardward components.  The Detector object stores Tanks, and the Tank
   * objects store Channels, which contain slots for PMTs to plug into.
   * Therefore, one can recurse through the hierarchy from the top level of the
   * Detector down to an individual Channel and its corresponding PMT.
   *
   * In practice, however, the layout of the Detector in physical memory is
   * a flat list of Channels inside the Detector object.  This structure
   * allows for very fast iteration over the range of available Channels.
   * A second sparse table of Channel pointers (with table indices
   * corresponding to Channel * IDs) is used for efficient random access to the
   * Channels by their ID number.
   *
   * To provide a nested hierarchy of Tanks and Channels, each Tank object
   * stores two Channel iterators that point to the start and end of the range
   * of Channels inside the Tank.  By only using two iterators for internal
   * state rather than a list of Channel pointers, the Tank objects can reduce
   * their memory use by at least one third.  Note that the Channel list
   * inside the Detector must be sorted before the Tank iterators are
   * initialized.
   *
   * Further details about the memory layout can be found at
   * http://hawc.umd.edu/wiki/index.php/Detector_Structure_Benchmark
   */
  class Detector {

    public:

      typedef std::vector<Channel> ChannelList;
      typedef ChannelList::iterator ChannelIterator;
      typedef ChannelList::const_iterator ConstChannelIterator;
      typedef std::vector<const Channel*> ChannelPtrList;

      typedef std::vector<Tank> TankList;
      typedef TankList::iterator TankIterator;
      typedef TankList::const_iterator ConstTankIterator;
      typedef std::vector<const Tank*> TankPtrList;

      Detector();
      Detector(const Detector& det);
      Detector& operator=(Detector det);

      virtual ~Detector() { }

      /// Read-write iterator to the start of the list of Tanks in the detector
      TankIterator TanksBegin() { return tanks_.begin(); }

      /// Read-write iterator to the end of the list of Tanks in the detector
      TankIterator TanksEnd() { return tanks_.end(); }

      /// Read-only iterator to the start of the list of Tanks in the detector
      ConstTankIterator TanksBegin() const { return tanks_.begin(); }

      /// Read-only iterator to the end of the list of Tanks in the detector
      ConstTankIterator TanksEnd() const { return tanks_.end(); }

      /// Add a tank to the tank list
      void AddTank(const Tank& tank);

      /// Get a Tank by its ID
      const Tank& GetTank(const IdType& id) const;

      /// Get a Tank by its name
      const Tank& GetTank(const std::string& tankName) const;

      /// Check for the presence of a Tank in the detector by its ID
      bool HasTank(const IdType& tankId) const
      { return tkAccessArray_[tankId] != NULL; }

      /// Check for the presence of a Tank in the detector by its name
      bool HasTank(const std::string& tankName) const;

      /// Return the number of tanks in the detector
      size_t GetNTanks() const { return tanks_.size(); }

      /// Read-write access to the start of the Channel list
      ChannelIterator ChannelsBegin() { return channels_.begin(); }

      /// Read-write access to the end of the Channel list
      ChannelIterator ChannelsEnd() { return channels_.end(); }

      /// Read-only access to the start of the Channel list
      ConstChannelIterator ChannelsBegin() const { return channels_.begin(); }

      /// Read-only access to the end of the Channel list
      ConstChannelIterator ChannelsEnd() const { return channels_.end(); }

      /// Add a Channel to the Channel list
      void AddChannel(const Channel& ch);

      /// Get a Channel by its global ID
      const Channel& GetChannel(const IdType& chId) const;

      /// Get a Channel by name
      const Channel& GetChannel(const std::string& chName) const;

      /// Check for the presence of a Channel in the Detector by its global ID
      bool HasChannel(const IdType& chId) const
      { return chAccessArray_[chId] != NULL; }

      /// Check for the presence of a Channel in the Detector by its name
      bool HasChannel(const std::string& chName) const;

      /// Return the number of Channels in the detector
      const size_t GetNChannels() const { return channels_.size(); }

      /// Set the name of the detector
      void SetName(const std::string& name) { name_ = name; }

      /// Retrieve the name of the detector
      const std::string& GetName() const { return name_; }

      /// Set the version of the detector survey used to define the geometry
      void SetVersion(const std::string& version) { sversion_ = version; }

      /// Retrieve the geometry survey version of the detector
      const std::string& GetVersion() const { return sversion_; }

      /// Return true if the Tanks and sparse table lists have been initialized
      bool IsInitialized() const { return isInitialized_; }

      /// Initialize the Tank list and sparse tables
      void InitializeDetector();

      /// Return the latitude, longitude, and altitude of the detector
      const LatLonAlt& GetLatitudeLongitudeHeight() const { return position_; }

      /// Set the latitude, longitude, and altitude of the detector
      void SetLatitudeLongitudeHeight(const LatLonAlt& lla) { position_ = lla; }

      /// Set the latitude, longitude, and altitude of the detector
      void SetLatitudeLongitudeHeight(const double lat, const double lon, const double alt)
      { position_.SetLatitudeLongitudeHeight(lat, lon, alt); }

       /// Return the array center vector
      const Vector& GetArrayCenter() const { return arrayCenter_; }

      const double GetArrayCenterX() const { return arrayCenter_.GetX(); }
      const double GetArrayCenterY() const { return arrayCenter_.GetY(); }
      const double GetArrayCenterZ() const { return arrayCenter_.GetZ(); }

      /// Set the array center
      void SetArrayCenter(const double x, const double y, const double z)
      { arrayCenter_ = Vector(x,y,z); }


    private:
      static const IdType maxChannelID_ = 5000; ///< Maximum channel ID which can be assigned
      static const IdType maxTankID_    = 1000; ///< Maximum tank ID which can be assigned
      ChannelList channels_;          ///< Dense Channel array (for iterators)
      ChannelPtrList chAccessArray_;  ///< Sparse Channel* array (random access)

      TankList tanks_;                ///< Dense Tank array (for iterators)
      TankPtrList tkAccessArray_;     ///< Dense Tank* array (for random access)

      bool isInitialized_;            ///< Check: Tanks & sparse arrays filled?

      std::string name_;              ///< Detector name
      std::string sversion_;          ///< Detector survey version
      LatLonAlt position_;            ///< Detector geodetic position
      Vector arrayCenter_;            ///< Center of array

      /// Map of tank name --> ID for name-based tank lookup
      std::map<std::string, IdType> tankNameLookupMap_;

      /// Map of channel name --> global ID for name-based channel lookup
      std::map<std::string, IdType> channelNameLookupMap_;

  };

  SHARED_POINTER_TYPEDEFS(Detector);

}

#endif // DATACLASSES_DETECTOR_DETECTOR_H_INCLUDED

