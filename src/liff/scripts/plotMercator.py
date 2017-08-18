#!/usr/bin/env python
################################################################################
# Plot HEALPix Map in column of FITS file on a Mercator Projection.
# Requires healpy to be installed.
################################################################################

__version__ = "$Id: plotMercator.py 40113 2017-08-09 18:36:18Z criviere $"

try:
    import argparse
    import numpy as np
    import healpy as hp
    import os
    from math import log, sqrt, cos, pi
    import pickle

    import matplotlib as mpl
    import matplotlib.pyplot as plt
    from matplotlib.patches import Ellipse

    import re

    import MapPalette

    degree = np.pi / 180.
except ImportError as e:
    print(e)
    raise SystemExit

# Try importing TeVCat
try:
    import TeVCat
    haveTeVCat = True
except ImportError as e:
    haveTeVCat = False
    print(e)

# Try importing the Fermi catalogs
try:
    import FGLCatalog
    import FHLCatalog
    haveFermi = True
except ImportError as e:
    haveFermi = False
    print(e)

# Try importing precession functions
try:
    import precess
    canPrecess = True
except ImportError as e:
    canPrecess = False
    print(e)

# Try importing pyfits.
# Needed to read header of HDU, where there are no maps so healpy can't read. Currently used to check map epoch 
try:
    import pyfits as pf
    havePyfits = True
except ImportError as e:
    havePyfits = False
    print(e)
    
# Try importing the Gumble distribution
try:
    from scipy.stats import gumbel_r
    haveGumbel = True
except ImportError as e:
    haveGumbel = False
    print(e)

def main():
    p = argparse.ArgumentParser(description="Plot map with Mercator projection")
    p.add_argument("fitsfile", nargs="*",
                   help="Input HEALPix FITS file. "
                   "To plot the weighted sum of multiple files, put a list of "
                   "file weight file weight pairs.")
    p.add_argument("-a", "--abthresh", default=None, type=float,
                   help="Apply an absolute (lower/upper) threshold to the map")
    p.add_argument("-c", "--coords", default="C",
                   help="C=equatorial (default), G=galactic")
    p.add_argument("-C", "--column", dest="col", default=0, type=int,
                   help="FITS column in which healpix map resides")
    p.add_argument("--file-diff", dest="diff", default="", type=str,
                   help="FITS File to take the difference from (optional) otherwise uses positional argument")
    p.add_argument("-D", "--coldiff", default=-1, type=int,
                   help="FITS column in which healpix map to be subtracted "
                   "(if any) resides")
    p.add_argument("--mjd", default=None,type=float,
                   help="MJD of the map. Will be plotted in J2000. Supersedes info in header")
    p.add_argument("-l", "--threshold", type=float, default=None,
                   help="Apply a lower threshold to the plot.")
    p.add_argument("--norm-sqdeg", dest="sqdeg", action="store_true",
                   default=False,
                   help="Normalize values to square degree, according to nSides"
                   " (makes sense only certain kinds of maps)")
    p.add_argument("--milagro", action="store_true",
                   help="Use Milagro color scale")
    p.add_argument("--contours", dest="contours", type=float, nargs='*',
                   default = None,
                   help="plot contours. If --contourmap is provided, it will be"
                        " used instead of plotted map. If no contours are"
                        " provided, it will plot confidence intervals"
                        " (1, 2, 3 sigma CI). Else, absolute sigmas are used,"
                        " e.g. --contours 1 2 3 4 [sigma] --contours")
    p.add_argument("--contourscolor", nargs='*',
                   default=None, help="Draw options for contours")
    p.add_argument("--contoursstyle",
                   default=None, help="Draw options for contours")
    p.add_argument("--contourswidth", type=float,
                   default=None, help="Draw options for contours")
    p.add_argument("--gamma", action="store_true",
                   help="Use GeV/TeV gamma-ray color scale")
    p.add_argument("-L", "--label",
                   default=None, help="Color bar label")
    p.add_argument("--cross", action="store_true",
                   help="Plot a cross at the center of the plot")
    p.add_argument("--moon", action="store_true",
                   dest="isMoon", help="is in moon centered coordinates")
    p.add_argument("-m", "--min", type=float, default=None,
                   help="Plot minimum value")
    p.add_argument("-M", "--max", type=float, default=None,
                   help="Plot maximum value")
    p.add_argument("-n", "--ncolors", type=int, default=256,
                   help="Number of contours to use in the colormap")
    p.add_argument("--nocolorbar", dest="colorbar", action="store_false",
                   help="Do not draw color bar.")
    p.add_argument("--nofill", action="store_true",
                   help="Do not draw fill plot with colors. "
                        "Useful for contours.")
    p.add_argument("-o", "--output", default=None,
                   help="Output file name (optional). "
                   "If file name ends in .fits or .fit, save as a healpix map with the pixels outside the plot area set to hp.UNSEEN.")
    p.add_argument("-s", "--scale", type=float, default=1.,
                   help="scale up or down values in map")
    p.add_argument("--sun", action="store_true",
                   dest="isSun", help="is in Sun centered coordinates")
    p.add_argument("--dpar", type=float,default=1.,
                  help="Interval in degrees between parallels")
    p.add_argument("--dmer", type=float, default=1.,
                  help="Interval in degrees between meridians.")
    p.add_argument("--nogrid", action="store_true",
                   help="Do not plot gridlines.")
    p.add_argument("--interpolation", action='store_true',
                   help="Uses bilinear interpolation in data map.")
    p.add_argument("--xsize", type=int, default=1000,
                  help="Number of X pixels, Y will be scaled acordingly.")
    p.add_argument("-t", "--ticks", nargs="+", type=float,
                   help="List of ticks for color bar, space-separated")
    p.add_argument("-T", "--title",
                   help="title of map")
    p.add_argument("--squareaspect", action='store_true',
                   help="Better Mercator X/Y ratio.")
    p.add_argument("--preliminary", action="store_true",
                   help="Add a watermark indicating the plot as preliminary")
    p.add_argument("--onlystats", action="store_true",
                   help="Exits after returning min, max and value at center. "
                   "If map is called 'flux', it will give the error if there "
                   "is a map called 'flux_error'")

    # Mutually exclusive option to specify plot xy range OR center + WxH
    argRange = p.add_mutually_exclusive_group(required=False)
    argRange.add_argument("--xyrange", type=float, nargs=4,
                          help="Xmin Xmax Ymin Ymax plot range [degree]")
    argRange.add_argument("--origin", type=float, nargs=4,
                          help="Central (x,y) coords + width + height [degree]")

    # Plot P-Value instead of sigma, expects location and scale of
    # a right-skewed Gumble distribution
    p.add_argument("--gumbel", type=float, nargs=2,
                   help="Plot P-Value instead of significance. "
                   "Arguments: location and scale of right-skewed "
                   "Gumble distribution.")

    # Download/plotting options for Fermi catalog sources
    p.add_argument("--download-fermi", dest="dfermi", default=None,
                   help="Download Fermi FITS data from NASA and exit. Enter version number")
    p.add_argument("--fermicat", default=None,
                   help="Fermi xFGL catalog FITS file name")
    p.add_argument("--fermicat-labels", dest="fermicatLabels",
                  action="store_true",
                  help="Draw Fermi sources with labels.")
  
    # plotting options for Fermi Healpix file
    p.add_argument("--contourmap", default=None,
                   help="Healpix map from which to grab contours.")
    p.add_argument("--contourmapcoord", default="G",
                  help="Coordinate system of contourmap. C=equatorial, G=galactic (default)")

    # Download/plotting options for TeVCat sources
    p.add_argument("--download-tevcat", dest="dtevcat", action="store_true",
                   help="Download data from tevcat.uchicago.edu and exit")
    p.add_argument("--tevcat", default=None,
                   help="Draw TeVCat sources using TeVCat ASCII file")
    p.add_argument("--tevcat-labels", dest="tevcatLabels",
                   action="store_true",
                   help="Draw TeVCat sources with labels.")
    p.add_argument("--cat-labels-angle", dest="catLabelsAngle",
                   default=90, help="Oriantation of catalog labels.")
    p.add_argument("--cat-labels-size", dest="catLabelsSize",
                   default=8, help="Size of catalog labels.")

    # Highlight hotspots listed in an ASCII file
    p.add_argument("--hotspots", default=None,
                   help="Hotspot coordinates in an ASCII file")
    p.add_argument("--hotspot-labels", dest="hotspotLabels",
                   action="store_true",
                   help="Draw hotspots sources with labels.")
    # Plot 2D Gaussian fit result listed in an ASCII file from fitMap.py
    p.add_argument("--gaussfit", default=None,
                   help="2D-Gaussian fit result in an ASCII file")

    args = p.parse_args()

    #Sanity checks
    if (args.mjd is not None) and (not canPrecess):
        print "Missing necessary packages to make precession"
        raise SystemExit
    
    if args.nofill:
        # If we don't fill with colors, we don't plot the colorbar either
        args.colorbar = False

    # Download TeVCat
    if args.dtevcat:
        if haveTeVCat:
            print("Fetching data from tevcat.uchicago.edu.")
            tevcat = TeVCat.TeVCat()
            return None
        else:
            print("Sorry, AERIE TeVCat python module is not available.")
            raise SystemExit

    # Downlaod Fermi catalog
    if args.dfermi:
        if haveFermi:
            print "Fetching 2FGL catalog, version %s" % args.dfermi
            FGLCatalog.fetch_catalog(args.dfermi)
            return None
        else:
            print "Sorry, AERIE Fermi python module is not available."
            raise SystemExit
    
    # Start normal processing
    fitsfile = args.fitsfile
    if fitsfile == []:
        print("Please specify an FITS file to plot.")
        raise SystemExit

    # Fill 2D array
    xmin=-180.
    xmax=180.
    ymax=90
    ymin=-90

    if (args.xyrange):
        xmin, xmax, ymin, ymax = args.xyrange
        xC = (xmin+xmax) / 2.
        yC = (ymin+ymax) / 2.
    elif (args.origin):
        xC, yC, w, h = args.origin
        xmin = xC - 0.5*w
        xmax = xC + 0.5*w
        ymin = yC - 0.5*h
        ymax = yC + 0.5*h
    else:
        print("Using default zoom window. "
              "To customize the zoom window you must specify either "
              "(xyrange) or (origin and width and height).")

    if args.isMoon or args.isSun:
            xmin+=180.
            xmax+=180.
        
    # Move to range expected by healpy
    while xmin < -180:
        xmin += 360
    while xmin > 180:
        xmin -= 360
    while xmax < -180:
        xmax += 360
    while xmax > 180:
        xmax -= 360

    if xmax < xmin:
        tmax = xmax
        tmin = xmin
        xmin = tmax
        xmax = tmin

    cxmin = xmin
    cxmax = xmax
    frot =0.
    if xmax > 90. and xmin < -90.:
      frot = 180.
      cxmin = xmax - 180.
      cxmax = xmin + 180.
    
    if args.origin:
        while xC>180:
          xC-=360
    
    
    # Read in the skymap and mask out empty pixels
    skymap, skymapHeader = hp.read_map(fitsfile[0], args.col, h=True)
    if len(fitsfile) > 1:
        skymap *= float(fitsfile[1])
    # If fitsfile has more parameters, they should be "mapfile weight" pairs
    for i in range(2, len(fitsfile), 2):
        skymap2 = hp.read_map(fitsfile[i], args.col)
        skymap2 *= float(fitsfile[i+1])
        skymap += skymap2
    # remove naughty values
    skymap[np.isnan(skymap)] = hp.UNSEEN
    skymap *= args.scale
    nside1 = hp.get_nside(skymap)
    npix   = hp.nside2npix(nside1)
    
    if args.coldiff > -1:
        if os.path.isfile(args.diff):
            print("Taking difference with {0}".format(args.diff))
            skymap2 = hp.read_map(args.diff, args.coldiff)
        else:
            print("No extra file provided, using same file as input")
            skymap2 = hp.read_map(fitsfile[0], args.coldiff)
            if len(fitsfile) > 1:
                skymap2 *= float(fitsfile[1])
           
        print("Subtracting column {0} from skymap...".format(args.coldiff))
        skymap -= skymap2
        # If fitsfile has more parameters, they should be "mapfile weight" pairs
        for i in range(2, len(fitsfile), 2):
            skymap2 = hp.read_map(fitsfile[i], args.coldiff)
            skymap2 *= float(fitsfile[i+1])
            skymap -= skymap2


    if (args.gumbel):
        assert haveGumbel
        gumbel_location, gumbel_scale = args.gumbel
        gumbel = gumbel_r(loc=gumbel_location, scale=gumbel_scale)
        skymap = gumbel.logsf(skymap)/log(10)

        def inf_suppressor(x):
            return x if np.isfinite(x) else 0.

        inf_suppressor = np.vectorize(inf_suppressor)
        skymap = inf_suppressor(skymap)

    # Normalize value to square degree
    if args.sqdeg:
        pixsizestr = 4*np.pi / (12*nside1**2)
        str2sqdeg = (180/np.pi)**2
        pixsizesqdeg = pixsizestr * str2sqdeg
        skymap /= pixsizesqdeg

    # I did not find header handler, thats all I came up with...
    toFind = 'TTYPE' + str(args.col+1)
    #print type(skymapHeader), type(skymapHeader[0]), skymapHeader, toFind, dict(skymapHeader)[toFind]
    skymapName = dict(skymapHeader)[toFind]

    #Check if it is flux
    isFlux=False
    hasFluxError=False
    if skymapName=='flux':
        isFlux=True

        keyname=dict((v,n) for n,v in skymapHeader).get("flux_error")

        if keyname is not None:
            if keyname.find('TTYPE')==0 and len(keyname)==6:
                hasFluxError=True
                fluxerrmap = hp.read_map(fitsfile[0], int(keyname[5])-1)

        if not hasFluxError:
            print "Map called 'flux_error' not present, will not print errors"

    
    isFluxError=False
    if skymapName=="flux_error":
        isFluxError=True
        
    # Find FOV
    pxls = np.arange(skymap.size)
    nZeroPix = pxls[(skymap != 0)]
    pxTh, pxPh = hp.pix2ang(nside1,pxls)
    
    # Mask outside FOV
    values = np.ma.masked_where((pxTh > pxTh[nZeroPix].max())
                                | (pxTh < pxTh[nZeroPix].min())
                                | (skymap == hp.UNSEEN)
                                | (skymap == 1e20) , skymap)

    # Plot the skymap as an image, setting up the color palette on the way
    mpl.rc("font", size=14, family="serif")
    faspect = abs(cxmax - cxmin)/abs(ymax-ymin)
    fysize = 4
    # Set up the figure frame and coordinate rotation angle
    coords = ["C","C"]
    gratcoord = "C"
    if args.coords == "G":
        coords = ["C","G"]
        gratcoord = "G"
        
    if args.mjd is not None:
        rotMap=precess.mjd2J2000ang(mjd=args.mjd,coord=coords,rot=frot)
        rotVertex=precess.mjd2J2000ang(mjd=args.mjd,coord=coords)
    elif not (args.isMoon or args.isSun):
        mjd = False;
        if havePyfits:
            hdulist = pf.open(fitsfile[0])
            header = hdulist[0].header
            if 'EPOCH' in header:
                epoch = header['EPOCH']
                if epoch=='J2000':
                    pass
                elif epoch=='current':
                    if 'STARTMJD' in header and 'STOPMJD' in header:
                        startmjd=header['STARTMJD']
                        stopmjd = header['STOPMJD']
                        if (startmjd>0 and stopmjd>0) or startmjd>stopmjd:
                            mjd = (stopmjd+startmjd)/2
                        else:
                            print "STARTMJD/STOPMJD are not well-definned, will not attempt to precess!"
                    else:
                        print "STARTMJD or STOPMJD not present in header, will not attempt to precess!"
                else:
                    print "Currently EPOCH can be J2000 or current. Will not attempt to precess"
            else:
                print "Key EPOCH not in header, will not attempt to precess!"
                     
        else:
            print "Pyfits not available -> Can't check map epoch -> Will not attempt to precess!"
        
        if mjd:
            print "Current epoch detected in header, will precess from MJD%g to J2000" %mjd
            rotMap=precess.mjd2J2000ang(mjd=mjd,coord=coords,rot=frot)
            rotVertex=precess.mjd2J2000ang(mjd=mjd,coord=coords)
        else:
            rotMap=frot
            rotVertex=0
    else:
        rotMap=frot
        rotVertex=0
    
    # Get extrema
    angverts = [[xmin,ymin],[xmax,ymin],\
                [xmax,ymax],[xmin,ymax]]
    vertlist = [ ]
    cRot = hp.Rotator(coord=coords,rot=rotVertex)
    for x,y in angverts:
        ctht,cph = np.deg2rad((y,x))
        ctht = 0.5*np.pi  - ctht
        if cph < 0:
            cph = 2.*np.pi + cph
        vertlist.append(cRot.I(hp.ang2vec(ctht,cph)))

    # Get pixels in image
    imgpix = hp.query_polygon(nside1, vertlist, inclusive=True)

    seenpix = imgpix[values[imgpix]>hp.UNSEEN] 

    #if output is fits file: clip input healpy map by setting all pixels outside the plotted area to UNSEEN,
    #then save the result. 
    if args.output and ( args.output.endswith(".fits") or args.output.endswith(".fit") ):
        pix=np.ones(npix, dtype=bool)
        pix[seenpix]=False
        values[pix]=hp.UNSEEN
        print "Saving clipped healpy map to %s" % args.output
        hp.write_map(args.output, values, partial=True, column_names=[skymapName])
        exit()

    #Get stats
    precRot = hp.Rotator(coord=coords,rot=rotVertex)

    pixMin,pixMax=seenpix[[np.argmin(values[seenpix]), np.argmax(values[seenpix])]]
    dMin,dMax = values[[pixMin,pixMax]]
    [[thMin,thMax],[phMin,phMax]] = np.rad2deg(precRot(hp.pix2ang(nside1,[pixMin,pixMax])))

    if args.coords == 'C':
        while phMin<0:
            phMin+=360
            
        while phMax<0:
            phMax+=360
    
    th0,ph0 = precRot.I((90.-yC)*degree, xC*degree)
    pix0 = hp.ang2pix(nside1, th0, ph0)
    
    if isFlux:
        if hasFluxError:
            fluxerrMin,fluxerrMax,fluxerr0 = fluxerrmap[[pixMin,pixMax,pix0]]
        else:
            fluxerrMin,fluxerrMax,fluxerr0 = [0,0,0]

        print "Flux units: TeV^-1 cm^-2 s^-1"
        print "Coord units: deg"
        print "Min:                 %11.2e +/- %11.2e (%6.2f, %5.2f)" % (dMin, fluxerrMin,
                                                                         phMin, 90-thMin)
        print "Max:                 %11.2e +/- %11.2e (%6.2f, %5.2f)" % (dMax, fluxerrMax,
                                                                         phMax, 90-thMax)
        print "Map value at origin: %11.2e +/- %11.2e" % (skymap[pix0], fluxerr0)
    
    elif isFluxError:
        print("Min:                 %5.4e (%6.2f, %5.2f)" % (dMin, phMin, 90-thMin))
        print("Max:                 %5.4e (%6.2f, %5.2f)" % (dMax, phMax, 90-thMax))
        print("Map value at origin: %5.4e" % skymap[pix0])    
    else:
        print("Min:                 %5.2f (%6.2f, %5.2f)" % (dMin, phMin, 90-thMin))
        print("Max:                 %5.2f (%6.2f, %5.2f)" % (dMax, phMax, 90-thMax))
        print("Map value at origin: %5.2f" % skymap[pix0])
        
    if args.onlystats:
        return 0
    
    figsize = (fysize*faspect+2, fysize+2.75)
    fig   = plt.figure(num=1, figsize=figsize)

    # Set  min/max value of map
    if args.min is not None:
        dMin = args.min
        values[(skymap<dMin) & (values > hp.UNSEEN)] = dMin
    if args.max is not None:
        dMax = args.max
        values[(skymap>dMax) & (values > hp.UNSEEN)] = dMax
    
    textcolor, colormap = MapPalette.setupDefaultColormap(args.ncolors)

    # Use the Fermi/HESS/VERITAS purply-red-yellow color map
    if args.gamma:
        textcolor, colormap = MapPalette.setupGammaColormap(args.ncolors)

    # Use the Milagro color map
    if args.milagro:
        dMin = -5
        dMax = 15
        dMin = args.min if args.min != None else -5
        dMax = args.max if args.max != None else 15
        thresh = args.threshold if args.threshold != None else 2.
        textcolor, colormap = \
            MapPalette.setupMilagroColormap(dMin, dMax, thresh, args.ncolors)
        print("Milagro", dMin, dMax, thresh)
    # Use a thresholded grayscale map with colors for extreme values
    else:
        if args.threshold != None:
            textcolor, colormap = \
                MapPalette.setupThresholdColormap(dMin, dMax, args.threshold,
                                                args.ncolors)
        elif args.abthresh != None:
            textcolor, colormap = \
                MapPalette.setupAbsThresholdColormap(dMin, dMax, args.abthresh,
                                                    args.ncolors)


    if args.interpolation:
        cRot = hp.Rotator(rot=rotMap,coord=coords)
        phi   = np.linspace(np.deg2rad(xmax), np.deg2rad(xmin), args.xsize)
        if xmin < 0 and xmax > 0 and (xmax - xmin) > 180.:
            phi   = np.linspace(np.deg2rad(xmin)+2.*np.pi,np.deg2rad(xmax), args.xsize)
            phi[(phi>2.*np.pi)] -= 2.*np.pi
        theta = 0.5*np.pi  - np.linspace(np.deg2rad(ymin), np.deg2rad(ymax),
                                        args.xsize/faspect)
        Phi, Theta = np.meshgrid(phi, theta)
        rTheta,rPhi = cRot.I(Theta.reshape(phi.size*theta.size),\
                           Phi.reshape(phi.size*theta.size))
        rotimg = hp.get_interp_val(values, rTheta.reshape(Phi.shape),\
                                           rPhi.reshape(Theta.shape))
    else:
        tfig   = plt.figure(num=2,figsize=figsize)
        rotimg = hp.cartview(values, fig=2,coord=coords,title="",\
                            cmap=colormap, cbar=False,\
                            lonra=[cxmin,cxmax],latra=[ymin,ymax],rot=rotMap,
                            notext=True, xsize=args.xsize,
                            return_projected_map=True)
        plt.close(tfig)

    
    ax = fig.add_subplot(111)
    ax.set_aspect(1.)

    # if plotting contours
    if args.contours != None:
        if args.contourmap:
            contourmap = args.contourmap
            contourmapcoord = args.contourmapcoord
        else:
            contourmap = fitsfile[0]
            contourmapcoord = 'C'

        contourSkyMap, contourSkyMapHeader = hp.read_map(contourmap, h=True)
        fnside1 = hp.get_nside(contourSkyMap)
        ftoFind = 'TTYPE' + str(1)
        contourSkyMapName = dict(contourSkyMapHeader)[ftoFind]
#<<<<<<< .mine
#        fvalues = contourSkyMap #np.ma.masked_where(contourSkyMap == 0, contourSkyMap)
#=======
#>>>>>>> .r38901
        if args.interpolation:
            cRot = hp.Rotator(rot=rotMap,coord=[contourmapcoord,coords[-1]])
            phi   = np.linspace(np.deg2rad(xmax), np.deg2rad(xmin), args.xsize)
            if xmin < 0 and xmax > 0 and (xmax - xmin) > 180.:
                phi   = np.linspace(np.deg2rad(xmin)+2.*np.pi,np.deg2rad(xmax), args.xsize)
                phi[(phi>2.*np.pi)] -= 2.*np.pi
            theta = 0.5*np.pi - np.linspace(np.deg2rad(ymin), np.deg2rad(ymax),
                                            args.xsize/faspect)
            Phi, Theta = np.meshgrid(phi, theta)
            rTheta,rPhi = cRot.I(Theta.reshape(phi.size*theta.size),\
                                Phi.reshape(phi.size*theta.size))
            frotimg = hp.get_interp_val(contourSkyMap,rTheta.reshape(Phi.shape),\
                                        rPhi.reshape(Theta.shape))
        else:
            tfig   = plt.figure(num=3,figsize=figsize)
            frotimg = hp.cartview(contourSkyMap,
                                  fig=3,
                                  coord=[contourmapcoord, coords[-1]],
                                  title="",
                                  cmap=colormap,
                                  cbar=False,
                                  lonra=[xmin,xmax],
                                  latra=[ymin,ymax],
                                  rot=rotMap,
                                  notext=True,
                                  xsize=1000,
                                  min=dMin, max=dMax,
                                  return_projected_map=True)
            plt.close(tfig)

        if args.contours == []:
            rMaxSig2 = (contourSkyMap[imgpix].max())**2
            if rMaxSig2 < 11.83:
                print "No spot detected with at least a 3sigma confidence contour"
                contours = [-float("inf")]
            else:
                contours = [sqrt(rMaxSig2-2.30) , sqrt(rMaxSig2-6.18) , sqrt(rMaxSig2-11.83)]
        else:
            contours = args.contours

        ccolor = args.contourscolor or 'g'
        cstyle = args.contoursstyle or '-'
        cwidth = args.contourswidth or 2.
        print 'Contours style: ',ccolor, cstyle, cwidth

        contp = ax.contour(frotimg,
                            levels=np.sort(contours),
                            colors=ccolor,
                            linestyles = cstyle,
                            linewidths = cwidth,
                            origin='upper',
                            extent=[cxmax, cxmin, ymax, ymin])
    
    rotimg[(rotimg>dMax)] = dMax
    rotimg[(rotimg<dMin)] = dMin
    if not args.nofill:
        imgp = ax.imshow(rotimg,extent=[cxmax, cxmin, ymax, ymin],\
                         vmin=dMin,vmax=dMax,cmap=colormap)

        imgp.get_cmap().set_under('w',alpha=0.)
        imgp.get_cmap().set_over('w',alpha=0.)


    # Draw grid lines
    xts = np.arange(np.floor(xmin), np.ceil(xmax+args.dmer), args.dmer)[1:-1]
    xtlbs = [ '%g'%(xt+360) if args.coords=='C' and xt<0 else '%g'%xt for xt in xts]
    if xmin < 0. and xmax > 0. and (xmax-xmin) > 180.:
        xts = np.arange(np.floor(cxmin), np.ceil(cxmax+args.dmer), args.dmer)[1:-1]
        xtlbs = [   ]
        for xt in xts:
            cval = xt - 180.
            if xt < 0:
                cval = 180.+xt
            if cval == -180:
                cval = 180
            if args.isMoon or args.isSun:
                cval -= 180.
                if cval < -180.:
                    cval+=360.
            elif args.coords=='C' and cval<0:
                cval+=360
            
            xtlbs.append('%g'%(cval))
    yts = np.arange(np.floor(ymin), np.ceil(ymax+args.dpar), args.dpar)[1:-1]

    if args.nogrid == False:
        ax.grid(color=textcolor)
        ax.xaxis.set_ticks(xts)
        ax.xaxis.set_ticklabels(xtlbs)
        ax.yaxis.set_ticks(yts)

    if args.preliminary:
        plt.text((xmin+xmax)/2., ymin+0.85*(ymax-ymin),
                 "PRELIMINARY",
                 color=textcolor,
                 alpha=0.8,
                 fontdict={"family":"sans-serif", "weight":"bold", "size":28},
                 horizontalalignment='center')

    # If TeVCat data are available, plot them
    if args.tevcat or args.tevcatLabels:
        if haveTeVCat:
            try:
                if args.tevcat:
                    tevcat = TeVCat.TeVCat(args.tevcat)
                elif args.tevcatLabels:
                    tevcat = TeVCat.TeVCat(args.tevcatLabels)
            except IOError as e:
                print(e)
                print("Downloading data from tevcat.uchicago.edu")
                tevcat = TeVCat.TeVCat()
            except:
                print("Why caught here?")
                print("Downloading data from tevcat.uchicago.edu")
                tevcat = TeVCat.TeVCat()

            cRot = hp.Rotator(coord=["C",coords[-1]])
            tnside=512
            fpix = np.zeros(hp.nside2npix(tnside))
            for cId in (1,2):
                catalog = tevcat.GetCatalog(cId)
                ra = catalog.GetRA()
                dec = catalog.GetDec()
                assoc = catalog.GetCanonicalName()
                cut = (assoc != 'Crab Pulsar')
                ra = ra[cut]
                dec = dec[cut]
                assoc = assoc[cut]
                cpix = hp.ang2pix(tnside,np.pi*0.5 - dec,ra)
                slpx = []
                for sx,px in enumerate(cpix):
                    fpix[px] += 1
                    if fpix[px] != 1:
                        print("%s is a duplicate" % (assoc[sx]))
                        slpx.append(sx)
                
                ra = np.delete(ra,slpx)
                dec = np.delete(dec,slpx)
                assoc = np.delete(assoc,slpx)
                y, x = cRot(np.pi*0.5 - dec,ra)
                x = np.rad2deg(x) + frot
                x[(x>180.)] -= 360.
                y = 90. - np.rad2deg(y)
                cut = (x > xmin) & (x < xmax) & (y>ymin) & (y<ymax)
                x = x[cut]
                y = y[cut]
                assoc = assoc[cut]
                ax.scatter(x,y, color=textcolor, facecolors="none", marker="s")

                if args.tevcatLabels:
                    for r, d, s in zip(x, y, assoc):
                        print(r, d, s)
                        ax.text(r,d, s+'   .', color=textcolor,
                                rotation=args.catLabelsAngle,
                                fontdict={'family': 'sans-serif',
                                          'size': args.catLabelsSize,
                                          'weight': 'bold'})
        else:
            print("Sorry, TeVCat could not be loaded.")

    # If Fermi data are available, plot them
    if args.fermicat:
        if haveFermi:
            fcat = None
            try:
                fcat = FGLCatalog.FGLCatalog(args.fermicat)
                aflag = fcat.GetAnalysisFlags()
                acut = (aflag == 0)                     # cut analysis errors

                flux = fcat.GetFlux1000()               # 1-100 GeV flux
                dflx = fcat.GetFlux1000Error()          # flux uncertainty
                fcut = dflx/flux < 0.5                  # cut poorly measured srcs

                cuts = np.logical_and(acut, fcut)
                print('Using FGL')
            except:
                try:
                    fcat = FHLCatalog.FHLCatalog(args.fermicat)
                    cuts = (fcat.GetFlux() > 0.)        # Dummy cut
                    print('Using FHL')
                except:
                    print('Fermi catalog could not be loaded!')
        if fcat != None:
            # Don't show TeV associations if plotting from TeVCat
            if args.tevcat or args.tevcatLabels:
                tcfg = fcat.GetTeVCatFlag()
                tcut = (tcfg == "N") | (tcfg == "C")
                cuts = np.logical_and(cuts, tcut)
            ra = fcat.GetRA()[cuts]
            dec = fcat.GetDec()[cuts]
            assoc = fcat.GetSourceName()[cuts]
            catnms = fcat.GetCatalogName()[cuts]
            for i in xrange(len(assoc)):
                if assoc[i] == '':
                    assoc[i] = catnms[i]

            cRot = hp.Rotator(coord=["C",coords[-1]])
            y, x = cRot(np.pi*0.5 - dec,ra)
            x = np.rad2deg(x) + frot
            x[(x>180.)] -= 360.
            y = 90. - np.rad2deg(y)
            cut = (x > xmin) & (x < xmax) & (y>ymin) & (y<ymax)
            x = x[cut]
            y = y[cut]
            assoc = assoc[cut]
            ax.scatter(x,y, color=textcolor, facecolors="none", marker="o")

            if args.fermicatLabels:
                for r, d, s in zip(x, y, assoc):
                    ax.text(r, d, s, color=textcolor,
                            rotation=args.catLabelsAngle,
                            fontdict={'family': 'sans-serif',
                                      'size': args.catLabelsSize,
                                      'weight': 'bold'})
        else:
            print("Sorry, the Fermi xFGL catalog could not be loaded.")

    # If a hotspot list is given, plot it
    if args.hotspots:
        fhot = open(args.hotspots, "r")
        ra = []
        dec = []
        assoc = []
        for line in fhot:
            if line.startswith("#"):
                continue
            larr = line.strip().split()
            ra.append(float(larr[1]))
            dec.append(float(larr[2]))
            assoc.append(larr[4])

        ra = np.deg2rad(ra)
        dec = np.deg2rad(dec)
        assoc = np.array(assoc)
        cRot = hp.Rotator(coord=["C",coords[-1]])
        y, x = cRot(np.pi*0.5 - dec,ra)
        x = np.rad2deg(x) + frot
        x[(x>180.)] -= 360.
        y = 90. - np.rad2deg(y)
        cut = (x > xmin) & (x < xmax) & (y>ymin) & (y<ymax)
        x = x[cut]
        y = y[cut]
        assoc = assoc[cut]
        ax.scatter(x,y, color=textcolor, facecolors="none", marker="o")
        if args.hotspotLabels:
            for r, d, s in zip(x, y, assoc):
                print(r, d, s)
                ax.text(r,d, s+'   .',
                        color=textcolor,
                        rotation=90,
                        fontdict={'family': 'sans-serif',
                                  'size': 8,
                                  'weight': 'bold'})

    # If a gaussian fit file is given, plot it
    if args.gaussfit:
        gfit = open(args.gaussfit, "r")
        gfit.next()
        gfit.next()
        ra,raErr     = [float(i) for i in gfit.next().strip().split()]
        dec,decErr   = [float(i) for i in gfit.next().strip().split()]
        raW,raWErr   = [float(i) for i in gfit.next().strip().split()]
        decW,decWErr = [float(i) for i in gfit.next().strip().split()]
        gfit.close()

        mrot = -180. if args.isMoon or args.isSun else 0.
      
        cRot = hp.Rotator(coord=["C",coords[-1]])
        y, x = cRot(np.pi*0.5 - np.deg2rad(dec),np.deg2rad(ra+mrot))
        x = np.rad2deg(x) + frot
        x = x-360. if x>180. else x
        y = 90. - np.rad2deg(y)
        ellip0  = Ellipse((x,y), width=2*raW, height=2*decW, edgecolor='black',facecolor='None')
        ax.add_patch(ellip0)
        ax.scatter(x,y, s=20, color='black',facecolors="black",marker="o")

        print(x, y, xmin, xmax, ymin, ymax)
        ax.text(x-1, y+1, 
              "%s=%.02f$\pm$%.02f\n$\Delta\delta$=%.02f$\pm$%.02f\n%s=%.02f$\pm$%.02f\n$\sigma_\delta$=%.02f$\pm$%.02f"
                  % (r"$\Delta\alpha$",ra,raErr,dec,decErr,r"$\sigma_\alpha$",raW,raWErr,decW,decWErr),
              color=textcolor,
              rotation=0,
              fontdict={"size":12})



    # Set up the color bar
    # Setup color tick marks
    if args.colorbar:
        cticks = args.ticks
        if args.ticks == None:
            if (dMax-dMin) > 3:
                clrmin = np.floor(dMin)
                clrmax = np.ceil(dMax)
                ctspc = np.round((clrmax - clrmin)/10.)
                if ctspc == 0.:
                    ctspc = 1.
                if clrmin < 0 and clrmax > 0:
                    cticks = -np.arange(0,-clrmin,ctspc)
                    cticks = np.sort(np.unique(np.append(cticks,np.arange(0,clrmax,ctspc))))
                else:
                    cticks = np.arange(clrmin,clrmax+ctspc,ctspc)
            else:
                cticks = None

        cb = fig.colorbar(imgp, orientation="horizontal",
                          shrink=0.85,
                          fraction=0.1,
                          #aspect=25,
                          pad=0.1,
                          ax=ax,
                          ticks=cticks)

    if args.label:
        skymapName = args.label
    else:
        if re.match("significance", skymapName):
            skymapName=r"significance [$\sigma$]"
    if args.gumbel:
        skymapName="log10(P-Value)"

    if args.colorbar:
        cb.set_label(skymapName)

    xlabel = r"$\alpha$ [$^\circ$]"
    ylabel = r"$\delta$ [$^\circ$]"


    # Set up the color bar and tick axis
    if args.coords == "G":
        xlabel = r"$l$ [$^\circ$]"
        ylabel = r"$b$ [$^\circ$]"
    if args.isMoon or args.isSun:
        xlabel = r"$\Delta\alpha$"
        ylabel = r"$\Delta\delta$"

    # X axis label
    ax.set_xlabel(xlabel, color='k')
    # Y axis label
    ax.set_ylabel(ylabel, color='k')

    # Title
    if args.title != None:
        ax.set_title(r"{0}".format(args.title.replace("\\n","\n")),color='k')

    if args.cross:
        # Indicate the center of the plot with a thick black cross
        ax.scatter(xC, yC, s=20**2,marker="+", facecolor="#000000", color="#000000")

    ax.set_ylim(ymin,ymax)
    ax.set_xlim(cxmax,cxmin)
    if args.squareaspect:
        plt.axes().set_aspect(1./cos((ymax+ymin)/2*pi/180))

    # Either output the image to a file and quit or plot it in a window
    if args.output:
        if not args.nofill:
            fig.savefig(args.output, dpi=300)
        else:
            fig.savefig(args.output, dpi=300, transparent=True)
    else:
        plt.show()

if __name__ == "__main__":
    main()
