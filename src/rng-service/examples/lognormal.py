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

p = argparse.ArgumentParser(description="Log-Normal generator")
p.add_argument("-m", "--mean", dest="mean", default=0., type=float,
               help="Mean of log-normal distribution")
p.add_argument("-v", "--variance", dest="var", default=1., type=float,
               help="Variance (sigma^2) of log-normal distribution")
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

mu = args.mean
sigma = np.sqrt(args.var)
x = [rng.LogNormal(mu, sigma) for n in range(0,args.number)]
bins = np.linspace(0, np.ceil(np.max(x)), 30)

mu = np.mean(x)
s2 = np.std(x)**2

print mu, s2

mpl.rc("font", family="serif", size=14)
fig = plt.figure()
ax = fig.add_subplot(111)
ax.hist(x, bins, fc="#9999ff", ec="#8888ff")
ax.set_xlabel(r"$x$")
ax.set_ylabel("count")
ax.set_title(r"Log-normal Random Variable: $\mu=%g$, $\sigma^2=%g$" %
    (args.mean, args.var))

fig.tight_layout()

plt.show()
