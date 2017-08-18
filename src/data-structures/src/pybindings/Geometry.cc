/*!
 * @file Geometry.cc 
 * @brief Python bindings to the geometry classes.
 * @author Segev BenZvi 
 * @date 17 Dec 2011 
 * @version $Id: Geometry.cc 36810 2017-01-11 21:56:43Z zhampel $
 */

#include <boost/python.hpp>

#include <data-structures/geometry/R3Vector.h>
#include <data-structures/geometry/R3Transform.h>
#include <data-structures/geometry/Vector.h>
#include <data-structures/geometry/AxialVector.h>
#include <data-structures/geometry/Point.h>
#include <data-structures/geometry/S2Point.h>
#include <data-structures/geometry/UTMPoint.h>
#include <data-structures/geometry/LatLonAlt.h>
#include <data-structures/geometry/LateralDistanceCalc.h>

#include <hawcnest/impl/pybindings.h>

using namespace boost::python;

// -----------------------------------------------------------------------------
namespace {

  /// Helper function: convert a Cartesian vector to a python tuple
  tuple
  geometry_r3vector_to_xyz(const R3Vector& v)
  {
    return make_tuple(v.GetX(), v.GetY(), v.GetZ());
  }

  /// Helper function: convert a python tuple to a Cartesian vector
  void
  geometry_xyz_to_r3vector(R3Vector& v, const tuple& xyz)
  {
    v.SetXYZ(extract<double>(xyz[0]),
             extract<double>(xyz[1]),
             extract<double>(xyz[2]));
  }

  /// Helper function: convert a Cylindrical polar vector to a python tuple
  tuple
  geometry_r3vector_to_cyl(const R3Vector& v)
  {
    return make_tuple(v.GetRho(), v.GetPhi(), v.GetZ());
  }

  /// Helper function: convert a python tuple to a Cylindrical polar vector
  void
  geometry_cyl_to_r3vector(R3Vector& v, const tuple& rpz)
  {
    v.SetRhoPhiZ(extract<double>(rpz[0]),
                 extract<double>(rpz[1]),
                 extract<double>(rpz[2]));
  }

  /// Helper function: convert a Spherical polar vector to a python tuple
  tuple
  geometry_r3vector_to_sph(const R3Vector& v)
  {
    return make_tuple(v.GetMag(), v.GetTheta(), v.GetPhi());
  }

  /// Helper function: convert a python tuple to a Spherical polar vector
  void
  geometry_sph_to_r3vector(R3Vector& v, const tuple& rtp)
  {
    v.SetRThetaPhi(extract<double>(rtp[0]),
                   extract<double>(rtp[1]),
                   extract<double>(rtp[2]));
  }

}

/// Expose R3Vector to boost::python
void
pybind_geometry_R3Vector()
{
  scope in_R3Vector =
  class_<R3Vector, bases<Baggable>, boost::shared_ptr<R3Vector> >
    ("R3Vector",
     "Representation of a 3D coordinate triplet.")
    .def(init<const double, const double, const double>())
    .def(init<const double, const double, const double,
              const R3Vector::CoordinateSystem>())

    // Cartesian coordinate properties
    .add_property("x", &R3Vector::GetX, &R3Vector::SetX, "Vector x component.")
    .add_property("y", &R3Vector::GetY, &R3Vector::SetY, "Vector y component.")
    .add_property("z", &R3Vector::GetZ, &R3Vector::SetZ, "Vector z component.")
    .add_property("xyz",
                  geometry_r3vector_to_xyz, geometry_xyz_to_r3vector,
                  "Cartesian component tuple.")

    // Cylindrical polar coordinate properties
    .add_property("rho", &R3Vector::GetRho, "Cylindrical radius (read-only).")
    .add_property("rho2", &R3Vector::GetRho2, "Cylindrical r^2 (read-only).")
    .add_property("phi", &R3Vector::GetPhi, "Polar azimuth (read-only).")
    .add_property("rho_phi_z",
                  geometry_r3vector_to_cyl, geometry_cyl_to_r3vector,
                  "Cylindrical component tuple")
    .add_property("theta", &R3Vector::GetTheta, "Spherical zenith (read-only).")

    // Spherical polar coordinate properties
    .add_property("r", &R3Vector::GetMag, "Vector magnitude (read-only).")
    .add_property("r2", &R3Vector::GetMag2, "Vector square mod (read-only).")
    .add_property("r_theta_phi",
                  geometry_r3vector_to_sph, geometry_sph_to_r3vector,
                  "Spherical component tuple.")
    .add_property("vec_unit", &R3Vector::GetUnitVector,
                  "Return a copy of the unit vector for this vector.")
    .add_property("vec_orthogonal", &R3Vector::GetOrthogonalVector,
                  "Return a vector orthogonal to this one.")

    // Simple vector algebra
    .def("normalize", &R3Vector::Normalize, "Normalize this vector.")
    .def("angle", &R3Vector::Angle, "Calculate the angle between two vectors.")
    .def("dot", &R3Vector::Dot, "The dot product of two vectors.")
    .def("cross", &R3Vector::Cross, "The cross product of two vectors.")

    // Overloaded vector arithmetic
    .def(self + self)
    .def(self - self)
    .def(-self)
    .def(self * self)
    .def(self * other<double>())
    .def(other<double>() * self)
    .def(self / other<double>())
    .def(self == self)
    .def(self != self)
    .def(self_ns::str(self))
    ;

  register_pointer_conversions<R3Vector>();

  enum_<R3Vector::CoordinateSystem>("CoordinateSystem")
    .value("CARTESIAN", R3Vector::CARTESIAN)
    .value("CYLINDRICAL", R3Vector::CYLINDRICAL)
    .value("SPHERICAL", R3Vector::SPHERICAL)
    .export_values()
    ;
}

// -----------------------------------------------------------------------------
namespace {

  /// Helper function: convert a transformation matrix to the sum of scaling,
  /// rotation, and translation matrices and return as a python tuple
  tuple
  geometry_r3transform_to_srt(const R3Transform& x)
  {
    Scale s;
    Rotate r;
    Translate t;
    x.GetDecomposition(s, r, t);

    return make_tuple(s, r, t);
  }

}

/// Expose R3Transform to boost::python
void
pybind_geometry_R3Transform()
{
  class_<R3Transform, bases<Baggable>, boost::shared_ptr<R3Transform> >
    ("R3Transform",
     "Generic transformation in 3D: scaling, rotation, and translation.")
    .def(init<const double, const double, const double, const double,
              const double, const double, const double, const double,
              const double, const double, const double, const double>())
    .def(init<const Point&, const Point&, const Point&,
              const Point&, const Point&, const Point&>())

    // Rotation/scaling + translation matrix components and properties
    .add_property("xx", &R3Transform::GetXX, "Rotate/scale XX component.")
    .add_property("xy", &R3Transform::GetXY, "Rotate/scale XY component.")
    .add_property("xz", &R3Transform::GetXZ, "Rotate/scale XZ component.")
    .add_property("yx", &R3Transform::GetYX, "Rotate/scale YX component.")
    .add_property("yy", &R3Transform::GetYY, "Rotate/scale YY component.")
    .add_property("yz", &R3Transform::GetYZ, "Rotate/scale YZ component.")
    .add_property("zx", &R3Transform::GetZX, "Rotate/scale ZX component.")
    .add_property("zy", &R3Transform::GetZY, "Rotate/scale ZY component.")
    .add_property("zz", &R3Transform::GetZZ, "Rotate/scale ZZ component.")
    .add_property("dx", &R3Transform::GetDX, "Translation X component.")
    .add_property("dy", &R3Transform::GetDY, "Translation Y component.")
    .add_property("dz", &R3Transform::GetDZ, "Translation Z component.")
    .add_property("inverse", &R3Transform::GetInverse,
                  "Inverse of the transformation matrix (if exists).")
    .add_property("decomposition", geometry_r3transform_to_srt,
                  "Decomposition into a scaling, rotation, and translation.")

    // Invert/convert the transformation matrix
    .def("invert", &R3Transform::Invert,
         return_value_policy<copy_non_const_reference>(),
         "Invert the transformation matrix, if possible.")
    .def("identity", &R3Transform::SetIdentity,
         "Convert transformation matrix to an identity matrix.")

    // Some matrix arithmetic and comparisons
    .def(self * self)
    .def(self == self)
    .def(self != self)
    .def(self_ns::str(self))
    ;

  register_pointer_conversions<R3Transform>();
}

/// Expose Rotate to boost::python
void
pybind_geometry_Rotate()
{
  class_<Rotate, bases<R3Transform>, boost::shared_ptr<Rotate> >
    ("Rotate",
     "A coordinate rotation in 3D")
    .def(init<const double, const Point&, const Point&>())
    .def(init<const double, const Vector&>())
    .def(init<const Point&, const Point&, const Point&, const Point&>())
    ;

  register_pointer_conversions<Rotate>();
}

/// Expose Scale to boost::python
void
pybind_geometry_Scale()
{
  class_<Scale, bases<R3Transform>, boost::shared_ptr<Scale> >
    ("Scale",
     "A scaling transformation in 3D")
    .def(init<const double, const double, const double>())
    .def(init<const double>())
    ;

  register_pointer_conversions<Scale>();
}

/// Expose Translate to boost::python
void
pybind_geometry_Translate()
{
  class_<Translate, bases<R3Transform>, boost::shared_ptr<Translate> >
    ("Translate",
     "A translation in 3D")
    .def(init<const double, const double, const double>())
    ;

  register_pointer_conversions<Translate>();
}

// -----------------------------------------------------------------------------
/// Expose Vector to boost::python
void
pybind_geometry_Vector()
{
  class_<Vector, bases<R3Vector>, boost::shared_ptr<Vector> >
    ("Vector", "Object which scales/rotates/reflects like a polar vector.")
    .def(init<const R3Vector&>())
    .def(init<const double, const double, const double>())
    .def(init<const double, const double, const double,
              const R3Vector::CoordinateSystem>())
    .def("transform",
         &Vector::Transform,
         return_value_policy<copy_non_const_reference>(),
         "Scale or rotate the vector using a transformation matrix.")
    .def(other<R3Transform>() * self)
    ;

  register_pointer_conversions<Vector>();
}

// -----------------------------------------------------------------------------
/// Expose AxialVector to boost::python
void
pybind_geometry_AxialVector()
{
  class_<AxialVector, bases<R3Vector>, boost::shared_ptr<AxialVector> >
    ("AxialVector", "Object which scales/rotates/reflects like a pseudovector.",
                    init<const R3Vector&>())
    .def(init<const double, const double, const double>())
    .def(init<const double, const double, const double,
              const R3Vector::CoordinateSystem>())
    .def("transform",
         &AxialVector::Transform,
         return_value_policy<copy_non_const_reference>(),
         "Scale or rotate the vector using a transformation matrix.")
    .def(other<R3Transform>() * self)
    ;

  register_pointer_conversions<AxialVector>();
}

// -----------------------------------------------------------------------------
/// Expose Point to boost::python
void
pybind_geometry_Point()
{
  class_<Point, bases<R3Vector>, boost::shared_ptr<Point> >
    ("Point",
     "Object which scales/rotates/reflects like a polar vector, and which can be translated with respect to a coordinate origin.")
    .def(init<const R3Vector&>())
    .def(init<const double, const double, const double>())
    .def(init<const double, const double, const double,
              const R3Vector::CoordinateSystem>())
    .def("transform",
         &Point::Transform,
         return_value_policy<copy_non_const_reference>(),
         "Scale, rotate, or translate a point using a transformation matrix.")
    .def(other<R3Transform>() * self)
    ;

  register_pointer_conversions<Point>();
}

// -----------------------------------------------------------------------------
namespace {

  /// Helper function to convert an S2Point into a python tuple
  tuple
  geometry_s2point_to_thetaphi(const S2Point& s)
  {
    return make_tuple(s.GetTheta(), s.GetPhi());
  }

  /// Helper function: convert a python tuple to an S2Point
  void
  geometry_thetaphi_to_s2point(S2Point& s, const tuple& thetaphi)
  {
    s.SetThetaPhi(extract<double>(thetaphi[0]), extract<double>(thetaphi[1]));
  }

}

/// Expose S2Point to boost::python
void
pybind_geometry_S2Point()
{
  class_<S2Point>
    ("S2Point",
     "Representation of a point on the 2-sphere.")
    .def(init<const Point&>())
    .def(init<const double, const double>())
    .add_property("theta", &S2Point::GetTheta, &S2Point::SetTheta,
                  "Spherical polar zenith angle.")
    .add_property("phi", &S2Point::GetPhi, &S2Point::SetPhi,
                  "Spherical polar azimuth angle.")
    .add_property("theta_phi",
                  geometry_s2point_to_thetaphi, geometry_thetaphi_to_s2point,
                  "Spherical polar coordinate tuple.")
    .add_property("point",
                  make_function(&S2Point::GetPoint,
                       return_value_policy<copy_const_reference>()),
                  "Return a representation as a unit vector.")
    .def("angle", &S2Point::Angle,
         "Calculate the angle with respect to another S2Point.")
    .def(other<R3Transform>() * self)
    .def(self_ns::str(self))
    ;
}

// -----------------------------------------------------------------------------
namespace {

  /// Helper function to convert a UTMPoint into a python tuple (easting,
  /// northing, height, zone, and band)
  tuple
  geometry_utm_to_tuple(const UTMPoint& u)
  {
    return make_tuple(u.GetEasting(), u.GetNorthing(), u.GetHeight(),
                      u.GetZone(), u.GetBand());
  }

  /// Helper function: convert a python tuple (E, N, h, z, b) to a UTMPoint
  void
  geometry_tuple_to_utm(UTMPoint& u, const tuple& ENhzb)
  {
    u = UTMPoint(extract<double>(ENhzb[0]),   // easting
                 extract<double>(ENhzb[1]),   // northing
                 extract<double>(ENhzb[2]),   // ellipsoidal height (altitude)
                 extract<int>(ENhzb[3]),      // zone
                 extract<char>(ENhzb[4]));    // band
  }

  /// Helper function to convert a UTMPoint into a python tuple
  /// (geodetic latitude, geodetic longitude, and ellipsoidal height)
  tuple
  geometry_utm_to_lla(const UTMPoint& u)
  {
    double lat, lon;
    u.GetLatitudeLongitude(lat, lon);
    return make_tuple(lat, lon, u.GetHeight());
  }

  /// Helper function: convert a python tuple (lat, lon, ht) to a UTMPoint
  void
  geometry_lla_to_utm(UTMPoint& u, const tuple& lla)
  {
    u = UTMPoint(extract<double>(lla[0]),
                 extract<double>(lla[1]),
                 extract<double>(lla[2]));
  }

}

/// Expose UTMPoint to boost::python
void
pybind_geometry_UTMPoint()
{
  class_<UTMPoint>
    ("UTMPoint",
     "A geodetic coordinate given by UTM easting, northing, and zone number")
    .def(init<const double, const double, const double,
              const int, const char>())
    .def(init<const double, const double, const double>())
    .def(init<const LatLonAlt&>())
    .add_property("easting", &UTMPoint::GetEasting)
    .add_property("northing", &UTMPoint::GetNorthing)
    .add_property("height", &UTMPoint::GetHeight)
    .add_property("zone", &UTMPoint::GetZone)
    .add_property("band", &UTMPoint::GetBand)
    .add_property("coord", geometry_utm_to_tuple, geometry_tuple_to_utm,
                  "Tuple access: (easting, northing, height, zone, band)")
    .add_property("lat_lon_alt", geometry_utm_to_lla, geometry_lla_to_utm,
                  "Tuple access: (latitude, longitude, altitude)")
    .def(self_ns::str(self))
    ;
}

// -----------------------------------------------------------------------------
namespace {

  /// Helper function to convert a LatLonAlt into a python tuple
  /// (geodetic latitude, geodetic longitude, and ellipsoidal height)
  tuple
  geometry_lla_to_tuple(const LatLonAlt& lla)
  {
    return make_tuple(lla.GetLatitude(), lla.GetLongitude(), lla.GetHeight());
  }

  /// Helper function: convert a python tuple (lat, lon, ht) to a LatLonAlt
  void
  geometry_tuple_to_lla(LatLonAlt& lla, const tuple& t)
  {
    lla.SetLatitudeLongitudeHeight(extract<double>(t[0]),
                                   extract<double>(t[1]),
                                   extract<double>(t[2]));
  }

}

/// Expose LatLonAlt to boost::python
void
pybind_geometry_LatLonAlt()
{
  class_<LatLonAlt>
    ("LatLonAlt",
     "A geodetic coordinate given by latitude, longitude, and ellipsoidal height")
    .def(init<const double, const double, const double>())
    .def(init<const Point&>())
    .def(init<const UTMPoint&>())
    .add_property("latitude", &LatLonAlt::GetLatitude,
                  "Geodetic latitude (read-only).")
    .add_property("longitude", &LatLonAlt::GetLongitude,
                  "Geodetic longitude (read-only).")
    .add_property("altitude", &LatLonAlt::GetHeight,
                  "Ellipsoidal height (read-only).")
    .add_property("geocentric_coord",
                  make_function(&LatLonAlt::GetGeocentricPoint,
                                return_value_policy<copy_const_reference>()),
                  &LatLonAlt::SetGeocentricPoint,
                  "Geocentric coordinates of this position.")
    .add_property("lat_lon_alt", geometry_lla_to_tuple, geometry_tuple_to_lla,
                  "Tuple access: (latitude, longitude, altitude)")
    .def(self_ns::str(self))
    ;
}

//// -----------------------------------------------------------------------------
namespace {
  
 // class LatDistCalcWrapper {
 //   public:
 //     
 //     static boost::shared_ptr<LatDistCalc>
 //     init(const R3Vector core, const double theta, const double phi)
 //     {
 //       return boost::make_shared<LatDistCalc>(core,theta,phi);
 //     }
 // };

      void
      set_core(LatDistCalc& ld, const R3Vector core)
      {
        ld.SetCore(core);
      }
      
      void
      set_axis(LatDistCalc& ld, const double theta, const double phi)
      {
        ld.SetAxis(theta,phi);
      }
      
      double
      rot_lat_dist(const LatDistCalc& ld, const R3Vector point)
      {
        return ld.RotLatDist(point);
        //return boost::python::make_tuple(ld.RotLatDistCalc(point));
      }

      double
      minr_lat_dist(const LatDistCalc& ld, const R3Vector point)
      {
        return ld.MinRLatDist(point);
      }
}

/// Expose LatDistCalc to boost::python
void
pybind_geometry_LatDistCalc()
{
  class_<LatDistCalc > //, bases<R3Vector> > //, boost::shared_ptr<LatDistCalc> >
    ("LatDistCalc",
     "Calculate the lateral distance of a point from an axis",
     init<const R3Vector&, const double&, const double&>(
     args("core", "theta", "phi"),
     "Calculate shortest distance from a point to a line defined by an origin and angles."
     "  core: Origin (R3Vector)"
     "  theta: Zenith angle of axis(double)"
     "  phi:   Azimuth angle of axis(double)")
     )
    //.def("__init__",
    //     make_constructor(&LatDistCalcWrapper::init))
    //.add_property("set_core", set_core,
    //              "Set the core location")
    //.add_property("set_axis", set_axis,
    //              "Set the axis")
    //.add_property("lat_dist_rot", rot_lat_dist,
    //              "Lateral distance via rotation")
    //.add_property("lat_dist_min_r", minr_lat_dist,
    //              "Lateral distance via min distance")
    .add_property("set_core", &LatDistCalc::SetCore,
                  "Set the axis")
    .add_property("set_axis", &LatDistCalc::SetAxis,
                  "Set the axis")
    .add_property("lat_dist_rot", &LatDistCalc::RotLatDist,
                  "Lateral distance via rotation")
    .add_property("lat_dist_min_r", &LatDistCalc::MinRLatDist,
                  "Lateral distance via min distance")
    //.add_property("set_axis", &LatDistCalcWrapper::set_axis,
    //              "Set the axis")
    //.add_property("lat_dist_rot", &LatDistCalcWrapper::rot_lat_dist,
    //              "Lateral distance via rotation")
    //.add_property("lat_dist_min_r", &LatDistCalcWrapper::minr_lat_dist,
    //              "Lateral distance via min distance")
    ;
}
