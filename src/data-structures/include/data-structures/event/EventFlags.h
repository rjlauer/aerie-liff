/*!
 * @file EventFlags.h
 * @brief Event error and trigger flags.
 * @author Jim Braun
 * @ingroup event_data
 * @version $Id: EventFlags.h 26795 2015-08-28 21:08:45Z jbraun $
 */

#ifndef DATACLASSES_EVENT_EVENTFLAGS_H_INCLUDED
#define DATACLASSES_EVENT_EVENTFLAGS_H_INCLUDED

#include <stdint.h>

namespace evt {

  // EventFlags
  static const uint16_t BAD_EVENT                   = 0x0001;
  static const uint16_t CALIBRATION_EVENT           = 0x0002;
  static const uint16_t TDC_CHIP_L1_BUFFER_ERROR    = 0x0004;
  static const uint16_t TDC_CHIP_FIFO_ERROR         = 0x0008;
  static const uint16_t TDC_CHIP_HITS_LOST          = 0x0010;
  static const uint16_t TDC_CHIP_ERROR              = 0x0020;
  static const uint16_t TDC_OUTPUT_BUFFER_OVERFLOW  = 0x0040;
  static const uint16_t LIGHT_IN_DETECTOR           = 0x0080;

  // TriggerFlags
  static const uint16_t SIMPLE_MULTIPLICITY_TRIGGER = 0x0001;
  static const uint16_t TANK_MULTIPLICITY_TRIGGER   = 0x0002;
  static const uint16_t MIN_BIAS_TRIGGER            = 0x0004;
  static const uint16_t MUON_TRIGGER                = 0x0008;
  static const uint16_t FPGA_TRIGGER                = 0x0010;
  static const uint16_t CALIBRATION_TRIGGER         = 0x0020;
  static const uint16_t QBALL_TRIGGER               = 0x0040;
  static const uint16_t MPF_TRIGGER                 = 0x0080;

}

#endif // DATACLASSES_EVENT_EVENTFLAGS_H_INCLUDED
