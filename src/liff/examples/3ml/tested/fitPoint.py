import warnings

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

# This disable momentarily the printing of warnings, which you might get
# if you don't have the Fermi ST or other software installed
with warnings.catch_warnings():

    warnings.simplefilter("ignore")

    from threeML import *

# Make sure that the HAWC plugin is available

assert is_plugin_available("HAWCLike"), "HAWCLike is not available. Check your configuration"

def go(args):

    spectrum = Powerlaw()
    source   = PointSource("TestSource",
                            args.RA,
                            args.Dec,
                            spectral_shape=spectrum)

    fluxUnit = 1. / (u.TeV * u.cm**2 * u.s)

    # Set spectral parameters (do it after the source definition to make sure
    # the units are handled correctly)
    spectrum.K = 1e-14 * fluxUnit
    spectrum.K.bounds = (1e-16 * fluxUnit, 1e-12*fluxUnit)

    spectrum.piv = 10 * u.TeV
    spectrum.piv.fix = True

    spectrum.index = -2.5
    spectrum.index.bounds = (-4., -1.)

    # Set up a likelihood model using the source
    lm = Model(source)
    llh = HAWCLike("TheLikelikood", args.maptreefile, args.responsefile)
    llh.set_active_measurements(args.startBin, args.stopBin)

    # Double check the free parameters
    print("Likelihood model:\n")
    print(lm)

    # Set up the likelihood and run the fit
    print("Performing likelihood fit...\n")
    datalist = DataList(llh)
    jl = JointLikelihood(lm, datalist, verbose=True)
    
    try:
        jl.set_minimizer("ROOT")
        jl.fit()
    except AttributeError:
        jl.set_minimizer("minuit")
        result = jl.fit()

    # Print the TS, significance, and fit parameters, and then plot stuff
    print("\nTest statistic:")
    TS = llh.calc_TS()

    print("Test statistic: %g" % TS)

    if args.outmodel is not None:
        llh.write_model_map(args.outmodel)

if __name__=="__main__":

    import argparse

    p = argparse.ArgumentParser(description="Example spectral fit with LiFF")
    p.add_argument("-m", "--maptreefile",
                   help="LiFF MapTree ROOT file", required=True)
    p.add_argument("-r", "--responsefile",
                   help="LiFF detector response ROOT file", required=True)
    p.add_argument("--startBin", default=1, type=int,
                   help="Starting analysis bin [0..9]")
    p.add_argument("--stopBin", default=9, type=int,
                   help="Stopping analysis bin [0..9]")
    p.add_argument("--RA", default=83.6, type=float,
                   help="Source RA in degrees")
    p.add_argument("--Dec", default=22, type=float,
                   help="Source Dec in degrees")
    p.add_argument("--outmodel", default=None, type=str,
                   help="Output model root maptree file.")
    args = p.parse_args()

    go(args)
