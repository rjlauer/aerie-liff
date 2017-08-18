/*!
 * @file integrator.cc 
 * @brief Demonstrate the use of integration classes in the framework.
 * @author Segev BenZvi 
 * @date 19 Apr 2012 
 * @version $Id: integrator.cc 40057 2017-08-03 17:01:37Z sybenzvi $
 */

#include <data-structures/math/CutoffPowerLaw.h>
#include <data-structures/math/SpecialFunctions.h>
#include <data-structures/math/GQIntegrator.h>

#include <hawcnest/HAWCUnits.h>

#include <cmath>
#include <iostream>

using namespace HAWCUnits;
using namespace SpecialFunctions;
using namespace std;

// Functor that provides access to PowerLaw::Evaluate
class PLFunctor {
  public:
    PLFunctor(const PowerLaw& pl) : pl_(pl) { }
    double operator()(const double E) const
    { return pl_.Evaluate(E); }
  private:
    const PowerLaw& pl_;
};

int main()
{
  // Parameters of the Crab flux at TeV as measured by HESS
  // Units have been stripped out to prevent confusion
  const double E0 = 0.4 * TeV;
  const double E1 = 30. * TeV;
  const double A  = 3.76e-11 / (TeV*cm2*second);
  const double En = 1. * TeV;
  const double gm = -2.39;
  const double Ec = 14.3 * TeV;

  CutoffPowerLaw pl(E0, E1, A, En, gm, Ec);
  PLFunctor plf(pl);

  cout << "\nFlux integral between "
       << E0/TeV << " and " << E1/TeV << " TeV:"
       << endl;

  // Naive integration, equivalent to algorithm in hmc-analysis flux weighter
  // but without the EBL attenuation
  {
    const double logE0 = log10(E0);
    const double logE1 = log10(E1);
    const int nIntegrationBins = 10000;
    double logElo = 0;
    double logEhi = 0;
    double logEcenter = 0;

    double E, dE, dFdE, deltaF;
    double fluxIntegral = 0.;

    double fluxIntegralAlt = 0.;

    double log10Emin = log10(E0);
    double log10Emax = log10(E1);
    double Ecenter = 0.;
    double Elow;
    double Ehigh;
    double fluxIntegralCor = 0.;
    double log10dE = (log10Emax - log10Emin)/nIntegrationBins;

    for (int i = 0; i < nIntegrationBins; ++i) {
      // Note: there is an implicit float to integer cast bug here:
      logElo = logE0 + i*(logE1 - logE0)/nIntegrationBins;
      logEhi = logE0 + (i + 1)*(logE1 - logE0)/nIntegrationBins;
      logEcenter = 0.5*(logEhi + logElo);

      E = pow(10., logEcenter);
      dE = pow(10., logEhi) - pow(10., logElo);

      dFdE = pl.Evaluate(E);
      deltaF = dFdE * dE;

      fluxIntegral += deltaF;

      // Alternate integral with casting bug removed
      logElo = logE0 + i*log10dE;
      logEhi = logE0 + (i + 1)*log10dE;
      logEcenter = 0.5*(logEhi + logElo);

      E = pow(10., logEcenter);
      dE = pow(10., logEhi) - pow(10., logElo);

      dFdE = pl.Evaluate(E);
      deltaF = dFdE * dE;

      fluxIntegralAlt += deltaF;

      // Correct Riemann sum with bin centered in E, not log(E)
      Elow  = pow(10,log10Emin + i * log10dE );
      Ehigh = pow(10,log10Emin + (i+1) * log10dE );
      Ecenter = (Elow + Ehigh)*0.5;
      dE = Ehigh - Elow;
      dFdE = pl.Evaluate(Ecenter);
      deltaF = dFdE * dE;
      fluxIntegralCor += deltaF;

    }
    cout << "\nRiemann integral .............. " << fluxIntegral*cm2*second
         << " cm2*second"
         << "\nAlternate Riemann integral .... " << fluxIntegralAlt*cm2*second
         << " cm2*second"
         << "\nCorrected Riemann integral .... " << fluxIntegralCor *cm2*second
         << " cm2*second"
         << endl;
  }

  // Integrate cutoff power law on [E0, E1].
  // Use Gaussian quadrature; trapezoidal integrator will fail
  GQIntegrator<PLFunctor> iPL = MakeIntegrator<GQIntegrator>(plf);
  cout << "\nNumerical integral ............ "
       << iPL.Integrate(E0, E1, 1e-18)*cm2*second
       << " cm2*second"
       << endl;

  // Analytical solution to the integral.  Note the extra factor of
  // TeV^gm that shows up in the normalization constant because of the power of
  // the cutoff energy
  const double norm =
    A * pow(Ec, gm+1) / pow(TeV, gm) *
    (Gamma::G(gm+1, E0/Ec) - Gamma::G(gm+1, E1/Ec));
  cout << "Analytical solution ........... " << norm*cm2*second
       << " cm2*second"
       << endl;

  return 0;
}

