HAWC Simulation
===============

The HAWC simulation occurs in three stages:

#. **CORSIKA** simulations of cosmic rays and gamma rays.
#. **HAWCSim** GEANT4-based simulations of the response of the tanks to CORSIKA particles at ground level.
#. **AERIE Reconstruction** of simulated events which produce signals in the HAWC tanks.

A library of showers produced with CORSIKA, HAWCSim, and AERIE is maintained at
UMD.  The simulation is being continuously generated and updated, and the best
place to track the current list of files you should use for analysis is given
in the `Monte Carlo Products
<http://private.hawc-observatory.org/wiki/index.php/Monte_Carlo_Products>`_
wiki page.

CORSIKA Air Showers
-------------------

`CORSIKA <https://web.ikp.kit.edu/corsika/>`_ is a simulation code that tracks
the nuclear and electromagnetic interactions which occur in extensive air
showers.  In HAWC we generate air showers produced by gamma rays, protons, and
the heavier nuclei \ :sup:`4`\ He, \ :sup:`12`\ C, \ :sup:`16`\ O,
\ :sup:`20`\ Ne, \ :sup:`24`\ Mg, \ :sup:`28`\ Si, and \ :sup:`56`\ Fe.

CORSIKA data at UMD can be found in the directory

.. code-block:: sh

   $HAWCROOT/sim/corsika

The details of the directory structure are described `here <http://private.hawc-observatory.org/wiki/index.php/MC_Production#Directory_Structure>`_.

The files are in a binary format which can be read using the code and scripts
in the :ref:`aerie_io` project.

HAWCSim Tank Simulations
------------------------

:ref:`hawcsim` is a part of AERIE that is built if the particle tracking code
GEANT4 has been detected on your system.  (GEANT4 and its associated
interaction tables can be installed with ``ape``; see :ref:`ape`.)

HAWCSim will propagate particles at ground level from CORSIKA into a model of
the HAWC tanks, calculate the Cherenkov photons produced when the particles
enter the water in the tanks, and convert photons to photoelectrons (PEs)
detected at each PMT.  The HAWCSim output at UMD can be found in

.. code-block:: sh

   $HAWCROOT/sim/corsika

See the `MC Products wiki page <http://private.hawc-observatory.org/wiki/index.php/MC_Production#Directory_Structure>`_
for more details.

Reconstructed Showers
---------------------

The final stage of simulation occurs when the PEs from HAWCSim are read into
AERIE.  The :ref:`sim_pmt_modeler` project adds random noise to each channel
and smears the charge and timing of the signal in each PMT using a simple model
of the detector resolution.  Then the events are reconstructed using the
standard offline reconstruction program.  At UMD, the output of simulation
reconstruction can be found in

.. code-block:: sh

   $HAWCROOT/sim/reco

