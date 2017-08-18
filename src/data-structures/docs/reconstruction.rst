.. _data_structures_reconstruction:

Reconstruction Classes
======================

.. contents::
   :local:
   :backlinks: top

The reconstruction classes are used to store the results of different
reconstruction algorithms.  They inherit from `Baggable
<../../doxygen/html/classBaggable.html>`_ via the class `RecoResult
<../../doxygen/html/classRecoResult.html>`_ and can be passed between AERIE
modules using a pointer to the `Bag <../../doxygen/html/classBag.html>`_ object
dictionary.

The reconstruction classes reflect the main types of algorithms present in HAWC
reconstruction and analysis code:

#. Core fitting
#. Track angle fitting
#. Energy estimation
#. Gamma-hadron separation

Several classes related to the construction of "events" from the TDC system are
also present in this area.

Core Fitting
------------

Core fitting refers to the reconstruction of the position of the shower core on
or off the array.

Reconstruction results related to core fitting are stored in objects which
inherit from the class `CoreFitResult
<../../doxygen/html/classCoreFitResult.html>`_.  The base class stores the
position of the core, the amplitude of the functional fit to the lateral
distribution of charge observed in the array, and uncertainties on these
quantities. Access to the channel or tank-wise position and charge data used to
estimate the core location is also provided.

Track Fitting
-------------

Track fitting refers to the reconstruction of the shower geometry. This means
the vector of the shower direction in the detector local coordinate system.

Reconstruction results related to track fitting are stored in objects which
inherit from the class `AngleFitResult
<../../doxygen/html/classAngleFitResult.html>`_.  The base class stores the
zenith and azimuth angles of the shower and the :math:`\chi^2` figure of merit
of the fit of the shower front to the timing distribution in the data.

Gamma-Hadron Separation
-----------------------

A set of classes related to gamma-hadron separation is defined in the directory

.. code:: bash

   data-structures/reconstruction/gamma-filter

These store data related to various quantities used in the HAWC analysis for
suppression of cosmic ray backgrounds, including:

#. Compactness
#. :math:`\chi^2`/NDF of the core fit
#. Particle Identification of Nuclear Cosmic rays (PINCness)

Details of these algorithms can be found in the :ref:`gamma_filter` project.

Energy Estimation
-----------------

HAWC does not currently have a shower-by-shower energy estimator in broad use,
but in anticipation of the acceptance of this quantity we provide the base
class `EnergyResult <../../doxygen/html/classEnergyResult.html>`_.  The class 
provides functions to access the total estimated energy of the primary particle
which initiated an air shower, as well as an estimate of the amount of energy
deposited at ground level.

Details on the current energy reconstruction algorithms can be found in the 
:ref:`energy_estimator` and :ref:`lh_energy_estimator` projects.
