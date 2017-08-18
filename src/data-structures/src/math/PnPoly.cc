/*!
 * @file PnPoly.cc
 * @brief Implementation of pnpoly algorithm.
 * @author Zig Hampel
 * @date 18 Sep 2015
 */

#include <hawcnest/Logging.h>
#include <data-structures/math/PnPoly.h>
#include <cmath>

using namespace std;

PnPoly::PnPoly(vector<double> X, vector<double> Y)
{
  SetPoly(X,Y);
  AreVertsSet_ = true;
}

void
PnPoly::SetPoly(vector<double> X, vector<double> Y)
{
  if ( X.empty() || Y.empty() ) 
    log_fatal("PnPoly: SetPoly: The input point vectors are empty. Please fill.");

  if ( X.size() != Y.size() )
    log_fatal("PnPoly: SetPoly: Input point vectors do not have the same size. Please remedy.");

  vertx_ = X;
  verty_ = Y;
  nvert_ = X.size();
  AreVertsSet_ = true;
}

/*
 * Modified from
 * PNPOLY - Point Inclusion in Polygon Test
 * W. Randolph Franklin (WRF)
 * http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
 */
int
PnPoly::Eval(double testx, double testy)
  const
{
  int i, j, c = 0;
  if (!AreVertsSet_) {
    log_fatal("Vertices defining polygon for PnPoly test are not set.");
  }
  else
  {
    for (i = 0, j = nvert_-1; i < nvert_; j = i++) {
      if ( ((verty_[i]>testy) != (verty_[j]>testy)) &&
       (testx < (vertx_[j]-vertx_[i]) * (testy-verty_[i]) / (verty_[j]-verty_[i]) + vertx_[i]) )
         c = !c;
    }

  }

  return c;
}
