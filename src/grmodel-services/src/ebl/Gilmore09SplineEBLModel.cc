/*!
 * @file Gilmore09SplineEBLModel.cc
 * @brief Implementation of EBL from Gilmore et al., 2009, using splines.
 * @author Segev BenZvi
 * @date 13 Jul 2012
 * @version $Id: Gilmore09SplineEBLModel.cc 19511 2014-04-16 15:55:08Z sybenzvi $
 */

#include <grmodel-services/ebl/Gilmore09SplineEBLModel.h>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/Logging.h>
#include <hawcnest/RegisterService.h>

#include <cmath>

using namespace std;
using namespace HAWCUnits;

REGISTER_SERVICE(Gilmore09SplineEBLModel);

Configuration
Gilmore09SplineEBLModel::DefaultConfiguration()
{
  Configuration config;
  config.Parameter<string>("coeffsFile", "");
  return config;
}

void
Gilmore09SplineEBLModel::Initialize(const Configuration& config)
{
  string coeffsFile;
  config.GetParameter("coeffsFile", coeffsFile);
  splineTable_ = boost::make_shared<SplineTable>(coeffsFile);
}

double
Gilmore09SplineEBLModel::GetOpticalDepth(const double E, const double z,
                                   const ErrorContour /*uc*/)
  const
{
  double tau = 0.;

  if (z < 0.01) {
    tau = 0.;
  }
  else if (z > 3. || E < 1*GeV || E > 100*TeV) {
    log_fatal("(z,E) = (" << z << "," << E/TeV 
              << ") not in range 0:3, 0.001:100 TeV");
  }
  else {
    double coords[2] = { log10(E/TeV), log10(z) };
    if (splineTable_->Eval(coords, &tau) != 0)
      log_fatal("Invalid evaluation.");
    tau = pow(10., tau);
  }

  return tau;
}

