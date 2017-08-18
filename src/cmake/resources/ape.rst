.. _ape:

Building with APE
-----------------

.. highlight:: sh

AERIE can be built and installed with the `Auger Package Environment
<https://devel.auger.unam.mx/doc/ape/current/>`_ (``ape``) or downloaded from
the version control system (VCS) hosted at the University of Maryland and built
manually.  The first approach is used to distribute official releases used for
data production on site and offline reconstructions at UNAM and UMD.  The
second approach is for users who want to stay up to date with the latest
changes to the AERIE trunk.

Regardless of the approach you use, you will need to use ``ape`` to install the
external libraries used by AERIE.  Ape will download a supported set of
dependencies and configure your user environment for you.  You may try to
install these dependencies manually without ``ape``, but this is not supported
by the AERIE development team.

Downloading APE
^^^^^^^^^^^^^^^

The ``ape`` tarballs for each AERIE release are listed on the `HAWC wiki
<http://private.hawc-observatory.org/wiki/index.php/AERIE_Releases>`_. You can
also go directly to the `ape download page
<https://devel.auger.unam.mx/trac/projects/ape/downloads/>`_ and search there
for the most recent release of ``ape``.

Configuring APE
^^^^^^^^^^^^^^^

Once ``ape`` is downloaded and unpacked you will need to configure it. The
details of how to do this are available in the `ape User Guide
<https://devel.auger.unam.mx/doc/ape/current/manual.html>`_.

In brief, without any configuration or options ``ape`` will install the AERIE
dependencies into your current directory.  If you want to install the software
to a particular location on your system, you can do three things:

1. Create an :file:`ape.rc` configuration file that specifies the base directory
for installation and build directory for compilation.  Then set up an
environment variable :envvar:`APERC` to point to this configuration file.

2. Create an :file:`ape.rc` file and use the :option:`--aperc` option when
running ``ape``.

3. Create a :file:`.aperc` file in your home directory, which will
automatically be picked up by ``ape`` and will override other configuration
options.

A typical configuration file will be very short.  Here is a simple example
adapted from the ``ape`` documentation:

.. code-block:: ini

   # A simple ~/.aperc
   [DEFAULT]
   base = %(home)s/HAWC/software/ApeInstalled

   [ape]
   jobs = 2
   mirrors = mx us

The variables set here are:

   *base*
    The path where packages are installed, relative to the user's
    :envvar:`HOME` environment variable.

   *jobs*
    The number of jobs for parallel builds (can lead to a significant speedup
    when building the libraries). Set to the number of cores on your machine.

   *mirrors*
    The preferred order of mirror sites for downloading packages.  When in
    Mexico, list ``mx`` first (UNAM), and when in the US list ``us`` first 
    (UMD).

Details on additional options and local modifications to packages are provided
in the `ape User Manual
<https://devel.auger.unam.mx/doc/ape/current/manual.html>`_.

Available Packages
^^^^^^^^^^^^^^^^^^

In addition to AERIE, ``ape`` will install external dependencies.  These
include:

#. **CMake**: a cross-platform build system (see :ref:`cmake_and_ape`)
#. **Boost**: the `Boost C++ <http://www.boost.org/>`_ extension library
#. **FFTW3**: an efficient C implementation of `FFTW <http://www.fftw.org/>`_
#. **GSL**: the `GNU Scientific Library <http://www.gnu.org/software/gsl/>`_
#. **ROOT**: the CERN `analysis and statistics package <http://root.cern.ch>`_
#. **Xerces-C++**: an `XML parser <http://xerces.apache.org/xerces-c/>`_ written in C++
#. **XCDF**: a binary `compression library <https://github.com/jimbraun/XCDF>`_ used for data storage in HAWC
#. **CFITSIO**: a C implementation of the `NASA FITS <http://heasarc.gsfc.nasa.gov/fitsio/fitsio.html>`_ image standard
#. **HEALPix**: an `equal-area pixelization <http://healpix.jpl.nasa.gov/>`_ of the sphere used for sky maps
#. **Photospline**: a high-dimensional B-spline library written for IceCube

Optional packages used for simulation or online data acquisition and
reconstruction are:

#. **GEANT4**: a `particle interaction code <http://geant4.cern.ch/>`_ for the detector simulation
#. **0MQ**: an `asynchronous messaging library <http://zeromq.org/>`_ used in the online DAQ system

You can see the list of all available packages in ape by running the command

.. code-block:: bash

   $ ./ape packages

The external libraries used in HAWC online and offline are bundled into the
packages ``externals`` and ``externals-online``.  To see the libraries in just
the ``externals`` package, run the command

.. code-block:: bash

   $ ./ape packages externals

Installing Libraries
^^^^^^^^^^^^^^^^^^^^

Libraries can be installed using the ``install`` command in ``ape``.  For
example, to install all of the libraries in the ``externals`` package, run

.. code-block:: bash

   $ ./ape install externals

To build the most recent release of AERIE and all of its dependencies run

.. code-block:: bash

   $ ./ape install aerie

If you want to attempt a dry run where ``ape`` will go through the motions of
the installation without actually executing anything (useful for checking your
configuration), use the :option:`-n` or :option:`--dry-run` options.

To install one or more individual packages, just specify the package name(s):

.. code-block:: bash

   $ ./ape install geant4 xcdf

and all necessary dependencies of just these packages will be installed into
the directory you specify in :file:`ape.rc`.

Setting up the User Environment
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

One of the most useful features of ``ape`` is that it will set up your user
enviroment after you have installed a set of libraries.  For example, users of
``bash`` or ``csh`` should run the command

.. code-block:: bash

   $ eval `./ape sh externals`

or

.. code-block:: bash

   $ eval `./ape csh externals`

to set up their shell.  This will set the environment variables
:envvar:`PATH`, :envvar:`PYTHONPATH`, and :envvar:`LD_LIBRARY_PATH` (or
:envvar:`DYLD_LIBRARY_PATH` on a Mac) with the paths needed to compile and run 
AERIE.
