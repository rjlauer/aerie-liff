#!/usr/bin/env python

try:
    from hawc import hawcnest, rng_service
    from HAWCNest import HAWCNest

    import argparse
    import numpy as np
    import matplotlib as mpl
    import matplotlib.pyplot as plt
except ImportError,e:
    print e
    raise SystemExit

p = argparse.ArgumentParser(description="Exponential generator")
p.add_argument("-r", "--rate", dest="rate", default=1., type=float,
               help="Exponential rate parameter")
p.add_argument("-n", "--number", dest="number", default=1000, type=int,
               help="Number of random variables to draw")
p.add_argument("-s", "--seed", dest="seed", default=-1, type=int,
               help="Random number seed (-1=/dev/random, 0=clock, n)")
args = p.parse_args()

nest = HAWCNest()

nest.Service("StdRNGService", "rng",
    seed=args.seed)

nest.Configure()

rng = rng_service.GetService("rng")

x = [rng.Exponential(args.rate) for n in range(0, args.number)]

# histogram data, calculate bin centers, calculate widths
#bins = np.logspace(0., np.log10(np.max(x)), 30)
bins = np.linspace(0, np.max(x), 30)
n, edges = np.histogram(x, bins)
center = 0.5*(bins[:-1]+bins[1:])
widths = (bins[1:] - bins[:-1])

mpl.rc("font", family="serif", size=14)
fig = plt.figure()
ax = fig.add_subplot(111)
ax.bar(bins[:-1], n, width=widths, fc="#9999ff", ec="#8888ff", log=True)
ax.set_xlabel(r"$x$")
ax.set_ylabel("count")
ax.set_yscale("log")
ax.set_title(r"Exponential Distribution: $\lambda=%g$" % args.rate)

fig.tight_layout()

plt.show()
