/*!
 * @file Physics.cc 
 * @brief Python bindings to structures in the physics classes.
 * @author Segev BenZvi 
 * @date 11 Mar 2010 
 * @version $Id: Physics.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <boost/python.hpp>
#include <boost/preprocessor.hpp>

#include <data-structures/physics/Particle.h>
#include <data-structures/physics/PhysicsConstants.h>

using namespace boost::python;
using namespace std;

// -----------------------------------------------------------------------------
/// Structure to expose PhysicsConstants to python
struct __physicsconstants__ {
  #define X(name, value, desc) \
  static double BOOST_PP_CAT(physicsconstants_return_,name)() \
  { return PhysicsConstants::name; }

  #include <data-structures/physics/constant_defs.h>
  #undef X
};

/// Define boost::python bindings for the PhysicsConstants
void
pybind_physics_PhysicsConstants()
{
  // The following definition ensures the constants are read-only in python.
  class_<__physicsconstants__, boost::noncopyable>
    ("PhysicsConstants",
     "'Namespace' holding values for physics constants", no_init)
    #define X(name, value, desc) \
    .add_static_property(#name, &__physicsconstants__::BOOST_PP_CAT(physicsconstants_return_, name))
    #include <data-structures/physics/constant_defs.h>
    #undef X
    ;
}

// -----------------------------------------------------------------------------
/// Expose ParticleType CORSIKA particle ID enum to python
void
pybind_physics_ParticleType()
{
  enum_<ParticleType>
    ("ParticleType",
     "Particle types from the CORSIKA particle ID scheme")
    .value("unknown", unknown)
    .value("Gamma", Gamma)
    .value("EPlus", EPlus)
    .value("EMinus", EMinus)
    .value("MuPlus", MuPlus)
    .value("MuMinus", MuMinus)
    .value("Pi0", Pi0)
    .value("PiPlus", PiPlus)
    .value("PiMinus", PiMinus)
    .value("K0_Long", K0_Long)
    .value("KPlus", KPlus)
    .value("KMinus", KMinus)
    .value("Neutron", Neutron)
    .value("PPlus", PPlus)
    .value("PMinus", PMinus)
    .value("K0_Short", K0_Short)
    .value("NeutronBar", NeutronBar)
    .value("NuE", NuE)
    .value("NuEBar", NuEBar)
    .value("NuMu", NuMu)
    .value("NuMuBar", NuMuBar)
    .value("TauPlus", TauPlus)
    .value("TauMinus", TauMinus)
    .value("NuTau", NuTau)
    .value("NuTauBar", NuTauBar)
    .value("He4Nucleus", He4Nucleus)
    .value("Li7Nucleus", Li7Nucleus)
    .value("Be9Nucleus", Be9Nucleus)
    .value("B11Nucleus", B11Nucleus)
    .value("C12Nucleus", C12Nucleus)
    .value("N14Nucleus", N14Nucleus)
    .value("O16Nucleus", O16Nucleus)
    .value("F19Nucleus", F19Nucleus)
    .value("Ne20Nucleus", Ne20Nucleus)
    .value("Na23Nucleus", Na23Nucleus)
    .value("Mg24Nucleus", Mg24Nucleus)
    .value("Al27Nucleus", Al27Nucleus)
    .value("Si28Nucleus", Si28Nucleus)
    .value("P31Nucleus", P31Nucleus)
    .value("S32Nucleus", S32Nucleus)
    .value("Cl35Nucleus", Cl35Nucleus)
    .value("Ar40Nucleus", Ar40Nucleus)
    .value("K39Nucleus", K39Nucleus)
    .value("Ca40Nucleus", Ca40Nucleus)
    .value("Sc45Nucleus", Sc45Nucleus)
    .value("Ti48Nucleus", Ti48Nucleus)
    .value("V51Nucleus", V51Nucleus)
    .value("Cr52Nucleus", Cr52Nucleus)
    .value("Mn55Nucleus", Mn55Nucleus)
    .value("Fe56Nucleus", Fe56Nucleus)
    .value("CherenkovPhoton", CherenkovPhoton)
    ;

  def("particleNameToType", Particle::GetParticleType,
      "Convert particle names to ParticleType.");

  def("particleMass", Particle::GetMass,
       "Get mass of a particular ParticleType.");

  def("particleCharge", Particle::GetCharge,
       "Get charge of a particular ParticleType (SI).");

}

