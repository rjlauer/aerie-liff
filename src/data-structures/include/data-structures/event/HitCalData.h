/*!
 * @file HitCalData.h
 * @brief Calibrated pulse data from an Event.
 * @author Segev BenZvi
 * @ingroup event_data
 * @version $Id: HitCalData.h 26577 2015-08-04 18:16:26Z criviere $
 */

#ifndef DATACLASSES_EVENT_HITCALDATA_H_INCLUDED
#define DATACLASSES_EVENT_HITCALDATA_H_INCLUDED

#include <hawcnest/processing/Bag.h>

namespace evt {

  /*!
   * @class HitCalData
   * @author Segev BenZvi
   * @date 6 Apr 2010
   * @ingroup event_data
   * @brief Storage of calibrated hit data
   */
  class HitCalData : public Baggable {

    public:

      HitCalData() : PEs_(-1000000.),
                     time_(-1000000.), 
                     loTOTCharge_(-1000000),
                     hiTOTCharge_(-1000000),
                     isSlewCalibrated_(false),
                     isTRCalibrated_(false),
                     isChargeCalibrated_(false),
                     maxCharge_(false) { }

      double PEs_;              /// Estimated photoelectron (PE) count
      double time_;             /// Time of the hit relative to event time
      double loTOTCharge_;      /// PE based only on low ToT 
      double hiTOTCharge_;      /// PE based only on high ToT 
      bool isSlewCalibrated_;   /// TRUE if slewing calibration applied 
      bool isTRCalibrated_;     /// TRUE if time residual correction applied 
      bool isChargeCalibrated_; /// TRUE if charge cal curve used; If FALSE: all hits 1 PE
      bool maxCharge_; /// TRUE if hiToT exceeded ChargeCal range: Used max fit/interpol value

      bool IsCalibrated() const {
        //return isSlewCalibrated_ && isTRCalibrated_ && isChargeCalibrated_;
        return isSlewCalibrated_ && isChargeCalibrated_;
      }

      bool IsMaxCharge() const {
        return maxCharge_; // Warning, maxCharge_ is only set in the LinearInterpolation and MilagroFit cases
      }

      bool IsChargeCalibrated() const {return isChargeCalibrated_;}
      bool IsTimeCalibrated() const {
        //return isSlewCalibrated_ && isTRCalibrated_;
        return isSlewCalibrated_;
      }

      bool operator==(const HitCalData& hitCalData) const {

        return (PEs_  == hitCalData.PEs_ &&
                time_ == hitCalData.time_ &&
                loTOTCharge_ == hitCalData.loTOTCharge_ &&
                hiTOTCharge_ == hitCalData.hiTOTCharge_
                );
      }

  };

  SHARED_POINTER_TYPEDEFS(HitCalData);

}

#endif // DATACLASSES_EVENT_HITCALDATA_H_INCLUDED

