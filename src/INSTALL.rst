.. contents::

Installation Instructions
=========================

The HAWC offline software is named the Analysis and Event Reconstruction
Integrated Environment (AERIE). AERIE depends on several third-party libraries
which are listed below. The package can be installed from source using the
CMake build system. To configure, build, and install AERIE, first download and
install the external dependences. Then unzip the AERIE source file and follow
these steps:

.. code-block:: sh

    % cd /path/to/source/build
    % cmake -DCMAKE_INSTALL_PREFIX=/path/to/install ../src
    % make -jN (N=number of processors to use in a parallel build)
    % make install

To run the likelihood fitter used in the analysis, you need to configure your
shell with the AERIE environment. This is done after installation as follows:

.. code-block:: sh

    % /path/to/install/bin/hawc-config --spawn-sh

This command will spawn a new bash shell with the correct environment loaded.
All python bindings to AERIE should be functional and you will be able to use
it with `3ML <https://github.com/giacomov/3ML>`_.

External Dependencies
=====================

AERIE depends on the following third-party libraries: `Python 2.7`_, 
`Boost 1.63`_, `GSL 2.2.1`_, `FFTW 3.3.3`_, `ROOT 5.34.36`_, `CFITSIO 3.370`_,
`HEALPix 3.31`_, `Xerces-C 3.1.1`_, and `PhotoSpline 1.1.0`_.

We have used these specific versions to perform the analysis with the HAWC
offline software. We provide links to the source code so that users may install
these libraries.

Note that the performance of the likelihood fit is guaranteed only for the
versions listed here. Users may try using different versions of the external
libraries or the default installation from their system package manager, but
results are not guaranteed. Proceed at your own risk.

.. _cmake:

CMake 3.7.2
-----------

Download from `cmake.org <https://cmake.org/files/v3.7/cmake-3.7.2.tar.gz>`_.

Python 2.7
----------

We recommend installation with `anaconda <https://www.continuum.io>`_.

Boost 1.63
----------

Download from `boost.org
<https://sourceforge.net/projects/boost/files/boost/1.63.0/>`_.

The following Boost modules are required:

* filesystem
* iostreams
* program_options
* python
* random
* regex
* system
* thread
* unit_test_framework

If installing to a non-standard directory, create the environment variable
BOOSTROOT to point to the installation location.

GSL 2.2.1
---------

`Download GSL here <ftp://ftp.gnu.org/gnu/gsl/gsl-2.2.1.tar.gz>`_.

If installing to a non-standard directory, create the environment variable
GSLROOT to point to the installation location.

FFTW 3.3.3
----------

`Download FFTW3 here <ftp://ftp.fftw.org/pub/fftw/fftw-3.3.3.tar.gz>`_.

If installing to a non-standard directory, create the environment variable
FFTW3ROOT to point to the installation location.

ROOT 5.34.36
------------

Download from `CERN
<https://root.cern.ch/download/root_v5.34.36.source.tar.gz>`_.

If installing to a non-standard directory, create the environment variable
ROOTSYS to point to the installation location.

CFITSIO 3.370
-------------

Download from `NASA/HEASARC
<https://heasarc.gsfc.nasa.gov/FTP/software/fitsio/c/cfitsio3370.tar.gz>`_.

If installing to a non-standard directory, create the environment variable
CFITSIOROOT to point to the installation location.

HEALPix 3.31
------------

HEALPix is distributed at
`sourceforge <https://sourceforge.net/projects/healpix/>`_, but we
provide a stripped-down version (C++ package only) with a more standard
CMake/autotools installation procedure, which can be obtained
`here <http://www.hawc-observatory.org/software/healpix-cxx-3.31.tar.bz2>`_.

If installing to a non-standard directory, create the environment variable
HEALPIXROOT to point to the installation location.

Xerces-C 3.1.1
--------------

`Download from apache
<http://archive.apache.org/dist/xerces/c/3/sources/xerces-c-3.1.1.tar.gz>`_.

If installing to a non-standard directory, create the environment variable
XERCESCROOT to point to the installation location.

PhotoSpline 1.1.0
-----------------

PhotoSpline is a package developed by Jakob van Santen and Nathan Whitehorn for
interpolation in high-dimensional tables. We provide a version of it 
`online <http://www.hawc-observatory.org/software/photospline-1.0.1.tar.bz2>`_.

If installing to a non-standard directory, create the environment variable
PHOTOSPLINEROOT to point to the installation location.
