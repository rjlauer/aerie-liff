.. _downloading_AERIE:

Downloading AERIE
=================

The AERIE code is maintained in a version control system (VCS) hosted at the
University of Maryland.  The code can be downloaded using a VCS client like git
or svn, or downloaded and installed using the
`ape <https://devel.auger.unam.mx/doc/ape/current/>`_ package manager.

Installation with APE
---------------------

The `Auger Package Environment <https://devel.auger.unam.mx/doc/ape/current/>`_
(APE, or ape) is a Python package that we use to distribute AERIE and the
external libraries it uses.  Ape will download and install all of the software
you need to run AERIE, and once the download is complete it can set up the user
environment in your Linux/OS X shell.

We strongly recommend that you use ape to install the AERIE dependencies, if
not AERIE itself, to ensure that you are using compatible versions of libraries
like ROOT, Boost, etc.  Working outside of ape is possible but not supported.

To review the details of how to use ape, see :ref:`cmake_and_ape`.

The ape tarballs for each AERIE release are listed on the `HAWC wiki
<http://private.hawc-observatory.org/wiki/index.php/AERIE_Releases>`_. You can
also go directly to the `ape download page
<https://devel.auger.unam.mx/trac/projects/ape/downloads/>`_ and search there
for the most recent release of ape.

Download AERIE with a VCS
-------------------------

We use the `subversion <https://subversion.apache.org/>`_ VCS to back up the
AERIE code at UMD.  You can use either subversion or `git
<http://git-scm.com/>`_ to access the code by following these steps:

1. Install either subversion or git on your system.

2. Create a username and password for yourself in the `HAWC Personnel Database <http://private.hawc-observatory.org/personnel/persons>`_.

3. Check out the AERIE trunk.  If using subversion, the command is

.. code-block:: bash

   $ mkdir aerie
   $ cd aerie
   $ svn co http://private.hawc-observatory.org/svn/hawc/workspaces/aerie/trunk

This will create a subdirectory in your aerie folder called **trunk** which
contains the source.  Build instructions can be found at :ref:`cmake_and_ape`.

Download the Configuration Database
-----------------------------------

Detector configuration data such as PMT surveys and calibration data are
distributed in a separate project called **config-hawc**.  Currently, the only
way to access these data is by downloading the project from the UMC VCS:

.. code-block:: bash

   $ svn co http://private.hawc-observatory.org/svn/hawc/workspaces/config-hawc

AERIE will automatically access this database if you define the environment
variable CONFIG_HAWC after the download.  In bash:

.. code-block:: bash

   $ export CONFIG_HAWC=/path/to/config-hawc

and in csh/tcsh:

.. code-block:: csh

   $ setenv CONFIG_HAWC /path/to/config-hawc

You will want to run updates regularly (at least once per week) in your working
copy to keep the configuration database in sync with changes to the detector
survey and calibration constants:

.. code-block:: bash

   $ svn update config-hawc
