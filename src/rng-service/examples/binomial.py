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

p = argparse.ArgumentParser(description="Binomial generator")
p.add_argument("-t", "--trials", dest="trials", default=10, type=int,
               help="Number of binomial trials")
p.add_argument("-p", "--prob", dest="prob", default=0.5, type=float,
               help="Binomial success rate")
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

x = [rng.Binomial(args.trials, args.prob) for n in range(0,args.number)]
bins = np.arange(np.min(x), np.max(x)+1, 1)

mu = np.mean(x)
s2 = np.std(x)**2
print mu, s2

mpl.rc("font", family="serif", size=14)
fig = plt.figure()
ax = fig.add_subplot(111)
ax.hist(x, bins, fc="#9999ff", ec="#8888ff")
ax.set_xlabel(r"$x$")
ax.set_ylabel("count")
ax.set_title(r"Binomial Random Variable: $n=%g$, $p=%g$" %
    (args.trials, args.prob))

fig.tight_layout()

plt.show()
