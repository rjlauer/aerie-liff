/*!
 * @file RunInfo.h 
 * @brief Run ID and time slice information.
 * @author Dmitry Zaborov
 * @version $Id: RunInfo.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_RECO_ONLINE_RUN_INFO_H_INCLUDED
#define DATA_STRUCTURES_RECO_ONLINE_RUN_INFO_H_INCLUDED

#include <hawcnest/HAWCNest.h>

#include <stdint.h>

/*!
 * @class RunInfo
 * @author Dmitry Zaborov
 * @date 2 Aug 2012
 * @ingroup online_reconstruction
 * @brief Baggable carrier of run number and time slice ID
 *
 * RunInfo is intended as a means to notify aerie modules of the current run
 * number and position of the current time slice within the run.  Only one
 * RunInfo object should be present in the bag at any given time.
 */

class RunInfo : public Baggable {

  public:

    RunInfo(uint32_t runNumber = 0,
            uint32_t timeSliceID = 0) : runNumber_(runNumber),
                                        timeSliceID_(timeSliceID) { }

    uint32_t runNumber_;   /// Current run number, e.g. from Experiment Control
    uint32_t timeSliceID_; /// Sequential ID of current time slice (relative to run start)
};

SHARED_POINTER_TYPEDEFS(RunInfo);

#endif // DATA_STRUCTURES_RECO_ONLINE_RUN_INFO_H_INCLUDED
