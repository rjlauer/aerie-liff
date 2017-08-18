/*!
 * @file Franceschini08EBLModel.h
 * @brief EBL model due to A. Franceschini et al., A&A 487:2008, 837-852
 * @author Segev BenZvi
 * @date 27 Oct 2010
 * @version $Id: Franceschini08EBLModel.h 39656 2017-06-22 21:32:33Z rlauer $
 */

#ifndef GRMODEL_SERVICES_EBL_FRANCESCHINI08_EBLMODEL_H_INCLUDED
#define GRMODEL_SERVICES_EBL_FRANCESCHINI08_EBLMODEL_H_INCLUDED

#include <grmodel-services/ebl/EBLAbsorptionService.h>

#include <hawcnest/Service.h>

/*!
 * @class Franceschini08EBLModel
 * @author Segev BenZvi
 * @date 17 Apr 2014
 * @ingroup gr_models
 * @brief EBL model due to A. Franceschini et al., A&A 487:2008, 837-852
 */
class Franceschini08EBLModel : public EBLAbsorptionService {

  public:

    typedef EBLAbsorptionService Interface;

    Configuration DefaultConfiguration();

    void Initialize(const Configuration& config);

    void Finish() { }

    /// Calculate the optical depth for a gamma-ray of energy E from redshift z
    double GetOpticalDepth(const double E, const double z,
                           const ErrorContour uc = CENTRAL) const;

};


#endif // GRMODEL_SERVICES_EBL_FRANCESCHINI08_EBLMODEL_H_INCLUDED

