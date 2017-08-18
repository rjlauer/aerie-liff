/*!
 * @file SimpleEnergyResult.h 
 * @brief Storage for result of the simple energy estimator module
 * @author Brian Baughman
 * @version $Id: SimpleEnergyResult.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_ENERGY_ESTIMATOR_SIMPLE_ENERGY_RESULT_H_INCLUDED
#define DATA_STRUCTURES_ENERGY_ESTIMATOR_SIMPLE_ENERGY_RESULT_H_INCLUDED

#include <data-structures/reconstruction/energy-estimator/EnergyResult.h>

#include <hawcnest/HAWCNest.h>

/*!
 * @class SimpleEnergyResult.h
 * @author Brian Baughman
 * @ingroup energy_estimators
 * @brief Simple energy estimate for a shower and energy reaching the ground.
 */
class SimpleEnergyResult : public EnergyResult {

public:

  SimpleEnergyResult() : nPEs_(0) { }

  unsigned int GetNPEs(void) const {return nPEs_;}

  void SetNPEs(const unsigned int nPEs) {nPEs_ = nPEs;}

protected:

  unsigned int nPEs_;

};

SHARED_POINTER_TYPEDEFS(SimpleEnergyResult);

#endif // DATA_STRUCTURES_ENERGY_ESTIMATOR_SIMPLE_ENERGY_RESULT_H_INCLUDED

