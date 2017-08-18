/*!
 * @file SimEventHeader.cc 
 * @brief Event meta-data from simulated events.
 * @author Segev BenZvi 
 * @date 27 Apr 2010 
 * @version $Id: SimEventHeader.cc 21819 2014-09-12 21:25:09Z sybenzvi $
 */

#include <data-structures/event/SimEventHeader.h>

#include <hawcnest/HAWCUnits.h>

using namespace HAWCUnits;

SimEventHeader::SimEventHeader() :
  particleId_(unknown),
  energy_(0),
  theta_(0),
  phi_(0),
  xcore_(0),
  ycore_(0),
  xcoreDet_(0),
  ycoreDet_(0),
  xrange_(0),
  yrange_(0),
  nmax_(0),
  xmax_(0),
  x0_(0),
  a_(0),
  b_(0),
  c_(0),
  chi2ndf_(0),
  height_(0),
  nGround_(0),
  nPond_(0),
  tOffset_(0),
  firstIntZ_(0),
  nUsed_(0),
  rWgt_(-1.),
  wgtSim_(-1.),
  evtNum_(0),
  inEvtNum_(0),
  nPMTs_(0),
  nPcls_(0)
{
}

std::ostream&
operator<<(std::ostream& os, const SimEventHeader& g)
{
  os <<   "Particle ID ....... " << g.particleId_
     << "\nTotal Energy ...... " << g.energy_/TeV << " TeV"
     << "\nTheta, Phi ........ " << g.theta_/degree << ", "
                                 << g.phi_/degree << " degree"
     << "\nCore (X,Y) ........ " << g.xcore_/meter << ", "
                                 << g.ycore_/meter << " meter"
     << "\nHeight ............ " << g.height_/cm << " cm"
     << "\nN_ground .......... " << g.nGround_
     << "\nN_pond ............ " << g.nPond_
     << "\nTime Offset ....... " << g.tOffset_/ns << " ns"
     << "\nEM particles ...... " << g.emShower_.nPcls_ 
     << "\nEM energy ......... " << g.emShower_.energy_/TeV << " TeV"
     << "\nMu particles ...... " << g.muonShower_.nPcls_
     << "\nMu energy ......... " << g.muonShower_.energy_/TeV << " TeV"
     << "\nHadron particles .. " << g.hadronShower_.nPcls_
     << "\nHadron energy ..... " << g.hadronShower_.energy_/TeV << " TeV"
     << "\nN_PMTs ............ " << g.nPMTs_
     << "\nN_Particles ....... " << g.nPcls_
     << "\nN_max ............. " << g.nmax_
     << "\nX_max ............. " << g.xmax_/(gram/cm2) << " g/cm2"
     << "\nX_0 ............... " << g.x0_/(gram/cm2) << " g/cm2"
     << "\nA ................. " << g.a_/(gram/cm2)  << " g/cm2"
     << "\nB ................. " << g.b_
     << "\nC ................. " << g.c_/(cm2/gram) << " cm2/g"
     << "\nchi2/ndf .......... " << g.chi2ndf_
     << "\nage ............... " << g.age_;

  return os;
}

