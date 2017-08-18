/*!
 * @file TDCIdentifier.h 
 * @brief Encapsulation of TDC used for a given trigger.
 * @author Jim Braun
 * @version $Id: TDCIdentifier.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_RECO_ONLINE_TDC_IDENTIFIER_H_INCLUDED
#define DATA_STRUCTURES_RECO_ONLINE_TDC_IDENTIFIER_H_INCLUDED

#include <stdint.h>

/*!
 * @class TDCIdentifier
 * @author Jim Braun
 * @date 31 Jul 2012
 * @ingroup online_reconstruction
 * @brief SBC ID and TDC Geo address uniquely identify a TDC
 */

class TDCIdentifier {

  public:

    virtual ~TDCIdentifier() { }

    TDCIdentifier(uint16_t sbcID = 0xFFFF,
                  uint16_t tdcGeoID = 0xFFFF) : sbcID_(sbcID),
                                                tdcGeoID_(tdcGeoID) { }

    bool operator==(const TDCIdentifier& id) const {
      return sbcID_ == id.sbcID_ &&
             tdcGeoID_ == id.tdcGeoID_;
    }

    bool operator!=(const TDCIdentifier& id) const {
      return !(*this == id);
    }

    bool operator<(const TDCIdentifier& id) const {

      if (sbcID_ < id.sbcID_) {
        return true;
      }

      if (sbcID_ == id.sbcID_) {
        if (tdcGeoID_ < id.tdcGeoID_) {
          return true;
        }
      }

      return false;
    }

    uint16_t GetSBCID() const {return sbcID_;}
    uint16_t GetTDCGeoID() const {return tdcGeoID_;}

    void SetSBCID(const uint16_t sbcID) {sbcID_ = sbcID;}
    void SetTDCGeoID(const uint16_t tdcGeoID) {tdcGeoID_ = tdcGeoID;}

  private:

    uint16_t sbcID_;
    uint16_t tdcGeoID_;
};

#endif // DATA_STRUCTURES_RECO_ONLINE_TDC_IDENTIFIER_H_INCLUDED
