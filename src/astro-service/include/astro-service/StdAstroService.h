/*!
 * @file StdAstroService.h
 * @brief Implementation of some astronomical transformations.
 * @author Segev BenZvi
 * @date 28 May 2010
 * @version $Id: StdAstroService.h 36302 2016-12-12 20:25:09Z dlennarz $
 */

#ifndef ASTROSERVICE_STDASTROSERVICE_H_INCLUDED
#define ASTROSERVICE_STDASTROSERVICE_H_INCLUDED

#include <astro-service/AstroService.h>

#include <data-structures/geometry/R3Transform.h>
#include <data-structures/geometry/Point.h>
#include <data-structures/astronomy/EquPoint.h>
#include <data-structures/time/ModifiedJulianDate.h>

#include <hawcnest/Service.h>

/*!
 * @class StdAstroService
 * @author Segev BenZvi
 * @date 28 May 2010
 * @ingroup astro_xforms
 * @brief Implement a default service for handling astronomical calculations
 */
class StdAstroService : public AstroService {

  public:

    typedef AstroService Interface;

    Configuration DefaultConfiguration();

    void Initialize(const Configuration& config);

    void Finish() { }

    /// Mean sidereal time at Greenwich
    double GetGMST(const ModifiedJulianDate& mjd) const;

    /// Anti-sidereal time, a non-physical time system for systematic checks
    double GetAST(const ModifiedJulianDate& mjd) const;

    /// Mean obliquity of the ecliptic \f$\varepsilon_0\f$ (IAU 1984)
    double GetMeanObliquity(const ModifiedJulianDate& mjd) const;

    /// Mean obliquity \f$\varepsilon_0\f$ (Laskar A&A 157 (1986), 68)
    double GetMeanObliquityLaskar(const ModifiedJulianDate& mjd) const;

    /// Local direction/axis to equatorial conversion
    void Loc2Equ(const ModifiedJulianDate& mjd, const LatLonAlt& lla,
                 const Vector& axis, EquPoint& equ,
                 const TimeSystem sys = SIDEREAL,
                 const bool toJ2000 = false) const;

    /// Equatorial to local direction/axis conversion
    void Equ2Loc(const ModifiedJulianDate& mjd, const LatLonAlt& lla,
                 const EquPoint& equ, Vector& axis,
                 const TimeSystem sys = SIDEREAL,
                 const bool fromJ2000 = false) const;

    /// Horizontal to equatorial conversion
    void Hor2Equ(const ModifiedJulianDate& mjd, const LatLonAlt& latLonAlt,
                 const HorPoint& hor, EquPoint& equ) const;

    /// Equatorial to horizontal conversion
    void Equ2Hor(const ModifiedJulianDate& mjd, const LatLonAlt& latLonAlt,
                 const EquPoint& equ, HorPoint& hor) const;

    /// Ecliptic to equatorial conversion at some modified Julian date
    void Ecl2Equ(const ModifiedJulianDate& mjd, const EclPoint& ecl,
                 EquPoint& equ) const;

    /// Equatorial to ecliptic conversion at some modified Julian date
    void Equ2Ecl(const ModifiedJulianDate& mjd, const EquPoint& equ,
                 EclPoint& ecl) const;

    /// Equatorial to galactic coordinate transformation
    void Equ2Gal(const EquPoint& equ, GalPoint& gal) const;

    /// Galactic to equatorial coordinate transformation
    void Gal2Equ(const GalPoint& gal, EquPoint& equ) const;

    /// Get the geocentric equatorial coordinates of the moon at a given time
    void GetLunarGeocentricPosition(const ModifiedJulianDate& mjd,
                                    EquPoint& equ) const;

    /// Get the topocentric equatorial coordinates of the moon at a given time
    /// (accounts for parallax due to position on the surface of the Earth)
    void GetLunarTopocentricPosition(const ModifiedJulianDate& mjd,
                                     const LatLonAlt& localPoint,
                                     EquPoint& equ) const;
    /// Get the geocentric equatorial coordinates of the sun at a given time
    void GetSolarGeocentricPosition(const ModifiedJulianDate& mjd,
                                    EquPoint& equ) const;

    /// Precess an equatorial coordinate from a given date to a given epoch
    void Precess(const ModifiedJulianDate& epoch, const ModifiedJulianDate& mjd,
                 EquPoint& equ) const;

    /// Precess an equatorial coordinate from J2000 to given epoch
    void PrecessFromJ2000ToEpoch(const ModifiedJulianDate& epoch,
                                 EquPoint& equ) const;

    /// Precess an equatorial coordinate from given epoch to J2000
    void PrecessFromEpochToJ2000(const ModifiedJulianDate& epoch,
                                 EquPoint& equ) const;

  private:

    bool cachePrecess_;          ///< Flag to cache precession/nutation matrices
    mutable Rotate nupreMtx_;    ///< Combined nutation and precession matrix

    mutable ModifiedJulianDatePtr cachedMJD_;   ///< MJD for cached precession
    mutable ModifiedJulianDatePtr cachedEpoch_; ///< MJD for cached precession

    mutable Point moonPos_;     ///< Geocentric position of the moon
    mutable EquPoint sunPos_;   ///< Geocentric position of the sun

};


#endif // ASTROSERVICE_STDASTROSERVICE_H_INCLUDED
