/*!
 * @file ChannelIdentifier.h 
 * @brief ID mangler for TDCIdentifier and TDC channel number.
 * @author Jim Braun
 * @version $Id: ChannelIdentifier.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_RECO_ONLINE_CHANNEL_IDENTIFIER_H_INCLUDED
#define DATA_STRUCTURES_RECO_ONLINE_CHANNEL_IDENTIFIER_H_INCLUDED

#include <data-structures/reconstruction/online/TDCIdentifier.h>

#include <stdint.h>
#include <iosfwd>

/*!
 * @class ChannelIdentifier
 * @author Jim Braun
 * @date 17 Aug 2012
 * @ingroup online_reconstruction
 * @brief A TDC channel is globally identified by a TDCIdentifier object
 * and TDC channel number
 */
class ChannelIdentifier {

  public:

    ChannelIdentifier(uint16_t channelNumber = 0xFFFF,
                      TDCIdentifier tdcIdentifier = TDCIdentifier()) :
                                           channelNumber_(channelNumber),
                                           tdcIdentifier_(tdcIdentifier) { }

    ChannelIdentifier(uint16_t channelNumber,
                      uint16_t sbcID,
                      uint16_t tdcGeoID) :
                                  channelNumber_(channelNumber),
                                  tdcIdentifier_(sbcID, tdcGeoID) { }

    bool operator==(const ChannelIdentifier& id) const {
      return tdcIdentifier_ == id.tdcIdentifier_ &&
             channelNumber_ == id.channelNumber_;
    }

    bool operator!=(const ChannelIdentifier& id) const {
      return !(*this == id);
    }

    bool operator<(const ChannelIdentifier& id) const {

      if (tdcIdentifier_ < id.tdcIdentifier_) {
        return true;
      }

      if (tdcIdentifier_ == id.tdcIdentifier_) {
        if (channelNumber_ < id.channelNumber_) {
          return true;
        }
      }

      return false;
    }

    uint16_t GetChannelNumber() const {return channelNumber_;}
    TDCIdentifier GetTDCIdentifier() const {return tdcIdentifier_;}
    uint16_t GetSBCID() const {return tdcIdentifier_.GetSBCID();}
    uint16_t GetTDCGeoID() const {return tdcIdentifier_.GetTDCGeoID();}

    void SetChannelNumber(const uint8_t channelNumber) {
      channelNumber_ = channelNumber;
    }
    void SetTDCIdentifier(const TDCIdentifier id) {tdcIdentifier_ = id;}
    void SetSBCID(const uint16_t sbcID) {tdcIdentifier_.SetSBCID(sbcID);}

    void SetTDCGeoID(const uint16_t tdcGeoID) {
      tdcIdentifier_.SetTDCGeoID(tdcGeoID);
    }

  private:

    uint16_t channelNumber_;
    TDCIdentifier tdcIdentifier_;
};

inline
std::ostream& operator<<(std::ostream& os, const ChannelIdentifier& id) {
  os << "SBC: " << id.GetSBCID() << " Geo: " <<
      id.GetTDCGeoID() << " Channel: " <<
            static_cast<uint32_t>(id.GetChannelNumber());
  return os;
}

#endif // DATA_STRUCTURES_RECO_ONLINE_CHANNEL_IDENTIFIER_H_INCLUDED
