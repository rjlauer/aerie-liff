#!/usr/bin/env python
"""Test access to the astro-service project via its python bindings.
.. codeauthor:: Segev BenZvi
"""

__version__ = "$Id"

try:
    from hawc import hawcnest, data_structures, astro_service
    from hawc.data_structures import *
    from hawc.hawcnest import HAWCUnits as U
    from HAWCNest import HAWCNest
except ImportError as e:
    print(e)
    raise SystemExit

nest = HAWCNest()
nest.Service("StdAstroService", "astroX")
nest.Configure()

astroX = astro_service.GetService("astroX")

# Define locations and times to be used in astronomical transformations
loc = LatLonAlt(float(DegMinSec(18*U.deg, 59*U.arcmin, 41.63*U.arcsec)),
               -float(DegMinSec(97*U.deg, 18*U.arcmin, 27.39*U.arcsec)),
                4096*U.meter)

utc0 = UTCDateTime(2007,10,4, 3,3,3)
mjd0 = ModifiedJulianDate(utc0)

utc1 = UTCDateTime(2010,4,27, 19,19,19)
mjd1 = ModifiedJulianDate(utc1)

tsys = astro_service.TimeSystem.SIDEREAL
doPrecess = True

# Make some coordinate transformations to/from the HAWC coordinate system,
# correcting J2000 coordinates to the current epoch if needed
axis = Vector()
equ = EquPoint()
equIC = EquPoint()

# 1a) Check local coord. conversion & precession: 2007-10-04 03:03:03UT
axis.r_theta_phi = (1., 11.97*U.deg, 23.46*U.deg);

astroX.Loc2Equ(mjd0, loc, axis, equ);
equIC.ra_dec = (332.8476313*U.deg, 23.35820262*U.deg);
assert(equ.angle(equIC) < 1*U.arcsec);

astroX.Loc2Equ(mjd0, loc, axis, equ, tsys, doPrecess);
equIC.ra_dec = (332.7561567*U.deg, 23.32024871*U.deg);
assert(equ.angle(equIC) < 1*U.arcsec);

# 1b) Check local coord. conversion & precession: 2010-04-27 19:19:19UT
astroX.Loc2Equ(mjd1, loc, axis, equ);
equIC.ra_dec = (60.148467*U.deg, 23.35820262*U.deg);
assert(equ.angle(equIC) < 1*U.arcsec);

astroX.Loc2Equ(mjd1, loc, axis, equ, tsys, doPrecess);
equIC.ra_dec = (59.99044059*U.deg, 23.32804854*U.deg);
assert(equ.angle(equIC) < 1*U.arcsec);

# 2a) Check local coord. conversion & precession: 2007-10-04 03:03:03UT
axis.r_theta_phi = (1., 22.97*U.deg, 97.07*U.deg);

astroX.Loc2Equ(mjd0, loc, axis, equ);
equIC.ra_dec = (317.1956604*U.deg, 41.74906637*U.deg);
assert(equ.angle(equIC) < 1*U.arcsec);

astroX.Loc2Equ(mjd0, loc, axis, equ, tsys, doPrecess);
equIC.ra_dec = (317.1228164*U.deg, 41.71851115*U.deg);
assert(equ.angle(equIC) < 1*U.arcsec);

# 2b) Check local coord. conversion & precession: 2010-04-27 19:19:19UT
astroX.Loc2Equ(mjd1, loc, axis, equ);
equIC.ra_dec = (44.49649616*U.deg, 41.74906637*U.deg);
assert(equ.angle(equIC) < 1*U.arcsec);

astroX.Loc2Equ(mjd1, loc, axis, equ, tsys, doPrecess);
equIC.ra_dec = (44.32400099*U.deg, 41.70630271*U.deg);
assert(equ.angle(equIC) < 1*U.arcsec);

