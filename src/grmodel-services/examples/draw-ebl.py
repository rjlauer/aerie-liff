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

from hawc import hawcnest, data_structures
from hawc.hawcnest import HAWCUnits as U
from hawc.data_structures import *
from hawc import grmodel_services as grmodel_services
from HAWCNest import HAWCNest

import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

nest = HAWCNest()
nest.Service("Franceschini08EBLModel", "fra08")
nest.Service("Gilmore09EBLModel", "gil09")
nest.Service("Gilmore12FiducialEBLModel", "gil12a")
nest.Service("Gilmore12FixedEBLModel", "gil12b")
nest.Configure()

fra08 = grmodel_services.GetEBLAbsorptionService("fra08")
gil09 = grmodel_services.GetEBLAbsorptionService("gil09")
gil12a = grmodel_services.GetEBLAbsorptionService("gil12a")
gil12b = grmodel_services.GetEBLAbsorptionService("gil12b")

# Create optical depth and attenuation tables for Franceschini model
logETeV = np.linspace(-1.4, 2.2, 250)
redshift = np.linspace(0.00, 2.00, 250)

logEmin, logEmax = logETeV[0], logETeV[-1]
zmin, zmax = redshift[0], redshift[-1]

od08 = np.zeros((len(logETeV), len(redshift)), dtype=float)
tr08 = np.zeros((len(logETeV), len(redshift)), dtype=float)

for i, logE in enumerate(logETeV):
    for j, z in enumerate(redshift):
        E = 10**logE * U.TeV
        od08[i][j] = fra08.GetOpticalDepth(E, z)
        tr08[i][j] = fra08.GetAttenuation(E, z)

# Create optical depth and attenuation tables for Gilmore models
logETeV = np.linspace(-3, 2, 250)
redshift = np.linspace(0.00, 9.00, 250)

od09 = np.zeros((len(logETeV), len(redshift)), dtype=float)
tr09 = np.zeros((len(logETeV), len(redshift)), dtype=float)
od12 = np.zeros((len(logETeV), len(redshift)), dtype=float)
tr12 = np.zeros((len(logETeV), len(redshift)), dtype=float)

for i, logE in enumerate(logETeV):
    for j, z in enumerate(redshift):
        E = 10**logE * U.TeV
        od09[i][j] = gil09.GetOpticalDepth(E, z)
        tr09[i][j] = gil09.GetAttenuation(E, z)
        od12[i][j] = gil12a.GetOpticalDepth(E, z)
        tr12[i][j] = gil12a.GetAttenuation(E, z)

# Figure showing the optical depth and attenuation table (2008)
mpl.rc("font", family="serif", size=14)

fig = plt.figure(1, figsize=(14,6))
ax = fig.add_subplot(121)
im = ax.imshow(od08.T, interpolation="nearest", origin="lower", aspect=1.7,
               norm=mpl.colors.LogNorm(vmin=1e-5, vmax=1e5),
               extent=[logEmin, logEmax, zmin, zmax])
ax.set_xlabel("log(E/TeV)")
ax.set_ylabel("redshift")
ax.set_title("Franceschini 2008")
fig.colorbar(im, label=r"optical depth $\tau$")

ax = fig.add_subplot(122)
im = ax.imshow(tr08.T, interpolation="nearest", origin="lower", aspect=1.7,
               norm=mpl.colors.LogNorm(vmin=1e-3, vmax=1),
               extent=[logEmin, logEmax, zmin, zmax])
ax.set_xlabel("log(E/TeV)")
ax.set_ylabel("redshift")
ax.set_title("Franceschini 2008")
fig.colorbar(im, label=r"attenuation (exp[-$\tau$])")

fig.subplots_adjust(left=0.05, right=0.97)

# Figure showing the optical depth and attenuation table (2009)
fig = plt.figure(2, figsize=(14,6))
ax = fig.add_subplot(121)
im = ax.imshow(od09.T, interpolation="nearest", origin="lower", aspect=0.5,
               norm=mpl.colors.LogNorm(vmin=1e-5, vmax=1e5),
               extent=[logETeV[0], logETeV[-1], redshift[0], redshift[-1]])
ax.set_xlabel("log(E/TeV)")
ax.set_ylabel("redshift")
ax.set_title("WMAP3 Low Model (Gilmore 2009)")
fig.colorbar(im, label=r"optical depth $\tau$")

ax = fig.add_subplot(122)
im = ax.imshow(tr09.T, interpolation="nearest", origin="lower", aspect=0.5,
               norm=mpl.colors.LogNorm(vmin=1e-3, vmax=1),
               extent=[logETeV[0], logETeV[-1], redshift[0], redshift[-1]])
ax.set_xlabel("log(E/TeV)")
ax.set_ylabel("redshift")
ax.set_title("WMAP3 Low Model (Gilmore 2009)")
fig.colorbar(im, label=r"attenuation (exp[-$\tau$])")

fig.subplots_adjust(left=0.03, right=0.97)

# Figure showing the optical depth and attenuation table (2012)
fig = plt.figure(3, figsize=(14,6))
ax = fig.add_subplot(121)
im = ax.imshow(od12.T, interpolation="nearest", origin="lower", aspect=0.5,
               norm=mpl.colors.LogNorm(vmin=1e-5, vmax=1e5),
               extent=[logETeV[0], logETeV[-1], redshift[0], redshift[-1]])
ax.set_xlabel("log(E/TeV)")
ax.set_ylabel("redshift")
ax.set_title("WMAP5 Fiducial Model (Gilmore 2012)")
fig.colorbar(im, label=r"optical depth $\tau$")

ax = fig.add_subplot(122)
im = ax.imshow(tr12.T, interpolation="nearest", origin="lower", aspect=0.5,
               norm=mpl.colors.LogNorm(vmin=1e-3, vmax=1),
               extent=[logETeV[0], logETeV[-1], redshift[0], redshift[-1]])
ax.set_xlabel("log(E/TeV)")
ax.set_ylabel("redshift")
ax.set_title("WMAP5 Fiducial Model (Gilmore 2012)")
fig.colorbar(im, label=r"attenuation (exp[-$\tau$])")

fig.subplots_adjust(left=0.03, right=0.97)

# Add a figure of attenuation curves vs. energy: Fig. 7 of MNRAS 422:2012, 3189,
# with the same curves from the Franceschini 2008 and Gilmore 2009 papers.
E = np.logspace(np.log10(50*U.GeV), np.log10(100*U.TeV))

fig = plt.figure(4)
ax = fig.add_subplot(111)

tr = [fra08.GetAttenuation(e, 0.03) for e in E]
ax.plot(E/U.TeV, tr, "-", color="black", label="Franceschini 2008")
tr = [gil09.GetAttenuation(e, 0.03) for e in E]
ax.plot(E/U.TeV, tr, "--", color="black", label="2009 WMAP3 Low")
tr = [gil12a.GetAttenuation(e, 0.03) for e in E]
ax.plot(E/U.TeV, tr, "red", label="2012 WMAP5 Fiducial")
tr = [gil12b.GetAttenuation(e, 0.03) for e in E]
ax.plot(E/U.TeV, tr, "--", color="red", label="2012 WMAP5 Fixed")
ax.text(18., 0.28, "z = 0.03", size=12)

tr = [fra08.GetAttenuation(e, 0.1) for e in E]
ax.plot(E/U.TeV, tr, "-", color="black")
tr = [gil09.GetAttenuation(e, 0.1) for e in E]
ax.plot(E/U.TeV, tr, "--", color="black")
tr = [gil12a.GetAttenuation(e, 0.1) for e in E]
ax.plot(E/U.TeV, tr, "red")
tr = [gil12b.GetAttenuation(e, 0.1) for e in E]
ax.plot(E/U.TeV, tr, "--", color="red")
ax.text(7., 0.1, "0.1", size=12)

tr = [fra08.GetAttenuation(e, 0.25) for e in E]
ax.plot(E/U.TeV, tr, "-", color="black")
tr = [gil09.GetAttenuation(e, 0.25) for e in E]
ax.plot(E/U.TeV, tr, "--", color="black")
tr = [gil12a.GetAttenuation(e, 0.25) for e in E]
ax.plot(E/U.TeV, tr, "red")
tr = [gil12b.GetAttenuation(e, 0.25) for e in E]
ax.plot(E/U.TeV, tr, "--", color="red")
ax.text(2., 0.04, "0.25", size=12)

tr = [fra08.GetAttenuation(e, 0.5) for e in E]
ax.plot(E/U.TeV, tr, "-", color="black")
tr = [gil09.GetAttenuation(e, 0.5) for e in E]
ax.plot(E/U.TeV, tr, "--", color="black")
tr = [gil12a.GetAttenuation(e, 0.5) for e in E]
ax.plot(E/U.TeV, tr, "red")
tr = [gil12b.GetAttenuation(e, 0.5) for e in E]
ax.plot(E/U.TeV, tr, "--", color="red")
ax.text(0.7, 0.02, "0.5", size=12)

tr = [fra08.GetAttenuation(e, 1.0) for e in E]
ax.plot(E/U.TeV, tr, "-", color="black")
tr = [gil09.GetAttenuation(e, 1.0) for e in E]
ax.plot(E/U.TeV, tr, "--", color="black")
tr = [gil12a.GetAttenuation(e, 1.0) for e in E]
ax.plot(E/U.TeV, tr, "red")
tr = [gil12b.GetAttenuation(e, 1.0) for e in E]
ax.plot(E/U.TeV, tr, "--", color="red")
ax.text(0.11, 0.007, "z = 1.0", size=12)

ax.set_xscale("log")
ax.xaxis.set_major_formatter(mpl.ticker.FormatStrFormatter("%g"))
ax.set_xlim([0.05, 1e2])
ax.set_xlabel(r"E$_\gamma$ (TeV)")
ax.set_yscale("log")
ax.yaxis.set_major_formatter(mpl.ticker.FormatStrFormatter("%g"))
ax.set_ylim([1e-3, 2])
ax.set_ylabel(r"attenuation (exp[-$\tau$])")
ax.set_title("EBL Attenuation")

handles, labels = ax.get_legend_handles_labels()
leg = ax.legend(handles, labels, fontsize=9)
leg.get_frame().set_linewidth(0)

plt.show()

