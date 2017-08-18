/*!
 * @file TDCFormat.cc
 * @brief Bit masks and constants for the CAEN TDC data format.
 * @author Segev BenZvi
 * @date 19 Feb 2014
 * @version $Id: TDCFormat.cc 18884 2014-02-19 22:08:38Z sybenzvi $
 */

#include <data-structures/hardware/caen/TDCFormat.h>

#include <hawcnest/Logging.h>

namespace caen {

  bool
  TDCError::HasL1Error(const int groupId)
    const
  {
    switch (groupId) {
      case 0:
        return (errorFlags & GROUP0_L1_OVERFLOW);
        break;
      case 1:
        return (errorFlags & GROUP1_L1_OVERFLOW);
        break;
      case 2:
        return (errorFlags & GROUP2_L1_OVERFLOW);
        break;
      case 3:
        return (errorFlags & GROUP3_L1_OVERFLOW);
        break;
      default:
        log_fatal("Group ID " << groupId << " is out of range [0..3]");
        break;
    }
    return false;
  }

  bool
  TDCError::HasFIFOError(const int groupId)
    const
  {
    switch (groupId) {
      case 0:
        return (errorFlags & GROUP0_FIFO_OVERFLOW);
        break;
      case 1:
        return (errorFlags & GROUP1_FIFO_OVERFLOW);
        break;
      case 2:
        return (errorFlags & GROUP2_FIFO_OVERFLOW);
        break;
      case 3:
        return (errorFlags & GROUP3_FIFO_OVERFLOW);
        break;
      default:
        log_fatal("Group ID " << groupId << " is out of range [0..3]");
        break;
    }
    return false;
  }

  bool
  TDCError::HasTDCHitError(const int groupId)
    const
  {
    switch (groupId) {
      case 0:
        return (errorFlags & GROUP0_HIT_ERROR);
        break;
      case 1:
        return (errorFlags & GROUP1_HIT_ERROR);
        break;
      case 2:
        return (errorFlags & GROUP2_HIT_ERROR);
        break;
      case 3:
        return (errorFlags & GROUP3_HIT_ERROR);
        break;
      default:
        log_fatal("Group ID " << groupId << " is out of range [0..3]");
        break;
    }
    return false;
  }

}

