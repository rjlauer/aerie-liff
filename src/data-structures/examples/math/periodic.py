#!/usr/bin/env python
"""
Test python bindings of the PeriodicFunction class.

.. codeauthor:: Segev BenZvi
"""

__version__ = "$Id: periodic.py 22108 2014-10-05 15:35:03Z sybenzvi $"

from hawc import hawcnest, data_structures
from hawc.data_structures import *

import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl

# Define phase table for periodic cycle
phase = np.linspace(0,1,101)
phase = phase.tolist()

# A periodic square wave with a "duty cycle" of 3/20
ffrac = np.ones(101, dtype=float) * 0.1
ffrac[0:6] = 1.
ffrac = ffrac.tolist()

p05 = PeriodicFunction(2., 5., phase, ffrac)     # offset 2, period 5
p10 = PeriodicFunction(2., 10., phase, ffrac)    # offset 2, period 10
p15 = PeriodicFunction(2., 15., phase, ffrac)    # offset 2, period 15
p20 = PeriodicFunction(2., 20., phase, ffrac)    # offset 2, period 20

t = np.arange(-20,40, 0.01)
f05 = [p05(tt) for tt in t]
f10 = [p10(tt) + 1 for tt in t]
f15 = [p15(tt) + 2 for tt in t]
f20 = [p20(tt) + 3 for tt in t]

mpl.rc("font", family="serif")

fig = plt.figure(1, figsize=(6,9))
ax = fig.add_subplot(311)
ax.plot(t, f05)
ax.plot(t, f10)
ax.plot(t, f15)
ax.plot(t, f20)
ax.set_title("Square Pulse")
ax.grid()

# A sinusoid with amplitude between 0.1 and 1
p = np.asarray(phase)
ffrac = 0.45 * (np.sin(2*np.pi*p) + 1) + 0.1
ffrac = ffrac.tolist()

p05 = PeriodicFunction(2., 5., phase, ffrac)     # offset 2, period 5
p10 = PeriodicFunction(2., 10., phase, ffrac)    # offset 2, period 10
p15 = PeriodicFunction(2., 15., phase, ffrac)    # offset 2, period 15
p20 = PeriodicFunction(2., 20., phase, ffrac)    # offset 2, period 20

f05 = [p05(tt) for tt in t]
f10 = [p10(tt) + 1 for tt in t]
f15 = [p15(tt) + 2 for tt in t]
f20 = [p20(tt) + 3 for tt in t]

ax = fig.add_subplot(312)
ax.plot(t, f05)
ax.plot(t, f10)
ax.plot(t, f15)
ax.plot(t, f20)
ax.set_title("Sinusoid")
ax.grid()

# A sawtooth with amplitude between 0.1 and 1
p = np.asarray(phase)
ffrac = p - np.floor(p+0.5) + 0.55
ffrac = ffrac.tolist()

p05 = PeriodicFunction(2., 5., phase, ffrac)     # offset 2, period 5
p10 = PeriodicFunction(2., 10., phase, ffrac)    # offset 2, period 10
p15 = PeriodicFunction(2., 15., phase, ffrac)    # offset 2, period 15
p20 = PeriodicFunction(2., 20., phase, ffrac)    # offset 2, period 20

f05 = [p05(tt) for tt in t]
f10 = [p10(tt) + 1 for tt in t]
f15 = [p15(tt) + 2 for tt in t]
f20 = [p20(tt) + 3 for tt in t]

ax = fig.add_subplot(313)
ax.plot(t, f05)
ax.plot(t, f10)
ax.plot(t, f15)
ax.plot(t, f20)
ax.set_title("Sawtooth")
ax.grid()

fig.tight_layout()

plt.show()

