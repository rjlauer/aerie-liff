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

equ = EquPoint()
equIC = EquPoint()

# Time 1: check position to <30" (0.01 deg)
mjd = ModifiedJulianDate(UTCDateTime(2008,11,8, 23,34,54))
astroX.GetLunarTopocentricPosition(mjd, loc, equ)
equIC.ra_dec = (350.2299855*U.deg, -1.581314809*U.deg)
assert(equ.angle(equIC) < 30*U.arcsec)

# Time 2: check position to <30" (0.01 deg)
mjd = ModifiedJulianDate(UTCDateTime(2008,10,22, 10,30,45))
astroX.GetLunarTopocentricPosition(mjd, loc, equ)
equIC.ra_dec = (134.6256410*U.deg, 17.62666958*U.deg)
assert(equ.angle(equIC) < 30*U.arcsec)

# Time 3: check position to <1' (0.02 deg)
mjd = ModifiedJulianDate(UTCDateTime(2002,3,2, 18,20,35))
astroX.GetLunarTopocentricPosition(mjd, loc, equ)
equIC.ra_dec = (208.3084724*U.deg, -7.795243285*U.deg)
assert(equ.angle(equIC) < 1.1*U.arcmin)

# Time 4: check position to <1' (0.02 deg)
mjd = ModifiedJulianDate(UTCDateTime(2000,11,15, 7,3,22))
astroX.GetLunarTopocentricPosition(mjd, loc, equ)
equIC.ra_dec = (100.2067691*U.deg, 22.42668680*U.deg)
assert(equ.angle(equIC) < 1.1*U.arcmin)
