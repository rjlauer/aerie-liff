
#ifndef GRMODEL_SERVICES_EBL_EBLABSORPTIONSERVICE_H_INCLUDED
#define GRMODEL_SERVICES_EBL_EBLABSORPTIONSERVICE_H_INCLUDED

#include <cmath>

/*!
 * @class EBLAbsorptionService
 * @author Segev BenZvi
 * @date 27 Oct 2010
 * @ingroup gr_models
 * @brief Abstract interface to services which calculate the attenuation of
 *        gamma rays due to extragalactic background light
 */
class EBLAbsorptionService {

  public:

    /// Lower bound, central value, or upper bound of optical depth and
    /// attenuation for models with error contours.  Bounds should correspond
    /// to "one-sigma" 68% contours.
    enum ErrorContour {
      LOWER = 0,
      CENTRAL,
      UPPER,
    };

    virtual ~EBLAbsorptionService() { }

    /// Calculate the optical depth for a gamma-ray of energy E from redshift z
    virtual double GetOpticalDepth(const double E, const double z,
                                   const ErrorContour uc = CENTRAL) const = 0;

    /// Calculate attenuation e^-tau for a gamma of energy E from redshift z
    double GetAttenuation(const double E, const double z,
                          const ErrorContour uc = CENTRAL) const {
      if (z > 0.)
        return std::exp(-GetOpticalDepth(E, z, uc)); 
      else
        return 1.0;
    }

};

#endif // GRMODEL_SERVICES_EBL_EBLABSORPTIONSERVICE_H_INCLUDED

