/*!
 * @file Particle.h
 * @brief Particle ID codes and I/O transformation.
 * @author Segev BenZvi
 * @version $Id: Particle.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_PHYSICS_PARTICLE_H_INCLUDED
#define DATACLASSES_PHYSICS_PARTICLE_H_INCLUDED

#include <iosfwd>

/*!
 * @enum ParticleType
 * @ingroup physics
 * @brief Particle types, from the CORSIKA particle ID scheme
 */
enum ParticleType {
  unknown = 0,
  Gamma = 1,
  EPlus = 2,
  EMinus = 3,
  MuPlus = 5,
  MuMinus = 6,
  Pi0 = 7,
  PiPlus = 8,
  PiMinus = 9,
  K0_Long = 10,
  KPlus = 11,
  KMinus = 12,
  Neutron = 13,
  PPlus = 14,
  PMinus = 15,
  K0_Short = 16,
  NeutronBar = 25,
  NuE = 66,
  NuEBar = 67,
  NuMu = 68,
  NuMuBar = 69,
  TauPlus = 131,
  TauMinus = 132,
  NuTau = 133,
  NuTauBar = 134,
  /**
   * In CORSIKA nuclei numbers are A x 100 + Z
   */
  He4Nucleus = 402,
  Li7Nucleus = 703,
  Be9Nucleus = 904,
  B11Nucleus = 1105,
  C12Nucleus = 1206,
  N14Nucleus = 1407,
  O16Nucleus = 1608,
  F19Nucleus = 1909,
  Ne20Nucleus = 2010,
  Na23Nucleus = 2311,
  Mg24Nucleus = 2412,
  Al27Nucleus = 2713,
  Si28Nucleus = 2814,
  P31Nucleus = 3115,
  S32Nucleus = 3216,
  Cl35Nucleus = 3517,
  Ar40Nucleus = 4018,
  K39Nucleus = 3919,
  Ca40Nucleus = 4020,
  Sc45Nucleus = 4521,
  Ti48Nucleus = 4822,
  V51Nucleus = 5123,
  Cr52Nucleus = 5224,
  Mn55Nucleus = 5525,
  Fe56Nucleus = 5626,
  CherenkovPhoton = 9900,
};

std::ostream& operator<<(std::ostream& os, const ParticleType& p);

namespace Particle {

  /// Convert a particle string name to the ParticleType enum
  ParticleType GetParticleType(const std::string& pName);

  /// Get particle mass (in base units)
  double GetMass(const ParticleType& p);

  /// Get particle charge (in base units)
  double GetCharge(const ParticleType& p);

};

#endif // DATACLASSES_PHYSICS_PARTICLE_H_INCLUDED

