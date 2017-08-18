#!/usr/bin/env python
"""Test the interface to the RNG service; just run and make sure no exceptions
are thrown.

.. codeauthor:: Segev BenZvi
"""

__version__ = "$Id"

from hawc import hawcnest, data_structures, rng_service
from HAWCNest import HAWCNest

nest = HAWCNest()

nest.Service("StdRNGService", "rng1",
    seed=-1)
nest.Service("StdRNGService", "rng2",
    seed=0)
nest.Service("StdRNGService", "rng3",
    seed=12345)

nest.Configure()
