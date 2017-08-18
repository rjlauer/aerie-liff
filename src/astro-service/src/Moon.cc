
#include <astro-service/Moon.h>

#include <data-structures/geometry/Ellipsoid.h>
#include <data-structures/geometry/Point.h>
#include <data-structures/astronomy/AstroCoords.h>
#include <data-structures/time/ModifiedJulianDate.h>

#include <hawcnest/HAWCUnits.h>

#include <cmath>
#include <numeric>

using namespace HAWCUnits;
using namespace std;

// This function and constants are adapted from the sla_DMOON subroutine of the
// SLALIB positional astronomy library, which is available under the GPLv2:
// 
// Copyright P.T.Wallace.  All rights reserved.
//
// License:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program (see SLA_CONDITIONS); if not, write to the
//   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
//   Boston, MA  02111-1307  USA
//
namespace {

  // Julian epoch of B1950
  const double B1950 = 1949.9997904423;

  // Mean lunar longitude
  const double elp0 = 270.434164;
  const double elp1 = 481267.8831;
  const double elp2 = -0.001133;
  const double elp3 = 0.0000019;

  // Mean solar anomaly
  const double em0 = 358.475833;
  const double em1 = 35999.0498;
  const double em2 = -0.000150;
  const double em3 = -0.0000033;

  // Mean lunar anomaly
  const double emp0 = 296.104608;
  const double emp1 = 477198.8491;
  const double emp2 = 0.009192;
  const double emp3 = 0.0000144;

  // Mean lunar elongation
  const double d0 = 350.737486;
  const double d1 = 445267.1142;
  const double d2 = -0.001436;
  const double d3 = 0.0000019;

  // Mean distance of the moon from its ascending node
  const double f0 = 11.250889;
  const double f1 = 483202.0251;
  const double f2 = -0.003211;
  const double f3 = -0.0000003;

  // Lunar longitude of the ascending node
  const double om0 = 259.183275;
  const double om1 = -1934.1420;
  const double om2 = 0.002078;
  const double om3 = 0.0000022;

  // Coefficients for (dimensionless) E factor */
  const double e1 = -0.002495;
  const double e2 = -0.00000752;

  // coefficients for periodic variations. */
  const double pac = 0.000233;
  const double pa0 = 51.2;
  const double pa1 = 20.2;

  const double pbc = -0.001778;
  const double pcc = 0.000817;
  const double pdc = 0.002011;

  const double pec = 0.003964;
  const double pe0 = 346.560;
  const double pe1 = 132.870;
  const double pe2 = -0.0091731;

  const double pfc = 0.001964;
  const double pgc = 0.002541;
  const double phc = 0.001964;
  const double pic = -0.024691;

  const double pjc = -0.004328;
  const double pj0 = 275.05;
  const double pj1 = -2.30;

  const double cw1 = 0.0004664;
  const double cw2 = 0.0000754;

  // Longitude series expansion coefficients
  const int NL = 50;
  const double TL[NL][6] = {
    //  coeff       M    M'   D    F    n
    {  6.288750,    0,   1,   0,   0,   0 },
    {  1.274018,    0,  -1,   2,   0,   0 },
    {  0.658309,    0,   0,   2,   0,   0 },
    {  0.213616,    0,   2,   0,   0,   0 },
    { -0.185596,    1,   0,   0,   0,   1 },
    { -0.114336,    0,   0,   0,   2,   0 },
    {  0.058793,    0,  -2,   2,   0,   0 },
    {  0.057212,   -1,  -1,   2,   0,   1 },
    {  0.053320,    0,   1,   2,   0,   0 },
    {  0.045874,   -1,   0,   2,   0,   1 },
    {  0.041024,   -1,   1,   0,   0,   1 },
    { -0.034718,    0,   0,   1,   0,   0 },
    { -0.030465,    1,   1,   0,   0,   1 },
    {  0.015326,    0,   0,   2,  -2,   0 },
    { -0.012528,    0,   1,   0,   2,   0 },
    { -0.010980,    0,  -1,   0,   2,   0 },
    {  0.010674,    0,  -1,   4,   0,   0 },
    {  0.010034,    0,   3,   0,   0,   0 },
    {  0.008548,    0,  -2,   4,   0,   0 },
    { -0.007910,    1,  -1,   2,   0,   1 },
    { -0.006783,    1,   0,   2,   0,   1 },
    {  0.005162,    0,   1,  -1,   0,   0 },
    {  0.005000,    1,   0,   1,   0,   1 },
    {  0.004049,   -1,   1,   2,   0,   1 },
    {  0.003996,    0,   2,   2,   0,   0 },
    {  0.003862,    0,   0,   4,   0,   0 },
    {  0.003665,    0,  -3,   2,   0,   0 },
    {  0.002695,   -1,   2,   0,   0,   1 },
    {  0.002602,    0,   1,  -2,  -2,   0 },
    {  0.002396,   -1,  -2,   2,   0,   1 },
    { -0.002349,    0,   1,   1,   0,   0 },
    {  0.002249,   -2,   0,   2,   0,   2 },
    { -0.002125,    1,   2,   0,   0,   1 },
    { -0.002079,    2,   0,   0,   0,   2 },
    {  0.002059,   -2,  -1,   2,   0,   2 },
    { -0.001773,    0,   1,   2,  -2,   0 },
    { -0.001595,    0,   0,   2,   2,   0 },
    {  0.001220,   -1,  -1,   4,   0,   1 },
    { -0.001110,    0,   2,   0,   2,   0 },
    {  0.000892,    0,   1,  -3,   0,   0 },
    { -0.000811,    1,   1,   2,   0,   1 },
    {  0.000761,   -1,  -2,   4,   0,   1 },
    {  0.000717,   -2,   1,   0,   0,   2 },
    {  0.000704,   -2,   1,  -2,   0,   2 },
    {  0.000693,    1,  -2,   2,   0,   1 },
    {  0.000598,   -1,   0,   2,  -2,   1 },
    {  0.000550,    0,   1,   4,   0,   0 },
    {  0.000538,    0,   4,   0,   0,   0 },
    {  0.000521,   -1,   0,   4,   0,   1 },
    {  0.000486,    0,   2,  -1,   0,   0 }
  };

  // Latitude series expansion coefficients
  const int NB = 45;
  const double TB[NB][6] = {
    //   coeff      M    M'   D    F    n
    {  5.128189,    0,   0,   0,   1,   0 },
    {  0.280606,    0,   1,   0,   1,   0 },
    {  0.277693,    0,   1,   0,  -1,   0 },
    {  0.173238,    0,   0,   2,  -1,   0 },
    {  0.055413,    0,  -1,   2,   1,   0 },
    {  0.046272,    0,  -1,   2,  -1,   0 },
    {  0.032573,    0,   0,   2,   1,   0 },
    {  0.017198,    0,   2,   0,   1,   0 },
    {  0.009267,    0,   1,   2,  -1,   0 },
    {  0.008823,    0,   2,   0,  -1,   0 },
    {  0.008247,   -1,   0,   2,  -1,   1 },
    {  0.004323,    0,  -2,   2,  -1,   0 },
    {  0.004200,    0,   1,   2,   1,   0 },
    {  0.003372,   -1,   0,  -2,   1,   1 },
    {  0.002472,   -1,  -1,   2,   1,   1 },
    {  0.002222,   -1,   0,   2,   1,   1 },
    {  0.002072,   -1,  -1,   2,  -1,   1 },
    {  0.001877,   -1,   1,   0,   1,   1 },
    {  0.001828,    0,  -1,   4,  -1,   0 },
    { -0.001803,    1,   0,   0,   1,   1 },
    { -0.001750,    0,   0,   0,   3,   0 },
    {  0.001570,   -1,   1,   0,  -1,   1 },
    { -0.001487,    0,   0,   1,   1,   0 },
    { -0.001481,    1,   1,   0,   1,   1 },
    {  0.001417,   -1,  -1,   0,   1,   1 },
    {  0.001350,   -1,   0,   0,   1,   1 },
    {  0.001330,    0,   0,  -1,   1,   0 },
    {  0.001106,    0,   3,   0,   1,   0 },
    {  0.001020,    0,   0,   4,  -1,   0 },
    {  0.000833,    0,  -1,   4,   1,   0 },
    {  0.000781,    0,   1,   0,  -3,   0 },
    {  0.000670,    0,  -2,   4,   1,   0 },
    {  0.000606,    0,   0,   2,  -3,   0 },
    {  0.000597,    0,   2,   2,  -1,   0 },
    {  0.000492,   -1,   1,   2,  -1,   1 },
    {  0.000450,    0,   2,  -2,  -1,   0 },
    {  0.000439,    0,   3,   0,  -1,   0 },
    {  0.000423,    0,   2,   2,   1,   0 },
    {  0.000422,    0,  -3,   2,  -1,   0 },
    { -0.000367,    1,  -1,   2,   1,   1 },
    { -0.000353,    1,   0,   2,   1,   1 },
    {  0.000331,    0,   0,   4,   1,   0 },
    {  0.000317,   -1,   1,   2,   1,   1 },
    {  0.000306,   -2,   0,   2,  -1,   2 },
    { -0.000283,    0,   1,   0,   3,   0 }
  };

  // Parallax series expansion coefficients
  const int NP = 31;
  const double TP[NP][6] = {
    //   coeff      M    M'   D    F    n
    {  0.950724,    0,   0,   0,   0,   0 },
    {  0.051818,    0,   1,   0,   0,   0 },
    {  0.009531,    0,  -1,   2,   0,   0 },
    {  0.007843,    0,   0,   2,   0,   0 },
    {  0.002824,    0,   2,   0,   0,   0 },
    {  0.000857,    0,   1,   2,   0,   0 },
    {  0.000533,   -1,   0,   2,   0,   1 },
    {  0.000401,   -1,  -1,   2,   0,   1 },
    {  0.000320,   -1,   1,   0,   0,   1 },
    { -0.000271,    0,   0,   1,   0,   0 },
    { -0.000264,    1,   1,   0,   0,   1 },
    { -0.000198,    0,  -1,   0,   2,   0 },
    {  0.000173,    0,   3,   0,   0,   0 },
    {  0.000167,    0,  -1,   4,   0,   0 },
    { -0.000111,    1,   0,   0,   0,   1 },
    {  0.000103,    0,  -2,   4,   0,   0 },
    { -0.000084,    0,   2,  -2,   0,   0 },
    { -0.000083,    1,   0,   2,   0,   1 },
    {  0.000079,    0,   2,   2,   0,   0 },
    {  0.000072,    0,   0,   4,   0,   0 },
    {  0.000064,   -1,   1,   2,   0,   1 },
    { -0.000063,    1,  -1,   2,   0,   1 },
    {  0.000041,    1,   0,   1,   0,   1 },
    {  0.000035,   -1,   2,   0,   0,   1 },
    { -0.000033,    0,   3,  -2,   0,   0 },
    { -0.000030,    0,   1,   1,   0,   0 },
    { -0.000029,    0,   0,  -2,   2,   0 },
    { -0.000029,    1,   2,   0,   0,   1 },
    {  0.000026,   -2,   0,   2,   0,   2 },
    { -0.000023,    0,   1,  -2,   2,   0 },
    {  0.000019,   -1,  -1,   4,   0,   1 }
  };

}

void
Moon::GetGeocentricPoint(const ModifiedJulianDate& mjd, Point& gp)
{
  // Time interval (in centuries) between J1900.0 and the current epoch.
  // Note: the SLALIB routine expects time in TDB, where TDB = TT +/- O(2 ms)
  double t = (mjd.GetDate(TT)/day - 15019.5) / 36525.;

  // Arguments (radians) and derivatives (radians per Julian century)
  // for the current epoch:
  //
  // 1) Mean lunar longitude:
  double elp = fmod(elp0 + (elp1 + (elp2 + elp3*t)*t)*t, 360.) * degree;

  // 2) Mean solar anomaly
  double em = fmod(em0 + (em1 + (em2 + em3*t)*t)*t, 360.) * degree;

  // 3) Mean lunar anomaly
  double emp = fmod(emp0 + (emp1 + (emp2 + emp3*t)*t)*t, 360.) * degree;

  // 4) Mean lunar elongation
  double d = fmod(d0 + (d1 + (d2 + d3*t)*t)*t, 360.) * degree;

  // 5) Mean distance of the Moon from its ascending node
  double f = fmod(f0 + (f1 + (f2 + f3*t)*t)*t, 360.) * degree;

  // 6) Lunar longitude of the ascending node
  const double om = fmod(om0 + (om1 + (om2 + om3*t)*t)*t, 360.) * degree;
  const double sinom = sin(om);
  const double cosom = cos(om);

  // Add the periodic variations
  double theta = (pa0 + pa1*t) * degree;
  const double wa = sin (theta);

  theta = (pe0 + (pe1 + pe2*t)*t) * degree;
  const double wb = pec * sin(theta);

  elp += (pac*wa + wb + pfc*sinom) * degree;

  em += pbc * wa * degree;

  emp += (pcc*wa + wb + pgc*sinom) * degree;

  d += (pdc*wa + wb + phc*sinom) * degree;

  const double wom = om + (pj0 + pj1*t) * degree;
  const double sinwom = sin(wom);
  const double coswom = cos(wom);
  f += (wb + pic*sinom + pjc*sinwom) * degree;

  // E-factor, and square
  const double e = 1. + (e1 + e2*t)*t;
  const double esq = e*e;

  // Longitude series expansion
  double v = 0.;
  double coeff, emn, empn, dn, fn, en, ftheta; //, den, dtheta;
  int i;
  for (int n = NL - 1; n >= 0; --n) {
    coeff = TL[n][0];
    emn = TL[n][1];
    empn = TL[n][2];
    dn = TL[n][3];
    fn = TL[n][4];
    i = TL[n][5];
    if (i == 0) {
      en = 1.0;
    }
    else if (i == 1) {
      en = e;
    }
    else {
      en = esq;
    }
    theta = emn * em + empn * emp + dn * d + fn * f;
    ftheta = sin ( theta );
    v += coeff * ftheta * en;
  }
  double el = elp + v * degree;

  // Latitude series expansion
  v = 0.0;
  for (int n = NB - 1; n >= 0; --n) {
    coeff = TB[n][0];
    emn = TB[n][1];
    empn = TB[n][2];
    dn = TB[n][3];
    fn = TB[n][4];
    i = TB[n][5];
    if (i == 0) {
      en = 1.0;
    }
    else if (i == 1) {
      en = e;
    }
    else {
      en = esq;
    }
    theta = emn * em + empn * emp + dn * d + fn * f;
    ftheta = sin(theta);
    v += coeff * ftheta * en;
  }
  double bf = 1.0 - cw1*cosom - cw2*coswom;
  double b = v*bf*degree;

  // Parallax series expansion
  v = 0.0;
  for (int n = NP - 1; n >= 0; --n) {
    coeff = TP[n][0];
    emn = TP[n][1];
    empn = TP[n][2];
    dn = TP[n][3];
    fn = TP[n][4];
    i = TP[n][5];
    if (i == 0) {
      en = 1.0;
    }
    else if (i == 1) {
      en = e;
    }
    else {
      en = esq;
    }
    theta = emn*em + empn*emp + dn*d + fn*f;
    ftheta = cos(theta);
    v += coeff * ftheta * en;
  }
  double p = v * degree;

  // Parallax to distance (AU, AU/sec)
  double sp = sin(p);
  double r = Ellipsoid::GetEquatorialRadius() / sp;

  // Longitude, latitude to x, y, z (AU) */
  double sel = sin(el);
  double cel = cos(el);
  double sb = sin(b);
  double cb = cos(b);
  double rcb = r*cb;
  double x = rcb*cel;
  double y = rcb*sel;
  double z = r*sb;

  // Julian centuries since J2000
  t = (mjd.GetDate(TT)/day - 51544.5) / 36525.;

  // Fricke equinox correction
  double epj = 2000.0 + t*100.0;
  double eqcor = (0.035 + 0.00085 * (epj - B1950))*second * (15*degree / hour);

  // Mean obliquity (IAU 1976)
  double eps = (84381.448 + (-46.8150 + (-0.00059 + 0.001813*t)*t)*t) * arcsecond;

  // To the equatorial system, mean of date, FK5 system
  double sineps = sin(eps);
  double coseps = cos(eps);
  double es = eqcor * sineps;
  double ec = eqcor * coseps;
  gp.SetXYZ(x - ec*y + es*z,
           eqcor*x + y*coseps - z*sineps,
           y*sineps + z*coseps);
}

