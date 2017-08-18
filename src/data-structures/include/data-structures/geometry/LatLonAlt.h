/*!
 * @file LatLonAlt.h
 * @brief Geodetic coordinates.
 * @author Segev BenZvi
 * @version $Id: LatLonAlt.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_GEOMETRY_LATLONALT_H_INCLUDED
#define DATACLASSES_GEOMETRY_LATLONALT_H_INCLUDED

#include <hawcnest/HAWCUnits.h>
#include <data-structures/geometry/Point.h>

class UTMPoint;

/*!
 * @class LatLonAlt
 * @author Segev BenZvi
 * @date 8 Jul 2010
 * @ingroup geometry_data
 * @brief A geodetic coordinate, given by geodetic latitude, longitude,
 *        and ellipsoidal height
 *
 * This class stores geodetic coordinates, along with a geocentric point that
 * can be accessed by constant reference.  Initialization can occur using
 * either a geocentric point or a latitude, longitude, and altitude triplet.
 *
 * Note that the altitude is expressed as "ellipsoidal" height, i.e., with
 * respect to the ellipsoid of the Earth.  This is typically the height given
 * by GPS devices, and differs from the mean height from sea level (orthometric
 * height).  The difference occurs because orthometric height is determined
 * with respect to the geoid.
 *
 * The absolute value of the difference between the ellipsoid and the geoid is
 * <100 meters at any point on the Earth's surface.  The geoid model is
 * necessary only if one needs cm/mm precision for orthometric measurements.
 */
class LatLonAlt {

  public:

    LatLonAlt();

    /// Creation from geocentric coordinates
    LatLonAlt(const Point& geocentricPoint);

    /// Creation from geodetic latitude, longitude, and ellipsoidal height
    LatLonAlt(const double lat, const double lon, const double ht);

    /// Creation from a position in UTM coordinates
    LatLonAlt(const UTMPoint& u);

    /// Geodetic latitude (gives a point on the earth's ellipsoid)
    double GetLatitude() const { return latitude_; }

    /// Geodetic longitude (gives a point on the earth's ellipsoid)
    double GetLongitude() const { return longitude_; }

    /// Retrieve ellipsoidal height
    double GetHeight() const { return height_; }

    /// Get the geocentric coordinates of this point
    const Point& GetGeocentricPoint() const { return p_; }

    /// Set the geodetic latitude, longitude, and ellipsoidal height
    void SetLatitudeLongitudeHeight(double lat, double lon, double ht);

    /// Set the geocentric point (and calculate latitude, longitude, and height)
    void SetGeocentricPoint(const Point& p);

  private:

    Point p_;           ///< Geocentric coordinates
    double latitude_;   ///< Geodetic latitude
    double longitude_;  ///< Geodetic longitude (positive measured toward east)
    double height_;     ///< Ellipsoidal height (given by GPS)

  friend std::ostream& operator<<(std::ostream& os, const LatLonAlt& s);

};

SHARED_POINTER_TYPEDEFS(LatLonAlt);

#endif // DATACLASSES_GEOMETRY_LATLONALT_H_INCLUDED

