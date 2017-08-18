"""Electron Inverse Compton calculations.

This module is designed to stitch together the likelihood fit in HAWC
with the calculation of gamma rays from Inverse Compton scattering by a
population of electrons. Several non-standard Python models are required
to run it:

3ML: https://github.com/giacomov/3ML

naima: http://naima.readthedocs.org/en/latest/

astropy: http://www.astropy.org/

AERIE must also be loaded in the user environment, which is usually done
by running the script:

    $ /path/to/aerie/install/bin/hawc-config --spawn-sh
"""

__version__ = "$Id"

try:
    from threeML import SpectralModel, Parameter
    from naima.models import InverseCompton, PowerLaw, ExponentialCutoffPowerLaw

    import numpy as np
    import collections

    import astropy.units as u
    from astropy.constants import m_e, c
except ImportError as e:
    print(e)
    raise SystemExit("Cannot use module ElectronIC.")


class ElectronIC(SpectralModel):
    """This class inherits from SpectralModel in 3ML, and can be used
    to infer the population of electrons in a source.  The fit works 
    as follows:

    1. Choose a parametric form for the electron spectrum, e.g., a power
       law with an exponential cutoff.
    2. Use Inverse Compton scattering to calculate the gamma-ray flux.
    3. Fit the calculated gamma-ray flux to the data using one of the
       plugins available in 3ML.

    The free parameters of this model include the parameters of the
    electron spectrum and the distance to the source.
    
    Additional quantities which affect the calculation are the components
    of the interstellar radiation field (ISRF) which contribute to the IC
    flux.  These are not floated in the fit.  Users may wish to obtain an
    estimate of the ISRF in the vicinity of the source of interest using
    GALPROP.
    """

    def setup(self):
        self.functionName = "ElectronIC"
        self.formula = r"$f(E)=A(E/E_{\rm piv}}^\gamma}$"

        self.parameters = collections.OrderedDict()
        self.parameters["gamma"] = Parameter("gamma", -2., -10., 10, 0.1,
                                             fixed=False,
                                             nuisance=False,
                                             dataset=None)
        self.parameters["logA"] = Parameter("logA", -4., -40., 30, 0.1,
                                            fixed=False,
                                            nuisance=False,
                                            dataset=None)
        self.parameters["Epiv"] = Parameter("Epiv", 1., 1e-10, 1e10, 1.,
                                            fixed=True,
                                            unit="keV")
        self.parameters["dist"] = Parameter("dist", 1., 1e-10, 1e10, 1.,
                                            fixed=True)
        self.ncalls = 0

        self.pdist_unit = 1. / u.Unit(m_e * c ** 2)
        self.PL = PowerLaw(1 * self.pdist_unit, 1 * u.keV, -2.5)
        self.IC = InverseCompton(
            self.PL,
            seed_photon_fields=[
                'CMB', ['FIR', 26.5 * u.K, 0.415 * u.eV / u.cm ** 3]
            ],
            Eemin=1 * u.TeV)

    #        def integral(E1, E2):
    #            a  = self.parameters["gamma"].value
    #            Ep = self.parameters["Epiv"].value
    #            A  = 10.**self.parameters["logA"].value
    #
    #            if a == -1.:
    #                def f(energy):
    #                    return A * E0 * np.log(energy)
    #            else:
    #                def f(energy):
    #                    return A * energy * (energy/Ep)**a / (a + 1.)
    #            return f(E2) - f(E1)
    #
    #        self.integral = integral

    def __call__(self, energy):
        #        pdist_unit = 1. / u.Unit(m_e * c**2)

        self.ncalls += 1

        # Convert spectral index to naima sign convention
        a = -self.parameters["gamma"].value

        # Convert to naima base units: all energies in keV
        Ep = self.parameters["Epiv"].value * u.keV

        # Normalization is actually for electron differential spectrum
        A = 10. ** self.parameters["logA"].value * self.pdist_unit

        # Initialize the electron particle distribution
        self.PL.amplitude = A
        self.PL.e_0 = Ep
        self.PL.alpha = a

        # Fix the distance of the source, in kpc
        dist = self.parameters["dist"].value * u.kpc

        if type(energy) is float or type(energy) is int:
            E = [energy] * u.keV
        else:
            E = energy * u.keV

        icFlux = self.IC.flux(E, distance=dist)
        return icFlux.to(1. / (u.keV * u.cm ** 2 * u.s)).value

    def electronFlux(self, energy):
        # Convert spectral index to naima sign convention
        a = -self.parameters["gamma"].value

        # Convert to naima base units: all energies in keV
        Ep = self.parameters["Epiv"].value * u.keV

        # Normalization is actually for electron differential spectrum
        A = 10. ** self.parameters["logA"].value * self.pdist_unit

        # Initialize the electron particle distribution
        PL = PowerLaw(A, Ep, a)

        if type(energy) is float or type(energy) is int:
            E = [energy] * u.keV
        else:
            E = energy * u.keV

        return PL(E).to(1. / u.keV).value

    def photonFlux(self, E1, E2):
        return self.integral(E1, E2)

    def energyFlux(self, E1, E2):
        a = self.parameters["gamma"].value
        Ep = self.parameters["Epiv"].value
        A = 10. ** self.parameters["logA"].value

        if a == 2:
            def eF(E):
                return np.maximum(A * np.log(E / Ep), 1e-100)
        else:
            def eF(E):
                return np.maximum(A * np.power(E / Ep, 2 - a) / (2 - a), 1e-100)

        return (eF(E2) - eF(E1)) * self.keVtoErg
