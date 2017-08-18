/*!
 * @file MultiSource.cc
 * @author Colas Riviere
 * @date Jul 2016
 * @brief Implementation of a storage class of multiple point and extended
 *        sources (locations, radii and spectral properties).
 *        Derived from MultiTF1PointSources by Segev BenZvi.
 * @version $Id: MultiSource.cc 37852 2017-02-24 21:21:41Z criviere $
 */

#include <liff/MultiSource.h>
#include <math.h>
#include <iostream>

using namespace std;

namespace threeML {


/// Create an empty list of point sources.
MultiSource::MultiSource() {
}

/// Add a point source to the internal list of sources, by copying
/// it from another MultiSource instance.
void
MultiSource::addPointSourceFrom(const MultiSource &sources, int i) {
  if (i >= sources.getNumberOfPointSources()) {
    log_fatal("Out of range");
  }
  pointSrcs_.push_back(sources.getPointSource(i));
  srcsTypes_.push_back(POINT);
}

/// Add an extended source to the internal list of sources, by copying
/// it from another MultiSource instance.
void
MultiSource::addExtendedSourceFrom(const MultiSource &sources, int i) {
  if (i >= sources.getNumberOfExtendedSources()) {
    log_fatal("Out of range");
  }
  extendedSrcs_.push_back(sources.getExtendedSource(i));
  srcsTypes_.push_back(EXTENDED);
}
  
TF1PointSource
MultiSource::getPointSource(int srcid) const {
  if(srcid>=pointSrcs_.size()) {
    log_fatal("Out of range: "<<srcid<<", "<<pointSrcs_.size());
  } else {
    log_debug("In range: "<<srcid<<", "<<pointSrcs_.size());
  }
  return pointSrcs_.at(srcid);
}

TF1ExtendedSource
MultiSource::getExtendedSource(int srcid) const {
  return extendedSrcs_.at(srcid);
}

void
MultiSource::describe()
const {
  for (ConstTF1PointSourceListIterator it = pointSrcs_.begin();
       it != pointSrcs_.end(); ++it)
    it->describe();
  for (ConstTF1ExtendedSourceListIterator it = extendedSrcs_.begin();
       it != extendedSrcs_.end(); ++it)
    it->describe();
}

bool
MultiSource::isInsideAnyExtendedSource(double j2000_ra, double j2000_dec)
const {
  for (ConstTF1ExtendedSourceListIterator it = extendedSrcs_.begin();
       it != extendedSrcs_.end(); ++it)
    if (it->isInsideAnyExtendedSource(j2000_ra, j2000_dec))
      return true;
  return false;
}

void
MultiSource::getPointSourcePosition(int srcid,
                                    double *j2000_ra,
                                    double *j2000_dec)
const {
  pointSrcs_.at(srcid).getPointSourcePosition(0, j2000_ra, j2000_dec);
}

void
MultiSource::setPointSourcePosition(int srcid,
                                    double j2000_ra,
                                    double j2000_dec) {
  pointSrcs_.at(srcid).setPointSourcePosition(0, j2000_ra, j2000_dec);
}

vector<double>
MultiSource::getPointSourceFluxes(int srcid,
                                  vector<double> energies)
const {
  return pointSrcs_.at(srcid).getPointSourceFluxes(0, energies);
}

vector<double>
MultiSource::getExtendedSourceFluxes(int srcid,
                                     double j2000_ra, double j2000_dec,
                                     vector<double> energies)
const {
  return extendedSrcs_.at(srcid).getExtendedSourceFluxes(0, j2000_ra, j2000_dec, energies);
}

string
MultiSource::getPointSourceName(int srcid)
const {
  return pointSrcs_.at(srcid).getPointSourceName(0);
}

string
MultiSource::getExtendedSourceName(int srcid)
const {
  return extendedSrcs_.at(srcid).getExtendedSourceName(0);
}

void
MultiSource::getExtendedSourceBoundaries(int srcid, double *j2000_ra_min,
                                         double *j2000_ra_max,
                                         double *j2000_dec_min,
                                         double *j2000_dec_max)
const {
  extendedSrcs_.at(srcid).getExtendedSourceBoundaries(0,
                                                      j2000_ra_min,
                                                      j2000_ra_max,
                                                      j2000_dec_min,
                                                      j2000_dec_max);
}

int
MultiSource::getTotalNumberOfSources() const {
  // A sanity check first
  if (pointSrcs_.size() + extendedSrcs_.size() != srcsTypes_.size()) {
    log_fatal("Inconsistent source sizes: "<<pointSrcs_.size()<<" + "<<extendedSrcs_.size()<<" != "<<srcsTypes_.size());
  } else {  
    log_debug("Consistent source sizes: "<<pointSrcs_.size()<<" + "<<extendedSrcs_.size()<<" == "<<srcsTypes_.size());
  }
  return int(pointSrcs_.size() + extendedSrcs_.size());
}

int
MultiSource::getPointSourceIndexFromTotalIndex(int srcid) const {
  if(srcid>=srcsTypes_.size()) {
    log_fatal("Out of range: "<<srcid<<" >= "<<srcsTypes_.size());
  }
  if (srcsTypes_.at(srcid) != POINT) {
    log_fatal("Source "<<srcid<<" is not a point source.");
  }
  int srcCount=-1;
  for (int i=0;i<=srcid;i++) {
    if (srcsTypes_.at(i) == POINT) {
      srcCount++;
    }
  }
  log_debug("Index of point source at global index "<<srcid<<": "<<srcCount);
  return srcCount;
}

int
MultiSource::getExtendedSourceIndexFromTotalIndex(int srcid) const {
  if(srcid>=srcsTypes_.size()) {
    log_fatal("Out of range: "<<srcid<<" >= "<<srcsTypes_.size());
  }
  if (srcsTypes_.at(srcid) != EXTENDED) {
    log_fatal("Source "<<srcid<<" is not a point source.");
  }
  int srcCount=-1;
  for (int i=0;i<=srcid;i++) {
    if (srcsTypes_.at(i) == EXTENDED) {
      srcCount++;
    }
  }
  log_debug("Index of extended source at global index "<<srcid<<": "<<srcCount);
  return srcCount;
}

void 
MultiSource::addSourceFrom(const MultiSource &sources, int i) {
  if (i >= sources.getTotalNumberOfSources()) {
    log_fatal("Out of range");
  }
  switch (sources.getTotalSourceType(i)) {
    case POINT:
      pointSrcs_.push_back(sources.getPointSource(sources.getPointSourceIndexFromTotalIndex(i)));
      srcsTypes_.push_back(POINT);
      break;
    case EXTENDED:
      extendedSrcs_.push_back(sources.getExtendedSource(sources.getExtendedSourceIndexFromTotalIndex(i)));
      srcsTypes_.push_back(EXTENDED);
      break;
    default:
      log_fatal("Unknown source type: "<<sources.getTotalSourceType(i));
  }
}

}
