/*!
 * @file TDCGroupID.h
 * @brief TDC Group IDs Struct Class  
 * @author Ian G. Wisher
 * @date Feb 19 2014
 * @version $Id$
 */

#ifndef DETECTOR_SERVICE_TDCGROUPID_H_INCLUDED
#define DETECTOR_SERVICE_TDCGROUPID_H_INCLUDED

#include <data-structures/detector/Detector.h>

#include <data-structures/reconstruction/online/ChannelIdentifier.h>

#include <map>
#include <string>
#include <vector>

/*!
 * @class TDCGroupID
 * @author Ian G. Wisher
 * @date 19 Feb 2014
 * @ingroup detector_service
 * @brief Mapping of TDC channel IDs to and from TDC Group IDs
 */ 

/*!
 * Group IDs are assigned by counting up by 8 from the lowestTDC channel 
 * over all TDC CHIPs and CAEN TDC modules  
 * 
 * Channels [0-7]  -> Group 0
 * Channels [8-15] -> Group 1
 *
 */

// Encode the GROUPID knowing that the chip and tdc boundaries are powers of 2
#ifndef GROUPID_ENCODE
#define GROUPID_ENCODE(sbcID, tdcChipID, groupID) \
   (((sbcID) << 4) + ((tdcChipID) << 2) + (groupID))
#endif

#ifndef GROUPID_DECODE_SBCID
#define GROUPID_DECODE_SBCID(groupID) \
   ((groupID) >> 4)
#endif
                    
#ifndef GROUPID_DECODE_TDCCHIP
#define GROUPID_DECODE_TDCCHIP(groupID) \
   (((groupID) >> 2) & 0x03)
#endif

#ifndef GROUPID_DECODE_TDCGROUPID
#define GROUPID_DECODE_TDCGROUPID(groupID) \
   ((groupID) & 0x03)
#endif

class TDCGroupID {

  public:

    typedef uint16_t id_t;
    
    /// Constructor 
    TDCGroupID(id_t sbcID, id_t tdcChipID, id_t TDCGroupID):
      groupID_(GROUPID_ENCODE( sbcID, tdcChipID, TDCGroupID)) { }
    
    /// Constructor with groupID
    TDCGroupID(id_t GroupID):
      groupID_(GroupID) { }
    
   ~TDCGroupID() { }

    bool HasHWChannel(uint32_t hwChannel) const 
    { return (groupID_ == (hwChannel >> 2)); }

    /// Get the SBC ID connected to the TDC board
    id_t GetSBC() const
    { return GROUPID_DECODE_SBCID(groupID_); }

    /// Get the TDC chip ID: [0..3], 4 chips per TDC board
    id_t GetTDCChip() const
    { return GROUPID_DECODE_TDCCHIP(groupID_); }

    /// Get the channel group ID within the TDC chip: [0..3], 4 groups per chip
    id_t GetTDCGroup() const
    { return GROUPID_DECODE_TDCGROUPID(groupID_); }

    /// Get the encoded SBC ID, TDC chip ID, and chip group, useful for I/O
    id_t GetGroupID() const 
    { return groupID_;}

  private: 

    id_t groupID_;    ///< Encoded SBC ID, TDC Chip ID, and chip group [0..3]

  // Boolean comparisons
  friend bool operator==(const TDCGroupID& a, const TDCGroupID& b)
  { return a.groupID_ == b.groupID_; }

  friend bool operator!=(const TDCGroupID& a, const TDCGroupID& b)
  { return !(a.groupID_ == b.groupID_); }

};

#endif // DETECTOR_SERVICE_TDCGROUPIDMAP_H_INCLUDED

