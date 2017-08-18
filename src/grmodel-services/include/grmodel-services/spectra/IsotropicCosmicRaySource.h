/*!
 * @file IsotropicCosmicRaySource.h
 * @brief An isotropic source of cosmic rays.
 * @author Segev BenZvi
 * @date 15 Nov 2012
 * @version $Id: IsotropicCosmicRaySource.h 27291 2015-10-20 14:19:55Z sybenzvi $
 */

#ifndef GRMODEL_SERVICES_SPECTRA_ISOTROPICCRSOURCE_H_INCLUDED
#define GRMODEL_SERVICES_SPECTRA_ISOTROPICCRSOURCE_H_INCLUDED

#include <grmodel-services/spectra/CosmicRaySource.h>

#include <hawcnest/Service.h>

/*!
 * @class IsotropicCosmicRaySource
 * @author Segev BenZvi
 * @date 15 Nov 2012
 * @ingroup gr_models
 * @brief Model an isotropic source of cosmic rays from a particular particle
 *        species
 */
class IsotropicCosmicRaySource : public CosmicRaySource {

  public:

    typedef CosmicRaySource Interface;

    Configuration DefaultConfiguration();

    void Initialize(const Configuration& config);

    void Finish() { }

    double GetFlux(const double E, const ModifiedJulianDate& mjd) const;

    double GetFluxWeight(const double E, const ModifiedJulianDate& mjd, const PowerLaw& p) const;

    double GetMinEnergy() const;

    double GetMaxEnergy() const;

    double GetRandomEnergy(const RNGService& rng,
                           const double E0, const double E1) const;

    double Integrate(const double E0, const double E1,
                     const ModifiedJulianDate& mjd) const;

};


#endif // GRMODEL_SERVICES_SPECTRA_ISOTROPICCRSOURCE_H_INCLUDED

