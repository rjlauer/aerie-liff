/*!
 * @file LatDist.cc
 * @brief Implementation of lateral distance calculators.
 * @author Zig Hampel
 * @date 19 Dec 2016
 */

#include <hawcnest/Logging.h>
#include <data-structures/geometry/LateralDistanceCalc.h>
#include <cmath>

using namespace std;



LatDistCalc::LatDistCalc(R3Vector core, double zenith, double azimuth)
{
  SetCore(core);
  SetAxis(zenith,azimuth);
}


// Get the lateral distance from
// a point to an axis and origin
// by a rotation
double
LatDistCalc::RotLatDist(R3Vector position)
  const
{
  CheckSettings();

  double dot_prod, lat_dist;
  R3Vector tempVec, scaledAxis;

  scaledAxis = axis_;

  tempVec = position - core_;
 
  dot_prod = tempVec*axis_;
  //dot_prod = tempVec.Dot(axis_);
  
  scaledAxis *= dot_prod;

  tempVec -= scaledAxis;

  lat_dist = tempVec.GetMag();

  return lat_dist;
}


double
LatDistCalc::MinRLatDist(R3Vector position)
  const
{

  CheckSettings();

  double rhit, xi, zi, norm, lat_dist, cross_norm;
  double coreX, coreY, coreZ, h_x, h_y, h_z;
  double theta, phi;
  R3Vector cross;
  
  coreX = core_.GetX();
  coreY = core_.GetY();
  coreZ = core_.GetZ();
  h_x = position.GetX();
  h_y = position.GetY();
  h_z = position.GetZ();
  theta = zenith_;
  phi = azimuth_;

  rhit = sqrt(pow(coreX-h_x,2)+pow(coreY-h_y,2));
  xi = (2*coreX*cos(phi)+rhit-rhit*cos(2*theta))/(2*cos(phi));
  zi = h_z + sqrt(pow(rhit,2)*pow(sin(theta),2)/pow(cos(phi),2) - pow(xi-coreX,2));
  cross_norm = sqrt(pow(zi-h_z,2)*pow(coreY-h_y,2)+pow(zi-h_z,2)*pow(coreX-h_x,2)+pow(xi-coreX,2)*pow(coreY-h_y,2));
  norm = sqrt(pow(xi-coreX,2)+pow(zi-h_z,2));
  lat_dist = cross_norm/norm;

  return lat_dist;
}
