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

axis = Vector()
axisIC = Vector()

# Make some coordinate transformations to/from the HAWC coordinate system,
# correcting J2000 coordinates to the current epoch if needed

# Position of Sirius: 2007-10-04 03:03:03UT
equ = EquPoint(float(HrMinSec(6*U.hour, 45*U.minute, 8.90*U.second)),
              -float(DegMinSec(16*U.deg, 42*U.arcmin, 58*U.arcsec)))

astroX.Equ2Loc(mjd0, loc, equ, axis, tsys, doPrecess)
axisIC.r_theta_phi = (1., 142.3997*U.deg, 357.033*U.deg)
assert(axis.angle(axisIC) < 10*U.arcsec)

# Position of Sirius: 2010-04-27 19:19:19UT
astroX.Equ2Loc(mjd1, loc, equ, axis, tsys, doPrecess)
axisIC.r_theta_phi = (1., 63.35031153*U.deg, 329.1150093*U.deg)
assert(axis.angle(axisIC) < 10*U.arcsec)

# Position of Canopus: 2007-10-04 03:03:03UT
equ.ra_dec = (float(HrMinSec(6*U.hour, 23*U.minute, 57.10*U.second)),
             -float(DegMinSec(52*U.deg, 41*U.arcmin, 45*U.arcsec)))

astroX.Equ2Loc(mjd0, loc, equ, axis, tsys, doPrecess)
axisIC.r_theta_phi = (1., 131.6908151*U.deg, 304.9177740*U.deg)
assert(axis.angle(axisIC) < 10*U.arcsec)

# Position of Canopus: 2010-04-27 19:19:19UT
astroX.Equ2Loc(mjd1, loc, equ, axis, tsys, doPrecess)
axisIC.r_theta_phi = (1., 82.78900343*U.deg, 296.9302978*U.deg)
assert(axis.angle(axisIC) <  10*U.arcsec)

# Position of Arcturus: 2007-10-04 03:03:03UT
equ.ra_dec = (float(HrMinSec(14*U.hour, 15*U.minute, 39.70*U.second)),
              float(DegMinSec(19*U.deg, 10*U.arcmin, 57*U.arcsec)))

astroX.Equ2Loc(mjd0, loc, equ, axis, tsys, doPrecess)
axisIC.r_theta_phi = (1., 98.77779040*U.deg, 156.1649493*U.deg)
assert(axis.angle(axisIC) <  10*U.arcsec)

# Position of Arcturus: 2010-04-27 19:19:19UT
astroX.Equ2Loc(mjd1, loc, equ, axis, tsys, doPrecess)
axisIC.r_theta_phi = (1., 139.4235886*U.deg, 69.20611779*U.deg)
assert(axis.angle(axisIC) <  10*U.arcsec)

# Position of Antares: 2007-10-04 03:03:03UT
equ.ra_dec = (float(HrMinSec(16*U.hour, 29*U.minute, 24.40*U.second)),
             -float(DegMinSec(26*U.deg, 25*U.arcmin, 55*U.arcsec)))

astroX.Equ2Loc(mjd0, loc, equ, axis, tsys, doPrecess)
axisIC.r_theta_phi = (1., 84.45277878*U.deg, 210.4464622*U.deg)
assert(axis.angle(axisIC) <  10*U.arcsec)

# Position of Antares: 2010-04-27 19:19:19UT
astroX.Equ2Loc(mjd1, loc, equ, axis, tsys, doPrecess)
axisIC.r_theta_phi = (1., 160.7048303*U.deg, 206.272360*U.deg)
assert(axis.angle(axisIC) <  10*U.arcsec)

