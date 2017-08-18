/*!
 * @file AerieVsIceCube.cc
 * @brief Astronomical transformations, compared to IceCube coordinate-service.
 * @author Segev BenZvi
 * @date 27 Jan 2014
 * @ingroup unit_test
 * @version $Id: AerieVsIceCube.cc 19008 2014-03-08 17:11:08Z sybenzvi $
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <hawcnest/HAWCNest.h>
#include <hawcnest/HAWCUnits.h>
#include <hawcnest/test/OutputConfig.h>

#include <data-structures/astronomy/EquPoint.h>
#include <data-structures/astronomy/GalPoint.h>
#include <data-structures/astronomy/AstroCoords.h>

#include <data-structures/geometry/Vector.h>
#include <data-structures/geometry/LatLonAlt.h>

#include <data-structures/time/UTCDateTime.h>
#include <data-structures/time/ModifiedJulianDate.h>

#include <astro-service/Moon.h>
#include <astro-service/StdAstroService.h>

using namespace HAWCUnits;
using namespace std;


// Define some time/date/location constants used in all tests in this suite
class TestCoords {

  public:

    TestCoords() :

      locale(DegMinSec(18*degree, 59*arcminute, 41.63*arcsecond),
            -DegMinSec(97*degree, 18*arcminute, 27.39*arcsecond),
             4096*meter),

      J2000_UT(2000, 1, 1, 11, 58, 55, 816000000),
      J2000(J2000_UT),

      t0(2007, 10, 4, 3, 3, 3),
      mjd0(t0),

      t1(2010, 4, 27, 19, 19, 19),
      mjd1(t1)
    { }

   ~TestCoords() { }

    const LatLonAlt locale;
    const UTCDateTime J2000_UT;
    const ModifiedJulianDate J2000;

    const UTCDateTime t0;
    const ModifiedJulianDate mjd0;

    const UTCDateTime t1;
    const ModifiedJulianDate mjd1;

};


BOOST_FIXTURE_TEST_SUITE(IceCubeVsAERIE, TestCoords)

  // Global logger settings.
  // Note: this will affect all test suites in the astro-convert project
  BOOST_GLOBAL_FIXTURE(OutputConfig);

  //____________________________________________________________________________
  // Equatorial to local: check agreement with IceTray to within 10"
  BOOST_AUTO_TEST_CASE(EquatorialToLocal)
  {
    HAWCNest nest;
    nest.Service("StdAstroService", "astroX");
    nest.Configure();

    const AstroService& astroX = GetService<AstroService>("astroX");

    EquPoint equ;             // Celestial coordinates (current epoch)
    EquPoint equJ2000;        // Celestial coordinates (J2000 epoch)

    Vector axis;              // Local coords from AERIE
    Vector axisIC;            // Local coords from IceTray coordinate-service

    // Position of Sirius: 2007-10-04 03:03:03UT
    equJ2000.SetRADec(HrMinSec(6*hour, 45*minute, 8.90*second),
                     -DegMinSec(16*degree, 42*arcminute, 58*arcsecond));
    
    equ = equJ2000;
    astroX.Precess(mjd0, J2000, equ);
    astroX.Equ2Loc(mjd0, locale, equ, axis);

    axisIC.SetRThetaPhi(1., 142.3996664*degree, 357.0334108*degree);

    BOOST_CHECK_SMALL(axis.Angle(axisIC), 10*arcsecond);

    // Position of Sirius: 2010-04-27 19:19:19UT
    equ = equJ2000;
    astroX.Precess(mjd1, J2000, equ);
    astroX.Equ2Loc(mjd1, locale, equ, axis);

    axisIC.SetRThetaPhi(1., 63.35031153*degree, 329.1150093*degree);

    BOOST_CHECK_SMALL(axis.Angle(axisIC), 10*arcsecond);

    // Position of Canopus: 2007-10-04 03:03:03UT
    equJ2000.SetRADec(HrMinSec(6*hour, 23*minute, 57.10*second),
                     -DegMinSec(52*degree, 41*arcminute, 45*arcsecond));
    
    equ = equJ2000;
    astroX.Precess(mjd0, J2000, equ);
    astroX.Equ2Loc(mjd0, locale, equ, axis);

    axisIC.SetRThetaPhi(1., 131.6908151*degree, 304.9177740*degree);

    BOOST_CHECK_SMALL(axis.Angle(axisIC), 10*arcsecond);

    // Position of Canopus: 2010-04-27 19:19:19UT
    equ = equJ2000;
    astroX.Precess(mjd1, J2000, equ);
    astroX.Equ2Loc(mjd1, locale, equ, axis);

    axisIC.SetRThetaPhi(1., 82.78900343*degree, 296.9302978*degree);

    BOOST_CHECK_SMALL(axis.Angle(axisIC), 10*arcsecond);

    // Position of Arcturus: 2007-10-04 03:03:03UT
    equJ2000.SetRADec(HrMinSec(14*hour, 15*minute, 39.70*second),
                      DegMinSec(19*degree, 10*arcminute, 57*arcsecond));
    
    equ = equJ2000;
    astroX.Precess(mjd0, J2000, equ);
    astroX.Equ2Loc(mjd0, locale, equ, axis);

    axisIC.SetRThetaPhi(1., 98.77779040*degree, 156.1649493*degree);

    BOOST_CHECK_SMALL(axis.Angle(axisIC), 10*arcsecond);

    // Position of Arcturus: 2010-04-27 19:19:19UT
    equ = equJ2000;
    astroX.Precess(mjd1, J2000, equ);
    astroX.Equ2Loc(mjd1, locale, equ, axis);

    axisIC.SetRThetaPhi(1., 139.4235886*degree, 69.20611779*degree);

    BOOST_CHECK_SMALL(axis.Angle(axisIC), 10*arcsecond);

    // Position of Antares: 2007-10-04 03:03:03UT
    equJ2000.SetRADec(HrMinSec(16*hour, 29*minute, 24.40*second),
                     -DegMinSec(26*degree, 25*arcminute, 55*arcsecond));
    
    equ = equJ2000;
    astroX.Precess(mjd0, J2000, equ);
    astroX.Equ2Loc(mjd0, locale, equ, axis);

    axisIC.SetRThetaPhi(1., 84.45277878*degree, 210.4464622*degree);

    BOOST_CHECK_SMALL(axis.Angle(axisIC), 10*arcsecond);

    // Position of Antares: 2010-04-27 19:19:19UT
    equ = equJ2000;
    astroX.Precess(mjd1, J2000, equ);
    astroX.Equ2Loc(mjd1, locale, equ, axis);

    axisIC.SetRThetaPhi(1., 160.7048303*degree, 206.272360*degree);

    BOOST_CHECK_SMALL(axis.Angle(axisIC), 10*arcsecond);
  }

  //____________________________________________________________________________
  // Local to equatorial: check agreement with IceTray to within 1"
  BOOST_AUTO_TEST_CASE(LocalToEquatorial)
  {
    HAWCNest nest;
    nest.Service("StdAstroService", "astroX");
    nest.Configure();

    const AstroService& astroX = GetService<AstroService>("astroX");

    Vector axis;              // Local coords from AERIE
    EquPoint equ;             // Celestial coordinates (current epoch)
    EquPoint equIC;           // Cel. coords. from IceTray coordinate-service

    // 1a) Check local coord. conversion & precession: 2007-10-04 03:03:03UT
    axis.SetRThetaPhi(1., 11.97*degree, 23.46*degree);

    astroX.Loc2Equ(mjd0, locale, axis, equ);
    equIC.SetRADec(332.8476313*degree, 23.35820262*degree);

    BOOST_CHECK_SMALL(equ.Angle(equIC), 1*arcsecond);

    astroX.Precess(J2000, mjd0, equ);
    equIC.SetRADec(332.7561567*degree, 23.32024871*degree);

    BOOST_CHECK_SMALL(equ.Angle(equIC), 1*arcsecond);

    // 1b) Check local coord. conversion & precession: 2010-04-27 19:19:19UT
    astroX.Loc2Equ(mjd1, locale, axis, equ);
    equIC.SetRADec(60.148467*degree, 23.35820262*degree);

    BOOST_CHECK_SMALL(equ.Angle(equIC), 1*arcsecond);

    astroX.Precess(J2000, mjd1, equ);
    equIC.SetRADec(59.99044059*degree, 23.32804854*degree);

    BOOST_CHECK_SMALL(equ.Angle(equIC), 1*arcsecond);

    // 2a) Check local coord. conversion & precession: 2007-10-04 03:03:03UT
    axis.SetRThetaPhi(1., 22.97*degree, 97.07*degree);

    astroX.Loc2Equ(mjd0, locale, axis, equ);
    equIC.SetRADec(317.1956604*degree, 41.74906637*degree);

    BOOST_CHECK_SMALL(equ.Angle(equIC), 1*arcsecond);

    astroX.Precess(J2000, mjd0, equ);
    equIC.SetRADec(317.1228164*degree, 41.71851115*degree);

    BOOST_CHECK_SMALL(equ.Angle(equIC), 1*arcsecond);

    // 2b) Check local coord. conversion & precession: 2010-04-27 19:19:19UT
    astroX.Loc2Equ(mjd1, locale, axis, equ);
    equIC.SetRADec(44.49649616*degree, 41.74906637*degree);

    BOOST_CHECK_SMALL(equ.Angle(equIC), 1*arcsecond);

    astroX.Precess(J2000, mjd1, equ);
    equIC.SetRADec(44.32400099*degree, 41.70630271*degree);

    BOOST_CHECK_SMALL(equ.Angle(equIC), 1*arcsecond);
  }

  //____________________________________________________________________________
  // Equatorial to galactic: check agreement with IceTray to within 1"
  BOOST_AUTO_TEST_CASE(EquatorialToGalactic)
  {
    HAWCNest nest;
    nest.Service("StdAstroService", "astroX");
    nest.Configure();

    const AstroService& astroX = GetService<AstroService>("astroX");

    EquPoint equ;             // Celestial coordinates (current epoch)
    GalPoint gal;             // Galactic coordinates (AERIE)
    GalPoint galIC;           // Galactic coorids (IceTray coordinate-service)

    // Point 1
    equ.SetRADec(332.7561567*degree, 23.32024871*degree);
    astroX.Equ2Gal(equ, gal);
    galIC.SetBL(-26.3987420*degree, 81.60074826*degree);

    BOOST_CHECK_SMALL(gal.Angle(galIC), 1*arcsecond);

    // Point 2 
    equ.SetRADec(317.1228164*degree, 41.71851115*degree);
    astroX.Equ2Gal(equ, gal);
    galIC.SetBL(-4.046108614*degree, 84.73038891*degree);

    BOOST_CHECK_SMALL(gal.Angle(galIC), 1*arcsecond);

    // Point 3
    equ.SetRADec(287.2718991*degree, 8.939760454*degree);
    astroX.Equ2Gal(equ, gal);
    galIC.SetBL(0.1844045145*degree, 42.8916759*degree);

    BOOST_CHECK_SMALL(gal.Angle(galIC), 1*arcsecond);

    // Point 4
    equ.SetRADec(36.66403088*degree, -23.35716293*degree);
    astroX.Equ2Gal(equ, gal);
    galIC.SetBL(-68.17535375*degree, -151.721019*degree);

    BOOST_CHECK_SMALL(gal.Angle(galIC), 1*arcsecond);

    // Point 5
    equ.SetRADec(64.02913095*degree, 20.17229567*degree);
    astroX.Equ2Gal(equ, gal);
    galIC.SetBL(-21.50384914*degree, 174.7384819*degree);

    BOOST_CHECK_SMALL(gal.Angle(galIC), 1*arcsecond);

    // Point 6
    equ.SetRADec(149.8390446*degree, 45.19139341*degree);
    astroX.Equ2Gal(equ, gal);
    galIC.SetBL(51.45002656*degree, 173.4234998*degree);

    BOOST_CHECK_SMALL(gal.Angle(galIC), 1*arcsecond);

    // Point 7
    equ.SetRADec(177.6681505*degree, -22.54625482*degree);
    astroX.Equ2Gal(equ, gal);
    galIC.SetBL(38.19985422*degree, -75.00459182*degree);

    BOOST_CHECK_SMALL(gal.Angle(galIC), 1*arcsecond);

    // Point 8
    equ.SetRADec(140.7488516*degree, -18.98483424*degree);
    astroX.Equ2Gal(equ, gal);
    galIC.SetBL(21.62346875*degree, -110.4628814*degree);

    BOOST_CHECK_SMALL(gal.Angle(galIC), 1*arcsecond);
  }

  //____________________________________________________________________________
  // Lunar position: check agreement with IceTray to within 30" (<0.01 deg)
  BOOST_AUTO_TEST_CASE(LunarPosition)
  {
    HAWCNest nest;
    nest.Service("StdAstroService", "astroX");
    nest.Configure();

    const AstroService& astroX = GetService<AstroService>("astroX");

    EquPoint equ;             // Celestial coordinates (current epoch)
    EquPoint equIC;           // Cel. coords. from IceTray coordinate-service

    // Time 1
    ModifiedJulianDate mjd(UTCDateTime(2008,11,8, 23,34,54));

    astroX.GetLunarTopocentricPosition(mjd, locale, equ);
    equIC.SetRADec(350.2299855*degree, -1.581314809*degree);

    BOOST_CHECK_SMALL(equ.Angle(equIC), 30*arcsecond);

    // Time 2 
    mjd = ModifiedJulianDate(UTCDateTime(2008,10,22, 10,30,45));

    astroX.GetLunarTopocentricPosition(mjd, locale, equ);
    equIC.SetRADec(134.6256410*degree, 17.62666958*degree);

    BOOST_CHECK_SMALL(equ.Angle(equIC), 30*arcsecond);

    // Time 3 
    mjd = ModifiedJulianDate(UTCDateTime(2002,3,2, 18,20,35));

    astroX.GetLunarTopocentricPosition(mjd, locale, equ);
    equIC.SetRADec(208.3084724*degree, -7.795243285*degree);

    BOOST_CHECK_SMALL(equ.Angle(equIC), 1.1*arcminute);

    // Time 4
    mjd = ModifiedJulianDate(UTCDateTime(2000,11,15, 7,3,22));

    astroX.GetLunarTopocentricPosition(mjd, locale, equ);
    equIC.SetRADec(100.2067691*degree, 22.42668680*degree);

    BOOST_CHECK_SMALL(equ.Angle(equIC), 1.1*arcminute);
  }

BOOST_AUTO_TEST_SUITE_END()

