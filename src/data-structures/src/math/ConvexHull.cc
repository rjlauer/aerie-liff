/*!
 * @file ConvexHull.cc
 * @brief Implementation of boost's convex hull algorithm.
 * @author Zig Hampel
 * @date 15 Sep 2015
 */

#include <hawcnest/Logging.h>
#include <data-structures/math/ConvexHull.h>
#include <cmath>

ConvexHull::ConvexHull(vector<double> X, vector<double> Y)
{

  if ( X.empty() || Y.empty() ) 
    log_fatal("ConvexHull: The input point vectors are empty. Please fill.");

  if ( X.size() != Y.size() )
    log_fatal("ConvexHull: Input point vectors do not have the same size. Please remedy.");

 
  // Put the input vectors into the allpoints_ boost multipoint object
  vector<double>::const_iterator xi;
  vector<double>::const_iterator yi;
  for (xi=X.begin(), yi=Y.begin(); xi!=X.end(); ++xi, ++yi)
    append(allpoints_,make<point_xy <double> >(*xi, *yi));
  
  // Find the centroid of allpoints_
  bg::centroid(allpoints_, centroid_);

  // Compute the convex hull of the allpoints_ multipoint object
  bg::convex_hull(allpoints_,hull_);
}

ConvexHull::~ConvexHull()
{
  bg::clear(allpoints_);
  bg::clear(centroid_);
  bg::clear(hull_);
}

// Access to the hull vertices
void
ConvexHull::GetHull(vector<double> &X, vector<double> &Y)
{
  if ( !X.empty() || !Y.empty() ) 
    log_fatal("ConvexHull: The hull vectors are not empty. Please clear them.");

  // Get the points defining the hull of allpoints_, saved to x,y vectors
  PointIterator hullIterator;
  for (hullIterator = hull_.begin(); hullIterator != hull_.end(); ++hullIterator)
  {
    X.push_back( get<0>( *hullIterator ) );
    Y.push_back( get<1>( *hullIterator ) ); 
  }
}


// Access the centroid x,y values
void
ConvexHull::GetCentroid(double &x, double &y)
{
  x = get<0>(centroid_);
  y = get<1>(centroid_);
}

// Scale the hull vertices centered on the 
// centroid of the allpoints_. Assumes that the 
// allpoints_ array is nearly uniformly dense 
// in XY in order to find the centroid.
void
ConvexHull::ScaleHull(double fscale, vector<double> &X, vector<double> &Y)
{
  
  if ( !X.empty() || !Y.empty())
    log_fatal("ConvexHull: Input vectors for saving scaled hull vertices are not empty! Please clear them.")

  if ( fscale <= 0)
    log_fatal("ConvexHull: Scale factor is <= 0. Please set to positive, non-zero value");

  Polygon translated_hull;
  Polygon scaled_hull;

  // Centroid x,y of hull
  double centX, centY;
  GetCentroid(centX, centY);
  
  // Translate hull_ to centroid of allpoints_
  trans::translate_transformer<double, 2, 2> translate(-centX, -centY);
  bg::transform(hull_, translated_hull, translate);

  // Scale translated hull by fscale
  trans::scale_transformer<double, 2, 2> scale(fscale);
  bg::transform(translated_hull, scaled_hull, scale);

  bg::clear(translated_hull);

  // Translate scaled hull back to original coord 
  trans::translate_transformer<double, 2, 2> translate2(centX, centY);
  bg::transform(scaled_hull, translated_hull, translate2);

  // Write scaled hull vertices to X,Y vectors
  PointIterator hullIterator;
  for (hullIterator = translated_hull.begin(); hullIterator != translated_hull.end(); ++hullIterator)
  {
    X.push_back( get<0>( *hullIterator));
    Y.push_back( get<1>( *hullIterator));
  }

  bg::clear(translated_hull);
  bg::clear(scaled_hull);
}
