/*!
 * @file Particle.cc 
 * @brief Particle properties for CORSIKA particle IDs.
 * @author Segev BenZvi 
 * @date 11 Mar 2010 
 * @version $Id: Particle.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <boost/algorithm/string.hpp>

#include <data-structures/physics/Particle.h>
#include <data-structures/physics/PhysicsConstants.h>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/Logging.h>

using namespace std;
using namespace HAWCUnits;
using namespace PhysicsConstants;

// Just a big switch statement for printing particle types.  There are many
// solutions on the web for automating the creation of this function, but the
// author was too lazy to try one.
ostream&
operator<<(ostream& os, const ParticleType& p)
{
  switch (p) {
    case Gamma:
      os << "Gamma";
      break;
    case EPlus:
      os << "Positron";
      break;
    case EMinus:
      os << "Electron";
      break;
    case MuPlus:
      os << "Mu+";
      break;
    case MuMinus:
      os << "Mu-";
      break;
    case Pi0:
      os << "Pi0";
      break;
    case PiPlus:
      os << "Pi+";
      break;
    case PiMinus:
      os << "Pi-";
      break;
    case K0_Long:
      os << "K0_Long";
      break;
    case KPlus:
      os << "K+";
      break;
    case KMinus:
      os << "K-";
      break;
    case Neutron:
      os << "Neutron";
      break;
    case PPlus:
      os << "Proton";
      break;
    case PMinus:
      os << "Antiproton";
      break;
    case K0_Short:
      os << "K0_Short";
      break;
    case NeutronBar:
      os << "NeutronBar";
      break;
    case NuE:
      os << "NuE";
      break;
    case NuEBar:
      os << "NuEBar";
      break;
    case NuMu:
      os << "NuMu";
      break;
    case NuMuBar:
      os << "NuMuBar";
      break;
    case TauPlus:
      os << "Tau+";
      break;
    case TauMinus:
      os << "Tau-";
      break;
    case NuTau:
      os << "NuTau";
      break;
    case NuTauBar:
      os << "NuTauBar";
      break;
    case He4Nucleus:
      os << "He4";
      break;
    case Li7Nucleus:
      os << "Li7";
      break;
    case Be9Nucleus:
      os << "Be9";
      break;
    case B11Nucleus:
      os << "B11";
      break;
    case C12Nucleus:
      os << "C12";
      break;
    case N14Nucleus:
      os << "N14";
      break;
    case O16Nucleus:
      os << "O16";
      break;
    case F19Nucleus:
      os << "F19";
      break;
    case Ne20Nucleus:
      os << "Ne20";
      break;
    case Na23Nucleus:
      os << "Na23";
      break;
    case Mg24Nucleus:
      os << "Mg24";
      break;
    case Al27Nucleus:
      os << "Al27";
      break;
    case Si28Nucleus:
      os << "Si28";
      break;
    case P31Nucleus:
      os << "P31";
      break;
    case S32Nucleus:
      os << "S32";
      break;
    case Cl35Nucleus:
      os << "Cl35";
      break;
    case Ar40Nucleus:
      os << "Ar40";
      break;
    case K39Nucleus:
      os << "K39";
      break;
    case Ca40Nucleus:
      os << "Ca40";
      break;
    case Sc45Nucleus:
      os << "Sc45";
      break;
    case Ti48Nucleus:
      os << "Ti48";
      break;
    case V51Nucleus:
      os << "V51";
      break;
    case Cr52Nucleus:
      os << "Cr52";
      break;
    case Mn55Nucleus:
      os << "Mn55";
      break;
    case Fe56Nucleus:
      os << "Fe56";
      break;
    case CherenkovPhoton:
      os << "CkvGamma";
      break;
    default:
      os << "unknown";
      break;
  }

  return os;
};

ParticleType
Particle::GetParticleType(const string& pName)
{
  if (boost::iequals(pName, "Gamma"))
    return Gamma;
  else if (boost::iequals(pName,"Positron"))
    return EPlus;
  else if (boost::iequals(pName,"Electron"))
    return EMinus;
  else if (boost::iequals(pName,"Mu+"))
    return MuPlus;
  else if (boost::iequals(pName,"Mu-"))
    return MuMinus;
  else if (boost::iequals(pName,"Pi0"))
    return Pi0;
  else if (boost::iequals(pName,"Pi+"))
    return PiPlus;
  else if (boost::iequals(pName,"Pi-"))
    return PiMinus;
  else if (boost::iequals(pName,"K0_Long"))
    return K0_Long;
  else if (boost::iequals(pName,"K+"))
    return KPlus;
  else if (boost::iequals(pName,"K-"))
    return KMinus;
  else if (boost::iequals(pName,"Neutron"))
    return Neutron;
  else if (boost::iequals(pName,"Proton"))
    return PPlus;
  else if (boost::iequals(pName,"Antiproton"))
    return PMinus;
  else if (boost::iequals(pName,"K0_Short"))
    return K0_Short;
  else if (boost::iequals(pName,"NeutronBar"))
    return NeutronBar;
  else if (boost::iequals(pName,"NuE"))
    return NuE;
  else if (boost::iequals(pName,"NuEBar"))
    return NuEBar;
  else if (boost::iequals(pName,"NuMu"))
    return NuMu;
  else if (boost::iequals(pName,"NuMuBar"))
    return NuMuBar;
  else if (boost::iequals(pName,"Tau+"))
    return TauPlus;
  else if (boost::iequals(pName,"Tau-"))
    return TauMinus;
  else if (boost::iequals(pName,"NuTau"))
    return NuTau;
  else if (boost::iequals(pName,"NuTauBar"))
    return NuTauBar;
  else if (boost::iequals(pName,"He4"))
    return He4Nucleus;
  else if (boost::iequals(pName,"Li7"))
    return Li7Nucleus;
  else if (boost::iequals(pName,"Be9"))
    return Be9Nucleus;
  else if (boost::iequals(pName,"B11"))
    return B11Nucleus;
  else if (boost::iequals(pName,"C12"))
    return C12Nucleus;
  else if (boost::iequals(pName,"N14"))
    return N14Nucleus;
  else if (boost::iequals(pName,"O16"))
    return O16Nucleus;
  else if (boost::iequals(pName,"F19"))
    return F19Nucleus;
  else if (boost::iequals(pName,"Ne20"))
    return Ne20Nucleus;
  else if (boost::iequals(pName,"Na23"))
    return Na23Nucleus;
  else if (boost::iequals(pName,"Mg24"))
    return Mg24Nucleus;
  else if (boost::iequals(pName,"Al27"))
    return Al27Nucleus;
  else if (boost::iequals(pName,"Si28"))
    return Si28Nucleus;
  else if (boost::iequals(pName,"P31"))
    return P31Nucleus;
  else if (boost::iequals(pName,"S32"))
    return S32Nucleus;
  else if (boost::iequals(pName,"Cl35"))
    return Cl35Nucleus;
  else if (boost::iequals(pName,"Ar40"))
    return Ar40Nucleus;
  else if (boost::iequals(pName,"K39"))
    return K39Nucleus;
  else if (boost::iequals(pName,"Ca40"))
    return Ca40Nucleus;
  else if (boost::iequals(pName,"Sc45"))
    return Sc45Nucleus;
  else if (boost::iequals(pName,"Ti48"))
    return Ti48Nucleus;
  else if (boost::iequals(pName,"V51"))
    return V51Nucleus;
  else if (boost::iequals(pName,"Cr52"))
    return Cr52Nucleus;
  else if (boost::iequals(pName,"Mn55"))
    return Mn55Nucleus;
  else if (boost::iequals(pName,"Fe56"))
    return Fe56Nucleus;
  else if (boost::iequals(pName,"CkvGamma"))
    return CherenkovPhoton;
  else
    return unknown;
};

double
Particle::GetMass(const ParticleType& p)
{
  double pmass = 0.;

  switch (p) {
    case Gamma:
      pmass = 0.;
      break;
    case EPlus:
      pmass = Me;
      break;
    case EMinus:
      pmass = Me;
      break;
    case MuPlus:
      pmass = Mmu;
      break;
    case MuMinus:
      pmass = Mmu;
      break;
    case Pi0:
      pmass = 134.9766 * MeV/(c*c);
      break;
    case PiPlus:
      pmass = 139.57018 * MeV/(c*c);
      break;
    case PiMinus:
      pmass = 139.57018 * MeV/(c*c);
      break;
    case K0_Long:
      pmass = 499.456 * MeV/(c*c);
      break;
    case KPlus:
      pmass = 501.551 * MeV/(c*c);
      break;
    case KMinus:
      pmass = 501.551 * MeV/(c*c);
      break;
    case Neutron:
      pmass = Mn;
      break;
    case PPlus:
      pmass = Mp;
      break;
    case PMinus:
      pmass = Mp;
      break;
    case K0_Short:
      pmass = 495.873 * MeV/(c*c);
      break;
    case NeutronBar:
      pmass = Mn;
      break;
    case NuE:
      pmass = 0.;   // Ignore neutrino masses...
      break;
    case NuEBar:
      pmass = 0.;   // Ignore neutrino masses...
      break;
    case NuMu:
      pmass = 0.;   // Ignore neutrino masses...
      break;
    case NuMuBar:
      pmass = 0.;   // Ignore neutrino masses...
      break;
    case TauPlus:
      pmass = 1776.84 * MeV/(c*c);
      break;
    case TauMinus:
      pmass = 1776.84 * MeV/(c*c);
      break;
    case NuTau:
      pmass = 0.;   // Ignore neutrino masses...
      break;
    case NuTauBar:
      pmass = 0.;   // Ignore neutrino masses...
      break;
    case He4Nucleus:
      pmass = 4.001506179 * u;
      break;
    case Li7Nucleus:
      pmass = 7.01600455 * u;
      break;
    case Be9Nucleus:
      pmass = 9.0121822 * u;
      break;
    case B11Nucleus:
      pmass = 11.0093054 * u;
      break;
    case C12Nucleus:
      pmass = 12. * u;
      break;
    case N14Nucleus:
      pmass = 14.0030740048 * u;
      break;
    case O16Nucleus:
      pmass = 15.99491461956 * u;
      break;
    case F19Nucleus:
      pmass = 18.99840322 * u;
      break;
    case Ne20Nucleus:
      pmass = 19.9924401754 * u;
      break;
    case Na23Nucleus:
      pmass = 22.9897692809 * u;
      break;
    case Mg24Nucleus:
      pmass = 23.985041700 * u;
      break;
    case Al27Nucleus:
      pmass = 26.98153863 * u;
      break;
    case Si28Nucleus:
      pmass = 27.9769265325 * u;
      break;
    case P31Nucleus:
      pmass = 30.97376163 * u;
      break;
    case S32Nucleus:
      pmass = 31.97207100 * u;
      break;
    case Cl35Nucleus:
      pmass = 34.96885268 * u;
      break;
    case Ar40Nucleus:
      pmass = 39.9623831225 * u;
      break;
    case K39Nucleus:
      pmass = 38.96370668 * u;
      break;
    case Ca40Nucleus:
      pmass = 39.96259098 * u;
      break;
    case Sc45Nucleus:
      pmass = 44.9559119 * u;
      break;
    case Ti48Nucleus:
      pmass = 47.9479463 * u;
      break;
    case V51Nucleus:
      pmass = 50.9439595 * u;
      break;
    case Cr52Nucleus:
      pmass = 51.9405075 * u;
      break;
    case Mn55Nucleus:
      pmass = 54.9380451 * u;
      break;
    case Fe56Nucleus:
      pmass = 55.9349375 * u;
      break;
    case CherenkovPhoton:
      pmass = 0.;
      break;
    default:
      log_fatal("No mass available for unidentified particle type.");
      break;
  }

  return pmass;
}

double
Particle::GetCharge(const ParticleType& p)
{
  double charge = 0.;

  switch (p) {
    case Gamma:
      charge = 0.;
      break;
    case EPlus:
      charge = 1.;
      break;
    case EMinus:
      charge = -1.;
      break;
    case MuPlus:
      charge = 1.;
      break;
    case MuMinus:
      charge = -1.;
      break;
    case Pi0:
      charge = 0.;
      break;
    case PiPlus:
      charge = 1.;
      break;
    case PiMinus:
      charge = -1.;
      break;
    case K0_Long:
      charge = 0.;
      break;
    case KPlus:
      charge = 1.;
      break;
    case KMinus:
      charge = -1.;
      break;
    case Neutron:
      charge = 0.;
      break;
    case PPlus:
      charge = 1.;
      break;
    case PMinus:
      charge = -1.;
      break;
    case K0_Short:
      charge = 0.;
      break;
    case NeutronBar:
      charge = 0.;
      break;
    case NuE:
      charge = 0.;
      break;
    case NuEBar:
      charge = 0.;
      break;
    case NuMu:
      charge = 0.;
      break;
    case NuMuBar:
      charge = 0.;
      break;
    case TauPlus:
      charge = 1.;
      break;
    case TauMinus:
      charge = -1.;
      break;
    case NuTau:
      charge = 0.;
      break;
    case NuTauBar:
      charge = 0.;
      break;
    case He4Nucleus:
      charge = 2.;
      break;
    case Li7Nucleus:
      charge = 3.;
      break;
    case Be9Nucleus:
      charge = 4.;
      break;
    case B11Nucleus:
      charge = 5.;
      break;
    case C12Nucleus:
      charge = 6.;
      break;
    case N14Nucleus:
      charge = 7.;
      break;
    case O16Nucleus:
      charge = 8.;
      break;
    case F19Nucleus:
      charge = 9.;
      break;
    case Ne20Nucleus:
      charge = 10.;
      break;
    case Na23Nucleus:
      charge = 11.;
      break;
    case Mg24Nucleus:
      charge = 12.;
      break;
    case Al27Nucleus:
      charge = 13.;
      break;
    case Si28Nucleus:
      charge = 14.;
      break;
    case P31Nucleus:
      charge = 15.;
      break;
    case S32Nucleus:
      charge = 16.;
      break;
    case Cl35Nucleus:
      charge = 17.;
      break;
    case Ar40Nucleus:
      charge = 18.;
      break;
    case K39Nucleus:
      charge = 19.;
      break;
    case Ca40Nucleus:
      charge = 20.;
      break;
    case Sc45Nucleus:
      charge = 21.;
      break;
    case Ti48Nucleus:
      charge = 22.;
      break;
    case V51Nucleus:
      charge = 23.;
      break;
    case Cr52Nucleus:
      charge = 24.;
      break;
    case Mn55Nucleus:
      charge = 25.;
      break;
    case Fe56Nucleus:
      charge = 26.;
      break;
    case CherenkovPhoton:
      charge = 0.;
      break;
    default:
      log_fatal("No charge available for unidentified particle type.");
      break;
  }

  return charge * eSI;
}

