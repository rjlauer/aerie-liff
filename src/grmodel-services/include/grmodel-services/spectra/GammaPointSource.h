
#ifndef GRMODEL_SERVICES_SPECTRA_GAMMAPOINTSOURCE_H_INCLUDED
#define GRMODEL_SERVICES_SPECTRA_GAMMAPOINTSOURCE_H_INCLUDED

#include <grmodel-services/spectra/PointSource.h>

#include <hawcnest/Service.h>

/*!
 * @class GammaPointSource
 * @author Segev BenZvi
 * @date 26 May 2011
 * @ingroup event_generators
 * @brief Encapsulate a point source with an energy spectrum from the
 *        CosmicRayService
 */
class GammaPointSource : public PointSource {

  public:

    typedef PointSource Interface;

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

  private:

    /// Service used to provide a light curve
    std::string lightCurveName_;

};


#endif // GRMODEL_SERVICES_SPECTRA_GAMMAPOINTSOURCE_H_INCLUDED

