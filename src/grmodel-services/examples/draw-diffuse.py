#!/usr/bin/env python
################################################################################
# A demonstration of how to call the AERIE grmodel-services inside of python.  #
#                                                                              #
# To use this program AERIE must be installed and /path/to/aerie/lib must be   #
# included in your PYTHONPATH environment variable.  The environment is set up #
# with the hawc-config script:                                                 #
#                                                                              #
#   $> eval `/path/to/aerie/bin/hawc-config --env-sh`                          #
################################################################################

try:
    from hawc import hawcnest, data_structures
    from hawc import astro_service, rng_service, grmodel_services
    from hawc.hawcnest import HAWCUnits as U
    from hawc.data_structures import *
    from HAWCNest import HAWCNest

    import matplotlib as mpl
    import matplotlib.pyplot as plt
    import numpy as np
    import healpy as hp
    import argparse
    import os
except ImportError,e:
    print e
    raise SystemExit

degree = U.degree
MeV = U.MeV
GeV = U.GeV
TeV = U.TeV
cm2 = U.cm2
m2  = U.meter2
sec = U.second
sr  = U.steradian

fluxTeV = 1. / (TeV*cm2*sec*sr)
fluxGeV = 1. / (GeV*m2*sec*sr)

class Histogram:
    """Histogram class that can be filled "online" while looping through data,
    as a replacement for numpy.histogram.
    """
    def __init__(self, binEdges):
        self.nBins = len(binEdges)-1
        self.edges = binEdges
        self.count = np.zeros(self.nBins, dtype=np.float)
        self.bins = 0.5*(self.edges[1:] + self.edges[:-1])
        self.widths = self.edges[1:] - self.edges[:-1]
        self.underflow = 0.
        self.overflow = 0.

    def Fill(self, x, weight=1.):
        if x > self.edges[-1]:
            self.overflow += weight
        elif x < self.edges[0]:
            self.underflow += weight
        else:
            i = np.searchsorted(self.edges, x)
            if i != self.nBins and self.edges[i] == x:
                self.count[i] += weight
            else:
                self.count[i-1] += weight

    def GetHistogram(self):
        return (self.count, self.bins, self.edges, self.widths)

    def GetUnderflow(self):
        return self.underflow

    def GetOverflow(self):
        return self.overflow

def createColormap(cmap, n=256):
    cmap0 = cmap
    newcm = mpl.colors.LinearSegmentedColormap('newcm', cmap0._segmentdata, n)
    newcm.set_over(newcm(1.0))
    newcm.set_under('w')
    newcm.set_bad('gray')
    return newcm
 
def setupColorbar(fig, title=None, ticks=None):
    """Create the color bar for a HEALPix figure."""

    for ax in fig.get_axes():
        if type(ax) is hp.projaxes.HpxMollweideAxes:
            cb = fig.colorbar(ax.get_images()[0], ax=ax,
                              orientation="horizontal",
                              shrink=0.8, aspect=35,
                              pad=0.05, fraction=0.1,
                              ticks=ticks,
                              format=mpl.ticker.FormatStrFormatter("%g"))
            for label in cb.ax.get_xticklabels():
                label.set_fontsize("large")
            cb.set_label(title, size="large")
            ax.annotate("0$^\circ$", xy=(1.65, 0.625), size="large")
            ax.annotate("360$^\circ$", xy=(-1.9, 0.625), size="large")

# Load up a diffuse FITS file (GALPROP) and plot it
p = argparse.ArgumentParser(description="Draw diffuse maps")
p.add_argument("fitsFile", nargs=1,
               help="GALPROP diffuse map FITS file")
p.add_argument("-e", "--energy", dest="energy", type=float, default=10.,
               help="Map energy [TeV]")
p.add_argument("-f", "--fmin", dest="fmin", type=float, default=None,
               help="Minimum scale for plotting")
p.add_argument("-F", "--fmax", dest="fmax", type=float, default=None,
               help="Maximum scale for plotting")
p.add_argument("-l", "--logz", dest="logz", action="store_true",
               default=False, help="Plot log flux")
p.add_argument("-m", "--mask", dest="mask", action="store_true",
               default=False, help="Mask out pixels invisible to HAWC")
p.add_argument("-n", "--ncolors", dest="ncolors", type=int, default=256,
               help="Number of colors to use in the color map")
p.add_argument("-N", "--nside", dest="nside", type=int, default=128,
               help="HEALPix map nside parameter (power of 2)")
p.add_argument("-t", "--ticks", dest="ticks", nargs="+", type=float,
               help="Ticks to use in plot color bar")
p.add_argument("-T", "--title", dest="title", default=None,
               help="Plot title")
args = p.parse_args()

# Initialize HAWCNest to create a nice framework
nest = HAWCNest()

nest.Service("StdAstroService", "astro")

cdir = "/".join([os.environ["HAWC_INSTALL"], "share/hawc/config"])
nest.Service("TabulatedSpectrum", "crabFlux",
    infilename="/".join([cdir, "CrabICSModel.txt"]))

nest.Service("StdRNGService", "rng",
    seed=12345)

nest.Configure()
nest.Finish()

# Open the GALPROP map and plot the diffuse flux at some energy
gm = grmodel_services.GALPROPMapTable(args.fitsFile[0])
cf = grmodel_services.GetCosmicRayService("crabFlux")

E = args.energy*TeV
nside = args.nside
npix = hp.nside2npix(nside)
dmap = np.zeros(npix, dtype=float)

for i in range(0, npix):
    z, l = hp.pix2ang(nside, i)
    b = 90*degree - z
    F = gm.GetFlux(E, GalPoint(b,l)) / fluxTeV
    if args.logz:
        logF = np.log10(F)
        dmap[i] = logF
    else:
        dmap[i] = F

    if args.mask:
        eq = EquPoint(0,0)
        astro = astro_service.GetService("astro")
        astro.Gal2Equ(GalPoint(b,l), eq)
        if eq.dec > 69*degree or eq.dec < -31*degree:
            dmap[i] = hp.UNSEEN

# Set map scale min/max
fmin = args.fmin if args.fmin else min(dmap[dmap != hp.UNSEEN])
fmax = args.fmax if args.fmax else max(dmap[dmap != hp.UNSEEN])
print fmin, fmax

unitstr = r"log(flux $\times$ TeV cm$^2$ s sr)" if args.logz \
          else r"flux [1/(TeV cm$^2$ s sr)]"

mpl.rc("font", family="serif", size=14)

# Set up title and color bar ticks
title = args.title if args.title else "Diffuse Flux: %g TeV" % args.energy

# Draw map
#fig = plt.figure(1, figsize=(9.5,6), dpi=100)
fig = plt.figure(1, figsize=(11.75, 5.5), dpi=100)
hp.mollview(dmap, fig=1, coord="GC", rot=180,
            title=title,
            unit=unitstr,
            cbar=False,
            notext=True,
            cmap=createColormap(mpl.cm.jet, args.ncolors),
            min=fmin, max=fmax)
hp.graticule(coord="G")

ax = fig.gca()
ax.set_ylim([-0.5,1])

setupColorbar(fig, unitstr, args.ticks)

# Plot spectrum at a point on the Galactic Plane, plus Crab spectrum
fig = plt.figure(2, figsize=(12,6))

ax = fig.add_subplot(121)
b = 0*degree
l = 55*degree
gal = GalPoint(b, l)
astro = astro_service.GetService("astro")

equ = EquPoint(0, 0)
astro.Gal2Equ(gal, equ)
print equ
#    astro.Equ2Gal(equ, gal)
#    print gal

solidAngle = 2*np.pi * (1. - np.cos(0.5*degree))

E = np.logspace(-1,2)
F = np.array([gm.GetFlux(e*TeV, gal)*solidAngle/fluxTeV for e in E])
ax.plot(E, F, lw=2, color="gray",
        label=r"Diffuse: b=%g$^\circ$, l=%g$^\circ$" % (b/degree,l/degree))

mjd = ModifiedJulianDate(55555*U.day)
F = np.array([cf.GetFlux(e*TeV, mjd, ParticleType.Gamma)/fluxTeV for e in E])
ax.plot(E, F, lw=2, color="red",
        label="Crab Nebula")

ax.set_xscale("log")
ax.set_xlabel("energy [TeV]")
ax.set_yscale("log")
ax.set_ylabel(r"flux [TeV$^{-1}$ cm$^{-2}$ s$^{-1}$]")
ax.set_ylim([1e-22, 1e-8])
ax.grid(True)

handles, labels = ax.get_legend_handles_labels()
ax.legend(handles, labels, loc="best")

ax = fig.add_subplot(122)
rng = rng_service.GetService("rng")
hE = Histogram(np.logspace(-1, 2, 30))
for i in range(0, 20000):
    E = gm.GetRandomEnergy(rng, 100*GeV, 100*TeV, GalPoint(0,0))
    hE.Fill(E/TeV)
nE, Ebins, Eedges, Ewidths = hE.GetHistogram()
nE = nE * 0.434 / Ebins
ax.bar(Eedges[:-1], nE, width=Ewidths, log=True, alpha=0.3)
ax.set_xlabel("energy [TeV]")
ax.set_xscale("log")
ax.set_ylabel("dN/dE")
ax.set_yscale("log")
ax.grid(True)

fig.subplots_adjust(left=0.08, right=0.92)

plt.show()

