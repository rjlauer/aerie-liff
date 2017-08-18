/*!
 * @file CosmicRayAnisotropyTable.cc
 * @brief Container for a 3D table of cosmic ray anisotropy
 * @author Segev BenZvi
 * @date 1 Jan 2013
 * @version $Id: CosmicRayAnisotropyTable.cc 13668 2013-01-01 21:41:07Z sybenzvi $
 */

#include <grmodel-services/diffuse/CosmicRayAnisotropyTable.h>

#include <rng-service/RNGService.h>

#include <data-structures/astronomy/EquPoint.h>
#include <data-structures/math/PowerLaw.h>

#include <hawcnest/HAWCUnits.h>

#include <cmath>

using namespace std;
using namespace HAWCUnits;

CosmicRayAnisotropyTable::CosmicRayAnisotropyTable(const std::string& filename) :
  MapTable(filename)
{
}

double
CosmicRayAnisotropyTable::GetPDF(const double E, const EquPoint& ep)
  const
{
  double coord[3] = {
    log10(E / TeV),
    ep.GetDec() / degree,
    fmod(360. + ep.GetRA() / degree, 360.)
  };

  return Interpolate(coord);
}

