/*!
 * @file AstroService.h
 * @brief Interface for astronomical transformations.
 * @author Segev BenZvi
 * @date 28 May 2010
 * @version $Id: AstroService.h 21267 2014-08-04 23:18:53Z dlennarz $
 */

#ifndef ASTROSERVICE_ASTROSERVICE_H_INCLUDED
#define ASTROSERVICE_ASTROSERVICE_H_INCLUDED

class Vector;
class EclPoint;
class EquPoint;
class GalPoint;
class HorPoint;
class LatLonAlt;

class ModifiedJulianDate;

/*!
 * @class AstroService
 * @author Segev BenZvi
 * @date 28 May 2010
 * @ingroup astro_xforms
 * @brief Abstract interface to services which perform astronomical
 *        transformations between coordinate systems
 */
class AstroService {

  public:

    enum TimeSystem {
      SIDEREAL,
      ANTISIDEREAL,
      SOLAR
    };

    virtual ~AstroService() { }

    /// Mean sidereal time at Greenwich
    virtual double GetGMST(const ModifiedJulianDate& mjd) const = 0;

    /// Anti-sidereal time, a non-physical time system for systematic checks
    virtual double GetAST(const ModifiedJulianDate& mjd) const = 0;

    /// Local direction/axis to equatorial conversion
    virtual void Loc2Equ(const ModifiedJulianDate& mjd, const LatLonAlt& lla,
                         const Vector& axis, EquPoint& equ,
                         const TimeSystem sys = SIDEREAL,
                         const bool toJ2000 = false) const = 0;

    /// Equatorial to local direction/axis conversion
    virtual void Equ2Loc(const ModifiedJulianDate& mjd, const LatLonAlt& lla,
                         const EquPoint& equ, Vector& axis,
                         const TimeSystem sys = SIDEREAL,
                         const bool fromJ2000 = false) const = 0;

    /// Horizontal to equatorial conversion
    virtual void Hor2Equ(const ModifiedJulianDate& mjd, const LatLonAlt& lla,
                         const HorPoint& hor, EquPoint& equ) const = 0;

    /// Equatorial to horizontal conversion
    virtual void Equ2Hor(const ModifiedJulianDate& mjd, const LatLonAlt& lla,
                         const EquPoint& equ, HorPoint& hor) const = 0;

    /// Ecliptic to equatorial conversion at some modified Julian date
    virtual void Ecl2Equ(const ModifiedJulianDate& mjd, const EclPoint& ecl,
                         EquPoint& equ) const = 0;

    /// Equatorial to ecliptic conversion at some modified Julian date
    virtual void Equ2Ecl(const ModifiedJulianDate& mjd, const EquPoint& equ,
                         EclPoint& ecl) const = 0;

    /// Equatorial to galactic coordinate transformation
    virtual void Equ2Gal(const EquPoint& equ, GalPoint& gal) const = 0;

    /// Galactic to equatorial coordinate transformation
    virtual void Gal2Equ(const GalPoint& gal, EquPoint& equ) const = 0;

    /// Get the geocentric equatorial coordinates of the moon at a given time
    virtual void GetLunarGeocentricPosition(const ModifiedJulianDate& mjd,
                                            EquPoint& equ) const = 0;

    /// Get the topocentric equatorial coordinates of the moon at a given time
    /// (accounts for parallax due to position on the surface of the Earth)
    virtual void GetLunarTopocentricPosition(const ModifiedJulianDate& mjd,
                                             const LatLonAlt& localPoint,
                                             EquPoint& equ) const = 0;

    /// Get the geocentric equatorial coordinates of the sun at a given time
    virtual void GetSolarGeocentricPosition(const ModifiedJulianDate& mjd,
                                            EquPoint& equ) const = 0;


    /// Precess an equatorial coordinate from a given date to a given epoch
    virtual void Precess(const ModifiedJulianDate& epoch,
                         const ModifiedJulianDate& mjd,
                         EquPoint& equ) const = 0;

    /// Precess an equatorial coordinate from J2000 to given epoch
    virtual void PrecessFromJ2000ToEpoch(const ModifiedJulianDate& epoch,
                                         EquPoint& equ) const = 0;

    /// Precess an equatorial coordinate from a given epoch to J2000
    virtual void PrecessFromEpochToJ2000(const ModifiedJulianDate& epoch,
                                         EquPoint& equ) const = 0;

//  protected:

};

#endif // ASTROSERVICE_ASTROSERVICE_H_INCLUDED
