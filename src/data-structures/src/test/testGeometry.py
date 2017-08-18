#!/usr/bin/env python
"""Test python access to the geometry classes in the AERIE python bindings.

.. codeauthor:: Segev BenZvi
"""

__version__ = "$Id"

from hawc import hawcnest, data_structures
from hawc.data_structures import *
from hawc.hawcnest import HAWCUnits as U

import numpy as np

# Basic vector algebra
v1 = Vector(3., 4., 0.)
assert(v1.x == 3. and v1.y == 4. and v1.z == 0.)
assert(v1.r == v1.rho)
assert(v1.rho == 5.)
assert(v1.theta == 90*U.degree)
assert(abs(v1.phi - 53.1301*U.degree) < 0.001*U.degree)

# Unit vector
v2 = v1.vec_unit
assert(v2.r == 1.)

# Orthogonal vector
v3 = v1.vec_orthogonal
assert(abs(v1.angle(v3) - 90*U.degree) < 1e-10*U.degree)

# Cross product
v4 = v1.cross(Vector(4., 3., 0.))
assert(v4.x == 0. and v4.y == 0. and v4.z == -7.)

# Overloading of * for dot product
assert(v1 * v3 < 1e-15)
assert(v1.dot(v3) < 1e-15)
assert(v1 * v1 == 25.)

# Equivalence
assert(v1 == v1)
assert(v1 != v3)

# Scaling
assert((v1*2.).r == 10.)

# Addition
v5 = v1 + v4
assert(v5.x == 3. and v5.y == 4. and v5.z == -7.)

# Subtraction
v6 = v1 - v5
assert(v6.x == 0. and v6.y == 0. and v6.z == 7.)

# Cylindrical and spherical coordinates
v6.rho_phi_z = (3., 90*U.degree, 4.)
assert(abs(v6.x) < 1e-15)
assert(abs(v6.y - 3.) < 1e-15)
assert(abs(v6.z - 4.) < 1e-15)

v6.r_theta_phi = (5., 90*U.degree, 0.)
assert(v6.x == 5.)
assert(abs(v6.y) < 1e-15)
assert(abs(v6.z) < 1e-15)

# Division
v6.xyz = (3., 4., 0.)
v6 /= 4.
assert(v6.x == 0.75 and v6.y == 1. and v6.z == 0.)

# Multiplication
v6 *= 4.
assert(v6.x == 3. and v6.y == 4. and v6.z == 0.)

# Scaling transformation
v = Vector(1,1,1)
s = Scale(3,4,5)
assert(s * v == Vector(3,4,5))

# Proper rotation about z
r1 = Rotate(90*U.degree, Vector(0,0,1))
v1 = r1 * v
assert(abs(v1.x + 1.) < 1e-15)
assert(abs(v1.y - 1.) < 1e-15)
assert(abs(v1.z - 1.) < 1e-15)

# Reflection through the y=0 (x-z) plane (improper rotation)
r2 = R3Transform(1.,  0.,  0.,  0,
                 0., -1.,  0.,  0,
                 0.,  0.,  1.,  0)
v2 = r2 * v
assert(abs(v2.x - 1.) < 1e-15)
assert(abs(v2.y + 1.) < 1e-15)
assert(abs(v2.z - 1.) < 1e-15)

# Proper rotation about x
r3 = Rotate(90*U.degree, Vector(1,0,0))
v3 = r3 * v
assert(abs(v3.x - 1.) < 1e-15)
assert(abs(v3.y + 1.) < 1e-15)
assert(abs(v3.z - 1.) < 1e-15)

# Proper rotation about y
r4 = Rotate(90*U.degree, Vector(0,1,0))
v4 = r4 * v
assert(abs(v4.x - 1.) < 1e-15)
assert(abs(v4.y - 1.) < 1e-15)
assert(abs(v4.z + 1.) < 1e-15)

# Translate a Point by 1 along each direction (Vectors do not translate)
p = Point(1,1,1)
tr = Translate(1,1,-1)
p2 = tr * p
assert(p2.x == 2. and p2.y == 2. and p2.z == 0.)

# Test transformations of AxialVector
# Transforms like a normal Vector under proper rotation, but flips sign under
# improper rotation
v = AxialVector(1,1,1)

v1 = r1 * v
assert(abs(v1.x + 1.) < 1e-15)
assert(abs(v1.y - 1.) < 1e-15)
assert(abs(v1.z - 1.) < 1e-15)

v2 = r2 * v
assert(abs(v2.x + 1.) < 1e-15)
assert(abs(v2.y - 1.) < 1e-15)
assert(abs(v2.z + 1.) < 1e-15)

# Algebra on the unit sphere
s1 = S2Point(90*U.degree, 90*U.degree)
assert(s1.theta == 90*U.degree and s1.phi == 90*U.degree)

s2 = S2Point()
s2.theta_phi = (45*U.degree, 0.)
assert(abs(s2.theta - 45*U.degree) < 1e-15 and s2.phi == 0.)

s3 = S2Point(Point(0,0,1))
assert(s3.theta == 0. and s3.phi == 0.)

s4 = s3

assert(abs(s1.angle(s2) - 90*U.degree) < 1e-15)
assert(abs(s2.angle(s3) - 45*U.degree) < 1e-15)
assert(abs(s1.angle(s3) - 90*U.degree) < 1e-15)
assert(s3.angle(s4) == 0.)

p1 = s1.point
assert(abs(p1.x) < 1e-15)
assert(abs(p1.y - 1.) < 1e-15)
assert(abs(p1.z) < 1e-15)

p2 = s2.point
assert(abs(p2.x - 0.5*np.sqrt(2.)) < 1e-15)
assert(abs(p2.y) < 1e-15)
assert(abs(p2.z - 0.5*np.sqrt(2.)) < 1e-15)

p3 = s3.point
assert(abs(p3.x) < 1e-15)
assert(abs(p3.y) < 1e-15)
assert(abs(p3.z - 1.) < 1e-15)

reflect = R3Transform(1.,  0.,  0.,  0,
                      0.,  1.,  0.,  0,
                      0.,  0., -1.,  0)
s3 = reflect * s3
assert(abs(s3.theta - 180*U.degree) < 1e-6*U.degree)
assert(abs(s3.phi) < 1e-6*U.degree)

rotate = Rotate(45*U.degree, Vector(0,0,1))
s2 = rotate * s2
assert(abs(s2.theta - 45*U.degree) < 1e-6*U.degree)
assert(abs(s2.phi - 45*U.degree) < 1e-6*U.degree)

