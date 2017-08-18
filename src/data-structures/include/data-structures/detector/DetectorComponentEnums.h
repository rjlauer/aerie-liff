/*!
 * @file DetectorComponentEnums.h
 * @brief Convenient enums for Detector components.
 * @author Segev BenZvi
 * @version $Id: DetectorComponentEnums.h 28995 2016-01-19 21:58:20Z sybenzvi $
 */

#ifndef DATACLASSES_DETECTOR_DETECTOR_COMPONENT_ENUMS_H_INCLUDED
#define DATACLASSES_DETECTOR_DETECTOR_COMPONENT_ENUMS_H_INCLUDED

namespace det {

  /// PMT types
  enum PMTType {
    R5912 = 1,      ///< Milagro R5912 8" Hamamatsu PMT
    R7081HQE,       ///< R7081 High Quantum Efficiency 10" Hamamatsu PMT
    R12199,         ///< R12199 3" Hamamatsu PMT
    UNKNOWN_PMT     ///< Unrecognized PMT type
  };

  /// Tank types
  enum TankType {
    HAWC_WCD = 1,   ///< HAWC standard 7.3m x 5m steel WCD
    HAWC_OR,        ///< HAWC outriggers
    UNKNOWN_TANK    ///< Unrecognized Tank type
  };
    
}

#endif // DATACLASSES_DETECTOR_DETECTOR_COMPONENT_ENUMS_H_INCLUDED

