HAWC Data
=========

HAWC data are mirrored at the `University of Maryland
<http://umdphysics.umd.edu>`_ and the `Instituto de Ciencias Nucleares (ICN)
<http://www.nucleares.unam.mx/>`_ at UNAM.  At each location the data are
stored in identical file hierarchies.  The base of the hierarchy is called
``$HAWCROOT``, a shell variable defined automatically for all users on each
system.  So if you want to access the data, log into your account at UMD or
ICN-UNAM and type

.. code-block:: sh

   $ cd $HAWCROOT

If you not have an account at UMD or UNAM, contact

* `Andrew Smith <mailto:asmith@umdgrb.umd.edu>`_ (UMD)
* `Lukas Nellen <mailto:lukas@nucleares.unam.mx>`_ (ICN-UNAM)

Access to both sites is available to all HAWC collaborators regardless of
institutional affiliation.

Updates on the current status of data copied from the site to ICN and UMD are
provided on the wiki page "`Getting HAWC Data
<http://private.hawc-observatory.org/wiki/index.php/Getting_HAWC_Data>`_."

Details on the data formats of interest to most HAWC users can be found in the
regularly updated *Data Format* technical note [Riv2014]_.
We summarize each of these formats and where to find the data below.

Minbias (RAW) Data
------------------

In the HAWC data acquisition (DAQ) system the signals from each PMT are
analyzed in software and the simple multiplicity trigger is applied to make
trigger decisions.  The data from the PMTs are in the form of logical "edges"
from the front-end electronics and CAEN time to digital converters (TDCs) used
to digitize the data.  The term "raw data" (or RAW) refers to the PMT edges
before the simple multiplicity trigger is applied.

The rate of RAW data from the complete detector is 500 MB/s, too large to save
to disk.  However, we do save 1:100 of the RAW edges for testing.  You can find
them in ``$HAWCROOT`` with names like:

.. code-block:: sh

   $HAWCROOT/data/hawc/data/YYYY/MM/runNNNNNN/raw_runNNNNNN_SSSSS.dat

where YYYY is a year, MM is a month (1-12), NNNNNN is a run number, and SSSSS
is a sub-run number.

The RAW files are written in a binary format read by the
:ref:`online_processing` project.

Scaler Data
-----------

The scaler data summarize the rate at which the signals in each individual PMT
go over the thresholds set in the front-end and TDC electronics.  They provide
a convenient measure of the health of each PMT and also enable us to look for
events that would otherwise have too little signal to trigger the experiment.

There are two scaler data formats used in HAWC, described below.

Scaler Format
^^^^^^^^^^^^^

When the PMT data are read out by the HAWC electronics, they are split into two
paths.  One path leads to the trigger system (described below), while the other
path leads to a separate readout system known as the Scaler Electronics.  The
use of two systems allows us to track the status of each PMT when normal DAQ is
turned off.  The scaler system also allows monitoring of sub-threshold physics
events like solar flares and GRBs.  Details on this system can be found in the
*Baseline Scaler Electronics Proposal* [Tab2010]_.

The output of the Scaler Electronics is compressed ASCII files with count
rates from each PMT in the experiment.  The files can be found in

.. code-block:: sh

   $HAWCROOT/data/hawc/scalers/YYYY/MM/Scaler_*.dat.gz

TDCScaler Format
^^^^^^^^^^^^^^^^

The HAWC software trigger (defined below) can also track the individual rates
at which PMTs cross the thresholds set in our electronics.  These data are
written in `XCDF <https://github.com/jimbraun/XCDF>`_ format and can be found
in the files

.. code-block:: sh

   $HAWCROOT/data/hawc/data/YYYY/MM/runNNNNNN/tdcscaler_runNNNNNN_SSSSS.xcd

Triggered (TRIG) Data
---------------------

The Simple Multiplicity Trigger (SMT) works by looking for some minimum number
of PMTs over threshold in a time window of 150 ns.  All events which pass the
trigger condition are written to disk at a rate of about 20 MB/s.  The disk
files with triggered events, called TRIG files, are written in `XCDF
<https://github.com/jimbraun/XCDF>`_ format.  The TRIG data are located in the
same location as the raw data.  You can find them by looking for files with
names like

.. code-block:: sh

   $HAWCROOT/data/hawc/data/YYYY/MM/runNNNNNN/trig_runNNNNNN_SSSSS.xcd

Reconstructed (REC) Data
------------------------

The AERIE reconstruction is applied to TRIG files, producing data in REC
format.  Reconstructions are carried out both online and offline.

Online Reconstruction
^^^^^^^^^^^^^^^^^^^^^

The online reconstruction is used to diagnose the health of the detector and
provide an early warning system for transient events such as AGN flares and
GRBs.  Files produced by the online reconstruction can be found in

.. code-block:: sh

   $HAWCROOT/data/hawc/data/YYYY/MM/runNNNNNN/reco_runNNNNNN_SSSSS.xcd

Offline Reconstruction
^^^^^^^^^^^^^^^^^^^^^^

The offline reconstruction is carried out by an automatic processing script
called `HAWCProd
<http://private.hawc-observatory.org/wiki/index.php/Running_a_Hawcprod_Production>`_.
HAWCProd is used to reconstruct files using stable releases of AERIE combined
with regularly updated calibration constants and survey data stored in
`config-hawc
<http://private.hawc-observatory.org/trac/svn/browser/workspaces/config-hawc>`_.
(See :ref:`downloading_AERIE` for more information on ``config-hawc``.)

HAWCProd produces output in two formats: a summary REC format described in the
Data Format memo [Riv2014]_, and an extended format with additional channel
data.  The two kinds of output can be found in

.. code-block:: sh

   $HAWCROOT/data/hawc/reconstructed/hawcprod/vA.BB.CC/config-revRRRRR/reco_xcdf/YYYY/MM/runNNNNNN/reco_*.xcd

and

.. code-block:: sh

   $HAWCROOT/data/hawc/reconstructed/hawcprod/vA.BB.CC/config-revRRRRR/reco_extended/YYYY/MM/runNNNNNN/reco_*.xcd
   
where A.BB.CC refers to the AERIE release used by HAWCProd, RRRRR refers to the
revision ID of ``config-hawc``, and the remainder of the path contains the
date, run, and sub-run of the data.
