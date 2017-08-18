/*!
 * @file SplineTable.h
 * @brief Wrappers for N-dimensional B-spline tables.
 * @author Segev BenZvi
 * @date 10 Jul 2012
 * @version $Id: SplineTable.h 13185 2012-11-10 15:46:03Z sybenzvi $
 */

#ifndef DATACLASSES_MATH_SPLINETABLE_H_INCLUDED
#define DATACLASSES_MATH_SPLINETABLE_H_INCLUDED

#include <photospline/core/splinetable.h>

#include <hawcnest/PointerTypedefs.h>

#include <string>

/*!
 * @class SplineTable
 * @brief Storage class for N-dimensional spline surfaces in FITS
 * @author Segev BenZvi
 * @ingroup math
 * @date 10 Jul 2012
 * @version $Id: SplineTable.h 13185 2012-11-10 15:46:03Z sybenzvi $
 *
 * This class reads spline coefficient tables from on-disk FITS files.  Based
 * on the I3SplineTable class in icetray.  The FITS tables are laid out as
 * follows:
 *
 * Header keys:
 * <ul>
 *   <li>TYPE: "Spline Coefficient Table"</li>
 *   <li>ORDERn: order of B-splines on the n-th axis</li>
 *   <li>PERIODn: periodicity of n-th axis, or 0 if not a periodic basis</li>
 *   <li>BIAS: logarithm offset (optional)</li>
 *   <li>GEOMETRY: photonics geometry (optional)</li>
 * </ul>
 *
 * Images:
 * <ul>
 *   <li>Primary: N-dimensional array of spline coefficients</li>
 *   <li>KNOTSn: Vector of knot locations on axis n</li>
 *   <li>EXTENTS: 2-D array of table boundaries (optional)</li>
 * </ul>
 */
class SplineTable {

  public:

    SplineTable(const std::string& fitsFile);

    virtual ~SplineTable();

    /// Get number of dimensions in the B-spline table
    int GetNDimensions() const { return table_.ndim; }

    /// Get the spline order of dimension i
    int GetOrder(const int i) const;

    /// Get the extents (min/max limits) of the table along dimension i
    void GetExtent(const int i, double& emin, double& emax) const;

    /// Evaluate the B-spline at a location vector x, returning status
    int Eval(double* x, double* result);

    /// Evaluate the B-spline at a location vector x, returning result
    double Eval(double* x);

  private:
    
    struct splinetable table_;

};

SHARED_POINTER_TYPEDEFS(SplineTable);

#endif // DATACLASSES_MATH_SPLINETABLE_H_INCLUDED

