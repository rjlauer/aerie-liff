/*!
 * @file MultiTF1PointSource.cc
 * @author Segev BenZvi
 * @date 21 Sep 2015
 * @brief Implementation of a storage class of multiple point sources
 *        (locations and spectral properties).
 * @version $Id: MultiTF1PointSource.cc 40126 2017-08-10 19:29:00Z criviere $
 */

#include <liff/MultiTF1PointSource.h>
#include <math.h>
#include <iostream>

using namespace std;

namespace threeML {


/// Create an empty list of point sources.
MultiTF1PointSource::MultiTF1PointSource() {
}

/// Initialize the list of point sources from an existing list created
/// elsewhere by the user.
MultiTF1PointSource::MultiTF1PointSource(const TF1PointSourceList &ps) :
    srcs_(ps) {
}

void
MultiTF1PointSource::addSource(const string &name,
                               const double ra, const double dec,
                               const double norm, const double index,
                               const double pivotE, const double cutoffE) {
  unsigned id = srcs_.size();
  Func1Ptr spectrum = Func1Ptr(new Func1(Form("spectrum_id%d", id), "[0]*pow(x/[3],-[1])*exp(-x/[2])", 0., 1e10));
  spectrum->SetParameterName(0, "Norm");
  spectrum->SetParameter(0, norm);
  spectrum->SetParameterError(0, norm / 2.);
  double normMin = 0;
  double normMax = 1e-5;
  spectrum->SetParameterBounds(0, normMin, normMax);
  spectrum->SetParameterName(1, "Index");
  spectrum->SetParameter(1, index);
  spectrum->SetParameterError(1, 0.1);
  spectrum->SetParameterBounds(1, 1., 10.);
  spectrum->SetParameterName(2, "CutOff");
  spectrum->SetParameter(2, cutoffE);
  spectrum->SetParameterError(2, cutoffE / 2.);
  spectrum->SetParameterBounds(2, 0., 1.e10);
  spectrum->SetParameterName(3, "Pivot");
  spectrum->SetParameter(3, pivotE);
  spectrum->SetParameterError(3, 0);
  spectrum->SetParameterBounds(3, pivotE, pivotE);

  srcs_.push_back(TF1PointSource(name, ra, dec, spectrum));
}

void
MultiTF1PointSource::addSource(const string& name,
                               const double ra, const double dec,
                               Func1Ptr spectrum) {
  srcs_.push_back(TF1PointSource(name, ra, dec, spectrum));
}

/// Add a point source to the internal list of point sources, by copying
/// it from another MultiTF1PointSource instance.
void
MultiTF1PointSource::addSourceFrom(const MultiTF1PointSource &sources, int i) {
  if (i >= sources.getNumberOfPointSources()) {
    log_fatal("Out of range");
  }
  srcs_.push_back(sources.getPointSource(i));
}
  
  // Add a point source to the list of point source from a file
void
MultiTF1PointSource::addSourceFromFile(const string& name,
                                       const double ra, const double dec,
                                       const string& filename)
{
  log_info("Adding spectrum from file " << filename << " for source: " << name);
  srcs_.push_back(TF1PointSource(name, ra, dec, filename));
}

TF1PointSource
MultiTF1PointSource::getPointSource(int srcid) const {
  return srcs_.at(srcid);
}

void
MultiTF1PointSource::describe()
const {
  for (ConstTF1PointSourceListIterator it = srcs_.begin(); it != srcs_.end(); ++it)
    it->describe();
}

bool
MultiTF1PointSource::isInsideAnyExtendedSource(double j2000_ra, double j2000_dec)
const {
  return false;
}

void
MultiTF1PointSource::getPointSourcePosition(int srcid,
                                            double *j2000_ra,
                                            double *j2000_dec)
const {
  srcs_[srcid].getPointSourcePosition(srcid, j2000_ra, j2000_dec);
}

void
MultiTF1PointSource::setPointSourcePosition(int srcid,
                                            double j2000_ra,
                                            double j2000_dec) {
  srcs_[srcid].setPointSourcePosition(srcid, j2000_ra, j2000_dec);
}

vector<double>
MultiTF1PointSource::getPointSourceFluxes(int srcid,
                                          vector<double> energies)
const {
  return srcs_[srcid].getPointSourceFluxes(srcid, energies);
}

int
MultiTF1PointSource::getNumberOfExtendedSources() const {
  return 0;
}

vector<double>
MultiTF1PointSource::getExtendedSourceFluxes(int srcid,
                                             double j2000_ra, double j2000_dec,
                                             vector<double> energies)
const {
  /*
  return srcs_[srcid].getExtendedSourceFluxes(srcid, j2000_ra, j2000_dec, energies);
  */
  return vector<double>();
}

string
MultiTF1PointSource::getPointSourceName(int srcid)
const {
  return srcs_[srcid].getPointSourceName(srcid);
}

string
MultiTF1PointSource::getExtendedSourceName(int srcid)
const {
  string name("test");
  return name;
}

}
