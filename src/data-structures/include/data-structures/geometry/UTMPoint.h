/*!
 * @file UTMPoint.h
 * @brief Representation of points in the Universal Transverse Mercator system.
 * @author Segev BenZvi
 * @version $Id: UTMPoint.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_GEOMETRY_UTMPOINT_H_INCLUDED
#define DATACLASSES_GEOMETRY_UTMPOINT_H_INCLUDED

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/PointerTypedefs.h>

class LatLonAlt;

/*!
 * @class UTMPoint 
 * @author Lukas Nellen
 * @author Segev BenZvi
 * @date 21 Jun 2011
 * @ingroup geometry_data
 * @brief A geodetic coordinate, given by UTM easting, northing, and zone
 *        number
 *
 * This class stores geodetic coordinates using the Universal Transverse
 * Mercator, or UTM, projection of the surface of the Earth.  UTM divides the
 * surface into a 60x20 grid.  The longitude zones labeled from 1 to 60 moving
 * east from the International Date Line.  The latitude bands are labeled 
 * C to X (with O and I omitted) starting from 80 degrees south and running to
 * 84 degrees north.  Hence, all bands >=N are in the northern hemisphere.
 * Grid points, usually expressed in meters, are referred to as easting
 * (x-direction) and northing (y-direction) and are mirrored above and below
 * the equator.
 *
 * Atitude is expressed as "ellipsoidal" height, i.e., with respect to the
 * ellipsoid of the Earth.  This is typically the height given by GPS devices,
 * and differs from the mean height from sea level (orthometric height).  The
 * difference occurs because orthometric height is determined with respect to
 * the geoid.
 *
 * The absolute value of the difference between the ellipsoid and the geoid is
 * <100 meters at any point on the Earth's surface.  The geoid model is
 * necessary only if one needs cm/mm precision for orthometric measurements.
 *
 * A point on the Earth's surface is expressed in terms of grid easting and
 * northing, plus zone information, and ellipsoidal height.  For example the
 * summit of the Pico de Orizaba volcano in Mexico is at
 * 682087E 2105099N 14Q (UTM), 5636 meters.
 *
 * A UTMPoint can be converted directly into a latitude, longitude, and
 * altitude triplet.
 */
class UTMPoint {

  public:

    UTMPoint() :
      easting_(682087*HAWCUnits::meter),
      northing_(2105099*HAWCUnits::meter),
      height_(5636*HAWCUnits::meter),
      zone_(14),
      band_('Q')
    { }

    /// Creation from easting, northing, zone, band, and height
    UTMPoint(const double east, const double north, const double ht,
             const int zone, const char band) :
      easting_(east),
      northing_(north),
      height_(ht),
      zone_(zone),
      band_(band)
    { }

    /// Creation from geodetic latitude, longitude, and ellipsoidal height
    UTMPoint(const double lat, const double lon, const double ht);

    /// Creation of a UTMPoint from LatLonAlt
    UTMPoint(const LatLonAlt& lla);

    /// Get the easting
    double GetEasting() const { return easting_; }

    /// Get the northing
    double GetNorthing() const { return northing_; }

    /// Retrieve ellipsoidal height
    double GetHeight() const { return height_; }

    /// Get the longitude zone
    int GetZone() const { return zone_; }

    /// Get the latitude band
    char GetBand() const { return band_; }

    /// Calculate geodetic latitude and longitude
    void GetLatitudeLongitude(double& lat, double& lon) const;

  private:

    double easting_;   ///< UTM easting
    double northing_;  ///< UTM northing
    double height_;    ///< Ellipsoidal height (given by GPS)
    int zone_;         ///< Longitude zone ID [1..60]
    char band_;        ///< Latitude band ID [C..X]

    /// Set point by latitude, longitude, and ellipsoidal height
    void SetLatitudeLongitudeHeight(double lat, double lon, double ht);

    /// Calculate the UTM longitude zone from latitude and longitude
    static int ZoneFromLatitudeLongitude(const double lat, const double lon);

    /// Calculate the UTM latitude band from latitude
    static char BandFromLatitude(const double lat);

  friend std::ostream& operator<<(std::ostream& os, const UTMPoint& u);

};

SHARED_POINTER_TYPEDEFS(UTMPoint);

#endif // DATACLASSES_GEOMETRY_UTMPOINT_H_INCLUDED

