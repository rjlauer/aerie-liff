/*!
 * @file HitFlags.h
 * @brief Bad Hit identification flags.
 * @author Jim Braun
 * @ingroup event_data
 * @version $Id: HitFlags.h 27422 2015-10-29 16:22:18Z joshwood $
 */

#ifndef DATACLASSES_EVENT_HITFLAGS_H_INCLUDED
#define DATACLASSES_EVENT_HITFLAGS_H_INCLUDED

#include <stdint.h>

namespace evt {

  // Hit Flags
  static const uint16_t INVALID_TOT           = 0x0001;
  static const uint16_t AFTERPULSE_VETO       = 0x0002;
  static const uint16_t AMBIGUOUS_EDGE_RESULT = 0x0004;
  static const uint16_t SIX_EDGE              = 0x0008;
  static const uint16_t L1_ERROR_IN_GROUP     = 0x0010;
  static const uint16_t FIFO_ERROR_IN_GROUP   = 0x0020;
  static const uint16_t PE_PROMOTED           = 0x0040;
  static const uint16_t KILL_CHANNEL          = 0x0080;
  static const uint16_t SIMULATED_NOISE       = 0x0100;
  static const uint16_t KILL_HIT              = 0x0200;
  static const uint16_t OUTSIDE_TRIGGER_CUT   = 0x0400;
  static const uint16_t PRIOR_HIT             = 0x0800;
  static const uint16_t MAX_CHARGE            = 0x1000;
  static const uint16_t BAD_CHANNEL           = 0x2000;

}

#endif // DATACLASSES_EVENT_HITFLAGS_H_INCLUDED
