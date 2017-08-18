#!/usr/bin/env python
################################################################################
# A demonstration of loading cosmic ray anisotropy tables and accessing the    #
# intensity PDF.                                                               #
#                                                                              #
# To use this program AERIE must be installed and /path/to/aerie/lib must be   #
# included in your PYTHONPATH environment variable.  The environment is set up #
# with the hawc-config script:                                                 #
#                                                                              #
#   $> eval `/path/to/aerie/bin/hawc-config --env-sh`                          #
################################################################################

try:
    from hawc import hawcnest, data_structures
    from hawc import astro_service, rng_service, grmodel_services
    from hawc.hawcnest import HAWCUnits as HAWCUnits
    from hawc.data_structures import *
    from HAWCNest import HAWCNest

    import matplotlib as mpl
    import matplotlib.pyplot as plt
    import numpy as np
    import healpy as hp
    import argparse
    import os
except ImportError,e:
    print e
    raise SystemExit

degree = HAWCUnits.degree
MeV = HAWCUnits.MeV
GeV = HAWCUnits.GeV
TeV = HAWCUnits.TeV
cm2 = HAWCUnits.cm2
m2  = HAWCUnits.meter2
sec = HAWCUnits.second
sr  = HAWCUnits.steradian

fluxTeV = 1. / (TeV*cm2*sec*sr)
fluxGeV = 1. / (GeV*m2*sec*sr)

def main():
    """ Main function: execute the program here.
    """
    # Parse command line options
    p = argparse.ArgumentParser(description="Draw diffuse maps")
    p.add_argument("fitsFile", nargs="?",
                   help="Cosmic ray anisotropy FITS file")
    p.add_argument("-e", "--energy", dest="energy", type=float, default=10.,
                   help="Map energy [TeV]")
    p.add_argument("-m", "--mask", dest="mask", action="store_true",
                   default=False, help="Mask out pixels invisible to HAWC")
    p.add_argument("-n", "--nside", dest="nside", type=int, default=128,
                   help="HEALPix map nside parameter (power of 2)")
    args = p.parse_args()

    if not args.fitsFile:
        p.print_help()
        return

    nest = HAWCNest()

    nest.Service("StdAstroService", "astro")

    cdir = "/".join([os.environ["HAWC_INSTALL"], "share/hawc/config"])
    nest.Service("TabulatedSpectrum", "crabFlux",
        infilename="/".join([cdir, "CrabICSModel.txt"]))

    nest.Service("StdRNGService", "rng",
        seed=12345)

    nest.Configure()
    nest.Finish()

    # Open the GALPROP map and plot the diffuse flux at some energy
    ct = grmodel_services.CosmicRayAnisotropyTable(args.fitsFile)

    E = args.energy*TeV
    nside = args.nside
    npix = hp.nside2npix(nside)
    dmap = np.zeros(npix, dtype=float)

    for i in range(0, npix):
        z, a = hp.pix2ang(nside, i)
        d = 90*degree - z
        dmap[i] = ct.GetPDF(E, EquPoint(a,d))

        if args.mask:
            if d > 69*degree or d < -31*degree:
                dmap[i] = hp.UNSEEN

    print 1. - np.min(dmap[dmap != hp.UNSEEN])
    mpl.rc("font", family="serif")

    fig = plt.figure(1)
    hp.mollview(dmap, fig=1, coord="C", rot=180,
                title="Cosmic Ray Anisotropy: %g TeV" % args.energy)
    hp.graticule()

    plt.show()

if __name__ == "__main__":
    main()

