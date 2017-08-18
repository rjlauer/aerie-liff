# Custom color palettes for use with HEALPix maps

__version__ = "$Id: MapPalette.py 26370 2015-07-16 13:37:17Z bbaugh $"

try:
    import matplotlib as mpl
except ImportError,e:
    print e
    raise SystemExit

def setupDefaultColormap(ncolors):
    """Create a color map based on the standard blue-green-red "jet" palette.

        Args:
            ncolors: number of colors in the palette
        Returns:
            textcolor: color for text and annotation in this map
            newcm: newly defined color palette
    """
    print "ncolors =", ncolors 

    newcm = mpl.colors.LinearSegmentedColormap("jet",
                                               mpl.cm.jet._segmentdata,
                                               ncolors)
    newcm.set_over(newcm(1.0))
    newcm.set_under("w")
    newcm.set_bad("gray")
    textcolor = "#000000"

    return textcolor, newcm

def setupThresholdColormap(amin, amax, threshold, ncolors):
    """ Create a color map that draws all values below the threshold in
        grayscale, and everything above in the usual "jet" rainbow RGB scale.

        Args:
            amin: minimum value in color scale
            amax: maximum value in color scale
            threshold: step between grayscale and full color scale
            ncolors: number of colors in the palette
        Returns:
            textcolor: color for text and annotation in this map
            newcm: newly defined color palette
    """
    thresh = (threshold - amin) / (amax - amin)
    if threshold <= amin or threshold >= amax:
        thresh = 0.
    dthresh = 1 - thresh
    threshDict = { "blue"  : ((0.0, 1.0, 1.0),
                              (thresh, 0.6, 0.5),
                              (thresh+0.11*dthresh,  1, 1),
                              (thresh+0.34*dthresh, 1, 1),
                              (thresh+0.65*dthresh, 0, 0),
                              (1, 0, 0)),
                   "green" : ((0.0, 1.0, 1.0),
                              (thresh, 0.6, 0.0),
                              (thresh+0.09*dthresh, 0, 0),
                              (thresh+0.36*dthresh, 1, 1),
                              (thresh+0.625*dthresh, 1, 1),
                              (thresh+0.875*dthresh, 0, 0),
                              (1, 0, 0)),
                   "red"   : ((0.0, 1.0, 1.0),
                              (thresh, 0.6, 0.0),
                              (thresh+0.35*dthresh, 0, 0),
                              (thresh+0.66*dthresh, 1, 1),
                              (thresh+0.89*dthresh, 1, 1),
                              (1, 0.5, 0.5)) }
    if threshold < 0.:
        thresh = (threshold-amin)/(amax-amin)
        if threshold <= amin or threshold >= amax:
            thresh = 0.
        threshDict = { "blue"  : (
                              (0.0, .5, .5),
                              (0.11*thresh, 1, 1),
                              (0.34*thresh,  1, 1),
                              (0.65*thresh, 0, 0),
                              (thresh, 0, .6),
                              (1, 1, 0)
                              ),
                   "green" : ((0.0, 0.0, 0.0),
                              (0.09*thresh, 0, 0),
                              (0.36*thresh, 1, 1),
                              (0.625*thresh, 1, 1),
                              (0.875*thresh, 0, 0),
                              (thresh, 0, .6),
                              (1, 1, 0)),
                   "red"   : ((0.0, 0.0, 0.0),
                              (0.35*thresh, 0, 0),
                              (0.66*thresh, 1, 1),
                              (0.89*thresh, 1, 1),
                              (thresh, 0.5, .6),
                              (1, 1, 0)) }


    newcm = mpl.colors.LinearSegmentedColormap("thresholdColormap",
                                               threshDict,
                                               ncolors)
    newcm.set_over(newcm(1.0))
    newcm.set_under("w")
    newcm.set_bad("gray")
    textcolor = "#000000"

    return textcolor, newcm

def setupAbsThresholdColormap(amin, amax, threshold, ncolors):
    """ Create a color map for "two-sided" thresholds.  Below the threshold,
        the map is a cool green-blue palette.  Between the lower and upper
        threshold, the map is gray-white-gray.  Above the upper threshold,
        the map is a warm red-yellow palette.

        Args:
            amin: minimum value in color scale
            amax: maximum value in color scale
            threshold: absolute value of step between grayscale and color scales
            ncolors: number of colors in the palette
        Returns:
            textcolor: color for text and annotation in this map
            newcm: newly defined color palette
    """
    x1 = (-threshold - amin) / (amax - amin)
    x3 = (amax - threshold) / (amax - amin)
    x2 = 1. - x1 - x3
    gvl = 0.5
    threshDict = {
        "red"    : ((0.0, 1.0, 0.5), (x1, 0.0, gvl), (x1 + 0.5*x2, 1.0, 1.0),
                    (x1 + x2, gvl, 0.7), (1.0, 1.0, 1.0)),
        "green"  : ((0.0, 1.0, 1.0), (x1, 0.0, gvl), (x1 + 0.5*x2, 1.0, 1.0),
                    (x1 + x2, gvl, 0.0), (1.0, 1.0, 1.0)),
        "blue"   : ((0.0, 1.0, 1.0), (x1, 0.7, gvl), (x1 + 0.5*x2, 1.0, 1.0),
                    (x1 + x2, gvl, 0.0), (1.0, 0.5, 1.0)) }

    newcm = mpl.colors.LinearSegmentedColormap("thresholdColormap",
                                               threshDict,
                                               ncolors)
    newcm.set_over(newcm(1.0))
    newcm.set_under("w")
    newcm.set_bad("gray")
    textcolor = "#000000"

    return textcolor, newcm

def setupMilagroColormap(amin, amax, threshold, ncolors):
    """ Create a color map that draws all values below the threshold in
        grayscale, and everything above in the Milagro-style
        red-yellow-green-blue-black palette.

        Args:
            amin: minimum value in color scale
            amax: maximum value in color scale
            threshold: step between grayscale and color scales
            ncolors: number of colors in the palette
        Returns:
            textcolor: color for text and annotation in this map
            newcm: newly defined color palette
    """
    thresh = (threshold - amin) / (amax - amin)
    if threshold <= amin or threshold >= amax:
        thresh = 0.
    dthresh = 1 - thresh
    threshDict = { "blue"  : ((0.0, 1.0, 1.0),
                              (thresh, 0.5, 0.5),
                              (thresh+0.077*dthresh, 0, 0),
                              (thresh+0.462*dthresh, 0, 0),
                              (thresh+0.615*dthresh, 1, 1),
                              (thresh+0.692*dthresh, 1, 1),
                              (thresh+0.769*dthresh, 0.6, 0.6),
                              (thresh+0.846*dthresh, 0.5, 0.5),
                              (thresh+0.923*dthresh, 0.1, 0.1),
                              (1, 0, 0)),
                   "green" : ((0.0, 1.0, 1.0),
                              (thresh, 0.5, 0.5),
                              (thresh+0.077*dthresh, 0, 0),
                              (thresh+0.231*dthresh, 0, 0),
                              (thresh+0.308*dthresh, 1, 1),
                              (thresh+0.385*dthresh, 0.8, 0.8),
                              (thresh+0.462*dthresh, 1, 1),
                              (thresh+0.615*dthresh, 0.8, 0.8),
                              (thresh+0.692*dthresh, 0, 0),
                              (thresh+0.846*dthresh, 0, 0),
                              (thresh+0.923*dthresh, 0.1, 0.1),
                              (1, 0, 0)),
                   "red"   : ((0.0, 1.0, 1.0),
                              (thresh, 0.5, 0.5),
                              (thresh+0.077*dthresh, 0.5, 0.5),
                              (thresh+0.231*dthresh, 1, 1),
                              (thresh+0.385*dthresh, 1, 1),
                              (thresh+0.462*dthresh, 0, 0),
                              (thresh+0.692*dthresh, 0, 0),
                              (thresh+0.769*dthresh, 0.6, 0.6),
                              (thresh+0.846*dthresh, 0.5, 0.5),
                              (thresh+0.923*dthresh, 0.1, 0.1),
                              (1, 0, 0)) }

    newcm = mpl.colors.LinearSegmentedColormap("thresholdColormap",
                                               threshDict,
                                               ncolors)
    newcm.set_over(newcm(1.0))
    newcm.set_under("w")
    newcm.set_bad("gray")
    textcolor = "#000000"

    return textcolor, newcm

def setupGammaColormap(ncolors):
    """Blue/purple/red/yellow color map used by Fermi, HESS, VERITAS, etc.

        Args:
            ncolors: number of colors in the palette
        Returns:
            textcolor: color for text and annotation in this map
            newcm: newly defined color palette
    """
    cdict = {
        "red"   : [(0.00,    0,    0),
                   (0.23,    0,    0),
                   (0.47,  0.3,  0.3),
                   (0.70,    1,    1),
                   (0.94,    1,    1),
                   (1.00,    1,    1)],
        "green" : [(0.00,    0,    0),
                   (0.23,    0,    0),
                   (0.47,  0.1,  0.1),
                   (0.70,    0,    0),
                   (0.94,    1,    1),
                   (1.00,    1,    1)],
        "blue"  : [(0.00, 0.25, 0.25),
                   (0.23, 0.55, 0.55),
                   (0.47, 0.85, 0.85),
                   (0.70,    0,    0),
                   (0.94,    0,    0),
                   (1.00,0.875,0.875)]
    }

    newcm = mpl.colors.LinearSegmentedColormap("gammaColorMap", cdict, ncolors)
    newcm.set_over(newcm(1.0))
    newcm.set_under("w")
    newcm.set_bad("gray")
    textcolor = "#00FF00"

    return textcolor, newcm

