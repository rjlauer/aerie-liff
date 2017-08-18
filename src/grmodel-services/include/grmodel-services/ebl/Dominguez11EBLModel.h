/*!
 * @file Dominguez11EBLModel.h
 * @brief EBL model due to A. Dominguez et al., MNRAS 410:2011, 2556.
 * @author Segev BenZvi
 * @date 24 Mar 2012
 * @version $Id: Dominguez11EBLModel.h 19516 2014-04-16 17:14:36Z sybenzvi $
 */

#ifndef GRMODEL_SERVICES_EBL_DOMINGUEZ11_EBLMODEL_H_INCLUDED
#define GRMODEL_SERVICES_EBL_DOMINGUEZ11_EBLMODEL_H_INCLUDED

#include <grmodel-services/ebl/EBLAbsorptionService.h>

#include <hawcnest/Service.h>

/*!
 * @class Dominguez11EBLModel
 * @author Segev BenZvi
 * @date 24 Mar 2012
 * @ingroup gr_models
 * @brief EBL model due to A. Dominguez et al., MNRAS 410:2011, 2556
 */
class Dominguez11EBLModel : public EBLAbsorptionService {

  public:

    typedef EBLAbsorptionService Interface;

    Configuration DefaultConfiguration();

    void Initialize(const Configuration& config);

    void Finish() { }

    /// Calculate the optical depth for a gamma-ray of energy E from redshift z
    double GetOpticalDepth(const double E, const double z,
                           const ErrorContour uc = CENTRAL) const;

};

#endif // GRMODEL_SERVICES_EBL_DOMINGUEZ11_EBLMODEL_H_INCLUDED

