/*!
 * @file Geometry.cc 
 * @brief Unit test for the geometry data structures.
 * @author Segev BenZvi 
 * @date 27 Jan 2012 
 * @ingroup unit_test
 * @version $Id: Geometry.cc 19006 2014-03-08 17:10:22Z sybenzvi $
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/output_test_stream.hpp>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/test/OutputConfig.h>

#include <data-structures/geometry/Ellipsoid.h>
#include <data-structures/geometry/AxialVector.h>
#include <data-structures/geometry/LatLonAlt.h>
#include <data-structures/geometry/Point.h>
#include <data-structures/geometry/S2Point.h>
#include <data-structures/geometry/UTMPoint.h>
#include <data-structures/geometry/Vector.h>
#include <data-structures/geometry/R3Transform.h>

using namespace HAWCUnits;
using boost::test_tools::output_test_stream;
using namespace std;


BOOST_AUTO_TEST_SUITE(GeometryTest)

  //____________________________________________________________________________
  // Check the constants in the WGS84 ellipsoidal earth model
  BOOST_AUTO_TEST_CASE(WGS84)
  {
    const double e2 = Ellipsoid::GetEccentricity2();
    BOOST_CHECK_EQUAL(e2, 0.00669437999013);

    const double rE = Ellipsoid::GetEquatorialRadius();
    BOOST_CHECK_EQUAL(rE, 6378137 * meter);

    const double f = 1. - sqrt(1. - e2);
    BOOST_CHECK_EQUAL(Ellipsoid::GetFlattening(), f);

    const double rP = (1. - f) * rE;
    BOOST_CHECK_EQUAL(Ellipsoid::GetPolarRadius(), rP);
  }

  //____________________________________________________________________________
  // Check some basic vector algebra
  BOOST_AUTO_TEST_CASE(PolarVectors)
  {
    // Assignment to a Cartesian vector
    const Vector v1(3., 4., 0.);
    BOOST_CHECK(v1.GetX() == 3. && v1.GetY() == 4. && v1.GetZ() == 0. &&
                v1.GetMag() == v1.GetRho() &&
                v1.GetRho() == 5.);

    BOOST_CHECK(v1.GetTheta() == 90.*degree);
    BOOST_CHECK_CLOSE(v1.GetPhi(), 53.1301*degree, 0.001);

    // Unit vector
    const Vector v2(v1.GetUnitVector());
    BOOST_CHECK_EQUAL(v2.GetMag(), 1.);

    // Orthogonal vector
    const Vector v3(v1.GetOrthogonalVector());
    BOOST_CHECK_CLOSE(v1.Angle(v3), 90*degree, 1e-6);

    // Cross product
    const Vector v4 = v1.Cross(Vector(4., 3., 0.));
    BOOST_CHECK(v4.GetX() == 0. && v4.GetY() == 0. && v4.GetZ() == -7.);

    // Dot product
    BOOST_CHECK_SMALL(v1 * v3, 1e-15);
    BOOST_CHECK_SMALL(v1.Dot(v3), 1e-15);
    BOOST_CHECK_EQUAL(v1 * v1, 25.);

    // Equivalence
    BOOST_CHECK(v1 == v1);
    BOOST_CHECK(v1 != v3);

    // Scaling by a constant
    BOOST_CHECK_EQUAL((2. * v1).GetMag(), 10.);

    // Vector addition
    const Vector v5 = v1 + v4;
    BOOST_CHECK(v5.GetX() == 3. && v5.GetY() == 4. && v5.GetZ() == -7.);

    // Vector subtraction
    Vector v6 = v1 - v5;
    BOOST_CHECK(v6.GetX() == 0. && v6.GetY() == 0. && v6.GetZ() == 7.);

    // Set cylindrical coordinates
    v6.SetRhoPhiZ(3., 90*degree, 4.);
    BOOST_CHECK_SMALL(v6.GetX(), 1e-15);
    BOOST_CHECK_CLOSE(v6.GetY(), 3., 1e-15);
    BOOST_CHECK_CLOSE(v6.GetZ(), 4., 1e-15);

    // Set spherical coordinates
    v6.SetRThetaPhi(5., 90*degree, 0.);
    BOOST_CHECK_EQUAL(v6.GetX(), 5.);
    BOOST_CHECK_SMALL(v6.GetY(), 1e-15);
    BOOST_CHECK_SMALL(v6.GetZ(), 1e-15);

    // Test division by a scalar
    v6.SetXYZ(3., 4., 0.);
    v6 /= 4.;
    BOOST_CHECK(v6.GetX() == 0.75 && v6.GetY() == 1. && v6.GetZ() == 0.);

    // Test multiplication by a scalar
    v6 *= 4.;
    BOOST_CHECK(v6.GetX() == 3. && v6.GetY() == 4. && v6.GetZ() == 0.);
  }

  //____________________________________________________________________________
  // Check transformations on vectors
  BOOST_AUTO_TEST_CASE(Transformations)
  {
    Vector v(1, 1, 1);

    // Scaling transformation
    Scale s(3, 4, 5);
    BOOST_CHECK_EQUAL(s * v, Vector(3, 4, 5));

    // Rotate about z by 90 degrees (proper rotation)
    Rotate r1(90*degree, Vector(0, 0, 1));
    const Vector v1 = r1 * v;
    BOOST_CHECK_CLOSE(v1.GetX(), -1., 1e-12);
    BOOST_CHECK_CLOSE(v1.GetY(),  1., 1e-12);
    BOOST_CHECK_CLOSE(v1.GetZ(),  1., 1e-12);

    // Reflection through the y=0 (x-z) plane (improper rotation)
    R3Transform r2(1.,  0., 0.,   0,
                   0., -1., 0.,   0,
                   0.,  0., 1.,   0);
    const Vector v2 = r2 * v;
    BOOST_CHECK_CLOSE(v2.GetX(),  1., 1e-12);
    BOOST_CHECK_CLOSE(v2.GetY(), -1., 1e-12);
    BOOST_CHECK_CLOSE(v2.GetZ(),  1., 1e-12);

    // Rotate about x by 90 degrees
    Rotate r3(90*degree, Point(0, 0, 0), Point(1, 0, 0));
    const Vector v3 = r3 * v;
    BOOST_CHECK_CLOSE(v3.GetX(),  1., 1e-12);
    BOOST_CHECK_CLOSE(v3.GetY(), -1., 1e-12);
    BOOST_CHECK_CLOSE(v3.GetZ(),  1., 1e-12);

    // Rotate about y by 90 degrees
    Rotate r4(Point(1, 0, 0), Point(0, 0, 1),     // x, z
              Point(0, 0, -1), Point(1, 0, 0));   // x', z' = -z, x
    const Vector v4 = r4 * v;
    BOOST_CHECK_CLOSE(v4.GetX(),  1., 1e-12);
    BOOST_CHECK_CLOSE(v4.GetY(),  1., 1e-12);
    BOOST_CHECK_CLOSE(v4.GetZ(), -1., 1e-12);

    // Translate a Point by 1 along each direction
    // Note that Vectors do not translate!
    Point p(1, 1, 1);
    Translate tr(1, 1, -1);
    BOOST_CHECK_EQUAL(tr * p, Point(2., 2., 0.));
  }

  //____________________________________________________________________________
  // Axial vectors have the same operations as vectors except for 
  // transformations
  BOOST_AUTO_TEST_CASE(AxialVectors)
  {
    AxialVector v(1, 1, 1);

    // Rotate about z by 90 degrees (proper rotation).
    // Should rotate like an ordinary polar vector
    Rotate r1(90*degree, Vector(0, 0, 1));
    const AxialVector v1 = r1 * v;
    BOOST_CHECK_CLOSE(v1.GetX(), -1., 1e-12);
    BOOST_CHECK_CLOSE(v1.GetY(),  1., 1e-12);
    BOOST_CHECK_CLOSE(v1.GetZ(),  1., 1e-12);

    // Reflection through the y=0 (x-z) plane (improper rotation).
    // Should gain an additional sign flip w.r.t. ordinary vector rotation
    R3Transform r2(1.,  0., 0.,   0,
                   0., -1., 0.,   0,
                   0.,  0., 1.,   0);
    const AxialVector v2 = r2 * v;
    BOOST_CHECK_CLOSE(v2.GetX(), -1., 1e-12);
    BOOST_CHECK_CLOSE(v2.GetY(),  1., 1e-12);
    BOOST_CHECK_CLOSE(v2.GetZ(), -1., 1e-12);
  }

  //____________________________________________________________________________
  // Algebra of points on the unit sphere
  BOOST_AUTO_TEST_CASE(UnitSphere)
  {
    // Check default constructor and setters
    S2Point s1;
    s1.SetThetaPhi(90.*degree, 90.*degree);
    BOOST_CHECK_EQUAL(s1.GetTheta(), 90*degree);
    BOOST_CHECK_EQUAL(s1.GetPhi(), 90*degree);

    // Check basic constructor
    S2Point s2(45*degree, 0.);
    BOOST_CHECK_CLOSE(s2.GetTheta(), 45*degree, 1e-12);
    BOOST_CHECK_EQUAL(s2.GetPhi(), 0.);

    // Check initialization with a Point object
    S2Point s3(Point(0, 0, 1));
    BOOST_CHECK_EQUAL(s3.GetTheta(), 0.);
    BOOST_CHECK_EQUAL(s3.GetPhi(), 0.);

    // Check copy construction
    S2Point s4(s3);
    BOOST_CHECK_EQUAL(s4.GetTheta(), 0.);
    BOOST_CHECK_EQUAL(s4.GetPhi(), 0.);

    // Check angle calculation
    BOOST_CHECK_CLOSE(s1.Angle(s2), 90*degree, 1e-6);
    BOOST_CHECK_CLOSE(s2.Angle(s3), 45*degree, 1e-6);
    BOOST_CHECK_CLOSE(s1.Angle(s3), 90*degree, 1e-6);
    BOOST_CHECK_SMALL(s3.Angle(s4), 1e-6*degree);

    // Check internal representation
    const Point& p1 = s1.GetPoint();
    BOOST_CHECK_SMALL(p1.GetX(), 1e-15);
    BOOST_CHECK_CLOSE(p1.GetY(), 1., 1e-15);
    BOOST_CHECK_SMALL(p1.GetZ(), 1e-15);

    const Point& p2 = s2.GetPoint();
    BOOST_CHECK_CLOSE(p2.GetX(), 0.5*sqrt(2.), 1e-10);
    BOOST_CHECK_SMALL(p2.GetY(), 1e-15);
    BOOST_CHECK_CLOSE(p2.GetZ(), 0.5*sqrt(2.), 1e-10);

    const Point& p3 = s3.GetPoint();
    BOOST_CHECK_SMALL(p3.GetX(), 1e-15);
    BOOST_CHECK_SMALL(p3.GetY(), 1e-15);
    BOOST_CHECK_CLOSE(p3.GetZ(), 1., 1e-15);

    // Check rotations and reflections
    
    R3Transform reflect(1., 0.,  0.,   0,     // reflection through x-y plane
                        0., 1.,  0.,   0,
                        0., 0., -1.,   0);
    s3 = reflect * s3;
    BOOST_CHECK_CLOSE(s3.GetTheta(), 180*degree, 1e-6);
    BOOST_CHECK_SMALL(s3.GetPhi(), 1e-6*degree);

    Rotate rotate(45*degree, Vector(0,0,1));  // rotation about z-axis
    s2 = rotate * s2;
    BOOST_CHECK_CLOSE(s2.GetTheta(), 45*degree, 1e-6);
    BOOST_CHECK_CLOSE(s2.GetPhi(), 45*degree, 1e-6);
  }

  //____________________________________________________________________________
  // Coordinates on the surface of the Earth
  BOOST_AUTO_TEST_CASE(Geodesy)
  {
    // UTM coordinates: default constructor
    UTMPoint utmOrizaba;
    BOOST_CHECK_EQUAL(utmOrizaba.GetEasting(), 682087*meter);
    BOOST_CHECK_EQUAL(utmOrizaba.GetNorthing(), 2105099*meter);
    BOOST_CHECK_EQUAL(utmOrizaba.GetHeight(), 5636*meter);
    BOOST_CHECK_EQUAL(utmOrizaba.GetZone(), 14);
    BOOST_CHECK_EQUAL(utmOrizaba.GetBand(), 'Q');

    // UTM coordinates: construction from coordinate values
    UTMPoint utmHAWC1(678142.1*meter, 2101110.4*meter, 4096*meter, 14, 'Q');
    BOOST_CHECK_EQUAL(utmHAWC1.GetEasting(), 678142.1*meter);
    BOOST_CHECK_EQUAL(utmHAWC1.GetNorthing(), 2101110.4*meter);
    BOOST_CHECK_EQUAL(utmHAWC1.GetHeight(), 4096*meter);
    BOOST_CHECK_EQUAL(utmHAWC1.GetZone(), 14);
    BOOST_CHECK_EQUAL(utmHAWC1.GetBand(), 'Q');

    // UTM coordinates: construction from latitude, longitude, and altitude
    UTMPoint utmHAWC2(18.99473658*degree, -97.30768523*degree, 4096*meter);
    BOOST_CHECK_CLOSE(utmHAWC2.GetEasting(), 678142.1*meter, 1e-5);
    BOOST_CHECK_CLOSE(utmHAWC2.GetNorthing(), 2101110.4*meter, 1e-3);
    BOOST_CHECK_EQUAL(utmHAWC2.GetHeight(), 4096*meter);
    BOOST_CHECK_EQUAL(utmHAWC2.GetZone(), 14);
    BOOST_CHECK_EQUAL(utmHAWC2.GetBand(), 'Q');

    // UTM coordinates: construction from lat/lon/alt object
    LatLonAlt llaHAWC3(18.99473658*degree, -97.30768523*degree, 4096*meter);
    UTMPoint utmHAWC3(llaHAWC3);
    BOOST_CHECK_CLOSE(utmHAWC3.GetEasting(), 678142.1*meter, 1e-5);
    BOOST_CHECK_CLOSE(utmHAWC3.GetNorthing(), 2101110.4*meter, 1e-3);
    BOOST_CHECK_EQUAL(utmHAWC3.GetHeight(), 4096*meter);
    BOOST_CHECK_EQUAL(utmHAWC3.GetZone(), 14);
    BOOST_CHECK_EQUAL(utmHAWC3.GetBand(), 'Q');

    // Check get latitude/longitude function
    double lat;
    double lon;
    utmHAWC1.GetLatitudeLongitude(lat, lon);
    BOOST_CHECK_CLOSE(lat, 18.99473658*degree, 1e-4);
    BOOST_CHECK_CLOSE(lon, -97.30768523*degree, 1e-4);
  }

BOOST_AUTO_TEST_SUITE_END()

