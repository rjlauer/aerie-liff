/*!
 * @file MCSummaryResult.h
 * @brief Storage for MC summary data of event.
 * @author Brian Baughman
 * @date 24 Nov 2014
 * @version $Id: MCSummaryResult.h 27534 2015-11-04 19:13:40Z zhampel $
 */

#ifndef DATA_STRUCTURES_RECO_MC_SUMMARY_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RECO_MC_SUMMARY_RESULT_H_INCLUDED

#include <data-structures/reconstruction/RecoResult.h>

#include <hawcnest/HAWCNest.h>

#include <stdint.h>

/*!
 * @class MCSummaryResult
 * @author Brian Baughman
 * @date 24 Nov 2014
 * @brief Storage for output of summary of event
 */
class MCSummaryResult : public virtual RecoResult {
  
public:
  
  /// Default constructor
  MCSummaryResult() :
    corsikaParticleId_(0),
    rWgt_(0.),
    wgtSim_(0.),
    coreX_(0.),
    coreY_(0.),
    coreR_(0.),
    zenithAngle_(0.),
    azimuthAngle_(0.),
    delCore_(0.),
    delAngle_(0.),
    logEnergy_(0.),
    logGroundEnergy_(0.),
    prescale_(0),
    coreFiduScale_(0)
  { }
  
  virtual ~MCSummaryResult() { }
  
  
public:
  
  uint64_t corsikaParticleId_;
  double   rWgt_;
  double   wgtSim_;
  double   coreX_;
  double   coreY_;
  double   coreR_;
  double   zenithAngle_;
  double   azimuthAngle_;
  double   delCore_;
  double   delAngle_;
  double   logEnergy_;
  double   logGroundEnergy_;
  int64_t   prescale_;
  unsigned int coreFiduScale_;
  
};

SHARED_POINTER_TYPEDEFS(MCSummaryResult);

#endif // DATA_STRUCTURES_RECO_MC_SUMMARY_RESULT_H_INCLUDED

