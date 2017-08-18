.. _data_structures_math:

Math Classes
============

.. contents::
   :local:
   :backlinks: top

The math classes in :ref:`data_structures` contain a grab-bag of useful classes
and functions that appear in a large number of AERIE modules.

Power Laws
----------

A class hierarchy defining power laws (useful for energy spectra) is defined
here. Member functions are also provided to reweight one kind of power law into
another, which is particularly useful for simulations and sensitivity studies.

PowerLaw
^^^^^^^^

`PowerLaw <../../doxygen/html/classPowerLaw.html>`_ implements a simple power
law of the form

.. math::
  f(x)=A\cdot\left(\frac{x}{x_N}\right)^\alpha

BrokenPowerLaw
^^^^^^^^^^^^^^

`BrokenPowerLaw <../../doxygen/html/classBrokenPowerLaw.html>`_ implements a
power law with a spectral break, like the knee in the all-particle cosmic-ray
spectrum:

.. math::
  f(x)=\begin{cases}
    A\cdot\left(\frac{x}{x_N}\right)^\alpha, & x < x_B \\
    A\cdot\left(\frac{x_B}{x_N}\right)^{\alpha-\beta}\left(\frac{x}{x_N}\right)^\beta, & x \geq x_B
  \end{cases}

DoubleBrokenPowerLaw
^^^^^^^^^^^^^^^^^^^^

`DoubleBrokenPowerLaw <../../doxygen/html/classDoubleBrokenPowerLaw.html>`_
implements a power law with two spectral breaks:

.. math::
  f(x)=\begin{cases}
    A\cdot\left(\frac{x}{x_N}\right)^\alpha, & x < x_{B_1} \\
    A\cdot\left(\frac{x_{B_1}}{x_N}\right)^{\alpha-\beta}\left(\frac{x}{x_N}\right)^\beta, & x_{B_1} \leq x < x_{B_2} \\
    A\cdot\left(\frac{x_{B_1}}{x_N}\right)^{\alpha-\beta} \left(\frac{x_{B_2}}{x_N}\right)^{\beta-\gamma} \left(\frac{x}{x_N}\right)^\gamma, & x \geq x_{B_2} \\
  \end{cases}

CutoffPowerLaw
^^^^^^^^^^^^^^

`CutoffPowerLaw <../../doxygen/html/classCutoffPowerLaw.html>`_ implements a
power law with an exponentially vanishing tail:

.. math::
  f(x)=A\cdot\left(\frac{x}{x_N}\right)^\alpha\ e^{x/x_c}

LogParabola
^^^^^^^^^^^

`LogParabola <../../doxygen/html/classLogParabola.html>`_ implements a
power law with "curvature" in the tail:

.. math::
  f(x)=A\cdot\left(\frac{x}{x_N}\right)^{\alpha+\beta\ln{(x/x_N)}}

Special Functions
-----------------

A collection of special functions is provided to perform calculations of
various kinds.  By "special" we refer to mathematical functions which appear
commonly in statistical and physical calculations but which may not be
expressible in closed form.  AERIE provides implementations of some of these
calculations when needed, but mainly the classes in this category wrap
calculations in external libraries such as Boost, ROOT, and GSL.

Gamma Functions
^^^^^^^^^^^^^^^

The `Gamma <../../doxygen/html/namespaceSpecialFunctions_1_1Gamma.html>`_
library provides calculations of:

#. The complete gamma function :math:`\Gamma(x)` and its logarithm
#. The lower incomplete gamma function :math:`\gamma(a, x)` and upper incomplete gamma function :math:`\Gamma(a,x)` as well as the regularized forms of these functions :math:`P(a,x)` and :math:`Q(a,x)`
#. The digamma function :math:`\psi(x)` to calculate the derivative of the complete gamma function
#. The error function :math:`\text{erf}(x)` and its complement :math:`\text{erfc}(x)`, which are special cases of the incomplete gamma function

The gamma function and its logarithmic form are extremely useful for
calculating factorials from the relation :math:`\Gamma(n+1)=n!`.  To calculate
factorials, we recommend users call this function rather than resort to
Stirling's approximation.

Beta Functions
^^^^^^^^^^^^^^

The beta function :math:`B(a,b)=\Gamma(a)\Gamma(b)/\Gamma(a+b)` where
:math:`a,b>0` tends to appear in ratios of factorials common to calculations
using binomial and Poisson statistics.  The calculation of the `beta and
incomplete beta functions
<../../doxygen/html/namespaceSpecialFunctions_1_1Beta.html>`_ is included to
aid in such calculations.

Legendre Polynomials
^^^^^^^^^^^^^^^^^^^^

The `Legendre polynomials and the Spherical harmonic functions
<../../doxygen/html/namespaceSpecialFunctions_1_1Legendre.html>`_ provide an
orthonormal basis on the unit sphere.  The spherical harmonics :math:`Y_{\ell
m}(\theta,\varphi)`, where :math:`\theta` and :math:`\varphi` are the spherical
polar zenith and azimuth, respectively, are particularly useful for expressing
functions on the sphere and calculating power spectra.  They are used in
extensively in the analysis of the cosmic ray anisotropy.

Lambert W Function
^^^^^^^^^^^^^^^^^^

The `Lambert W Function
<../../doxygen/html/namespaceSpecialFunctions_1_1Lambert.html>`_ is the
solution to the equation

.. math::
  z = W(z)\ e^{W(z)}

Equations involving exponential functions are often expressable in terms of the
W function and can be useful in calculations relevant for HAWC (for example,
inverting atmospheric density to get altitude).

Tabulated Functions
-------------------

`TabulatedFunction <../../doxygen/html/classTabulatedFunction.html>`_ is a
storage class for x-y tables, with x an independent variable and y a dependent
variable.  It contains basic functions to interpolate :math:`y(x)`, as well as
iterators over the x-y pairs stored in the table.

For more sophisticated interpolation, the `SplineTable
<../../doxygen/html/classSplineTable.html>`_ class can be used to read
*N*-dimensional spline tables produced by the Photospline library and
interpolate in some set of coordinates.

Histogramming
-------------

A set of classes are provided to implement "natural binning" of data using the
`Bayesian Blocks
<http://iopscience.iop.org/0004-637X/764/2/167/pdf/apj_764_2_167.pdf>`_
algorithm of Scargle et al. (2012). These classes are used for realtime flare
monitoring in the online system.

Numerical Integration
---------------------

An abstract `Integrator <../../doxygen/html/classIntegrator.html>`_ class
provides a generic interface for implementing numerical integration in AERIE.
Two concrete subclasses are provided:

#. `TrapIntegrator <../../doxygen/html/classTrapIntegrator.html>`_ to implement trapezoidal rule integration.
#. `GQIntegrator <../../doxygen/html/classGQIntegrator.html>`_ for integration using Gaussian quadrature.

Digital Logic
-------------

Several classes can be used to implement simple electronics simulations and
digital logic:

#. `Trace <../../doxygen/html/classTrace.html>`_, a pre-allocated storage container for finely-sampled signal traces, e.g., the voltage signal from a PMT as a function of time. Various arithmetic and transcendental functions such as :code:`pow` and :code:`log` have been overloaded for use with this class.
#. `Edges <../../doxygen/html/classDigitalLogic_1_1Edge.html>`_ represent a state transition in a digital circuit element, and can be combined into collections called `Pulses <../../doxygen/html/classDigitalLogic_1_1Pulse.html>`_ .

These classes mimic the analog and digital signals produced in the HAWC
electronics.  A set of `"logic gates (AND, OR, NOT)
<../../doxygen/html/namespaceDigitalLogic.html>`_ has been implemented as
functions to combine one or more logic Pulses.
