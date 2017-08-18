#!/usr/bin/env python
"""Demonstrate subclassing of AERIE sources and modules and their use in the
framework.

.. codeauthor:: Segev BenZvi
"""

__version__ = "$Id: testSubclassing.py 19005 2014-03-08 17:09:37Z sybenzvi $"

from hawc import hawcnest
from HAWCNest import HAWCNest

class CountInserter(hawcnest.Source):
    """Subclassed AERIE source.  Generates integers up to some max value and
    stuffs them into the Bag.  Keyword arguments are used to set up the counter.
    """
    def __init__(self, **kwargs):
        hawcnest.Source.__init__(self)
        self.counter = kwargs.get("startCount", 0)
        self.maxCount = kwargs.get("maxCount", 10)
        self.step = kwargs.get("stepCount", 1)

    def Next(self):
        if self.counter <= self.maxCount:
            x = hawcnest.BaggableInt(self.counter)
            self.counter += self.step
            bag = hawcnest.Bag()
            bag["count"] = x
            return bag
        else:
            return None

class CountMultiplier(hawcnest.Module):
    """Subclassed AERIE module.  Takes integers in the Bag, multiplies their
    value by some factor given as a keyword parameter, and the stuffs the
    result into the Bag.
    """
    def __init__(self, **kwargs):
        hawcnest.Module.__init__(self)
        self.factor = kwargs.get("factor", 1)

    def Process(self, bag):
        if "count" in bag:
            x = hawcnest.BaggableInt(bag["count"].value * self.factor)
            bag["count2x"] = x
        return hawcnest.Module.CONTINUE

class CountPrinter(hawcnest.Module):
    """Subclassed AERIE module.  Take integers in the Bag and print them out."""
    def __init__(self):
        hawcnest.Module.__init__(self)

    def Process(self, bag):
        if "count" in bag and "count2x" in bag:
            print("%6d -> %6d" % (bag["count"].value, bag["count2x"].value))
        return hawcnest.Module.CONTINUE

class CountKeeper(hawcnest.Module):
    """Subclassed AERIE module.  Take integers in the Bag and store them."""
    def __init__(self):
        hawcnest.Module.__init__(self)
        self.count   = []
        self.count2x = []

    def Process(self, bag):
        if "count" in bag and "count2x" in bag:
            self.count.append(bag["count"].value)
            self.count2x.append(bag["count2x"].value)
        return hawcnest.Module.CONTINUE

# Create source and module instances
nest = HAWCNest()

nest.Service(CountInserter, "countInserter",
    startCount=1, maxCount=22, stepCount=3)

nest.Service(CountMultiplier, "countMultiplier",
    factor=4)

nest.Service(CountPrinter, "countPrinter")

keeper = CountKeeper()
nest.Service(keeper, "countKeeper")

# Set up module chain
chain = ["countMultiplier", "countPrinter", "countKeeper"]

nest.Service("SequentialMainLoop", "mainloop",
    modulechain=chain,
    source="countInserter")

# Configure and run
nest.Configure()
loop = hawcnest.GetService_MainLoop("mainloop")
loop.Execute()
nest.Finish()

# Test the generated count sequences
assert(keeper.count == [1, 4, 7, 10, 13, 16, 19, 22])
assert(keeper.count2x == [4, 16, 28, 40, 52, 64, 76, 88])

