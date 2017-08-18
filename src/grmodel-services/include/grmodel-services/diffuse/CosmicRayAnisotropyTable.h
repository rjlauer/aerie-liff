/*!
 * @file CosmicRayAnisotropyTable.h
 * @brief Container for a 3D cosmic ray anisotropy table
 * @author Segev BenZvi
 * @date 1 Jan 2013
 * @version $Id: CosmicRayAnisotropyTable.h 13668 2013-01-01 21:41:07Z sybenzvi $
 */

#ifndef GRMODEL_SERVICES_DIFFUSE_COSMICRAYANISOTROPYTABLE_H_INCLUDED
#define GRMODEL_SERVICES_DIFFUSE_COSMICRAYANISOTROPYTABLE_H_INCLUDED

#include <grmodel-services/diffuse/MapTable.h>

class EquPoint;

/*!
 * @class CosmicRayAnisotropyTable
 * @author Segev BenZvi
 * @date 1 Jan 2013
 * @ingroup gr_models
 * @brief A container for cosmic ray anisotropy tables as a function of energy
 *        and equatorial coordinates
 *
 * The CosmicRayAnisotropyTable stores the relative intensity PDF of the cosmic
 * ray anisotropy, including the large isotropic background.  It is stored in
 * FITS format; this class simply provides a wrapper for extracting the PDF as
 * a function of energy and sky position.
 */
class CosmicRayAnisotropyTable : public MapTable {

  public:

    CosmicRayAnisotropyTable(const std::string& filename);

    virtual ~CosmicRayAnisotropyTable() { }

    /// Get interpolated flux PDF at a fixed energy and equatorial position
    double GetPDF(const double E, const EquPoint& ep) const;

};

SHARED_POINTER_TYPEDEFS(CosmicRayAnisotropyTable);

#endif // GRMODEL_SERVICES_DIFFUSE_COSMICRAYANISOTROPYTABLE_H_INCLUDED

