#!/usr/bin/env python

################################################################################
# Demonstrate how to perform a maximum likelihood estimate of the normalization
# of the flux from the Crab Nebula with LiFF + 3ML.
# ------------------------------------------------------------------------------
# The fit assumes a log parabolic spectrum with three parameters:
#   - normalization log(A), in units of 1/(keV cm2 s)
#   - spectral index gamma
#   - curvature parameter beta
#
# Requires that 3ML be installed (https://threeml.stanford.edu/). The AERIE
# environment must also be loaded.
# ------------------------------------------------------------------------------
# $Id: testCrab3ML.py 39428 2017-06-01 19:44:19Z criviere $
################################################################################

import numpy as np
import matplotlib as mpl

mpl.use("Agg")

import matplotlib.pyplot as plt

mpl.rc("font", family="serif", size=14)

import warnings

try:
    import ROOT
    ROOT.PyConfig.IgnoreCommandLineOptions = True

except:
    
    pass

# This disable momentarily the printing of warnings, which you might get
# if you don't have the Fermi ST or other software installed
with warnings.catch_warnings():

    warnings.simplefilter("ignore")
    
    from threeML import *

# Make sure that the HAWC plugin is available

assert is_plugin_available("HAWCLike"), "HAWCLike is not available. Check your configuration"

def go(args):
    
    # Define the spectral and spatial models for the source
    spectrum = Log_parabola()
    
    source = PointSource("CrabNebula", ra=args.RA, dec=args.Dec, spectral_shape=spectrum)
    
    # NOTE: if you use units, you have to set up the values for the parameters
    # AFTER you create the source, because during creation the function Log_parabola
    # gets its units
    
    spectrum.piv = 10 * u.TeV  # Pivot energy
    spectrum.piv.fix = True
    
    spectrum.K = 1e-14 / (u.TeV * u.cm**2 * u.s)  # norm (in 1/(keV cm2 s))
    spectrum.K.bounds = (1e-25, 1e-21) # without units energies are in keV
    
    spectrum.beta = 0  # log parabolic beta
    spectrum.beta.bounds = (-4., 2.)
    
    spectrum.alpha = -2.5  # log parabolic alpha (index)
    spectrum.alpha.bounds = (-4., 2.)
    
    print(source(1 * u.keV))
    
    # Set up a likelihood model using the source.
    # Then create a HAWCLike object using the model, the maptree, and detector
    # response.
    lm = Model(source)
    llh = HAWCLike("CrabNebula", args.maptreefile, args.responsefile)
    llh.set_active_measurements(args.startBin, args.stopBin)
    
    # Double check the free parameters
    print("Likelihood model:\n")
    print(lm)
    
    # Set up the likelihood and run the fit
    print("Performing likelihood fit...\n")
    datalist = DataList(llh)
    jl = JointLikelihood(lm, datalist)
    
    # Pre-fit with root
    # The ROOT minimizer is still under dev and does
    # not provide a printer. This is the reason for the try/except
    try:
        jl.set_minimizer("ROOT")
        jl.fit()
    
    except AttributeError:
        pass
    
    jl.set_minimizer("minuit")
    
    result = jl.fit()
    
    # Print up the TS, and fit parameters, and then plot stuff
    print("\nTest statistic:")
    TS = llh.calc_TS()
    
    print("Test statistic: %g" % TS)
    
    # Get the differential flux at 1 TeV
    diff_flux = spectrum(1 * u.TeV)
    # Convert it to 1 / (TeV cm2 s)
    diff_flux_TeV = diff_flux.to(1/(u.TeV * u.cm**2 * u.s))
    
    print("Norm @ 1 TeV:  %s \n" % diff_flux_TeV)
    
    spectrum.display()
    
    E = np.logspace(np.log10(1), np.log10(100), 100) * u.TeV
            
    fML = spectrum(E)
    
    fMin68 = None
    fMax68 = None
    fMin95 = None
    fMax95 = None
    
    # Get a flux uncertainty envelope from MCMC samples
    if args.nMCMC > 0:
        
        print("Running MCMC...")
        
        # The set_uninformative_prior will use the given prior type
        # and the current minimum and maximum to set the prior
        
        spectrum.K.set_uninformative_prior(Log_uniform_prior)
        spectrum.beta.set_uninformative_prior(Uniform_prior)
        spectrum.alpha.set_uninformative_prior(Uniform_prior)
        
        ba = BayesianAnalysis(lm, datalist)
    
        nW = 10  # number of MCMC walkers
        nB = 100  # number of samples for burning in
        nS = args.nMCMC  # number of samples
    
        samples = ba.sample(nW, nB, nS)
        
        sigmas = [1., 2., 3.]
        levels = [1. - np.exp(-0.5 * s ** 2) for s in sigmas]
        
        # Make a corner plot of the sampled parameters from the MCMC
        fig = ba.corner_plot(labels=[r"$K$ [keV$^{-1}$ cm$^{-2}$ s$^{-1}$]",
                                     r"$\alpha$",
                                     r"$\beta$"],
                       #range=[[-2.5, -2.], [-0.4, 0.], [-10.6, -10.45]],
                       show_titles=True,
                       title_kwargs={"fontsize": 10},
                       quantiles=[0.16, 0.5, 0.84],
                       levels=levels,
                       verbose=True)
        fig.savefig("crab_logParabola_MCMC.png")
        
        # Construct the uncertainty envelope on the flux using the sampled
        # parameter space
        fMin68, fMax68 = [], []
        fMin95, fMax95 = [], []
        
        # Just to be fancy, let's use the progress bar
        
        print("\nMaking spectrum plot...\n")
        
        from threeML.io.progress_bar import progress_bar
        
        with progress_bar(E.shape[0], title='Plotting') as progress:

            for i, energy in enumerate(E):
            
                progress.animate(i)
            
                flux = []
            
                for (kk, a, b) in ba.raw_samples:
                
                    spectrum.K = kk
                    spectrum.alpha = a
                    spectrum.beta = b
                
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
    
        conv = (E)**2.5

        if fMin95 is not None and fMax95 is not None:
            # Plot the 95% C.I. on the flux as a function of energy
            ax.fill_between(E.to(u.TeV).value,
                            (conv * fMin95).to(u.TeV**2.5 / (u.TeV*u.cm**2*u.s)).value,
                            (conv * fMax95).to(u.TeV**2.5 / (u.TeV*u.cm**2*u.s)).value,
                            color="royalblue",
                            alpha=0.3,
                            label="95% C.I.")
    
        if fMin68 is not None and fMax68 is not None:
            # Plot the 68% C.I. on the flux as a function of energy
            ax.fill_between(E.to(u.TeV).value,
                            (conv * fMin68).to(u.TeV**2.5 / (u.TeV*u.cm**2*u.s)).value,
                            (conv * fMax68).to(u.TeV**2.5 / (u.TeV*u.cm**2*u.s)).value,
                            color="royalblue",
                            alpha=0.5,
                            label="68% C.I.")
    
        ax.plot(E.to(u.TeV).value,
                (conv * fML).to(u.TeV**2.5 / (u.TeV*u.cm**2*u.s)).value,
                "r--", lw=2, label="Max LL")

        ax.set(xscale="log",
               xlabel="energy [TeV]",
               yscale="log",
               ylabel=r"$E^{2.5}$ flux [TeV$^{1.5}$ cm$^{-2}$ s$^{-1}$]",
               ylim=[5e-12, 5e-11],
               title="Crab Nebula"
               )
        ax.xaxis.set_major_formatter(mpl.ticker.FormatStrFormatter("%g"))
    
        h, l = ax.get_legend_handles_labels()
        leg = ax.legend(h, l, loc=3)
        leg.get_frame().set_linewidth(0)
    
        fig.tight_layout()
        fig.savefig("crab_logParabola.png")
    
        plt.show()

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
    p.add_argument("--RA", default=83.63, type=float,
                   help="Source RA in degrees (Crab default)")
    p.add_argument("--Dec", default=22.01, type=float,
                   help="Source Dec in degrees (Crab default)")
    p.add_argument("--nMCMC", default=100, type=int,
                   help="Number of MCMC samples to obtain (<=0 to disable).")
    args = p.parse_args()
    
    go(args)
