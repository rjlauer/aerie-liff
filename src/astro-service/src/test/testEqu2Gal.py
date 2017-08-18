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

# Define locations to be used in astronomical transformations
loc = LatLonAlt(float(DegMinSec(18*U.deg, 59*U.arcmin, 41.63*U.arcsec)),
               -float(DegMinSec(97*U.deg, 18*U.arcmin, 27.39*U.arcsec)),
                4096*U.meter)

equ = EquPoint()
gal = GalPoint()
galIC = GalPoint()

# Point 1
equ.ra_dec = (332.7561567*U.deg, 23.32024871*U.deg)
astroX.Equ2Gal(equ, gal)
galIC.b_l = (-26.3987420*U.deg, 81.60074826*U.deg)
assert(gal.angle(galIC) < 1*U.arcsec)

# Point 2 
equ.ra_dec = (317.1228164*U.deg, 41.71851115*U.deg)
astroX.Equ2Gal(equ, gal)
galIC.b_l = (-4.046108614*U.deg, 84.73038891*U.deg)
assert(gal.angle(galIC) < 1*U.arcsec)

# Point 3
equ.ra_dec = (287.2718991*U.deg, 8.939760454*U.deg)
astroX.Equ2Gal(equ, gal)
galIC.b_l = (0.1844045145*U.deg, 42.8916759*U.deg)
assert(gal.angle(galIC) < 1*U.arcsec)

# Point 4
equ.ra_dec = (36.66403088*U.deg, -23.35716293*U.deg)
astroX.Equ2Gal(equ, gal)
galIC.b_l = (-68.17535375*U.deg, -151.721019*U.deg)
assert(gal.angle(galIC) < 1*U.arcsec)

# Point 5
equ.ra_dec = (64.02913095*U.deg, 20.17229567*U.deg)
astroX.Equ2Gal(equ, gal)
galIC.b_l = (-21.50384914*U.deg, 174.7384819*U.deg)
assert(gal.angle(galIC) < 1*U.arcsec)

# Point 6
equ.ra_dec = (149.8390446*U.deg, 45.19139341*U.deg)
astroX.Equ2Gal(equ, gal)
galIC.b_l = (51.45002656*U.deg, 173.4234998*U.deg)
assert(gal.angle(galIC) < 1*U.arcsec)

# Point 7
equ.ra_dec = (177.6681505*U.deg, -22.54625482*U.deg)
astroX.Equ2Gal(equ, gal)
galIC.b_l = (38.19985422*U.deg, -75.00459182*U.deg)
assert(gal.angle(galIC) < 1*U.arcsec)
