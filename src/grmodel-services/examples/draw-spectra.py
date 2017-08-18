#!/usr/bin/env python
"""Test access to the CRCatalog interface, which produces CosmicRaySources
instances from an XML file.  To execute this script you must have the AERIE
environment loaded in your shell.

.. codeauthor: Segev BenZvi
"""

__version__ = "$Id"

from hawc import hawcnest, data_structures, grmodel_services
from hawc.data_structures import *
from hawc.hawcnest import HAWCUnits as U
from HAWCNest import HAWCNest

import argparse
import numpy as np
import os
import matplotlib as mpl
import matplotlib.pyplot as plt

mjd = ModifiedJulianDate(55555*U.day)

def getFlux(srcName):
    """Given a CosmicRaySource name, look up the source and get its flux."""
    c = grmodel_services.GetCosmicRaySource(srcName)
    Emin, Emax = c.GetMinEnergy(), c.GetMaxEnergy()
    E = np.logspace(np.log10(Emin), np.log10(Emax))
    F = np.array([c.GetFlux(e, mjd) for e in E])
    return E/U.TeV, F*(U.TeV*U.meter**2*U.second*U.sr)

# Parse the command line
p = argparse.ArgumentParser(description="Cosmic ray flux plotter")
p.add_argument("-s", "--scale", dest="scale", type=float, default=0.,
               help="Energy scaling applied to flux.")
args = p.parse_args()

# Initialize cosmic-ray and eletron spectra
cfdir = os.environ["HAWC_CONFIG"]
cconf = "/".join([cfdir, "CREAM2-spectrum.xml"])
econf1 = "/".join([cfdir, "HESS-electron-spectrum.xml"])
econf2 = "/".join([cfdir, "Combined-electron-spectrum.xml"])

nest = HAWCNest()

crs = grmodel_services.BuildCRCatalog(cconf, nest.hawcnest_, True)
els1 = grmodel_services.BuildCRCatalog(econf1, nest.hawcnest_, True)
els2 = grmodel_services.BuildCRCatalog(econf2, nest.hawcnest_, True)

nest.Configure()

mpl.rc("font", family="serif", size=14)

# Plot cosmic-ray fluxes
fig = plt.figure(figsize=(8,7))
ax = fig.add_subplot(111)

colors = ["red", "blue", "green"]
styles = ["-", "--", "-."]

scale = args.scale

for i, s in enumerate(crs):
    ptype, name = s.key(), s.data()
    E, F = getFlux(name)
    ax.plot(E, E**scale*F, label=("%s" % ptype), color=colors[i%3], ls=styles[i/3])

for s in els1:
    ptype, name = s.key(), s.data()
    E, F = getFlux(name)
    ax.plot(E, E**scale*F, label=r"HESS e+e-", color="black")

for s in els2:
    ptype, name = s.key(), s.data()
    E, F = getFlux(name)
    ax.plot(E, E**scale*F, label=r"Combined e+e-", color="black", ls="--")

# Fermi-LAT diffuse extragalactic flux
fEGB = lambda E: (1.45e-7/(U.MeV*U.cm**2*U.second*U.sr)) * (E/(100*U.MeV))**-2.41

# EGRET diffuse extragalactic flux
fEGR = lambda E: (1.45e-7/(U.MeV*U.cm**2*U.second*U.sr)) * (E/(100*U.MeV))**-2
E = np.logspace(np.log10(10*U.GeV), np.log10(10*U.TeV))
F = fEGB(E) * (U.TeV*U.meter**2*U.second*U.sr)
E /= U.TeV
ax.plot(E, E**scale*F, label=r"EGB", color="blue", lw=2)

F = fEGR(E*U.TeV) * (U.TeV*U.meter**2*U.second*U.sr)
ax.plot(E, E**scale*F, label=r"EGRET", color="red", lw=2)

ax.set_xscale("log")
ax.set_xlabel("energy [TeV]")
ax.set_xlim([1e-2, 1e2])
ax.xaxis.set_major_formatter(mpl.ticker.FormatStrFormatter("%g"))
ax.set_yscale("log")
if scale == 0:
    ax.set_ylabel(r"flux [TeV$^{-1}$ m$^{-2}$ s$^{-1}$ sr$^{-1}$]")
elif scale == 1:
    ax.set_ylabel(r"E$\cdot$flux [m$^{-2}$ s$^{-1}$ sr$^{-1}$]")
else:
    ax.set_ylabel(r"E$^{%g}\cdot$flux [TeV$^{%g}$ m$^{-2}$ s$^{-1}$ sr$^{-1}$]" % \
                  (scale, scale-1.))
ax.set_title("Isotropic Cosmic Ray Flux")

handles, labels = ax.get_legend_handles_labels()
leg = ax.legend(handles, labels, fontsize=10, loc="best")
leg.get_frame().set_linewidth(0)

fig = plt.figure(2)
ax = fig.add_subplot(111)
E = np.logspace(np.log10(30*U.MeV), np.log10(100*U.GeV))
F = fEGB(E) * (U.MeV*U.cm**2*U.second*U.sr)
E /= U.MeV
ax.plot(E, E**scale*F, label=r"EGB", color="blue", lw=2)
F = fEGR(E*U.MeV) * (U.MeV*U.cm**2*U.second*U.sr)
ax.plot(E, E**scale*F, label=r"EGRET", color="red", lw=2)
ax.set_xlim([30., 1e5])
ax.set_xscale("log")
ax.set_yscale("log")

plt.show()
