/*!
 * @file G4SimHeader.cc 
 * @brief Meta-data from a Geant4 simulation of the detector.
 * @author Segev BenZvi 
 * @date 7 Apr 2010 
 * @version $Id: G4SimHeader.cc 17766 2013-11-01 04:23:51Z sybenzvi $
 */

#include <data-structures/event/G4SimHeader.h>
#include <hawcnest/HAWCUnits.h>

using namespace HAWCUnits;
using namespace std;

G4SimHeader::G4SimHeader() :
  geantVersion_(0),
  g4simVersion_(0),
  detectorType_(0),
  dynamicCore_(false)
{
}

ostream&
operator<<(ostream& os, const G4SimHeader& ge)
{
  os <<   "Geant4 version ...... " << ge.geantVersion_
     << "\nG4Sim version ....... " << ge.g4simVersion_
     << "\nGHEISHA flag ........ " << ge.genFlags_.GHEISHA_
     << "\nVENUS flag .......... " << ge.genFlags_.VENUS_
     << "\nDetector survey ..... " << ge.tankSurveyFile_
     << "\nDetector type ....... " << ge.detectorType_
     << "\nDynamic core ........ " << (ge.dynamicCore_ ? "true" : "false")
     << "\nPrimary particle .... " << ge.primPartID_
     << "\nSpectral index ...... " << ge.energy_.spectralIndex_
     << "\nEnergy min/max ...... " << ge.energy_.min_ / GeV << ", "
                                   << ge.energy_.max_ / GeV << " GeV"
     << "\nCore pos. x/y max ... " << ge.corePos_.xMax_ / meter << ", "
                                   << ge.corePos_.yMax_ / meter << " GeV"
     << "\nB field (x,z) ....... " << ge.B_.x_ / (HAWCUnits::micro*tesla) << ", " 
                                   << ge.B_.z_ / (HAWCUnits::micro*tesla) << " uT";

  return os;
}

