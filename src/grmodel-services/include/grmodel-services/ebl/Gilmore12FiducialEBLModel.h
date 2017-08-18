/*!
 * @file Gilmore12FiducialEBLModel.h
 * @brief The WMAP5 fiducial model of EBL attenuation, Gilmore et al., 2012.
 * @author Segev BenZvi
 * @date 14 Apr 2014
 * @version $Id: Gilmore12FiducialEBLModel.h 19516 2014-04-16 17:14:36Z sybenzvi $
 */

#ifndef GRMODEL_SERVICES_EBL_GILMORE12_FIDUCIALEBLMODEL_H_INCLUDED
#define GRMODEL_SERVICES_EBL_GILMORE12_FIDUCIALEBLMODEL_H_INCLUDED

#include <grmodel-services/ebl/EBLAbsorptionService.h>

#include <hawcnest/Service.h>

/*!
 * @class Gilmore12FiducialEBLModel
 * @author Segev BenZvi
 * @date 14 Apr 2014
 * @ingroup gr_models
 * @brief EBL model due to R.C. Gilmore et al., MNRAS 422:2012, 3189-3207.
 *
 * This class implements the optical depth of gamma rays between 1 GeV and 100
 * TeV for redshifts between 0.01 and 9 published by Gilmore, Somerville,
 * Primack, and Dominguez in 2012 (see arXiv:1104.0671).  The authors produce
 * two models of the EBL:
 * <ol>
 * <li>A WMAP5 cosmology with fixed dust extintion parameters.</li>
 * <li>A WMAP5 cosmology with redshift-dependent dust extinction.</li>
 * </ol>
 * The class implements the second model, called the "WMAP5 fiducial model"
 * in the paper.  Data tables were taken from the text files posted at
 *
 * http://physics.ucsc.edu/~joel/EBLdata-Gilmore2012/
 */
class Gilmore12FiducialEBLModel : public EBLAbsorptionService {

  public:

    typedef EBLAbsorptionService Interface;

    Configuration DefaultConfiguration();

    void Initialize(const Configuration& config);

    void Finish() { }

    /// Calculate the optical depth for a gamma-ray of energy E from redshift z
    double GetOpticalDepth(const double E, const double z,
                           const ErrorContour uc = CENTRAL) const;

};


#endif // GRMODEL_SERVICES_EBL_GILMORE12_FIDUCIALEBLMODEL_H_INCLUDED

