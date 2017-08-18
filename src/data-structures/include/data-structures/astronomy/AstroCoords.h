/*!
 * @file AstroCoords.h
 * @brief Conversion of astronomical coordinates to floating-point numbers.
 * @author Segev BenZvi
 * @version $Id: AstroCoords.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_ASTRONOMY_ASTROCOORDS_H_INCLUDED
#define DATACLASSES_ASTRONOMY_ASTROCOORDS_H_INCLUDED

#include <iostream>

#include <hawcnest/HAWCUnits.h>

/*!
 * @class HrMinSec
 * @author Segev BenZvi
 * @date 28 May 2010
 * @ingroup astronomy_data
 * @brief Storage/conversion of a celestial angle in hour:minute:second format
 *
 * This class provides a syntactic shortcut for angles defined in the
 * hour:minute:second astronomical format, such as right ascension or sidereal
 * time.  The double operator is overloaded to automatically convert the angle
 * to a floating-point angle in calculations.  Inputs are expected to include
 * proper dimensions; casting converts the angle to base angle units.
 *
 * Example: right ascension of Sgr A*
   @verbatim
   // Conversion to a double is automatic
   const double ra = HrMinSec(17*hour, 45*minute, 40.045*second);

   ...

   // Pretty printing: will output 17h 45m 40.045s
   cout << HrMinSec(ra) << endl;
   @endverbatim
 *
 * @todo Range check inputs?
 */
class HrMinSec {

  public:

    HrMinSec() : hr_(0), min_(0), sec_(0) { }

    /// Construction with separate hour, minute, second arguments
    HrMinSec(double h, double m, double s) : hr_(h), min_(m), sec_(s) { }

    /// Construction from decimal degrees
    HrMinSec(double hmsDeg);

    /// Automatic cast to double: hour:minute:second -> decimal degrees
    operator double() const
    { return (hr_ + min_ + sec_) * 15*HAWCUnits::degree/HAWCUnits::hour; }

    double GetHour() const { return hr_; }
    double GetMinute() const { return min_; }
    double GetSecond() const { return sec_; }

  private:

    double hr_;
    double min_;
    double sec_;

  friend std::ostream& operator<<(std::ostream& os, const HrMinSec& hms);

};

/*!
 * @class DegMinSec
 * @author Segev BenZvi
 * @date 28 May 2010
 * @ingroup astronomy_data
 * @brief Storage/conversion of an angle in degree:arcminute:arcsecond format
 *
 * This class provides a syntactic shortcut for angles defined in the
 * traditional degree:arcminute:arcsecond astronomical format.  The double
 * operator is overloaded to automatically convert the angle to a
 * floating-point angle in calculations.  Inputs are expected to include proper
 * dimensions; casting converts the angle to base angle units.
 *
 * Example: declination of Sgr A*
   @verbatim
   // In southern hemisphere, so declination is < 0
   const double dec = -DegMinSec(29*degree, 0, 27.9*arcsec);

   // Also (but more error-prone): DegMinSec(-29*degree, 0, -27.9*arcsec);

   ...
 
   // Pretty printing: will output -29d 0' 27.9"
   cout << DegMinSec(dec) << endl;
   @endverbatim
 *
 * @todo Range check inputs?
 */
class DegMinSec {

  public:

    DegMinSec() : deg_(0), min_(0), sec_(0) { }

    /// Construction from separate degree, arcminute, arcsecond arguments
    DegMinSec(double d, double m, double s) : deg_(d), min_(m), sec_(s) { }

    /// Construction from decimal degrees
    DegMinSec(double dmsDeg);

    /// Automatic cast to double: deg:arcmin:arcsec -> decimal degrees
    operator double() const { return deg_ + min_ + sec_; }

    double GetDegree() const { return deg_; }
    double GetArcminute() const { return min_; }
    double GetArcsecond() const { return sec_; }

  private:

    double deg_;
    double min_;
    double sec_;

  friend std::ostream& operator<<(std::ostream& os, const DegMinSec& dms);
};

#endif // DATACLASSES_ASTRONOMY_ASTROCOORDS_H_INCLUDED

