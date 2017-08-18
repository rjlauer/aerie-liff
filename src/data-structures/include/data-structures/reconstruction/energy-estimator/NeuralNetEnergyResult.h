#ifndef DATA_STRUCTURES_ENERGY_ESTIMATOR_NEURAL_NET_ENERGY_RESULT_H_INCLUDED
#define DATA_STRUCTURES_ENERGY_ESTIMATOR_NEURAL_NET_ENERGY_RESULT_H_INCLUDED

#include <boost/array.hpp>

#include <data-structures/reconstruction/energy-estimator/SimpleEnergyResult.h>
  
class NeuralNetEnergyResult : public SimpleEnergyResult {

  public:
  
    static const unsigned nAnnuli_ = 10;

    typedef boost::array<float, NeuralNetEnergyResult::nAnnuli_> AnnulusArray;
      
    const AnnulusArray& GetFAnnulusCharge() const {return fAnnulusCharge_;}
      
    void SetFAnnulusCharge(const AnnulusArray& fAnnulusCharge) {fAnnulusCharge_
      = fAnnulusCharge;}

  protected:

    AnnulusArray fAnnulusCharge_;

};

SHARED_POINTER_TYPEDEFS(NeuralNetEnergyResult)

#endif
