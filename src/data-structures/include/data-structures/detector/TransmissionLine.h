/*!
 * @file TransmissionLine.h
 * @brief Geometrical and electronic properties of a transmission line.
 * @author Segev BenZvi
 * @version $Id: TransmissionLine.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_DETECTOR_TRANSMISSIONLINE_H_INCLUDED
#define DATACLASSES_DETECTOR_TRANSMISSIONLINE_H_INCLUDED

#include <string>

namespace det {

  /*!
   * @class TransmissionLine
   * @author Segev BenZvi
   * @date 23 Sep 2010
   * @ingroup detector_props
   * @brief Abstract interface for a transmission line which is used to
   *        propagate pulses from point A to point B
   */
  class TransmissionLine {
    
    public:

      TransmissionLine();

      /// Initialize given a cable type, length, and impedance
      TransmissionLine(const std::string& type, const double ln, const double Z);

      virtual ~TransmissionLine() { }

      /// Retrieve cable type
      const std::string& GetType() const { return type_; }

      /// Retrieve cable length
      double GetLength() const { return len_; }
      void SetLength(const double l) { len_ = l; }

      /// Retrieve cable impedance
      double GetImpedance() const { return Z_; }
      void SetImpedance(const double z) { Z_ = z; }

    protected:

      std::string type_;      ///< Name of the cable type (RG59, TOSLINK, etc.)
      double len_;            ///< Length of the line [m, ft, etc.]
      double Z_;              ///< Input impedance of the cable

  };

}

#endif // DATACLASSES_DETECTOR_TRANSMISSIONLINE_H_INCLUDED

