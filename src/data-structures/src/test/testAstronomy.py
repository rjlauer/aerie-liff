#!/usr/bin/env python
"""Test python access to the astronomy classes in the AERIE python bindings.

.. codeauthor:: Segev BenZvi
"""

__version__ = "$Id"

from hawc import hawcnest, data_structures
from hawc.data_structures import *
from hawc.hawcnest import HAWCUnits as U

import numpy as np

# Test astro coords initialization and access
ra = HrMinSec(5*U.hour, 34*U.minute, 31.94*U.second)
assert(ra.hour == 5*U.hour)
assert(ra.minute == 34*U.minute)
assert(ra.second == 31.94*U.second)
assert(ra.hr_min_sec == (5*U.hour, 34*U.minute, 31.94*U.second))

dec = DegMinSec(22*U.degree, 0, 52.2*U.arcsecond)
assert(dec.degree == 22*U.degree)
assert(dec.arcmin == 0.)
assert(dec.arcsec == 52.2*U.arcsecond)
assert(dec.deg_min_sec == (22*U.degree, 0, 52.2*U.arcsecond))

# Test float conversion
assert(abs(float(ra) - 83.63308*U.degree) < 1e-6)
assert(abs(float(dec) - 22.0145*U.degree) < 1e-6)

# Test deep copying
ra2 = HrMinSec(ra)
dec2 = DegMinSec(dec)

# Test equatorial coordinates
eq = EquPoint(float(ra), float(dec))
assert(abs(eq.ra - 83.63308*U.degree) < 1e-6)
assert(abs(eq.dec - 22.0145*U.degree) < 1e-6)
eq2 = EquPoint(eq)

# Test galactic coordinates
b = -5.78436*U.degree
l = -175.443*U.degree
gal = GalPoint(b, l)
assert(abs(gal.b - b) < 1e-10)
assert(abs(gal.l - l) < 1e-10)
gal2 = GalPoint(gal)

