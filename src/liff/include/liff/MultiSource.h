/*!
 * @file MultiSource.h
 * @author Segev BenZvi
 * @date 21 Sep 2015
 * @brief Storage class for spectrum input to likelihood fitter from one or
 *        more point or extended sources.
 * @version $Id: MultiSource.h 37852 2017-02-24 21:21:41Z criviere $
 */

#ifndef MULTI_SOURCE_H_INCLUDED
#define MULTI_SOURCE_H_INCLUDED

#include <string>
#include <vector>
#include <liff/ModelInterface.h>
#include <liff/Func1.h>
#include <liff/TF1PointSource.h>
#include <liff/TF1ExtendedSource.h>

namespace threeML {


class MultiSource;
SHARED_POINTER_TYPEDEFS(MultiSource);

/*
 * @class MultiSource
 * @brief Storage for a list of point and extended sources with associated
 *        spectra
 */
class MultiSource: public ModelInterface {
  public:

    enum sourcetype {POINT, EXTENDED};

    MultiSource();

    void addPointSource(TF1PointSource source) {
      pointSrcs_.push_back(source);
      srcsTypes_.push_back(POINT);
    }

    void addExtendedSource(TF1ExtendedSource source) {
      extendedSrcs_.push_back(source);
      srcsTypes_.push_back(EXTENDED);
    }

    /// Add a source to the internal lists of sources, by copying
    /// it from another MultiSource instance.
    void addPointSourceFrom(const MultiSource &sources, int i);
    void addExtendedSourceFrom(const MultiSource &sources, int i);

    /// Get the full TF1PointSource object for source srcid
    TF1PointSource getPointSource(int srcid) const;

    /// Get the full TF1ExtendedSource object for source srcid
    TF1ExtendedSource getExtendedSource(int srcid) const;

    /// Iterate through the list of point sources and describe them
    void describe() const;

    /// Empty source list
    void clear() { pointSrcs_.clear(); extendedSrcs_.clear(); }

    /// Number of point sources modeled in the list
    int getNumberOfPointSources() const { return int(pointSrcs_.size()); }

    /// Get the position of the source with ID srcid
    void getPointSourcePosition(int srcid, double *j2000_ra, double *j2000_dec) const;

    /// Set the position of the source with ID srcid
    void setPointSourcePosition(int srcid, double j2000_ra, double j2000_dec);

    /// Retrieve differential fluxes in units of MeV^-1 cm^-1 s^-1
    std::vector<double> getPointSourceFluxes(int srcid, std::vector<double> energies) const;

    std::string getPointSourceName(int srcid) const;

    /// Extended source interface
    /// Number of point sources modeled in the list
    int getNumberOfExtendedSources() const { return int(extendedSrcs_.size()); }

    std::vector<double> getExtendedSourceFluxes(int srcid, double j2000_ra,
                                                double j2000_dec, std::vector<double> energies) const;

    std::string getExtendedSourceName(int srcid) const;

    bool isInsideAnyExtendedSource(double j2000_ra, double j2000_dec) const;

    void getExtendedSourceBoundaries(int srcid, double *j2000_ra_min,
                                     double *j2000_ra_max,
                                     double *j2000_dec_min,
                                     double *j2000_dec_max) const;

    Func1Ptr getPointSourceSpectrum(int srcid) const {
      return pointSrcs_.at(srcid).getFunction();
    }

    Func1Ptr getExtendedSourceSpectrum(int srcid) const {
      return extendedSrcs_.at(srcid).getFunction();
    }
    
    double getExtendedSourceRadius(int srcid) const {
      return extendedSrcs_.at(srcid).getRadius();
    }

    void getExtendedSourcePosition(int srcid, double *j2000_ra, double *j2000_dec) const {
      return extendedSrcs_.at(srcid).getExtendedSourcePosition( 987654321, j2000_ra, j2000_dec);
    };

    /// Set the position of the source with ID srcid
    void setExtendedSourcePosition(int srcid, double j2000_ra, double j2000_dec) {
      extendedSrcs_.at(srcid).setSourcePosition(j2000_ra, j2000_dec);
    };

    /// Number of sources modeled in the list (point + extended)
    int getTotalNumberOfSources() const;

    sourcetype getTotalSourceType(int srcid) const {
      return srcsTypes_.at(srcid);
    }

    // Returns the index in pointSrcs_ of the point source at global index srcid
    // in srcsTypes_
    int getPointSourceIndexFromTotalIndex(int srcid) const;

    // Returns the index in extendedSrcs_ of the extended source at global index
    // srcid in srcsTypes_
    int getExtendedSourceIndexFromTotalIndex(int srcid) const;
    
    /// Add a source to the corresponding internal list of sources, by copying
    /// it from another MultiSource instance.
    void addSourceFrom(const MultiSource &sources, int i);

  private:
    
    typedef std::vector<TF1PointSource> TF1PointSourceList;
    typedef TF1PointSourceList::iterator TF1PointSourceListIterator;
    typedef TF1PointSourceList::const_iterator ConstTF1PointSourceListIterator;

    typedef std::vector<TF1ExtendedSource> TF1ExtendedSourceList;
    typedef TF1ExtendedSourceList::iterator TF1ExtendedSourceListIterator;
    typedef TF1ExtendedSourceList::const_iterator ConstTF1ExtendedSourceListIterator;

    // Vector of size pointSrcs_.size()+extendedSrcs_.size(), which indicates
    // the order of the sources, e.g. {POINT, EXTENDED, EXTENDED, POINT, POINT}.
    // CR: I don't really like this solution, as we must enforce explicitely
    // that srcsTypes_, pointSrcs_, and extendedSrcs_ stay sync'ed. Any idea?
    std::vector<sourcetype> srcsTypes_;

    TF1PointSourceList pointSrcs_;
    TF1ExtendedSourceList extendedSrcs_;

};

}

#endif // MULTI_SOURCE_H_INCLUDED
