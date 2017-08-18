#!/usr/bin/env python
################################################################################
# A demonstration of spectrum reweighting from a simple power law to something #
# more complex.                                                                #
#                                                                              #
# To use this program AERIE must be installed and /path/to/aerie/lib must be   #
# included in your PYTHONPATH environment variable.  The environment is set up #
# with the hawc-config script.                                                 #
################################################################################

from hawc import hawcnest, data_structures, rng_service, grmodel_services
from hawc.data_structures import *
from hawc.hawcnest import HAWCUnits as U
from HAWCNest import HAWCNest

import argparse
import os
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
parser.add_argument("-a", "--Emin", dest="Emin", type=float, default=10.,
                    help="Minimum energy [GeV].")
parser.add_argument("-b", "--Emax", dest="Emax", type=float, default=200e3,
                    help="Maximum energy [GeV].")
args = parser.parse_args()

# Set up spectrum parameters
Emin = args.Emin*U.GeV
Emax = args.Emax*U.GeV
idx1 = args.idx1
idx2 = args.idx2

# Set up the HAWCNest framework and RNG service
nest = HAWCNest()

nest.Service("StdRNGService", "rng",
    seed=args.seed)

nest.Service("GenericSpectrum", "injection",
    fluxNorm = 1./(U.GeV*U.meter2*U.s*U.sr),
    energyNorm = Emin,
    spIndex = idx2,
    energyMin = Emin,
    energyMax = Emax)

nest.Service("IsotropicCosmicRaySource", "crsource",
    sourceSpectrum = "injection",
    particleType = "Proton")

cconf = "/".join([os.environ["HAWC_CONFIG"], "CREAM2-spectrum.xml"])
crs = grmodel_services.BuildCRCatalog(cconf, nest.hawcnest_, True)

nest.Configure()

rng = rng_service.GetService("rng")

#x0, x1, A, idx = [args.a, args.b, 1., args.idx1]
p1 = PowerLaw(Emin, Emax, 1., 0.5*(Emin+Emax), idx1)
fl = grmodel_services.GetCosmicRaySource("crsource")
cr = grmodel_services.GetCosmicRaySource(crs[ParticleType.Fe56Nucleus])

mjd = ModifiedJulianDate(55555*U.day)

E = []
wt = []
w2 = []
for i in range(args.size[0]):
    u = rng.PowerLaw(p1.spectral_index(Emin), p1.xmin, p1.xmax)
    E.append(u/U.GeV)

    w = fl.GetFluxWeight(u, mjd, p1) * U.s
    wt.append(w)

    w = cr.GetFluxWeight(u, mjd, p1) * U.s
    w2.append(w)

mpl.rc("font", family="serif", size=16)
fig = plt.figure(figsize=(12,5))

# Plot the reweighting of our original spectrum to our IsotropicCosmicRaySource:
# - Make a log-log histogram of the random numbers
# - Calculate bin centers and widths
# - Correct bin counts for the logarithmic binning in energy
ax = fig.add_subplot(121)
ax.set_xscale("log")

edges = 10**np.linspace(np.log10(Emin/U.GeV), np.log10(Emax/U.GeV), 20)
n1, bins = np.histogram(E, bins=edges)
n2, bins = np.histogram(E, bins=edges, weights=wt)

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

ax.set_xlabel(r"$E$ [GeV]")
ax.set_ylabel("count")
ax.set_xlim(Emin/U.GeV, Emax/U.GeV)
ax.set_ylim(nmin, nmax)
ax.grid()

legend = ax.legend([h1, h2],
                  ["spectral index = %.1f" % args.idx1,
                   "reweighted index = %.1f" % args.idx2],
                  prop=FontProperties(size="small"),
                  numpoints=1,
                  loc="best")
legend.get_frame().set_linewidth(0)

# Plot the reweighting of our original spectrum to our CREAMProton model
# - Make a log-log histogram of the random numbers
# - Calculate bin centers and widths
# - Correct bin counts for the logarithmic binning in energy
ax = fig.add_subplot(122)
ax.set_xscale("log")

n2, bins = np.histogram(E, bins=edges, weights=w2)

center = 0.5*(bins[:-1]+bins[1:])
widths = (bins[1:] - bins[:-1])

# Note: dn/dx = 1/(x ln(10)) * dn/dlog10(x)
n2 = n2 * 0.434 / center

h1=ax.bar(bins[:-1], n1, width=widths, color="#ff0000", log=True, alpha=0.3)
h2=ax.bar(bins[:-1], n2, width=widths, color="#0000ff", log=True, alpha=0.3)

nmin = min(n1.min(), n2.min())
nmin = 10**np.floor(np.log10(nmin))

nmax = max(n1.max(), n2.max())
nmax = 10**np.ceil(np.log10(nmax))

ax.set_xlabel(r"$E$ [GeV]")
ax.set_ylabel("count")
ax.set_xlim(Emin/U.GeV, Emax/U.GeV)
ax.set_ylim(nmin, nmax)
ax.grid()

legend = ax.legend([h1, h2],
                  ["spectral index = %.1f" % args.idx1,
                   "reweighted to %s" % crs[ParticleType.Fe56Nucleus]],
                  prop=FontProperties(size="small"),
                  numpoints=1,
                  loc="best")
legend.get_frame().set_linewidth(0)

fig.tight_layout()

plt.show()

