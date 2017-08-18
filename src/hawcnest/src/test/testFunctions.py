#!/usr/bin/env python
"""Demonstrate the use of python functions as services in the framework,
including anonymous lambdas.

.. codeauthor:: Segev BenZvi
"""

__version__ = "$Id: testFunctions.py 19005 2014-03-08 17:09:37Z sybenzvi $"

from hawc import hawcnest
from HAWCNest import HAWCNest

count  = []
countx = []

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

def scaleBy3(bag):
    """Scale the 'count' value in the Bag by 3"""
    if "count" in bag:
        x = hawcnest.BaggableInt(bag["count"].value * 3)
        bag["countx"] = x
        return True
    return False

def printer(bag):
    """Print the 'count' and 'countx' elements in the bag"""
    if "count" in bag and "countx" in bag:
        print("%6d -> %6d" % (bag["count"].value, bag["countx"].value))
        return True
    return False

def keeper(bag):
    """Stuff the 'count' and 'countx' elements in the bag into global arrays"""
    if "count" in bag and "countx" in bag:
        count.append(bag["count"].value)
        countx.append(bag["countx"].value)
        return True
    return False

# Create source and module instances
nest = HAWCNest()

nest.Service(CountInserter, "countInserter",
    startCount=1, maxCount=20)

# Module sequence:
#  1) scale values by 4 using a named function
#  2) filter out even-numbered results using an anonymous (lambda) function
#  3) print results using a named function
#  4) stuff results into global arrays to be checked at the end
nest.Service(scaleBy3, "scaler")

nest.Service(
    lambda bag: True if "countx" in bag and bag["countx"].value % 2 else False,
    "oddFilter")

nest.Service(printer, "printer")

nest.Service(keeper, "keeper")

# Set up module chain
chain = ["scaler", "oddFilter", "printer", "keeper"]

nest.Service("SequentialMainLoop", "mainloop",
    modulechain=chain,
    source="countInserter")

# Configure and run
nest.Configure()
loop = hawcnest.GetService_MainLoop("mainloop")
loop.Execute()
nest.Finish()

# Test that the counter worked as advertised
assert(count  == [1, 3, 5, 7, 9, 11, 13, 15, 17, 19])
assert(countx == [3, 9, 15, 21, 27, 33, 39, 45, 51, 57])

