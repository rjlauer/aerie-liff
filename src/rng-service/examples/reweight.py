#!/usr/bin/env python
################################################################################
# A demonstration showing how to use the AERIE rng-service inside python.      #
#                                                                              #
# To use this program AERIE must be installed and /path/to/aerie/lib must be   #
# included in your PYTHONPATH environment variable.  The environment is set up #
# with the hawc-config script.                                                 #
################################################################################

from hawc import hawcnest, data_structures
from hawc.data_structures import *
from hawc import rng_service as rng_service
from HAWCNest import HAWCNest

import argparse
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib.font_manager import FontProperties

# Set up command line options
parser = argparse.ArgumentParser(description="Reweight a power law")
parser.add_argument("size", type=int, nargs=1,
                    help="Size N of random dataset")
parser.add_argument("-s", "--seed", dest="seed", type=int, default=12345,
                    help="Seed for random number generator")
parser.add_argument("-i", "--index1", dest="idx1", type=float, default=-2.,
                    help="Power law spectral index.")
parser.add_argument("-I", "--index2", dest="idx2", type=float, default=-4.,
                    help="Reweighted power law spectral index.")
parser.add_argument("-a", "--xmin", dest="a", type=float, default=1.,
                    help="Minimum power law range (a > 0).")
parser.add_argument("-b", "--xmax", dest="b", type=float, default=1e2,
                    help="Maximum power law range (0 < a <= b).")
args = parser.parse_args()

# Set up the HAWCNest framework and RNG service
nest = HAWCNest()

nest.Service("StdRNGService", "rng",
    seed=args.seed)

nest.Configure()

rng = rng_service.GetService("rng")

x0, x1, A, idx = [args.a, args.b, 1., args.idx1]
p1 = PowerLaw(x0, x1, A, x0, idx)
p2 = PowerLaw(x0, x1, A, x0, args.idx2)

x = []
wt = []
for i in range(args.size[0]):
    u = rng.PowerLaw(p1.spectral_index(x0), p1.xmin, p1.xmax)
    x.append(u)

    w = p2.reweight(p1, u)
    wt.append(w)

mpl.rc("font", family="serif", size=14)
fig = plt.figure()
ax = fig.add_subplot(111)
ax.set_xscale("log")

# Make a log-log histogram of the random numbers
edges = 10**np.linspace(np.log10(x0), np.log10(x1), 20)
n1, bins = np.histogram(x, bins=edges)
n2, bins = np.histogram(x, bins=edges, weights=wt)

# Calculate bin centers and widths
center = 0.5*(bins[:-1]+bins[1:])
widths = (bins[1:] - bins[:-1])

# Note: dn/dx = 1/(x ln(10)) * dn/dlog10(x)
n1 = n1 * 0.434 / center
n2 = n2 * 0.434 / center

h1=ax.bar(bins[:-1], n1, width=widths, color="#ff0000", log=True, alpha=0.3)
h2=ax.bar(bins[:-1], n2, width=widths, color="#0000ff", log=True, alpha=0.3)

nmin = min(n1.min(), n2.min())
nmin = 10**np.floor(np.log10(nmin))

nmax = max(n1.max(), n2.max())
nmax = 10**np.ceil(np.log10(nmax))

ax.set_xlabel("x")
ax.set_ylabel("count")
ax.set_xlim([x0, x1])
ax.set_ylim([nmin, nmax])
ax.grid()

legend = ax.legend([h1, h2],
                  ["spectral index = %.1f" % p1.spectral_index(x0),
                   "reweighted index = %.1f" % p2.spectral_index(x0)],
                  prop=FontProperties(size="small"),
                  numpoints=1,
                  loc="best")
legend.get_frame().set_linewidth(0)

plt.show()

