/*!
 * @file EMS.h
 * @brief Detector EMS access.
 * @author Andreas Tepe
 * @version $Id: EMS.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_DETECTOR_EMS_H_INCLUDED
#define DATACLASSES_DETECTOR_EMS_H_INCLUDED

#include <string>
#include <sstream>
#include <ctime>

#include <hawcnest/HAWCNest.h>
#include <hawcnest/PointerTypedefs.h>
#include <scaler-classes/event/EventTime.h>

namespace det {

  /**
   * @class EMS
   * @author Andreas Tepe
   * @date 24 Jan 2012
   * @ingroup detector_props
   * @brief EMS sensor information
   *
   * This class stores the information of one EMS sensor. The output files of
   * the EMS contain information about one sensor at a time.
   * Because the EMS software is using UNIX seconds as time stamp, this class
   * is interpreting UNIX seconds using the ctime library :
   * GetDayOfYear: [0, 365]
   * GetMonth:     [0, 11]
   * GetDayOfMon:  [1, 31]
   */
  class EMS : public Baggable
  {

    public:
      Scaler::EventTime
        Time_;

      EMS();

      ~EMS() { }

      bool           SetSensor(const std::string &Sensor);
      std::string    GetSensorString() const;
      unsigned short GetTypeID()       const;
      unsigned short GetSensorID()     const;
      
      bool           SetRawValue    (const std::string &Value);
      void           SetPhysicsValue(const std::string &Value);
      void           SetRawValue    (double Value);
      void           SetPhysicsValue(double Value);
      bool           SetValues      (const std::string &Raw,
                                     const std::string &Phys);
      double         GetRawValue()     const;
      double         GetPhysicsValue() const;
      
      void           SetValid (bool Valid = true);
      bool           IsValid () const;
      void           Clear();

    private:

      unsigned short TypeID_;    ///< Sensor type
      unsigned short SensorID_;  ///< Sensor ID
      double         RawValue_;  ///< Value as measured
      double         PhysValue_; ///< Measured value in physical units
      bool           Valid_;     ///< Flag for good sensor reading
  };

  SHARED_POINTER_TYPEDEFS(EMS);

}

#endif // DATACLASSES_DETECTOR_EMS_H_INCLUDED

