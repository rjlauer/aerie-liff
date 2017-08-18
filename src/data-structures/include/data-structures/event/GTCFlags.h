/*!
 * @file GTCFlags.h
 * @brief List of GTC error states.
 * @author Jim Braun
 * @ingroup event_data
 * @version $Id: GTCFlags.h 19156 2014-03-24 17:29:57Z tilan $
 */

#ifndef DATACLASSES_EVENT_GTC_FLAGS_H_INCLUDED
#define DATACLASSES_EVENT_GTC_FLAGS_H_INCLUDED

#include <stdint.h>

namespace evt {

// Indicate that the GTC time is in use!
static const uint64_t GPS_GTC_TIME_IS_USED = 0x00000001;

// GPS is not fixed!
static const uint64_t GPS_NOT_FIX_ERROR = 0x00000002;

// GPS Time String Error
static const uint64_t GPS_TIME_STRING_ERROR = 0x00000004;

// Communication Lost with the GPS receiver
static const uint64_t GPS_COMMUNICATION_LOST_ERROR = 0x00000008;

// Internal Clock Overwritten
static const uint64_t GPS_INT_CLOCK_OVERWRITTEN_ERROR = 0x00000010;

// Decimal BCD digit cannot be greater than 9
static const uint64_t BCD_ERROR = 0x00000020;

// Seconds cannot be greater than 59
static const uint64_t BCD_SECOND_ERROR = 0x00000040;

// Minutes cannot be greater than 59
static const uint64_t BCD_MINUTES_ERROR = 0x00000080;

// Hours cannot be greater than 23
static const uint64_t BCD_HOURS_ERROR = 0x00000100;

// Day cannot be greater than 31
static const uint64_t BCD_DAY_ERROR = 0x00000200;

// Month cannot be greater than 12
static const uint64_t BCD_MONTH_ERROR = 0x00000400;

// Edges are missing
static const uint64_t EDGE_MISSING_ERROR = 0x00000800;

// Not enough edges to extract a timestamp
static const uint64_t NOT_ENOUGH_EDGES_ERROR = 0x00001000;

// Leading edge is falling, second pair is used to extract timestamp
static const uint64_t FALLING_LEADING_EDGE_ERROR = 0x00002000;

// Time width of timestamp edges beyond acceptable range
static const uint64_t TIME_WIDTH_ERROR = 0x00004000;

// TDC and NTP times does not match
static const uint64_t NTP_TDC_MISMATCH_ERROR = 0x00000800;

// GTC data should not be used
static const uint64_t GTC_TIMESTAMP_INVALID = 0x80000000;

}

#endif // DATACLASSES_EVENT_GTC_FLAGS_H_INCLUDED
