/*!
 * @file GALPROPMapTable.h
 * @brief Container for a 3D GALPROP flux table
 * @author Segev BenZvi
 * @date 26 Dec 2012
 * @version $Id: GALPROPMapTable.h 13653 2012-12-27 23:17:51Z sybenzvi $
 */

#ifndef GRMODEL_SERVICES_DIFFUSE_GALPROPMAPTABLE_H_INCLUDED
#define GRMODEL_SERVICES_DIFFUSE_GALPROPMAPTABLE_H_INCLUDED

#include <grmodel-services/diffuse/MapTable.h>

class GalPoint;
class RNGService;

/*!
 * @class GALPROPMapTable
 * @author Segev BenZvi
 * @date 26 Dec 2012
 * @ingroup gr_models
 * @brief A container for gamma-ray flux tables produced by GALPROP
 *
 * A GALPROPMapTable stores flux data to be read from a FITS file as a function
 * of gamma-ray energy, galactic latitude, and galactic longitude.  For
 * example, the Milagro diffuse maps produced by Andy Strong:
 *
 * <ol>
 * <li><a href="http://www.mpe.mpg.de/~aws/galprop_private/ics_isotropic_skymap_54_milagroCS.gz">Inverse Compton Scattering</a></li>
 * <li><a href="http://www.mpe.mpg.de/~aws/galprop_private/pion_decay_skymap_54_milagroCS.gz">Pion Decay</a></li>
 * <li><a href="http://www.mpe.mpg.de/~aws/galprop_private/bremss_skymap_54_milagroCS.gz">Bremsstrahlung</a></li>
 * </ol>
 *
 * These FITS tables contain galactic maps with E<sup>2</sup>&times;Flux in 35
 * energy bins between 100 MeV and 100 TeV.  The base units of the flux maps
 * are cm<sup>-2</sup>s<sup>-1</sup>sr<sup>-1</sup>MeV.  This class converts
 * the data table into particle flux.
 */
class GALPROPMapTable : public MapTable {

  public:

    GALPROPMapTable(const std::string& filename);

    virtual ~GALPROPMapTable() { }

    /// Get interpolated flux at a fixed energy and Galactic position
    double GetFlux(const double E, const GalPoint& gp) const;

    /// Get the maximum flux at a fixed energy
    double GetMaxFlux(const double E) const;

    /// Sample a random energy between E0 and E1 at a Galactic position
    double GetRandomEnergy(const RNGService& rng,
                           const double E0, const double E1,
                           const GalPoint& g) const;

    /// Integrate the flux at some Galactic position
    double Integrate(const double E0, const double E1, const GalPoint& g) const;

};

SHARED_POINTER_TYPEDEFS(GALPROPMapTable);

#endif // GRMODEL_SERVICES_DIFFUSE_GALPROPMAPTABLE_H_INCLUDED

