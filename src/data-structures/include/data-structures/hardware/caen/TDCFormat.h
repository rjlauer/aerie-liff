/*!
 * @file TDCFormat.h
 * @brief Bit masks and constants for the CAEN TDC data format.
 * @author Jim Braun
 * @version $Id: TDCFormat.h 18884 2014-02-19 22:08:38Z sybenzvi $
 */

#ifndef DATA_STRUCTURES_HARDWARE_CAEN_TDC_FORMAT_H_INCLUDED
#define DATA_STRUCTURES_HARDWARE_CAEN_TDC_FORMAT_H_INCLUDED

#include <data-structures/hardware/caen/CCAENV1x90Data.h>

#include <stdint.h>

/*!
 * TDCFormat.h
 * @author Jim Braun
 * @brief Representation of CAEN TDC Format
 */

namespace caen {

  /* Max TDC channel */
  static const int MAX_TDC_CHANNEL = 128;

  /* Max number of edges.  TDC internal buffer is 32 K words */
  static const int MAX_EVENT_EDGES = 0x8000;

  /* Max number of TDC chips on the card */
  static const int MAX_TDC = 4;

  /* Max number of TDC groups on the card */
  static const int MAX_TDC_GROUP = 16;

  /*!
   * Definitions for TDC --> time conversions
   */

  /* Number of TDC ticks until TDC fine counter rolls over */
  static const uint64_t
  TDC_CLOCK_ROLLOVER_PERIOD = 524288;  // 2^19

  static const uint64_t
  TDC_MEASUREMENT_MASK      = 0x7FFFF; // 19 bits

  /* Number of TDC ticks until TDC trigger tag counter rolls over */
  static const uint64_t
  EXTENDED_TRIG_TIME_TAG_ROLLOVER_PERIOD = 1099511627776ULL; // 2^40

  /* Integer conversions from TDC counts to time units */
  static const uint64_t TDC_TIME_US = 10240ULL;
  static const uint64_t TDC_TIME_MS = 10240000ULL;
  static const uint64_t TDC_TIME_S  = 10240000000ULL;

  /* TDC counts per 25 ns bunch */
  static const uint64_t BUNCH_TIME = 256;  // 2^8

  /* TDC counts per 25 ns extended trigger time, now same as bunch time */
  static const uint64_t EXTENDED_TRIGGER_TIME = 256;   // 2^8

  /* Number of ETTT bits that come from the trailer geo field */
  static const uint32_t N_EXTENDED_TRIGGER_TIME_LOW_BITS = 5;

  /* Maximum global event number possible */
  static const uint32_t GLOBAL_EVENT_NUMBER_ROLLOVER_COUNT = 0x00400000; // 2^22
  static const uint32_t GLOBAL_EVENT_NUMBER_MASK = 0x003FFFFF; // 2^22 - 1

  /* Bunch Time mask */
  static const uint32_t BUNCH_TIME_MASK = 0x7FF;

  // Floating point conversions between ns and TDC counts
  static const double CHTONS = 0.097656;
  static const double NSTOCH = 10.24;

  /* TDC L1 overflow error masks.  There are 4 channel groups per TDC chip */
  static const uint16_t GROUP0_L1_OVERFLOW          = 0x0002;
  static const uint16_t GROUP1_L1_OVERFLOW          = 0x0010;
  static const uint16_t GROUP2_L1_OVERFLOW          = 0x0080;
  static const uint16_t GROUP3_L1_OVERFLOW          = 0x0400;

  /* TDC FIFO overflow error masks.  There are 4 channel groups per TDC chip */
  static const uint16_t GROUP0_FIFO_OVERFLOW        = 0x0001;
  static const uint16_t GROUP1_FIFO_OVERFLOW        = 0x0008;
  static const uint16_t GROUP2_FIFO_OVERFLOW        = 0x0040;
  static const uint16_t GROUP3_FIFO_OVERFLOW        = 0x0200;

  /* TDC Hit errors.  There are 4 channel groups per TDC chip */
  static const uint16_t GROUP0_HIT_ERROR            = 0x0004;
  static const uint16_t GROUP1_HIT_ERROR            = 0x0020;
  static const uint16_t GROUP2_HIT_ERROR            = 0x0100;
  static const uint16_t GROUP3_HIT_ERROR            = 0x0800;

  /* TDC trigger FIFO overflow error. */
  static const uint16_t TRIGGER_OVERFLOW            = 0x2000;

  /* TDC fatal chip error.  Hope you never see this. */
  static const uint16_t FATAL_CHIP_ERROR            = 0x4000;

  /* Number hits > size limit (should never see this; we don't have a limit). */
  static const uint16_t HIT_SIZE_LIMIT_ERROR        = 0x1000;

  /* Global Header */
  class GlobalHeader {
    public:

      GlobalHeader() { }
      ~GlobalHeader() { }
      uint32_t eventCount;
      uint8_t  geoAddress;
  };

  /* TDC Header */
  class TDCHeader {
    public:

      TDCHeader() { }
      ~TDCHeader() { }
      uint16_t eventID;
      uint16_t bunchID;
      uint8_t  tdcID;
  };

  /* TDC Measurement */
  class TDCMeasurement {
    public:

      TDCMeasurement() { }
      ~TDCMeasurement() { }
      uint32_t measurement;
      uint8_t  channelID;
      uint8_t  isTrailingEdge;

      // Get the equivalent trigger-time subtracted measurement
      uint32_t GetTriggerSubtractedMeasurement(uint32_t bunchID) const {
        return (measurement - (bunchID & BUNCH_TIME_MASK) * BUNCH_TIME) &
                                                          TDC_MEASUREMENT_MASK;
      }

      bool operator <(const uint32_t m) const {return measurement < m;}
      bool operator <(const TDCMeasurement m) const {
        return measurement < m.measurement;
      }

      bool operator >(const uint32_t m) const {return measurement > m;}
      bool operator >(const TDCMeasurement m) const {
        return measurement > m.measurement;
      }

      bool operator ==(const uint32_t m) const {return measurement == m;}
      bool operator ==(const TDCMeasurement m) const {
        return measurement == m.measurement;
      }
  };

  /* TDC Trailer */
  class TDCTrailer {
    public:

      TDCTrailer() { }
      ~TDCTrailer() { }
      uint16_t eventID;
      uint16_t wordCount;
      uint8_t  tdcID;
  };

  /* TDC Error */
  class TDCError {
    public:

      TDCError() { }
      ~TDCError() { }

      bool HasL1Error(const int groupId) const;

      bool HasFIFOError(const int groupId) const;

      bool HasTDCHitError(const int groupId) const;
      
      bool HasTriggerOverflowError() const
      { return (errorFlags & TRIGGER_OVERFLOW); }

      bool HasFatalChipError() const
      { return (errorFlags & FATAL_CHIP_ERROR); }

      bool HasHitSizeLimitError() const
      { return (errorFlags & HIT_SIZE_LIMIT_ERROR); }

      uint16_t errorFlags;
      uint8_t  tdcID;

  };

  /* Extended Trigger Time Tag */
  class ExtendedTriggerTimeTag {
    public:

      ExtendedTriggerTimeTag() { }
      ~ExtendedTriggerTimeTag() { }
      uint32_t extendedTriggerTime;
  };

  /* Global Trailer */
  class GlobalTrailer {
    public:

      GlobalTrailer() { }
      ~GlobalTrailer() { }
      uint16_t wordCount;
      uint8_t  etttLowBits;
      uint8_t  triggerLost;
      uint8_t  overflow;
      uint8_t  error;
  };

  static inline GlobalHeader
  ParseGlobalHeader(unsigned long data) {
    GlobalHeader h;
    h.eventCount = CCAENV1x90Data::TriggerNumber(data);
    h.geoAddress = CCAENV1x90Data::BoardNumber(data);
    return h;
  }

  static inline TDCHeader
  ParseTDCHeader(unsigned long data) {
    TDCHeader h;
    h.eventID = CCAENV1x90Data::EventId(data);
    h.bunchID = CCAENV1x90Data::BunchId(data);
    h.tdcID   = CCAENV1x90Data::TDCChip(data);
    return h;
  }

  static inline TDCMeasurement
  ParseTDCMeasurement(unsigned long data) {
    TDCMeasurement m;
    m.measurement    = CCAENV1x90Data::ChannelValue(data);
    m.channelID      = CCAENV1x90Data::ChannelNumber(data);
    m.isTrailingEdge = CCAENV1x90Data::isTrailing(data);
    return m;
  }

  static inline TDCTrailer
  ParseTDCTrailer(unsigned long data) {
    TDCTrailer t;
    t.eventID   = CCAENV1x90Data::EventId(data);
    t.wordCount = CCAENV1x90Data::TDCWordCount(data);
    t.tdcID     = CCAENV1x90Data::TDCChip(data);
    return t;
  }

  static inline TDCError
  ParseTDCError(unsigned long data) {
    TDCError e;
    e.errorFlags = CCAENV1x90Data::TDCErrorBits(data);
    e.tdcID      = CCAENV1x90Data::TDCChip(data);
    return e;
  }

  static inline ExtendedTriggerTimeTag
  ParseExtendedTriggerTimeTag(unsigned long data) {
    ExtendedTriggerTimeTag e;
    e.extendedTriggerTime = CCAENV1x90Data::ExtendedTriggerTime(data) <<
                                             N_EXTENDED_TRIGGER_TIME_LOW_BITS;
    return e;
  }

  static inline GlobalTrailer
  ParseGlobalTrailer(unsigned long data) {
    GlobalTrailer g;
    g.wordCount   = CCAENV1x90Data::EventSize(data);
    g.etttLowBits = CCAENV1x90Data::ETTTLowBits(data);
    g.triggerLost = CCAENV1x90Data::Lost(data);
    g.overflow    = CCAENV1x90Data::Overflow(data);
    g.error       = CCAENV1x90Data::Error(data);
    return g;
  }

}

#endif // DATA_STRUCTURES_HARDWARE_CAEN_TDC_FORMAT_H_INCLUDED
