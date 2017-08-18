#include <boost/python.hpp>

#include <data-structures/time/ModifiedJulianDate.h>

#include <data-structures/geometry/Vector.h>
#include <data-structures/geometry/LatLonAlt.h>

#include <data-structures/astronomy/EquPoint.h>
#include <data-structures/astronomy/EclPoint.h>
#include <data-structures/astronomy/HorPoint.h>
#include <data-structures/astronomy/GalPoint.h>

#include <astro-service/AstroService.h>
#include <astro-service/StdAstroService.h>
#include <astro-service/Moon.h>
#include <astro-service/GeoDipoleService.h>

using namespace boost::python;

/*!
 * @class AstroServiceWrap
 * @brief Expose AstroService abstract base class to boost::python
 */
class AstroServiceWrap : public AstroService, public wrapper<AstroService> {

  public:

    double GetGMST(const ModifiedJulianDate& mjd) const
    { return this->get_override("GetGMST")(mjd); }

    double GetAST(const ModifiedJulianDate& mjd) const
    { return this->get_override("GetAST")(mjd); }

    void Loc2Equ(const ModifiedJulianDate& mjd, const LatLonAlt& lla,
                 const Vector& a, EquPoint& e, const TimeSystem sys=SIDEREAL,
                 bool toJ2000=false) const
    { this->get_override("Loc2Equ")(mjd, lla, a, e, sys, toJ2000); }

    void Equ2Loc(const ModifiedJulianDate& mjd, const LatLonAlt& lla,
                 const EquPoint& e, Vector& a, const TimeSystem sys=SIDEREAL,
                 bool fromJ2000=false) const
    { this->get_override("Equ2Loc")(mjd, lla, e, a, sys, fromJ2000); }

    void Hor2Equ(const ModifiedJulianDate& mjd, const LatLonAlt& lla,
                 const HorPoint& h, EquPoint& e) const
    { this->get_override("Hor2Equ")(mjd, lla, h, e); }

    void Equ2Hor(const ModifiedJulianDate& mjd, const LatLonAlt& lla,
                 const EquPoint& e, HorPoint& h) const
    { this->get_override("Equ2Hor")(mjd, lla, e, h); }

    void Ecl2Equ(const ModifiedJulianDate& mjd, const EclPoint& ec,
                 EquPoint& eq) const
    { this->get_override("Ecl2Equ")(mjd, ec, eq); }

    void Equ2Ecl(const ModifiedJulianDate& mjd, const EquPoint& eq,
                 EclPoint& ec) const
    { this->get_override("Equ2Ecl")(mjd, eq, ec); }

    void Gal2Equ(const GalPoint& gp, EquPoint& eq) const
    { this->get_override("Gal2Equ")(gp, eq); }

    void Equ2Gal(const EquPoint& eq, GalPoint& gp) const
    { this->get_override("Equ2Gal")(eq, gp); }

    void GetLunarGeocentricPosition(const ModifiedJulianDate& mjd,
                                    EquPoint& eq) const
    { this->get_override("GetLunarGeocentricPosition")(mjd, eq); }

    void GetLunarTopocentricPosition(const ModifiedJulianDate& mjd,
                                     const LatLonAlt& lla, EquPoint& eq) const
    { this->get_override("GetLunarTopocentricPosition")(mjd, lla, eq); }

    void GetSolarGeocentricPosition(const ModifiedJulianDate& mjd,
                                    EquPoint& eq) const
    { this->get_override("GetSolarGeocentricPosition")(mjd, eq); }

    void Precess(const ModifiedJulianDate& mjd1, const ModifiedJulianDate& mjd2,
                 EquPoint& eq) const
    { this->get_override("Precess")(mjd1, mjd2, eq); }

};

void
pybind_astro_service_AstroService()
{
  enum_<AstroService::TimeSystem>
    ("TimeSystem",
     "Time system enum for local to celestial coordinate conversions")
    .value("SIDEREAL", AstroService::SIDEREAL)
    .value("ANTISIDEREAL", AstroService::ANTISIDEREAL)
    .value("SOLAR", AstroService::SOLAR)
    .export_values()
    ;

  class_<AstroServiceWrap, boost::noncopyable>
    ("AstroService",
     "Abstract interface for common astronomical transformations",
    no_init)
    .def("GetGMST", pure_virtual(&AstroService::GetGMST),
         "Convert MJD to mean sidereal time at Greenwich.")
    .def("GetAST", pure_virtual(&AstroService::GetAST),
         "Convert MJD to anti-sidereal time, for systematic checks.")
    .def("Loc2Equ", pure_virtual(&AstroService::Loc2Equ),
        (arg("mjd"), arg("lla"), arg("a"), arg("e"),
         arg("sys")=AstroService::SIDEREAL,
         arg("toJ2000")=false),
         "Convert local to celestial coordinates (current epoch).")
    .def("Equ2Loc", pure_virtual(&AstroService::Equ2Loc),
        (arg("mjd"), arg("lla"), arg("e"), arg("a"),
         arg("sys")=AstroService::SIDEREAL,
         arg("fromJ2000")=false),
         "Convert celestial to local coordinates (current epoch)")
    .def("Hor2Equ", pure_virtual(&AstroService::Hor2Equ),
         "Convert horizon to celestial coordinates.")
    .def("Equ2Hor", pure_virtual(&AstroService::Equ2Hor),
         "Convert celestial to horizon coordinates.")
    .def("Ecl2Equ", pure_virtual(&AstroService::Ecl2Equ),
         "Convert ecliptic to equatorial coordinates.")
    .def("Equ2Ecl", pure_virtual(&AstroService::Equ2Ecl),
         "Convert equatorial to ecliptic coordinates.")
    .def("Gal2Equ", pure_virtual(&AstroService::Gal2Equ),
         "Convert galactic to celestial coordinates (J2000 epoch).")
    .def("Equ2Gal", pure_virtual(&AstroService::Equ2Gal),
         "Convert celestial to galactic coordinates (J2000 epoch).")
    .def("GetLunarGeocentricPosition",
         pure_virtual(&AstroService::GetLunarGeocentricPosition),
         "Get the geocentric coordinates of the Moon at some time and place.")
    .def("GetLunarTopocentricPosition",
         pure_virtual(&AstroService::GetLunarTopocentricPosition),
         "Get the topocentric coordinates of the Moon at some time and place.")
    .def("GetSolarGeocentricPosition",
         pure_virtual(&AstroService::GetSolarGeocentricPosition),
         "Get the geocentric coordinates of the Sun at some time and place.")
    .def("Precess", pure_virtual(&AstroService::Precess),
         "Precess a celestial coordinate from one epoch to another.")
    ;
}

void
pybind_astro_service_StdAstroService()
{
  class_<StdAstroService, bases<AstroService> >
    ("StdAstroService",
     "Default service for handling astronomical calculations")
    .def("DefaultConfiguration", &StdAstroService::DefaultConfiguration)
    .def("Initialize", &StdAstroService::Initialize)
    .def("Finish", &StdAstroService::Finish)
    ;
}

void
pybind_astro_service_Moon()
{
  class_<Moon>("Moon")
    .def("GetGeocentricPoint", &Moon::GetGeocentricPoint,
         return_value_policy<manage_new_object>())
    .staticmethod("GetGeocentricPoint")
    ;
}

/*!
 * @class GeomagneticFieldWrap
 * @brief Expose GeomagneticField abstract base class to python
 */
class GeomagneticFieldWrap : public GeomagneticField,
                             public wrapper<GeomagneticField> {

  public:

    void GetField(const Point& p, const ModifiedJulianDate& m, Vector& B) const
    { this->get_override("GetField")(p, m, B); }

};

void
pybind_astro_service_GeomagneticField()
{
  class_<GeomagneticFieldWrap, boost::noncopyable>
    ("GeomagneticField",
     "Abstract interface for geomagnetic field models.",
    no_init)
    .def("GetField", pure_virtual(&GeomagneticField::GetField),
         "Return field orientation at a given location and time.")
    ;
}

void
pybind_astro_service_GeoDipoleService()
{
  class_<GeoDipoleService, bases<GeomagneticField> >
    ("GeoDipoleService",
     "Calculate Earth's magnetic field using a basic dipole model.")
    .def("DefaultConfiguration", &StdAstroService::DefaultConfiguration)
    .def("Initialize", &StdAstroService::Initialize)
    .def("Finish", &StdAstroService::Finish)
    ;
}

