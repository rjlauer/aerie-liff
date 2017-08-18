/*!
 * @file Reco.h
 * @brief Reconstruction status flags.
 * @author Jim Braun
 * @version $Id: Reco.h 22771 2014-11-14 20:02:41Z pretz $
 */

#ifndef DATA_STRUCTURES_RECO_RECO_H_INCLUDED
#define DATA_STRUCTURES_RECO_RECO_H_INCLUDED

/*!
 * @enum RecoStatus
 * @author Jim Braun
 * @ingroup modules
 * @brief Common status values returned by reconstruction modules
 *        (incomplete list)
 */
enum RecoStatus {

  RECO_SUCCESS        = 0,
  RECO_FAIL           = 1,
  RECO_UNKNOWN        = 2,
  RECO_NO_CONVERGENCE = 3,
  RECO_SEED_USED        = 4
};

static const unsigned RECO_STATUS_RANGE_MIN = 0;
static const unsigned RECO_STATUS_RANGE_MAX = 3;

/*! 
 * @enum RecoAction
 * @author Jim Braun
 * @ingroup modules
 * @brief Action of reconstruction module on shower summary
 */
enum RecoAction {

  RECO_ACTIVE            = 0,  // Always update shower summary
  RECO_INACTIVE          = 1,  // Do not update shower summary
  RECO_ACTIVE_IF_SUCCESS = 2  // Update summary only if RECO_SUCCESS is returned

};

#endif // DATA_STRUCTURES_RECO_RECO_H_INCLUDED

