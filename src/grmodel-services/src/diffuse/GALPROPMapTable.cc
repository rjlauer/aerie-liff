/*!
 * @file GALPROPMapTable.cc
 * @brief Container for a GALPROP 3D table
 * @author Segev BenZvi
 * @date 26 Dec 2012
 * @version $Id: GALPROPMapTable.cc 13656 2012-12-28 02:07:04Z sybenzvi $
 */

#include <grmodel-services/diffuse/GALPROPMapTable.h>

#include <rng-service/RNGService.h>

#include <data-structures/astronomy/GalPoint.h>
#include <data-structures/math/PowerLaw.h>

#include <hawcnest/HAWCUnits.h>

#include <cmath>

using namespace std;
using namespace HAWCUnits;

GALPROPMapTable::GALPROPMapTable(const std::string& filename) :
  MapTable(filename)
{
  // Convert the internal data map from E^2 x Flux to log(Flux), which should
  // make interpolation between energy bins more sensible
  int indices[ndim_];
  memset(indices, 0, sizeof(indices));
  long* n = naxes_ + 1;

  float E2;
  float E2xF;
  for (int i = 0; i < n[0]; ++i) {
    E2 = pow(10, 2*xc_[0][i]);
    indices[1] = i;
    for (int j = 0; j < n[1]; ++j) {
      indices[2] = j;
      for (int k = 0; k < n[2]; ++k) {
        indices[3] = k;
        E2xF = data_[Index(indices)];
        data_[Index(indices)] = log10(E2xF / E2);
      }
    }
  }
}

double
GALPROPMapTable::GetFlux(const double E, const GalPoint& gp)
  const
{
  double coord[3] = {
    log10(E / MeV),
    gp.GetB() / degree,
    fmod(360. + gp.GetL() / degree, 360.)
  };

  double logF = Interpolate(coord);
  return pow(10, logF) / (cm2 * MeV * s * sr);
}

double
GALPROPMapTable::GetMaxFlux(const double E)
  const
{
  const double coord[1] = { log10(E/MeV) };
  double logF = GetMaximum(coord, 1);
  return pow(10, logF) / (cm2 * MeV * s * sr);
}

double
GALPROPMapTable::GetRandomEnergy(
  const RNGService& rng, const double E0, const double E1, const GalPoint& g)
  const
{
  // Start from a power law E^-2 and sample from that
  double E;
  static const double F0 = GetFlux(E0, g);
  static const PowerLaw pl(E0, E1, F0, E0, -2.);

  while (true) {
    E = rng.PowerLaw(-2., E0, E1);
    if (rng.Uniform() <= GetFlux(E, g) / pl.Evaluate(E))
      break;
  }

  return E;
}

double
GALPROPMapTable::Integrate(const double E0, const double E1, const GalPoint& g)
  const
{
  // Perform a Riemann sum of the spectrum at some Galactic position
  const int nInt = 10000;
  double Elo, Ehi, Ecenter, dE, dFdE, deltaF, iF = 0.;
  double logEmin = log10(E0);
  double logEmax = log10(E1);
  double logdE = (logEmax - logEmin) / nInt;

  for (int i = 0; i < nInt; ++i) {
    Elo = pow(10, logEmin + i*logdE);
    Ehi = pow(10, logEmin + (i+1)*logdE);
    Ecenter = 0.5*(Elo + Ehi);
    dE = Ehi - Elo;
    dFdE = GetFlux(Ecenter, g);
    deltaF = dFdE * dE;
    iF += deltaF;
  }

  return iF;
}

