/*!
 * @file EnergyResult.h 
 * @brief Storage for output of the ground and primary energy estimator.
 * @author Jim Braun
 * @version $Id: EnergyResult.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_ENERGY_ESTIMATOR_ENERGY_RESULT_H_INCLUDED
#define DATA_STRUCTURES_ENERGY_ESTIMATOR_ENERGY_RESULT_H_INCLUDED

#include <data-structures/reconstruction/RecoResult.h>

#include <hawcnest/HAWCNest.h>

/*!
 * @class EnergyResult
 * @author Jim Braun
 * @ingroup energy_estimators
 * @brief Provides ground and primary energy
 */
class EnergyResult : public RecoResult {

  public:

    EnergyResult() : groundEnergy_(0.),
                     primaryEnergy_(0.) { }

    double GetGroundEnergy() const {return groundEnergy_;}
    double GetPrimaryEnergy() const {return primaryEnergy_;}

    void SetGroundEnergy(const double groundEnergy) {
      groundEnergy_ = groundEnergy;
    }
    void SetPrimaryEnergy(const double primaryEnergy) {
      primaryEnergy_ = primaryEnergy;
    }

protected:

  double groundEnergy_;
  double primaryEnergy_;
};

SHARED_POINTER_TYPEDEFS(EnergyResult);

#endif // DATA_STRUCTURES_ENERGY_ESTIMATOR_ENERGY_RESULT_H_INCLUDED

