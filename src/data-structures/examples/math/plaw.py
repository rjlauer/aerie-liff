#!/usr/bin/env python

from hawc import hawcnest, data_structures
from hawc.hawcnest import HAWCUnits as HAWCUnits
from hawc.data_structures import *

def main():
    x0, x1, A, idx = [ 1., HAWCUnits.infinity, 1., -2. ]
    p1 = PowerLaw(x0, x1, A, idx)

    x0, x1, A, idx = [ 1., HAWCUnits.infinity, 1., -3. ]
    p2 = PowerLaw(x0, x1, A, idx)

    print p1.evaluate(1e2)
    print p2.evaluate(1e2)

    print p1.normweight(10., HAWCUnits.infinity)
    print p2.normweight(10., HAWCUnits.infinity)

    print p2.reweight(p1, 10.)

if __name__ == "__main__":
    main()

