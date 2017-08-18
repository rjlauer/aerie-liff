/*!
 * @file LightCurve.cc
 * @brief A time-dependent light curve.
 * @author Segev BenZvi
 * @date 23 Jan 2011
 * @version $Id: LightCurve.cc 22113 2014-10-06 02:17:25Z sybenzvi $
 */

#include <grmodel-services/spectra/LightCurve.h>

#include <data-structures/time/ModifiedJulianDate.h>

using namespace std;

double
LightCurve::GetFlux(const ModifiedJulianDate& tMJD)
  const
{
  return GetFlux(tMJD.GetDate());
}

double
LightCurve::GetFluxFraction(const ModifiedJulianDate& tMJD)
  const
{
  return GetFluxFraction(tMJD.GetDate());
}

