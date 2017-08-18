/*!
 * @file GeomagneticField.h
 * @brief Calculate the orientation of the geomagnetic field.
 * @author Segev BenZvi
 * @date 10 Jan 2013
 * @version $Id: GeomagneticField.h 27616 2015-11-09 13:58:19Z sybenzvi $
 */

#ifndef ASTROSERVICE_GEOMAGNETICFIELD_H_INCLUDED
#define ASTROSERVICE_GEOMAGNETICFIELD_H_INCLUDED

class ModifiedJulianDate;
class Point;
class Vector;

/*!
 * @class GeomagneticField
 * @author Segev BenZvi
 * @date 10 Jan 2013
 * @ingroup astro_xforms
 * @brief Calculate the orientation of the geomagnetic field given a geocentric
 *        position and time.
 */
class GeomagneticField {

  public:

    virtual ~GeomagneticField() { }

    /// Return field orientation at a geocentric location p and time MJD
    virtual void GetField(const Point& p, const ModifiedJulianDate& mjd,
                          Vector& B) const = 0;

};

#endif // ASTROSERVICE_GEOMAGNETICFIELD_H_INCLUDED

