/*!
 * @file Nutation.cc
 * @brief Calculate wobble of the Earth about its axis (nutation) using the
 *        1980 IAU Theory of Nutation as described in "Astronomical Algorithms,"
 *        J. Meeus, Willman-Bell 1998.
 * @author Segev BenZvi
 * @date 29 May 2010
 * @version $Id: Nutation.cc 21278 2014-08-05 18:34:03Z sybenzvi $
 */

#include <cmath>

#include <hawcnest/HAWCUnits.h>

#include <data-structures/geometry/Point.h>
#include <data-structures/astronomy/AstroCoords.h>
#include <data-structures/time/ModifiedJulianDate.h>

#include <astro-service/Nutation.h>

using namespace HAWCUnits;
using namespace std;

Rotate Nutation::nutMatrix_ = Rotate();

const Rotate&
Nutation::GetRotationMatrix(const ModifiedJulianDate& mjd)
{
  // Argument series needed to calculate nutation in longitude and obliquity,
  // from Table 22.A of Meeus.
  const static double nutArgs[63][5] = {
    { 0,  0,  0,  0,  1},
    {-2,  0,  0,  2,  2},
    { 0,  0,  0,  2,  2},
    { 0,  0,  0,  0,  2},
    { 0,  1,  0,  0,  0},
    { 0,  0,  1,  0,  0},
    {-2,  1,  0,  2,  2},
    { 0,  0,  0,  2,  1},
    { 0,  0,  1,  2,  2},
    {-2, -1,  0,  2,  2},
    {-2,  0,  1,  0,  0},
    {-2,  0,  0,  2,  1},
    { 0,  0, -1,  2,  2},
    { 2,  0,  0,  0,  0},
    { 0,  0,  1,  0,  1},
    { 2,  0, -1,  2,  2},
    { 0,  0, -1,  0,  1},
    { 0,  0,  1,  2,  1},
    {-2,  0,  2,  0,  0},
    { 0,  0, -2,  2,  1},
    { 2,  0,  0,  2,  2},
    { 0,  0,  2,  2,  2},
    { 0,  0,  2,  0,  0},
    {-2,  0,  1,  2,  2},
    { 0,  0,  0,  2,  0},
    {-2,  0,  0,  2,  0},
    { 0,  0, -1,  2,  1},
    { 0,  2,  0,  0,  0},
    { 2,  0, -1,  0,  1},
    {-2,  2,  0,  2,  2},
    { 0,  1,  0,  0,  1},
    {-2,  0,  1,  0,  1},
    { 0, -1,  0,  0,  1},
    { 0,  0,  2, -2,  0},
    { 2,  0, -1,  2,  1},
    { 2,  0,  1,  2,  2},
    { 0,  1,  0,  2,  2},
    {-2,  1,  1,  0,  0},
    { 0, -1,  0,  2,  2},
    { 2,  0,  0,  2,  1},
    { 2,  0,  1,  0,  0},
    {-2,  0,  2,  2,  2},
    {-2,  0,  1,  2,  1},
    { 2,  0, -2,  0,  1},
    { 2,  0,  0,  0,  1},
    { 0, -1,  1,  0,  0},
    {-2, -1,  0,  2,  1},
    {-2,  0,  0,  0,  1},
    { 0,  0,  2,  2,  1},
    {-2,  0,  2,  0,  1},
    {-2,  1,  0,  2,  1},
    { 0,  0,  1, -2,  0},
    {-1,  0,  1,  0,  0},
    {-2,  1,  0,  0,  0},
    { 1,  0,  0,  0,  0},
    { 0,  0,  1,  2,  0},
    { 0,  0, -2,  2,  2},
    {-1, -1,  1,  0,  0},
    { 0,  1,  1,  0,  0},
    { 0, -1,  1,  2,  2},
    { 2, -1, -1,  2,  2},
    { 0,  0,  3,  2,  2},
    { 2, -1,  0,  2,  2}
  };

  // Coefficients of sin and cos of arguments above, Meeus table 22.A
  const static double nutCoef[63][4] = {
    {-171996, -174.2, 92025,    8.9},
    { -13187,   -1.6,  5736,   -3.1},
    {  -2274,   -0.2,   977,   -0.5},
    {   2062,    0.2,  -895,    0.5},
    {   1426,   -3.4,    54,   -0.1},
    {    712,    0.1,    -7,    0},
    {   -517,    1.2,   224,   -0.6},
    {   -386,   -0.4,   200,    0},
    {   -301,    0,     129,   -0.1},
    {    217,   -0.5,   -95,    0.3},
    {   -158,    0,       0,    0},
    {    129,    0.1,   -70,    0},
    {    123,    0,     -53,    0},
    {     63,    0,       0,    0},
    {     63,    0.1,   -33,    0},
    {    -59,    0,      26,    0},
    {    -58,   -0.1,    32,    0},
    {    -51,    0,      27,    0},
    {     48,    0,       0,    0},
    {     46,    0,     -24,    0},
    {    -38,    0,      16,    0},
    {    -31,    0,      13,    0},
    {     29,    0,       0,    0},
    {     29,    0,     -12,    0},
    {     26,    0,       0,    0},
    {    -22,    0,       0,    0},
    {     21,    0,     -10,    0},
    {     17,   -0.1,     0,    0},
    {     16,    0,      -8,    0},
    {    -16,    0.1,     7,    0},
    {    -15,    0,       9,    0},
    {    -13,    0,       7,    0},
    {    -12,    0,       6,    0},
    {     11,    0,       0,    0},
    {    -10,    0,       5,    0},
    {     -8,    0,       3,    0},
    {      7,    0,      -3,    0},
    {     -7,    0,       0,    0},
    {     -7,    0,       3,    0},
    {     -7,    0,       3,    0},
    {      6,    0,       0,    0},
    {      6,    0,      -3,    0},
    {      6,    0,      -3,    0},
    {     -6,    0,       3,    0},
    {     -6,    0,       3,    0},
    {      5,    0,       0,    0},
    {     -5,    0,       3,    0},
    {     -5,    0,       3,    0},
    {     -5,    0,       3,    0},
    {      4,    0,       0,    0},
    {      4,    0,       0,    0},
    {      4,    0,       0,    0},
    {     -4,    0,       0,    0},
    {     -4,    0,       0,    0},
    {     -4,    0,       0,    0},
    {      3,    0,       0,    0},
    {     -3,    0,       0,    0},
    {     -3,    0,       0,    0},
    {     -3,    0,       0,    0},
    {     -3,    0,       0,    0},
    {     -3,    0,       0,    0},
    {     -3,    0,       0,    0},
    {     -3,    0,       0,    0}
  };

  // Time interval (in centuries) between J2000 and the current epoch
  const double T = (mjd.GetDate(TT)/day - 51544.5) / 36525.;
  const double T2 = T*T;
  const double T3 = T2*T;

  // Mean elongation of the Moon from the Sun, Meeus p. 144
  const double D = (297.85036 + 445267.11148*T - 0.0019142*T2 + T3/189474.)*deg;

  // Mean anomaly of the Sun from the Earth, Meeus p. 144
  const double M = (357.52772 + 35999.05034*T - 0.0001603*T2 - T3/300000.)*deg;

  // Mean anomaly of the Moon, Meeus p. 144
  const double M_ = (134.96298 + 477198.867398*T + 0.0086972*T2 + T3/56250)*deg;

  // Lunar argument of latitude, Meeus p. 144
  const double F = (93.27191 + 483202.017538*T - 0.0036825*T2 + T3/327270.)*deg;

  // Longitude of the ascending node of the Moon's mean orbit on the ecliptic,
  // measured from the mean equinox of the date, Meeus p. 144
  const double W = (125.04452 - 1934.136261*T + 0.0020708*T2 + T3/450000.)*deg;

  // Calculate the sum of the terms in table 22.A of Meeus, p. 145 to get the
  // nutations in longitude (dpsi) and obliquity (deps)
  double dpsi = 0;
  double deps = 0;
  double args = 0;
  double ccos = 0;
  double csin = 0;
  for (int i = 0; i < 63; ++i) {
    args = nutArgs[i][0] * D +
           nutArgs[i][1] * M +
           nutArgs[i][2] * M_ +
           nutArgs[i][3] * F +
           nutArgs[i][4] * W;
    csin = nutCoef[i][0] + nutCoef[i][1]*T;
    ccos = nutCoef[i][2] + nutCoef[i][3]*T;

    dpsi += csin * sin(args);
    deps += ccos * cos(args);
  }
  // Don't forget to convert to units of table 22.A (0.0001")
  dpsi *= 1e-4*arcsec;
  deps *= 1e-4*arcsec;

  // Calculate the mean obliquity using the function by J. Laskar, A&A 157:68,
  // 1986 and given in Meeus eq. 22.3, p. 147
  const double U = 1e-2 * T;
  const double eps0 = ((23*3600 + 26*60 + 21.448) +
      U*(-4680.93 + U*(   -1.55 + U*( 1999.25 + U*(  -51.38 + U*( -249.67 +
      U*(  -39.05 + U*(    7.12 + U*(   27.87 + U*(    5.79 +
      U*2.45)))))))))) * arcsec;
  double eps = eps0 + deps;

  // Nutation matrix
  nutMatrix_ =
    R3Transform(1.,            -dpsi*cos(eps),      -dpsi*sin(eps), 0,
                dpsi*cos(eps),  1.,                 -deps,          0,
                dpsi*sin(eps),  deps,                1.,            0);

  return nutMatrix_;
}

