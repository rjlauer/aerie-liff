/*!
 * @file Gilmore09EBLModel.h
 * @brief Implementation of the WMAP3 low EBL model due to Gilmore et al., 2009.
 * @author Segev BenZvi
 * @date 27 Oct 2010
 * @version $Id: Gilmore09EBLModel.h 39656 2017-06-22 21:32:33Z rlauer $
 */

#ifndef GRMODEL_SERVICES_EBL_GILMORE09_EBLMODEL_H_INCLUDED
#define GRMODEL_SERVICES_EBL_GILMORE09_EBLMODEL_H_INCLUDED

#include <grmodel-services/ebl/EBLAbsorptionService.h>

#include <hawcnest/Service.h>

/*!
 * @class Gilmore09EBLModel
 * @author Kathryne Sparks 
 * @author Segev BenZvi
 * @date 27 Oct 2010
 * @ingroup gr_models
 * @brief EBL model due to R.C. Gilmore et al., MNRAS 399:2009, 1694.
 *
 * This class calculates the optical depth of gamma rays between 1 GeV and 100
 * TeV for redshifts between 0.01 and 9 published by Gilmore, Madau,
 * Primack, Somerville, and Haardt in 2009 (see arXiv:0905.1144).  The authors
 * produce several models of the EBL:
 * <ol>
 * <li>A "fiducial" model of star formation rate using concordant cosmological
 * parameters.</li>
 * <li>A "low" model of the star formation rate using best-fit cosmological
 * parameters from WMAP3.</li>
 * <li></li>
 * </ol>
 * In this context, low refers to the lower RMS density fluctuation
 * \f$\sigma_8\f$ in the WMAP3 fit, and at large redshift it results in a
 * smaller optical depth than the fiducial model.  This class implements the 
 * "low" model (see Table 1 of Gilmore et al.).
 */
class Gilmore09EBLModel : public EBLAbsorptionService {

  public:

    typedef EBLAbsorptionService Interface;

    Configuration DefaultConfiguration();

    void Initialize(const Configuration& config);

    void Finish() { }

    /// Calculate the optical depth for a gamma-ray of energy E from redshift z
    double GetOpticalDepth(const double E, const double z,
                           const ErrorContour uc = CENTRAL) const;

};


#endif // GRMODEL_SERVICES_EBL_GILMORE09_EBLMODEL_H_INCLUDED

