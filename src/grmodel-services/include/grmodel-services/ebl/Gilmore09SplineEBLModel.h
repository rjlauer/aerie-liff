/*!
 * @file Gilmore09SplineEBLModel.h
 * @brief EBL model due to Gilmore et al., 2009, implemented in a spline table.
 * @author Segev BenZvi
 * @date 13 Jul 2012
 * @version $Id: Gilmore09SplineEBLModel.h 19511 2014-04-16 15:55:08Z sybenzvi $
 */

#ifndef GRMODEL_SERVICES_EBL_GILMORE09_SPLINEEBLMODEL_H_INCLUDED
#define GRMODEL_SERVICES_EBL_GILMORE09_SPLINEEBLMODEL_H_INCLUDED

#include <grmodel-services/ebl/EBLAbsorptionService.h>

#include <data-structures/math/SplineTable.h>

#include <hawcnest/Service.h>

/*!
 * @class Gilmore09SplineEBLModel
 * @author Segev BenZvi
 * @date 13 Jul 2012
 * @ingroup gr_models
 * @brief EBL model due to R.C. Gilmore et al., MNRAS 399 (2009) 1694
 */
class Gilmore09SplineEBLModel : public EBLAbsorptionService {

  public:

    typedef EBLAbsorptionService Interface;

    Configuration DefaultConfiguration();

    void Initialize(const Configuration& config);

    void Finish() { }

    /// Calculate the optical depth for a gamma-ray of energy E from redshift z
    double GetOpticalDepth(const double E, const double z,
                           const ErrorContour uc = CENTRAL) const;

  private:

    SplineTablePtr splineTable_;

};


#endif // GRMODEL_SERVICES_EBL_GILMORE09_SPLINEEBLMODEL_H_INCLUDED

