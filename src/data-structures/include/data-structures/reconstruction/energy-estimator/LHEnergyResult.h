/*!
 * @file LHEnergyResult.h 
 * @brief Storage for output of the likelihood (LH) energy estimator.
 * @author Jim Braun
 * @version $Id: LHEnergyResult.h 27384 2015-10-25 23:25:28Z zhampel $
 */ 

#ifndef DATA_STRUCTURES_ENERGY_ESTIMATOR_LH_ENERGY_RESULT_H_INCLUDED
#define DATA_STRUCTURES_ENERGY_ESTIMATOR_LH_ENERGY_RESULT_H_INCLUDED

#include <data-structures/reconstruction/RecoResult.h>
#include <string>
#include <hawcnest/HAWCNest.h>

/*!
 * @class LHEnergyResult.h
 * @author Zig Hampel, Brian Baughman
 * @ingroup energy_estimators
 * @brief LH energy estimate for a shower.
 */
class LHEnergyResult : public virtual RecoResult {

public:

  LHEnergyResult() { }
  typedef std::vector<double> llhVec;
  typedef std::vector<int> llhVecInt;

  void PrepResultArrays(const std::vector<std::string> species) {
    
    nspecies_ = species.size();

    for (int j = 0; j < nspecies_; j++)
      species_.push_back(species[j]);

    MaxLLH_ = llhVec(nspecies_,0);
    MaxBin_ = llhVecInt(nspecies_,0);
    Energy_ = llhVec(nspecies_,0);
    CoreX_ = llhVec(nspecies_,0);
    CoreY_ = llhVec(nspecies_,0);
  }
  
  // Set Result Variables
  void SetMaxVars(const unsigned int index, const double llh, 
                  const int bin, const double energy, 
                  const double xcore, const double ycore)
  {
    MaxLLH_[index] = llh;
    MaxBin_[index] = bin;
    Energy_[index] = energy;
    CoreX_[index] = xcore;
    CoreY_[index] = ycore;
  }

  // Get Result Variables
  int GetNSpecies(void) const { return nspecies_; }
  std::string GetSpeciesName(const int i) const { return species_[i]; }
  int GetMaxBin(int i) const { return MaxBin_[i]; }
  double GetLLH(int i) const { return MaxLLH_[i]; }
  double GetEnergy(int i) const { return Energy_[i];}
  double GetCoreX(int i) const { return CoreX_[i]; }
  double GetCoreY(int i) const { return CoreY_[i]; }
  
  

protected:
  int nspecies_;
  std::vector<std::string> species_;
  llhVec MaxLLH_;
  llhVecInt MaxBin_;
  llhVec Energy_;
  llhVec CoreX_;
  llhVec CoreY_;
};

SHARED_POINTER_TYPEDEFS(LHEnergyResult);

#endif // DATA_STRUCTURES_ENERGY_ESTIMATOR_LH_ENERGY_RESULT_H_INCLUDED
