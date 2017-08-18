/*!
 * @file HAWCTDCDAQ.h 
 * @brief DAQ time stamp conversions.
 * @author Jim Braun
 * @version $Id: HAWCTDCDAQ.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_RECO_ONLINE_HAWC_TDC_DAQ_H_INCLUDED
#define DATA_STRUCTURES_RECO_ONLINE_HAWC_TDC_DAQ_H_INCLUDED

#include <data-structures/time/LeapSeconds.h>
#include <data-structures/time/TimeStamp.h>

/// DAQ time stamp is in microseconds.  This defines the seconds conversion.
static const uint64_t DAQ_SECONDS = 1000000;

/*
 *  Convert the DAQ time stamp (in microseconds after the
 *  UNIX epoch) into a TimeStamp.
 */
inline
TimeStamp ConvertDAQTimeStamp(uint64_t daqTimeStamp) {

  uint64_t secs = daqTimeStamp / DAQ_SECONDS;
  time_t unixSecs = static_cast<uint32_t>(secs);
  unsigned gpsSecs;
  LeapSeconds::GetInstance().ConvertUnixToGPS(unixSecs, gpsSecs);

  uint32_t ns = 1000*(daqTimeStamp - secs * DAQ_SECONDS);
  return TimeStamp(gpsSecs, ns);
}

/*!
 *  Number of channels in GTC time stamp
 */
const uint32_t NUMBER_GTC_CHANNELS = 32;

#endif // DATA_STRUCTURES_RECO_ONLINE_HAWC_TDC_DAQ_H_INCLUDED
