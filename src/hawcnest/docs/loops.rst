.. _hawcnest_loops:

The MainLoop
============

.. contents::
   :local:
   :backlinks: top

Description
-----------

The class `MainLoop <../../doxygen/html/classMainLoop.html>`_ is an interface
with subclasses responsible for taking the names of :ref:`hawcnest_modules`
used in event processing and executing them in a specified order.  It has one
simple method, ``Execute()``, which calls ``Source::Next()`` to generate a new
event and ``Module::Process(BagPtr bag)`` for each module in the module
sequence.  Depending on the value of ``Module::Result`` returned by each
module, the loop may skip to the next event or break the event processing loop
altogether.

The Module Sequence
-------------------

A ``MainLoop`` subclass is expected to take two configuration arguments: the
name of an event ``Source`` and a list of modules to be executed in order.  The
``Execute()`` function is then called to run the processing loop.

C++ Example
^^^^^^^^^^^

To illustrate the initialization and operation of a ``MainLoop``, we will use
the example classes defined in the documentation on :ref:`hawcnest_modules`:

.. code-block:: c++

   class CountInserter : public Source {
     ...
     BagPtr Next() { ... }
   };

   class ScaleEvenBy3 : public Module {
     ...
     Module::Result Process(BagPtr bag) { ... }
   };

   int main()
   {
     HAWCNest nest;

     // Data source
     nest.Service<CountInserter>("countInserter")
       ("counter", 0)
       ("maxCount", 100)
       ("stepCount", 2);

     // A module that operates on the data
     nest.Service<ScaleEvenBy3>("scaler");

     // Define the module sequence, configure the framework, and run
     vector<double> chain;
     chain.push_back("scaler");         // List of modules to run

     nest.Service<SequentialMainLoop>("mainloop")
       ("source", "countInserter")
       ("modulechain", chain);

     nest.Configure();

     MainLoop& loop = GetService<MainLoop>("mainloop");
     loop.Execute();

     nest.Finish();

     return 0;
   }

In this case the module sequence is not particularly interesting, consisting
only of the instance of the ``Module`` ScaleEvenBy3 named "scaler."  However,
the module chain can be arbitrarily long, with an ordered sequence of modules
passed to the ``MainLoop`` using a ``std::vector<std::string>`` list of module
instance names.

Python Example
^^^^^^^^^^^^^^

As usual, the Python example is slightly more concise but does the same thing:

.. code-block:: python

   # Define sources and modules
   class CountInserter(hawcnest.Source):
       def __init__(self):
           ...
       def Next():
           ...

   def scaleEvenBy3(bag):
       ...

   # Initialize the framework and required sources and modules
   nest = HAWCNest()

   nest.Service(CountInserter, "countInserter",
       counter=0, maxCount=100, stepCount=2)

   nest.Service(scaleEvenBy3, "scaler")

   nest.Service("SequentialMainLoop", "mainloop",
       source="countInserter",
       modulechain=["scaler"])

   # Configure, run, and clean up
   nest.Configure()

   loop = hawcnest.GetService_MainLoop("mainloop")
   loop.Execute()

   nest.Finish()

Available Implementations
-------------------------

Several implementations of the ``MainLoop`` interface are available in the
`hawcnest <../../doxygen/html/group__hawcnest__api.html>`_ project.  You can
find a complete list in the `MainLoop doxygen documentation
<../../doxygen/html/classMainLoop.html>`_.

Of the various subclasses of ``MainLoop``, only two are likely to be of
interest to most users:

SequentialMainLoop
^^^^^^^^^^^^^^^^^^

The `SequentialMainLoop <doxygen/html/classSequentialMainLoop.html>`_ is the
class used by almost all AERIE programs.  Its job is to obtain ``Bag`` data from
a ``Source`` and execute the ``Process()`` function of every ``Module`` in the
modulechain on the data. 

When the ``Execute()`` function is called, this service will loop through all
events until the ``Source`` is exhausted.  Hence, it is useful for batch
processing of events, which is what most programs are built to do.

SingleEventMainLoop
^^^^^^^^^^^^^^^^^^^

The `SingleEventMainLoop <doxygen/html/classSingleEventMainLoop.html>`_ differs
from the `SequentialMainLoop <doxygen/html/classSequentialMainLoop.html>`_ in
that it does not loop through all events when ``Execute()`` is called.
Instead, it will step forward by one event in the data stream.

This is not particularly useful for batch processing, but it does offer the
possibility of moving forward or backward by one event through the data, which
is very useful for applications such as event displays.
