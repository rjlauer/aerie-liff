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

p = argparse.ArgumentParser(description="Power law generator")
p.add_argument("-a", "--xmin", dest="a", default=1., type=float,
               help="RNG lower limit")
p.add_argument("-b", "--xmax", dest="b", default=100., type=float,
               help="RNG upper limit")
p.add_argument("-c", "--cutoff", dest="cutoff", default=None, type=float,
               help="Power law spectral cutoff")
p.add_argument("-i", "--index", dest="index", default=-2., type=float,
               help="Power law spectral index")
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

if args.cutoff:
    a, b, c, i = args.a, args.b, 1./args.cutoff, args.index
    x = [rng.CutoffPowerLaw(i, c, a, b) for n in range(0, args.number)]
else:
    a, b, i = args.a, args.b, args.index
    x = [rng.PowerLaw(i, a, b) for n in range(0, args.number)]

# histogram data, calculate bin centers, calculate widths
bins = np.logspace(np.log10(a), np.log10(b), 30)
n, edges = np.histogram(x, bins)
center = 0.5*(bins[:-1]+bins[1:])
widths = (bins[1:] - bins[:-1])

# Apply correction: log(e)/E * dN/dlogE = dN/dE
n = n * 0.434 / center

mpl.rc("font", family="serif", size=14)
fig = plt.figure()
ax = fig.add_subplot(111)
ax.bar(bins[:-1], n, width=widths, fc="#9999ff", ec="#8888ff", log=True)
ax.set_xlabel(r"$x$")
ax.set_xscale("log")
ax.set_ylabel("count")
ax.set_yscale("log")
#ax.set_title(r"Poisson Random Variable: $\mu=%.1f$" % args.mean)

fig.tight_layout()

plt.show()
