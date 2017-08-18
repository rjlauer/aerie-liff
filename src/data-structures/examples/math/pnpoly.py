#!/usr/bin/env python
"""
Test python bindings of the PnPoly class.

.. codeauthor:: Zig Hampel
"""

from hawc import hawcnest, data_structures
from hawc.data_structures import *

import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
import matplotlib.patches as patches
import pylab


side_len = 5.

# Polygon vertices
x = np.array((side_len,0,0,side_len), dtype=float)
y = np.array((side_len,side_len,0,0), dtype=float)
xy = np.zeros((len(x),2))

for i in xrange(0,len(x)):
    xy[i,0] = x[i]
    xy[i,1] = y[i]

# New PnPoly
pnpoly = PnPoly(x,y)

# Test points, a circle
t = np.linspace(0,2*np.pi,100)
testx = side_len*np.cos(t)
testy = side_len*np.sin(t)

pn_test = []
for i in xrange(0,len(t)):
    eval = pnpoly.evaluate(testx[i],testy[i])
    pn_test.append(eval)

pn_test = np.asarray(pn_test)

# Identify in/out points for plotting
inx = testx[(pn_test==1)]
iny = testy[(pn_test==1)]
outx = testx[(pn_test==0)]
outy = testy[(pn_test==0)]

# Plot PnPoly inclusion test results
mpl.rc("font", family="serif")

fig = plt.figure(1, figsize=(9,9))
ax = fig.add_subplot(111)
ax.set_title("PnPoly Inclusion Example")

# Inclusion Polygon
pylab.scatter([p[0] for p in xy],[p[1] for p in xy], s=0.01, c='k')
pylab.gca().add_patch(patches.Polygon(xy,closed=True,fill=True, alpha=0.25, color='k', label='Inclusion Polygon'))

# Test Points
ax.plot(inx, iny, 'r.', label='In points')
ax.plot(outx, outy, 'b.', label='Out points')

ax.set_aspect('equal', 'datalim')
ax.grid()
legend = ax.legend(loc='upper left')
plt.show()
