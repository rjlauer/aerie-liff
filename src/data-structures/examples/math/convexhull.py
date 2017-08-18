#!/usr/bin/env python
"""
Test python bindings of the ConvexHull class.

.. codeauthor:: Zig Hampel
"""

from hawc import hawcnest, data_structures
from hawc.data_structures import *

import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
import matplotlib.patches as patches
import pylab
import argparse
from optparse import OptionParser


def main():
    global args
    p = argparse.ArgumentParser(description="Convex Hull Example Script")
    p.add_argument("-n", "--numpoints", dest="numpoints", type=int, default=100,
                   help="Number of points for model array (>0).")
    p.add_argument("-s", "--scale", dest="scale", type=np.double, default=.75,
                   help="Scale factor to scale convex hull (>0.).")
    args = p.parse_args()
    usage = "%prog options"
    
    if ( (args.numpoints <= 0) or (args.scale<=0) ):
        print ""
        print ""
        print "Sorry. There was a problem with your input parameters."
        print ""
        print ""
        p.print_help()
        import sys
        sys.exit()

    numpoints = args.numpoints
    scale = args.scale


    # Random points in unit square centered at (.5,.5)
    # used as initial 'array' points to form hull
    x = np.random.uniform(0,1,numpoints)
    y = np.random.uniform(0,1,numpoints)
    
    # New ConvexHull
    convexhull = ConvexHull(x,y)
    
    print "" 
    print "Center of points: ", convexhull.get_centroid()
    hx, hy = convexhull.get_hull()
    
    hx = np.asarray(hx)
    hy = np.asarray(hy)
    h_xy = np.zeros((len(hx),2))
    
    print "" 
    print "Area of calculated Convex Hull: ", convexhull.area()
    print "" 
    print "Vertices of Calculated Convex Hull: "
    for i in xrange(0,len(hx)):
        print hx[i], hy[i]
        h_xy[i,0] = hx[i]
        h_xy[i,1] = hy[i]
    
    # Scale hull
    sx, sy = convexhull.scale_hull(scale)
    
    sx = np.asarray(sx)
    sy = np.asarray(sy)
    s_xy = np.zeros((len(sx),2))
    for i in xrange(0,len(sx)):
        s_xy[i,0] = sx[i]
        s_xy[i,1] = sy[i]
    
    # Set up an inclusion test for newly calculated Convex Hull
    pnpoly = PnPoly(sx,sy) 
    
    t = np.linspace(0,2*np.pi,100)
    testx = 1.*np.cos(t)
    testy = 1.*np.sin(t)
    
    pn_test = []
    for i in xrange(0,len(t)):
        eval = pnpoly.evaluate(testx[i],testy[i])
        pn_test.append(eval)
    
    pn_test = np.asarray(pn_test)
    
    # Identify scaled hull in/out points for plotting
    inx = testx[(pn_test==1)]
    iny = testy[(pn_test==1)]
    outx = testx[(pn_test==0)]
    outy = testy[(pn_test==0)]
    
    # Plot ConvexHull, input data, and inclusion test results
    mpl.rc("font", family="serif")
    
    fig = plt.figure(1, figsize=(9,9))
    ax = fig.add_subplot(111)
    title="Convex Hull Calculation Example"
    ax.set_title(title)
    
    # Convex Hull
    pylab.scatter([p[0] for p in h_xy],[p[1] for p in h_xy], s=10., c='k')
    alf=.25
    if (scale > 1.):
        alf = 0.45
    pylab.gca().add_patch(patches.Polygon(h_xy,closed=True,fill=True, alpha=alf, color='k', label='Array Hull'))
    
    # Scaled Hull
    pylab.scatter([p[0] for p in s_xy],[p[1] for p in s_xy], s=.01, c='k')
    alf=.45
    if (scale > 1.):
        alf = 0.25
    pylab.gca().add_patch(patches.Polygon(s_xy,closed=True,fill=True, alpha=alf, color='k', label='Scaled Hull'))
    
    # Input 'array' points
    pylab.scatter(x, y, c='k', s=1.01, label='Array points')
    # Test Points
    ax.plot(inx, iny, 'r.', label='In points')
    ax.plot(outx, outy, 'b.', label='Out points')
    
    ax.set_aspect('equal', 'datalim')
    ax.grid()
    legend = ax.legend(loc='upper left')
    plt.show()
    
    import sys
    sys.exit()
    

if __name__ == "__main__":
    main()
