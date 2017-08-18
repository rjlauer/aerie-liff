#!/usr/bin/env python

try:
    from hawc import hawcnest, data_structures, grmodel_services
    from hawc.data_structures import *
    from hawc.hawcnest import HAWCUnits as U 
    from HAWCNest import HAWCNest

    import matplotlib.pyplot as plt
    import matplotlib as mpl
    import numpy as np
    import healpy as hp
    import argparse
    import os

    from scipy.integrate import trapz

    TeV = U.TeV
    inv_cm2sTeV = 1./(TeV * U.cm**2 * U.second)
    degree = U.degree
except ImportError as e:
    print(e)
    raise SystemExit

def getTeVFlux(srcName):
    mjd = ModifiedJulianDate(56961*U.day)

    ps = grmodel_services.GetPointSource(srcName)
    logEmin = np.log10(ps.GetMinEnergy()/TeV)
    logEmax = np.log10(ps.GetMaxEnergy()/TeV)
    E = np.logspace(logEmin, logEmax) * TeV
    F = np.array([ps.GetFlux(energy, mjd) for energy in E])

    if ps.GetFlux(1*TeV, mjd) > 2e-11*inv_cm2sTeV:
        print(srcName)

    return E/TeV, F/inv_cm2sTeV

def getFlux(srcName, **kwargs):
    mjd = ModifiedJulianDate(56961*U.day)

    ebl = None
    zcut = None

    for key in kwargs:
        if key == "ebl":
            ebl = grmodel_services.GetEBLAbsorptionService(kwargs[key])
        if key == "zcut":
            zcut = kwargs[key]

    ps = grmodel_services.GetPointSource(srcName)
    z = ps.GetRedshift()
    if zcut and z > 0.:
        if z > zcut:
            return z, None, None

    E = np.logspace(-2, 2, 100) * TeV
    F = np.array([ps.GetFlux(energy, mjd) for energy in E])

    if ebl:
        tr = np.array([ebl.GetAttenuation(energy, z) for energy in E])
        F *= tr

    return z, E/TeV, F/inv_cm2sTeV

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

# Main part of the script
p = argparse.ArgumentParser(description="Plot PS catalog data")
p.add_argument("-m", "--map-title", dest="mapTitle", default=None,
               help="Map plot title")
p.add_argument("-o", "--outtab", dest="outTab", action="store_true",
               default=False,
               help="Output GeV spectra to tab-delimited files")
p.add_argument("-t", "--title", dest="title", default=None,
               help="Spectrum plot title")
args = p.parse_args()

# Get the TeV source catalog
cdir = "/".join([os.environ["HAWC_INSTALL"], "share/hawc/config"])
tevConf = "/".join([cdir, "TeV-src-catalog.xml"])
#gevConf = "/".join([cdir, "1FHL-PS-catalog.xml"])
gevConf = "/".join([cdir, "1FHL-NoTeVCat-PS-catalog.xml"])

# Initialize the framework and a list of point sources
nest = HAWCNest()
tevSources = grmodel_services.BuildPSCatalog(tevConf, nest.hawcnest_, False)
gevSources = grmodel_services.BuildPSCatalog(gevConf, nest.hawcnest_, False)
nest.Service("Gilmore09EBLModel", "gilmoreEBL")
nest.Configure()

# Extract and plot the spectrum of each source
mpl.rc("font", family="serif", size=14)

fig = plt.figure(1, figsize=(12,6))
ax = fig.add_subplot(111)

crab = "TeV J0534+220 : Crab"
scale = 2.

crabFlux = 0.
for i, s in enumerate(tevSources):
    lc = "#000000" if s == crab else "#ff7777" 
    lw = 3 if s == crab else 1
    label = s if s == crab else "TeV Sources"
    if i > 0 and s != crab:
        label = "%s %d" % (label, i)
    E, F = getTeVFlux(s)
    ax.plot(E, E**scale * F, color=lc, lw=lw, label=label)

    ps = grmodel_services.GetPointSource(s)
    mjd = ModifiedJulianDate(56961*U.day)

    if s == crab:
        crabFlux = ps.Integrate(100*U.GeV, 100*U.TeV, mjd)

    # Optionally output flux to a text file 
    if args.outTab:
        E = np.logspace(-2, 2, 100) * TeV
        F = np.array([ps.GetFlux(energy, mjd) for energy in E])
        E /= TeV
        F /= inv_cm2sTeV
        radec = ps.GetLocation()

        if E != None and F != None:
            outfile = s.replace(" : ", "_")
            outfile = outfile.replace(" ", "_")
            outfile = "flux_%s_dec%.1f.txt" % (outfile, radec.dec/degree)
            print(outfile)
            out = open(outfile, "w")
            for e,f in zip(E, F):
                if f > 0:
                    print >>out, "%16g%16g" % (e, f)

zhist = Histogram(np.linspace(0,3,30))
f1TeV = {}

Decs = []
RAs = []
iFlux = []
mjd = ModifiedJulianDate(56961*U.day)

for i, s in enumerate(gevSources):
    lc = "#7777ff"
    z, E, F = getFlux(s, ebl="gilmoreEBL", zcut=1)
    zhist.Fill(z)
    label = "GeV Sources"
    if i > 0:
        label = "%s %d" % (label, i)
    if E != None:
        ax.plot(E, E**scale * F, color=lc, lw=1, label=label)

    ps = grmodel_services.GetPointSource(s)
    radec = ps.GetLocation()
    RAs.append(radec.ra)
    Decs.append(radec.dec)
    iFlux.append(ps.Integrate(100*U.GeV, 100*U.TeV, mjd))

    if iFlux[-1] > 0.025*crabFlux:
        print(s, iFlux[-1]/crabFlux)

    # Optionally output flux to a text file 
    if args.outTab:
        if E != None and F != None:
            if iFlux[-1] > 0.01*crabFlux:
                outfile = s.replace(" : ", "_")
                outfile = outfile.replace(" ", "_")
                outfile = "flux_%s_dec%.1f.txt" % (outfile, radec.dec/degree)
                print(outfile)
                out = open(outfile, "w")
                for e,f in zip(E, F):
                    if f > 0:
                        print >>out, "%16g%16g" % (e, f)

    if F != None:
        f1TeV[F[50]] = s

print("Top 10 fluxes at 1 TeV:")
for i, k in enumerate(sorted(f1TeV.iterkeys(), reverse=True)):
    print("%-4d%s" % (i, f1TeV[k]))
    if i+1 > 10:
        break

title = r"GeV + TeV Sources within 50$^\circ$ of HAWC Zenith"
if args.title:
    title = args.title

ax.set_xscale("log")
ax.set_yscale("log")
ax.set_xlim([1e-2, 1e2])
ax.set_xlabel("energy [TeV]")
ax.xaxis.set_major_formatter(mpl.ticker.FormatStrFormatter("%g"))
ax.set_ylabel(r"E$^2$ flux [TeV cm$^{-2}$ s$^{-1}$]")
ax.set_ylim([1e-14, 1e-10])
ax.set_title(title)
ax.grid(True)

h, l = ax.get_legend_handles_labels()
handles, labels = np.array(h), np.array(l)
cut = np.logical_or(labels=="GeV Sources", labels=="TeV Sources")
cut = np.logical_or(cut, labels==crab)
ax.legend(handles[cut], labels[cut], loc=2)
fig.tight_layout()

# Plot the histogram of redshifts
fig = plt.figure(2)
ax = fig.add_subplot(111)

nZ, Zbins, Zedges, Zwidths = zhist.GetHistogram()
ax.bar(Zedges[:-1], nZ, width=Zwidths, fc="#aaaaff") #log=True, alpha=0.3)
ax.set_xlabel("redshift")
ax.set_ylabel("count")
ax.set_title("1FHL Redshift Distribution")
ax.grid(True)
fig.tight_layout()

# Plot the distribution of points on the sky
title = r"GeV Sources within 50$^\circ$ of HAWC Zenith"
if args.mapTitle:
    title = args.mapTitle

fig = plt.figure(3, figsize=(9,5))
hp.mollview(fig=3, coord="CG", title=title, rot=0)
Decs = [90*degree - d for d in Decs]
fmax = np.max(iFlux)
size = [100 * np.sqrt(1e14*f) for f in iFlux]
hp.projscatter(Decs, RAs, coord="CG", s=size, alpha=0.3)
hp.graticule(coord="G", dpar=15, dmer=30)

plt.show()

