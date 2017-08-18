/*!
 * @file ConvexHull.h
 * @brief Wrapper for boost's convex hull.
 * @author Zig Hampel
 * @date 15 Sep 2015
 */

#ifndef DATACLASSES_MATH_CONVEXHULL_H_INCLUDED
#define DATACLASSES_MATH_CONVEXHULL_H_INCLUDED

#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/multi/geometries/multi_point.hpp>
#include <boost/geometry/geometries/polygon.hpp>

#include <hawcnest/PointerTypedefs.h>

#include <string>
#include <vector>

/*!
 * @class ConvexHull
 * @brief Wrapper for boost implementation of convex hull.
 * @author Zig Hampel
 * @ingroup math
 * @date 15 Sep 2015
 *
 * This class is a wrapper for the boost implementation of of the
 * convex hull algorithm. Included is functionality to scale the
 * calculated hull points by a non-zero positive factor.
 *
 */

using namespace std;
using std::vector;

namespace bg = boost::geometry;
using bg::model::d2::point_xy;
using bg::dsv;
using bg::append;
using bg::make;
using bg::get;
namespace trans = bg::strategy::transform;

class ConvexHull {

  typedef boost::geometry::model::d2::point_xy<double> Point;
  typedef boost::geometry::model::multi_point<Point> MultiPoint;
  typedef boost::geometry::model::ring<Point> Polygon;
  typedef vector<Point>::iterator PointIterator;
  
  public:

    ConvexHull(vector<double> x, vector<double> y);

    virtual ~ConvexHull();

    void GetHull(vector<double> &x, vector<double> &y);
    void ScaleHull(double scale, vector<double> &x, vector<double> &y);
    void GetCentroid(double &x, double &y);

    double Area() const { return boost::geometry::area(hull_); }

  private:
    
    MultiPoint allpoints_;
    Point centroid_;
    Polygon hull_;
};

SHARED_POINTER_TYPEDEFS(ConvexHull);

#endif // DATACLASSES_MATH_CONVEXHULL_H_INCLUDED

