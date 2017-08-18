.. _hawcnest_hawcunits:

HAWC Units
==========

.. contents::
   :local:
   :backlinks: top

Motivation
----------

A very common problem in scientific programming is dimensional error, by which
we mean the improper conversion of a physical quantity from one system of units
to another system expected by a function or subroutine.  One simple example of
this mistake is passing an angle specified in degrees to trigonometric
functions, which in most programming languages expect the function argument to
be expressed in radians.  Another might be using a length in meters when a
function expects the length to be in kilometers or some other `non-SI unit of
length <http://en.wikipedia.org/wiki/Mars_Climate_Orbiter>`_.

The GEANT4 Convention
^^^^^^^^^^^^^^^^^^^^^

To avoid this problem, we use the "System of Units" convention adopted by the
`GEANT4 <http://geant4.cern.ch/>`_ software package.  In GEANT, there is a
header file defining all basic units in terms of 9 base values.  All user code
is required to define dimension-full quantities with a unit, which in turn can
be expressed in terms of one of the base quantities.  So, for example,
quantities must be defined as follows:

.. code-block:: c++

   double badLength = 10;             // WRONG! No units given. 10 what?
   double goodLength = 10*meter;      // Right! Unit is provided

In HAWC, units are defined in the `hawcnest
<../../doxygen/html/group__hawcnest__api.html>`_ header file `HAWCUnits.h
<../../doxygen/html/HAWCUnits_8h_source.html>`_. The base units are

=================== =============== ===== ===============
Dimension           Unit            Value Symbol
=================== =============== ===== ===============
Length              meter           1.0   meter,m
Time                nanosecond      1.0   nanosecond,ns
Energy              electron-volt   1.0   electronvolt,eV
Charge              positron charge 1.0   eplus
Amount of Substance mole            1.0   mole,mol
Luminous Intensity  candela         1.0   candela
Angle               radian          1.0   radian,rad
Solid Angle         steradian       1.0   steradian,sr
=================== =============== ===== ===============

These units were chosen because most quantities in HAWC are naturally expressed
on these scales (e.g., eV, meters, nanoseconds, etc.).  All other units are
defined with respect to the base units.

How to Use the Units
--------------------

Using the units in software is straightforward.  To define a quantity in terms
of a unit, multiply it by the name of the unit:

.. code-block:: c++

   double altitude = 4100*meter;

To use a quantity in another desired unit, divide by the name of the unit:

.. code-block:: c++

   // Print altitude in miles:
   cout << altitude/mile << endl;

This scheme becomes very useful when converting complex combinations of
units, such as flux values in counts/(energy-time-area-solid angle).

In C++ Code
^^^^^^^^^^^

In C++, the units are defined as double-precision constants inside the
namespace `HAWCUnits <../../doxygen/html/namespaceHAWCUnits.html>`_. Using them
is straightforward:

.. code-block:: c++

   #include <hawcnest/HAWCUnits.h>
   ...

   int main()
   {
     double energy = 0.1 * HAWCUnits::TeV;  // 0.1 TeV

     // Easier: import the namespace
     {
       using namespace HAWCUnits;

       double E = 100 * GeV;                // 100 GeV == 0.1 TeV
     }
     ...
   }

In Python Code
^^^^^^^^^^^^^^

In python, HAWCUnits is a module that must be imported, but using it is also
very easy:

.. code-block:: python

   from hawc import hawcnest, data_structures
   from hawc.hawcnest import HAWCUnits as U

   energy = 0.1 * U.TeV
   E = 100 * U.GeV
   ...

The unit definitions in C++ and Python are automatically kept in sync using the
`X Macro <http://en.wikipedia.org/wiki/X_Macro>`_ design pattern.
