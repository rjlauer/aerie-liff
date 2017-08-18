/*!
 * @file MapTable.h
 * @brief A multi-dimensional flux table (energy and map coordinates)
 * @author Segev BenZvi
 * @date 26 Dec 2012
 * @version $Id: MapTable.h 17916 2013-11-15 18:10:09Z bbaugh $
 */

#ifndef GRMODEL_SERVICES_DIFFUSE_MAPTABLE_H_INCLUDED
#define GRMODEL_SERVICES_DIFFUSE_MAPTABLE_H_INCLUDED

#include <hawcnest/PointerTypedefs.h>
#include <string>
/*!
 * @class MapTable
 * @author Segev BenZvi
 * @date 26 Dec 2012
 * @ingroup gr_models
 * @brief A multi-dimensional flux table read from a FITS file.
 *
 * The MapTable is meant to be read from a FITS file, and represents a particle
 * flux (or perhaps a scaled flux) as a function of sky coordinates.  An
 * arbitrary number of coordinate axes is possible; most common would be an
 * additional energy axis.
 */
class MapTable {

  public:

    MapTable(const std::string& filename);

    virtual ~MapTable();

    /// Return the total number of dimensions in the table
    int GetNDimensions() const { return ndim_; }

    /// Return the number of coordinate dimensions
    int GetNCoords() const { return ncoord_; }

    /// Get the axis length of dimension i
    int GetNCoordBins(const int i) const { return naxes_[i+1]; }

    /// Get the number of the bin of axis i containing value x
    int GetBin(const int i, const double x) const;

    /// Get the central value of the bin of axis i containing value x
    float GetBinCenter(const int i, const double x) const;

    /// Bin centers for axis i of the table
    const float* GetBinCenters(const int i) const { return xc_[i]; }

    /// Bin edges for axis i of the table
    const float* GetBinEdges(const int i) const { return xe_[i]; }

    /// Return the maximum value in the table.  If n>0 and coords>0, get the
    /// maximum of the sub-table at location coords and depth n in the table
    double GetMaximum(const double* coord=0, const int n=0) const;

    /// Get the value of the table at position coords (not interpolated)
    double GetTableValue(const double* coord) const;

    /// Get the interpolated density of the table at the location of coords.
    /// If n>0, interpolate a subtable starting at depth n.
    double Interpolate(const double* coord, const int n=0) const;

  protected:

    /// Extract table data from a FITS file
    int ReadFITS(const char* filename);

    /// Collapse a list of table indices to a single stride index
    int Index(int* indices) const;

    /// Deallocate resources used in table creation
    void Cleanup();

    int ndim_;                  ///< Number of dimensions in data table
    long* naxes_;               ///< Array of axis lengths for each dimension

    float* data_;               ///< Flux (maybe energy-weighted) vs E,th,ph,...
    unsigned long* strides_;    ///< Stride lengths to traverse each dimension

    int ncoord_;                ///< Number of coordinate dimensions (ndim-1)
    float** xc_;                ///< Axis coordinate centers
    float** xe_;                ///< Axis coordinate edges

};

SHARED_POINTER_TYPEDEFS(MapTable);

#endif // GRMODEL_SERVICES_DIFFUSE_MAPTABLE_H_INCLUDED

