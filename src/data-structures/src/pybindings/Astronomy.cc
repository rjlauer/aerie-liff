/*!
 * @file Astronomy.cc 
 * @brief Python bindings to astronomical data structures.
 * @author Segev BenZvi 
 * @date 17 Dec 2011 
 * @version $Id: Astronomy.cc 19006 2014-03-08 17:10:22Z sybenzvi $
 */

#include <boost/python.hpp>

#include <data-structures/astronomy/EquPoint.h>
#include <data-structures/astronomy/GalPoint.h>
#include <data-structures/astronomy/EclPoint.h>
#include <data-structures/astronomy/HorPoint.h>
#include <data-structures/astronomy/AstroCoords.h>

using namespace boost::python;

typedef return_value_policy<copy_const_reference> RVPCCR;

// -----------------------------------------------------------------------------
namespace {

  /// Helper function to convert EquPoint into a python (ra, dec) tuple
  tuple
  astronomy_EquPoint_to_radec(const EquPoint& e)
  {
    return make_tuple(e.GetRA(), e.GetDec());
  }

  /// Helper function to convert a python (ra, dec) tuple into an EquPoint
  void
  astronomy_radec_to_EquPoint(EquPoint& e, const tuple& radec)
  {
    e.SetRADec(extract<double>(radec[0]),
               extract<double>(radec[1]));
  }

}

/// Expose EquPoint to boost::python
void
pybind_astronomy_EquPoint()
{
  class_<EquPoint>
    ("EquPoint",
     "Right ascension and declination (celestial/equatorial coordinates)")
    .def(init<const double, const double>())
    .def(init<const EquPoint&>())
    .add_property("ra", &EquPoint::GetRA, "Right ascension (read-only).")
    .add_property("dec", &EquPoint::GetDec, "Declination (read-only).")
    .add_property("ra_dec",
                  astronomy_EquPoint_to_radec,
                  astronomy_radec_to_EquPoint,
                  "Tuple access as (RA, Dec).")
    .def("angle", &EquPoint::Angle, "Opening angle between two EquPoints.")
    .def(self_ns::str(self))
    ;
}

// -----------------------------------------------------------------------------
namespace {

  /// Helper function to convert GalPoint into a python (b, l) tuple
  tuple
  astronomy_GalPoint_to_bl(const GalPoint& g)
  {
    return make_tuple(g.GetB(), g.GetL());
  }

  /// Helper function to convert a python (b, l) tuple into a GalPoint
  void
  astronomy_bl_to_GalPoint(GalPoint& g, const tuple& bl)
  {
    g.SetBL(extract<double>(bl[0]),
            extract<double>(bl[1]));
  }

}

/// Expose GalPoint to boost::python
void
pybind_astronomy_GalPoint()
{
  class_<GalPoint>
    ("GalPoint",
     "Galactic latitude and longitude")
    .def(init<const double, const double>())
    .def(init<const GalPoint&>())
    .add_property("b", &GalPoint::GetB, "Galactic latitude (read-only).")
    .add_property("l", &GalPoint::GetL, "Galactic longitude (read-only).")
    .add_property("b_l",
                  astronomy_GalPoint_to_bl,
                  astronomy_bl_to_GalPoint,
                  "Tuple access as (b, l).")
    .def("angle", &GalPoint::Angle, "Opening angle between two GalPoints.")
    .def(self_ns::str(self))
    ;
}

// -----------------------------------------------------------------------------
namespace {

  /// Helper function to convert an EclPoint into a python (beta, lambda) tuple
  tuple
  astronomy_EclPoint_to_bl(const EclPoint& e)
  {
    return make_tuple(e.GetBeta(), e.GetLambda());
  }

  /// Helper function to convert a python (beta, lambda) tuple into an EclPoint
  void
  astronomy_bl_to_EclPoint(EclPoint& e, const tuple& bl)
  {
    e.SetBetaLambda(extract<double>(bl[0]),
                    extract<double>(bl[1]));
  }

}

/// Expose EclPoint to boost::python
void
pybind_astronomy_EclPoint()
{
  class_<EclPoint>
    ("EclPoint",
     "Ecliptic latitude and longitude")
    .def(init<const double, const double>())
    .def(init<const EclPoint&>())
    .add_property("beta", &EclPoint::GetBeta,
                  "Ecliptic latitude (read-only).")
    .add_property("lambda", &EclPoint::GetLambda,
                  "Ecliptic longitude (read-only).")
    .add_property("beta_lambda",
                  astronomy_EclPoint_to_bl,
                  astronomy_bl_to_EclPoint,
                  "Tuple access as (beta, lambda).")
    .def("angle", &EclPoint::Angle, "Opening angle between two EclPoints.")
    .def(self_ns::str(self))
    ;
}

// -----------------------------------------------------------------------------
namespace {

  /// Helper function to convert HorPoint into a python (elev, azi) tuple
  tuple
  astronomy_HorPoint_to_elaz(const HorPoint& h)
  {
    return make_tuple(h.GetElevation(), h.GetAzimuth());
  }

  /// Helper function to convert a python (elev, azi) tuple into a HorPoint
  void
  astronomy_elaz_to_HorPoint(HorPoint& h, const tuple& elaz)
  {
    h.SetElevationAzimuth(extract<double>(elaz[0]),
                          extract<double>(elaz[1]));
  }

}

/// Expose HorPoint to boost::python
void
pybind_astronomy_HorPoint()
{
  class_<HorPoint>
    ("HorPoint",
     "Left-handed horizon coordinates (elevation and azimuth)")
    .def(init<const double, const double>())
    .def(init<const HorPoint&>())
    .add_property("elevation", &HorPoint::GetElevation,
                  "Horizon elevation (read-only).")
    .add_property("azimuth", &HorPoint::GetAzimuth,
                  "Horizon azimuth (read-only).")
    .add_property("elev_azim",
                  astronomy_HorPoint_to_elaz,
                  astronomy_elaz_to_HorPoint,
                  "Tuple access as (elev, azim).")
    .def("angle", &HorPoint::Angle, "Opening angle between two HorPoints.")
    .def(self_ns::str(self))
    ;
}

// -----------------------------------------------------------------------------
namespace {

  /// Helper function to convert HrMinSec into a python (hr, min, sec) tuple
  tuple
  astronomy_HrMinSec_to_tuple(const HrMinSec& a)
  {
    return make_tuple(a.GetHour(), a.GetMinute(), a.GetSecond());
  }

  void
  astronomy_tuple_to_HrMinSec(HrMinSec& a, const tuple& t)
  {
    a = HrMinSec(extract<double>(t[0]),
                 extract<double>(t[1]),
                 extract<double>(t[2]));
  }

  /// Helper function to convert DegMinSec into a python (deg, amin, asec) tuple
  tuple
  astronomy_DegMinSec_to_tuple(const DegMinSec& a)
  {
    return make_tuple(a.GetDegree(), a.GetArcminute(), a.GetArcsecond());
  }

  void
  astronomy_tuple_to_DegMinSec(DegMinSec& a, const tuple& t)
  {
    a = DegMinSec(extract<double>(t[0]),
                  extract<double>(t[1]),
                  extract<double>(t[2]));
  }

}

/// Define python bindings for the HrMinSec and DegMinSec classes
void
pybind_astronomy_AstroCoords()
{
  class_<HrMinSec>
    ("HrMinSec",
     "Storage of a celestial angle in hour:minute:second format.")
    .def(init<double>())
    .def(init<double, double, double>())
    .def(init<const HrMinSec&>())
    .add_property("hour", &HrMinSec::GetHour, "Angle hour [HAWC Units].")
    .add_property("minute", &HrMinSec::GetMinute, "Angle minute [HAWC Units].")
    .add_property("second", &HrMinSec::GetSecond, "Angle second [HAWC Units].")
    .add_property("hr_min_sec",
                  astronomy_HrMinSec_to_tuple,
                  astronomy_tuple_to_HrMinSec,
                  "Tuple access (hr, min, sec) in HAWC base units.")

    // Conversion to string/float
    .def(self_ns::str(self))
    .def(self_ns::float_(self))
    ;

  class_<DegMinSec>
    ("DegMinSec",
     "Storage of a celestial angle in degree:arcmin:arcsec format."
     )
    .def(init<double>())
    .def(init<double, double, double>())
    .def(init<const DegMinSec&>())
    .add_property("degree", &DegMinSec::GetDegree, "Angle degree [HAWC Units].")
    .add_property("arcmin", &DegMinSec::GetArcminute,
                  "Angle arcminutes [HAWC Units].")
    .add_property("arcsec", &DegMinSec::GetArcsecond,
                  "Angle arcseconds [HAWC Units].")
    .add_property("deg_min_sec",
                  astronomy_DegMinSec_to_tuple,
                  astronomy_tuple_to_DegMinSec,
                  "Tuple access (deg, min, sec) in HAWC base units.")

    // Conversion to string/float
    .def(self_ns::str(self))
    .def(self_ns::float_(self))
    ;
}

