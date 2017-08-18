#!/usr/bin/env python

import warnings
import sys

import matplotlib as mpl
mpl.use("Agg") # To prevent crash when using corner plot on OS X

import matplotlib.pyplot as plt

import ROOT
# Prevent ROOT from stealing command line arguments... ROOT is evil.
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
    if args.shape == 'ps':
        source   = PointSource("TestSource",
                               args.RA,
                               args.Dec,
                               spectral_shape=spectrum)
    elif args.shape == 'disk':
        shape    = Disk_on_sphere()
        source   = ExtendedSource("TestSource",
                                  spatial_shape=shape,
                                  spectral_shape=spectrum)
        shape.lon0 = args.RA * u.degree
        shape.lon0.fix = True
        shape.lat0 = args.Dec * u.degree
        shape.lat0.fix = True
        shape.radius = 1. * u.degree
        shape.radius.bounds = (0.05 * u.degree, 1.5 * u.degree)
    elif args.shape == 'gauss':
        shape    = Gaussian_on_sphere()
        source   = ExtendedSource("TestSource",
                                  spatial_shape=shape,
                                  spectral_shape=spectrum)
        shape.lon0 = args.RA * u.degree
        shape.lon0.fix = True
        shape.lat0 = args.Dec * u.degree
        shape.lat0.fix = True
        shape.sigma = 1.0 * u.degree
        shape.sigma.fix = False
        shape.sigma.max_value = 5.
    elif args.shape == 'diffusion':
        shape    = Continuous_injection_diffusion()
        source   = ExtendedSource("TestSource",
                                  spatial_shape=shape,
                                  spectral_shape=spectrum)
        shape.lon0 = args.RA * u.degree
        shape.lon0.fix = True
        shape.lat0 = args.Dec * u.degree
        shape.lat0.fix = True
        shape.rdiff0 = 6.0 * u.degree
        shape.rdiff0.fix = False
        shape.rdiff0.max_value = 12.
        shape.delta = 0.33
        shape.delta.fix = True
        shape.uratio = 0.5 # Diffusion spectral index (0.3 to 0.6)
        shape.uratio.fix = True
        shape.piv = 2e10
        shape.piv.fix = True
        shape.piv2 = 1e9
        shape.piv2.fix = True
    elif args.shape == 'powerlaw':
        shape = Power_law_on_sphere()
        source   = ExtendedSource("TestSource",
                                  spatial_shape=shape,
                                  spectral_shape=spectrum)
        shape.lon0 = args.RA * u.degree
        shape.lon0.fix = True
        shape.lat0 = args.Dec * u.degree
        shape.lat0.fix = True
        shape.index = -1.1
        shape.index.fix = False
    else:
        print 'Error, invalid shape:', args.shape
        sys.exit(0)

    fluxUnit = 1. / (u.TeV * u.cm**2 * u.s)

    # Set spectral parameters (do it after the source definition to make sure
    # the units are handled correctly)
    spectrum.K = 1e-14 * fluxUnit
    spectrum.K.bounds = (1e-16 * fluxUnit, 1e-12*fluxUnit) # Factor 1e9 to what's output

    spectrum.piv = 10 * u.TeV
    spectrum.piv.fix = True

    spectrum.index = -2.5
    spectrum.index.bounds = (-4., -1.)
    spectrum.index.fix = (not args.freeindex)

    spectrum.display()
    if args.shape == 'ps':
        print("Morphology: point source.")
    else:
        shape.display()

    # Set up a likelihood model using the source
    lm = Model(source)
    llh = HAWCLike("TheLikelikood", args.maptreefile, args.responsefile)
    llh.set_active_measurements(args.startBin, args.stopBin)

    llh.set_ROI(args.RA, args.Dec, args.roiradius, True)

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

    # Print the TS
    TS = llh.calc_TS()
    print("\nTest statistic: %g" % TS)

    # Write model and residual maps if needed
    if args.outmodel is not None:
        llh.write_model_map(args.outmodel)
    if args.outresidual is not None:
        llh.write_residual_map(args.outresidual)

    # Get a flux uncertainty envelope from MCMC samples and plot it
    if args.nMCMC > 0 and len(lm.free_parameters)>1:

        print("Running MCMC...")

        # The set_uninformative_prior will use the given prior type
        # and the current minimum and maximum to set the prior
        # Set all priors to uniform, except the spectrum.K
        for parameter_name, parameter in lm.free_parameters.iteritems():
            parameter.set_uninformative_prior(Uniform_prior)
        spectrum.K.set_uninformative_prior(Log_uniform_prior)

        ba = BayesianAnalysis(lm, datalist)

        nW = 10  # number of MCMC walkers
        nB = 100  # number of samples for burning in
        nS = args.nMCMC  # number of samples

        print("Sampling")
        samples = ba.sample(nW, nB, nS)
        print("Sampling complete")

        print("\nMaking corner plot...\n")
        # Make a corner plot of the sampled parameters from the MCMC
        sigmas = [1., 2., 3.]
        levels = [1. - np.exp(-0.5 * s ** 2) for s in sigmas]

        fig = ba.corner_plot(show_titles=True,
                             title_kwargs={"fontsize": 10},
                             quantiles=[0.16, 0.5, 0.84],
                             levels=levels,
                             verbose=True
                            )
        filename = "%s/corner_%s.png" %(args.outplotdir, args.outplottag)
        fig.savefig(filename,dpi=200)
        print("Save corner plot in: %s" %filename)


        print("\nMaking spectrum plot...\n")
        E = np.logspace(np.log10(1), np.log10(50), 100) * u.TeV
        fML = spectrum(E)

        # Construct the uncertainty envelope on the flux using the sampled
        # parameter space
        fMin68, fMax68 = [], []
        fMin95, fMax95 = [], []

        # Just to be fancy, let's use the progress bar
        from threeML.io.progress_bar import progress_bar

        with progress_bar(E.shape[0], title='Plotting') as progress:

            for i, energy in enumerate(E):

                progress.animate(i)

                flux = []

                for rsamp in ba.raw_samples:

                    # Dirty, but it looks like the morphology parameter come
                    # first. Is there a safe way to do this?
                    spectrum.K = rsamp[-2]
                    spectrum.index = rsamp[-1]

                    # Note that here energy has units, so the computation
                    # would be made with units and the output will be a
                    # astropy.Quantity instance. However that is slow,
                    # so let's use the value instead (the computation without
                    # unit is much faster)

                    this_diff_flux = spectrum(energy.to('keV').value)

                    flux.append( this_diff_flux )

                    # Evaluate the percentiles of the flux
                    f02, f16, f84, f97 = np.percentile(np.asarray(flux),
                                               [2.5, 16, 84, 97.5])

                fMin68.append(f16)
                fMax68.append(f84)

                fMin95.append(f02)
                fMax95.append(f97)

        print(" done")

        # Save the percentiles with units
        # y_unit is the unit of the output if the function is 1d,
        # as in y = f(x)

        fMin68 = np.asarray(fMin68) * spectrum.y_unit
        fMax68 = np.asarray(fMax68) * spectrum.y_unit

        fMin95 = np.asarray(fMin95) * spectrum.y_unit
        fMax95 = np.asarray(fMax95) * spectrum.y_unit

        fig, ax = plt.subplots(1, 1, figsize=(8, 6))

        energyPower = 2.0 # To plot E**eP dN/dE
        conv = (E)**energyPower

        if fMin95 is not None and fMax95 is not None:
            # Plot the 95% C.I. on the flux as a function of energy
            ax.fill_between(E.to(u.TeV).value,
                            (conv * fMin95).to(u.TeV**energyPower / (u.TeV*u.cm**2*u.s)).value,
                            (conv * fMax95).to(u.TeV**energyPower / (u.TeV*u.cm**2*u.s)).value,
                            color="royalblue",
                            alpha=0.3,
                            label="95% C.I.")

        if fMin68 is not None and fMax68 is not None:
            # Plot the 68% C.I. on the flux as a function of energy
            ax.fill_between(E.to(u.TeV).value,
                            (conv * fMin68).to(u.TeV**energyPower / (u.TeV*u.cm**2*u.s)).value,
                            (conv * fMax68).to(u.TeV**energyPower / (u.TeV*u.cm**2*u.s)).value,
                            color="royalblue",
                            alpha=0.5,
                            label="68% C.I.")

        ax.plot(E.to(u.TeV).value,
                (conv * fML).to(u.TeV**energyPower / (u.TeV*u.cm**2*u.s)).value,
                "r--", lw=2, label="Max LL")

        ax.set(xscale="log",
               xlabel="energy [TeV]",
               yscale="log",
               ylabel=r"$E^{%.1f}$ flux [TeV$^{%.1f}$ cm$^{-2}$ s$^{-1}$]" %(energyPower, energyPower-1),
               title="Spectrum for %s shape" %args.shape
               )
        ax.xaxis.set_major_formatter(mpl.ticker.FormatStrFormatter("%g"))

        h, l = ax.get_legend_handles_labels()
        leg = ax.legend(h, l)

        fig.tight_layout()
        filename = "%s/spectrum_%s.png" %(args.outplotdir, args.outplottag)
        fig.savefig(filename,dpi=200)
        print("Save spectrum plot in: %s" %filename)

        plt.show()



if __name__=="__main__":

    import argparse

    p = argparse.ArgumentParser(description="Example spectral fit with LiFF, "
                                "multiple source shape options.")
    p.add_argument("-m", "--maptreefile",
                   help="LiFF MapTree ROOT file.", required=True)
    p.add_argument("-r", "--responsefile",
                   help="LiFF detector response ROOT file.", required=True)
    p.add_argument("--startBin", default=1, type=int,
                   help="Starting analysis bin [0..9].")
    p.add_argument("--stopBin", default=9, type=int,
                   help="Stopping analysis bin [0..9].")
    p.add_argument("--RA", default=287.05, type=float,
                   help="Source RA in degrees.")
    p.add_argument("--Dec", default=6.39, type=float,
                   help="Source Dec in degrees.")
    p.add_argument("--roiradius", default=2.0, type=float,
                   help="ROI radius, around (RA, Dec), in degrees.")
    p.add_argument('--shape', default='ps', type=str,
                   help='Source shape, can be: ps, disk, gauss, diffusion.')
    p.add_argument('--freeindex', action='store_true',
                   help='Fit the spectral index')
    p.add_argument("--outmodel", default=None, type=str,
                   help="Output model root maptree file.")
    p.add_argument("--outresidual", default=None, type=str,
                   help="Output residual root maptree file.")
    p.add_argument("--nMCMC", default=-1, type=int,
                   help="Number of MCMC samples to obtain (<=0 to disable).")
    p.add_argument("--outplotdir", default=".", type=str,
                   help="Output directory for MCMC plots.")
    p.add_argument("--outplottag", default="plottag", type=str,
                   help="Suffix for MCMC plots.")
    args = p.parse_args()

    go(args)
