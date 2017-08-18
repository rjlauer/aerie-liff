/*!
 * @file MultiTF1PointSource.h
 * @author Segev BenZvi
 * @date 21 Sep 2015
 * @brief Storage class for spectrum input to likelihood fitter from one or
 *        more point sources.
 * @version $Id: MultiTF1PointSource.h 37694 2017-02-17 21:50:06Z criviere $
 */

#ifndef MULTI_TF1_POINT_SOURCE_H_INCLUDED
#define MULTI_TF1_POINT_SOURCE_H_INCLUDED

#include <string>
#include <vector>
#include <liff/ModelInterface.h>
#include <liff/Func1.h>
#include <liff/TF1PointSource.h>

namespace threeML {


class MultiTF1PointSource;
SHARED_POINTER_TYPEDEFS(MultiTF1PointSource);

/*
 * @class MultiTF1PointSource
 * @brief Storage for a list of point sources with associated spectra
 */
class MultiTF1PointSource: public ModelInterface {
  public:

    typedef std::vector<TF1PointSource> TF1PointSourceList;
    typedef TF1PointSourceList::iterator TF1PointSourceListIterator;
    typedef TF1PointSourceList::const_iterator ConstTF1PointSourceListIterator;

    MultiTF1PointSource();

    MultiTF1PointSource(const TF1PointSourceList &ps);

    /// Add a point source to the internal list of point sources.  Index
    /// according to the number of previously entered sources.
    void addSource(const std::string &name,
                   const double ra, const double dec,
                   const double norm, const double idx,
                   const double pivotE, const double cutoffE);
    /// Add a point source to the internal list of point sources with spectrum.
    void addSource(const std::string& name,
                   const double ra, const double dec,
                   Func1Ptr spectrum);
    /// Add a point source to the internal list of point sources from file.
    /// Index according to the number of previously entered sources.
    void addSourceFromFile(const std::string& name,
                           const double ra, const double dec,
                           const std::string& filename);

    /// Add a point source to the internal list of point sources, by copying
    /// it from another MultiTF1PointSource instance.
    void addSourceFrom(const MultiTF1PointSource &sources, int i);

    /// Get the full TF1PointSource object for source srcid
    TF1PointSource getPointSource(int srcid) const;

    /// Iterate through the list of point sources and describe them
    void describe() const;

    /// Empty source list
    void clear() { srcs_.clear(); }

    /// Number of point sources modeled in the list
    int getNumberOfPointSources() const { return int(srcs_.size()); }

    /// Get the position of the source with ID srcid
    void getPointSourcePosition(int srcid, double *j2000_ra, double *j2000_dec) const;

    /// Set the position of the source with ID srcid
    void setPointSourcePosition(int srcid, double j2000_ra, double j2000_dec);

    /// Retrieve differential fluxes in units of MeV^-1 cm^-1 s^-1
    std::vector<double> getPointSourceFluxes(int srcid, std::vector<double> energies) const;

    std::string getPointSourceName(int srcid) const;

    /// Extended source interface
    int getNumberOfExtendedSources() const;

    std::vector<double> getExtendedSourceFluxes(int srcid, double j2000_ra,
                                                double j2000_dec, std::vector<double> energies) const;

    std::string getExtendedSourceName(int srcid) const;

    bool isInsideAnyExtendedSource(double j2000_ra, double j2000_dec) const;

    void getExtendedSourceBoundaries(int srcid, double *j2000_ra_min,
                                     double *j2000_ra_max,
                                     double *j2000_dec_min,
                                     double *j2000_dec_max) const { }

    Func1Ptr getPointSourceSpectrum(int srcid) const {
      return srcs_[srcid].getFunction();
    }

  private:

    TF1PointSourceList srcs_;

};

}

#endif // MULTI_TF1_POINT_SOURCE_H_INCLUDED
