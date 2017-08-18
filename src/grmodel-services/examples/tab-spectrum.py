#!/usr/bin/env python

try:
    from hawc import hawcnest, data_structures, rng_service, grmodel_services
    from hawc.data_structures import *
    from hawc.hawcnest import HAWCUnits as U
    from HAWCNest import HAWCNest

    import matplotlib.pyplot as plt
    import matplotlib as mpl
    import numpy as np
    import os
except ImportError,e:
    print e
    raise SystemExit

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

def getFlux(srcname):
    """Get energy and flux data from grmodel_services"""
    tf = grmodel_services.GetCosmicRayService(srcname)
    TeV = U.TeV
    fluxU = 1./(U.TeV*U.cm2*U.s)
    mjd = ModifiedJulianDate(55555*U.day)
    pt = ParticleType.Gamma

    logEmin = np.log10(tf.GetMinEnergy(pt)/TeV)
    logEmax = np.log10(tf.GetMaxEnergy(pt)/TeV)
    E = np.logspace(logEmin, logEmax)
    F = np.array([tf.GetFlux(e*TeV, mjd, pt)/fluxU for e in E])

    return E, F

# Get a flux table and plot fluxes
cdir = "/".join([os.environ["HAWC_INSTALL"], "share/hawc/config"])

# Initialize the framework and a flux table service
nest = HAWCNest()

nest.Service("StdRNGService", "rng",
    seed=12345)

nest.Service("TabulatedSpectrum", "dmFlux",
    infilename="/".join([cdir, "DarkMatter_50TeVToTauDecay.txt"]))

nest.Service("TabulatedSpectrum", "crabFlux",
    infilename="/".join([cdir, "CrabICSModel.txt"]))

nest.Configure()

# Plot the spectrum
mpl.rc("font", family="serif")

fig = plt.figure(1, figsize=(12,6))
ax = fig.add_subplot(111)

E, F = getFlux("crabFlux")
ax.plot(E, E**2.5 * F, color="gray", lw=2, label="Crab Nebula")

E, F = getFlux("dmFlux")
ax.plot(E, E**2.5 * F, color="red", lw=2, label="DM Decay")

ax.set_xscale("log")
ax.set_yscale("log")
ax.set_xlabel("energy [TeV]")
ax.set_ylabel(r"E$^{2.5}$ flux [TeV$^{1.5}$ cm$^{-2}$ s$^{-1}$]")
ax.set_title(r"50 TeV Dark Matter $\rightarrow\tau\bar{\tau}$ " \
             r"($\langle\sigma v\rangle = 5\times10^{-22}$ " \
             r"cm$^3$ s$^{-1}$, $J=7.7\times10^{18}$)")
ax.grid(True)

h, l = ax.get_legend_handles_labels()
handles, labels = np.array(h), np.array(l)
ax.legend(handles, labels, loc="best")

fig.subplots_adjust(left=0.08, right=0.92)

# Make a histogram of the dark matter spectrum
flux = grmodel_services.GetCosmicRayService("dmFlux")
rng = rng_service.GetService("rng")
GeV = U.GeV
TeV = U.TeV

hE = Histogram(np.logspace(-2, 2, 30))
for i in range(0, 10000):
    e = flux.GetRandomEnergy(rng, 10*GeV, 100*TeV, ParticleType.Gamma)
    hE.Fill(e/TeV)

nE, Ebins, Eedges, Ewidths = hE.GetHistogram()
nE = nE * 0.434 / Ebins

fig = plt.figure(2, figsize=(12,6))
ax = fig.add_subplot(121)
ax.plot(E, F, color="red", lw=2, label="DM Decay")
ax.set_xscale("log")
ax.set_yscale("log")
ax.set_xlabel("energy [TeV]")
ax.set_ylabel(r"flux [TeV$^{-1}$ cm$^{-2}$ s$^{-1}$]")
ax.grid(True)

ax = fig.add_subplot(122)
ax.bar(Eedges[:-1], nE, width=Ewidths, log=True, alpha=0.3)
ax.set_xlabel("energy [TeV]")
ax.set_xscale("log")
ax.set_ylabel("dN/dE")
ax.set_yscale("log")
ax.grid(True)

fig.subplots_adjust(left=0.08, right=0.92)

plt.show()

