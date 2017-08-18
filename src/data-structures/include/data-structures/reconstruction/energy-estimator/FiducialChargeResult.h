/*!
 * @file LHEnergyResult.h 
 * @brief Storage for output of the likelihood (LH) energy estimator.
 * @author Jim Braun
 * @version $Id: LHEnergyResult.h 16849 2013-08-17 14:42:25Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_ENERGY_ESTIMATOR_FIDUCIAL_CALC_RESULT_H_INCLUDED
#define DATA_STRUCTURES_ENERGY_ESTIMATOR_FIDUCIAL_CALC_RESULT_H_INCLUDED

#include <data-structures/reconstruction/RecoResult.h>

#include <hawcnest/HAWCNest.h>

/*!
 * @class FiducialChargeResult.h
 * @author Zig Hampel
 * @ingroup energy_estimators
 * @brief LH energy estimate for a shower.
 */
class FiducialChargeResult : public virtual RecoResult {

public:
  typedef std::vector<double> dVec;

  FiducialChargeResult() { }

  void PrepResultArrays(const int i)
  {
    nScales_ = i;
    fScales_ = dVec(i,0);
    fracQ_ = dVec(i,0);
  }
  // Set Result Variables
  void SetFracQ(const int i, const double fScale,  const double fracQ)
  {
    fScales_[i] = fScale;
    fracQ_[i] = fracQ;
  }

  // Get Result Variables
  double GetFracQ(int i) const { return fracQ_[i]; }
  double GetfScale(int i) const { return fScales_[i]; }
  double GetnScales(void) const { return nScales_; }
  
  

protected:
  int nScales_;
  dVec fScales_;
  dVec fracQ_;
};

SHARED_POINTER_TYPEDEFS(FiducialChargeResult);

#endif // DATA_STRUCTURES_ENERGY_ESTIMATOR_FIDUCIAL_CALC_RESULT_H_INCLUDED
