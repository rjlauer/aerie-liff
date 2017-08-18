.. _data_structures_hardware:

Hardware Classes
================

.. contents::
   :local:
   :backlinks: top

The hardware header files and classes in data structures define the various
status and error flags produced by the front-end electronics and store data
from the time-to-digital converters (TDCs) before they are formed into physics
events. The code can be found in

.. code::

   data-structures/reconstruction/hardware

The hardware classes are meant for experts only.  Low-level information in
these classes relevant to the reconstruction is made available to user
algorithms via the :ref:`data_structures_event`.
