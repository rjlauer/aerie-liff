/*!
 * @file CoaxialCable.h
 * @brief Coaxial cable geometry and transmission properties
 * @author Segev BenZvi
 * @version $Id: CoaxialCable.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_DETECTOR_COAXIALCABLE_H_INCLUDED
#define DATACLASSES_DETECTOR_COAXIALCABLE_H_INCLUDED

#include <data-structures/detector/TransmissionLine.h>

namespace det {

  /*!
   * @class CoaxialCable
   * @author Ian Wisher
   * @author Segev BenZvi 
   * @date 23 Sep 2010
   * @brief Simulation of a lossy transmission line
   *
   * This class stores configuration data for a lossy transmission line, such
   * as RG59.  Users can choose the length, capacitance, and attenuation
   * characteristics of the line as needed.
   */
  class CoaxialCable : public TransmissionLine {
    
    public:

      CoaxialCable();

      /// Initialize given a cable type, length, impedance, capacitance,
      /// propagation velocity, attenuation/100m and attenuation frequency
      CoaxialCable(const std::string& type,
                   const double ln, const double Z,
                   const double C, const double v,
                   const double a, const double af);

      virtual ~CoaxialCable() { }

      /// Retrieve the capacitance of the cable
      double GetCapacitancePerUnitLength() const { return C_L_; }
      void SetCapacitancePerUnitLength(const double cl) { C_L_ = cl; }

      /// Retrieve the propagation velocity of signals in the cable
      double GetPropagationVelocity() const { return vel_; }
      void SetPropagactionVelocity(const double cn) { vel_ = cn; }

      /// Retrieve the attenuation in dB / 100 [ft|m] @ some frequency 
      double GetNdB100() const { return ndb100_; }
      void SetNdB100(const double atten) { ndb100_ = atten; }

      /// Retrieve the frequency of the attenuation property
      double GetNdB100Frequency() const { return ndb100Freq_; }
      void SetNdB100Frequency(const double aFreq) { ndb100Freq_ = aFreq; }

    protected:

      double C_L_;            ///< Capacitance per unit length
      double vel_;            ///< Signal propagation velocity
      double ndb100_;         ///< Attenuation [dB/100' or dB/100m @ XYZ MHz]
      double ndb100Freq_;     ///< Frequency where attenuation is reported

  };

}

#endif // DATACLASSES_DETECTOR_COAXIALCABLE_H_INCLUDED

