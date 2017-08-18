.. _hawcnest_modules:

Modules and Sources
===================

.. note::

   In this context, a module refers to the C++ class `Module
   <../../doxygen/html/classModule.html>`_ defined in the `hawcnest
   <../../doxygen/html/group__hawcnest__api.html>`_ project.  It is not a
   module in the Python sense.

.. contents::
   :local:
   :backlinks: top

Description
-----------

The `Module <../../doxygen/html/classModule.html>`_ and `Source
<../../doxygen/html/classSource.html>`_ are two interfaces to
:ref:`hawcnest_services` used to perform the processing steps in AERIE. 

A ``Source`` is called at the start of a processing loop.  Its job is to
allocate a new ``Bag`` (see :ref:`hawcnest_bag`) and push data into it.  When
there are no more data or events to process, the ``Source`` returns a ``NULL``
pointer, which is interpreted by the framework as an end-of-stream, halting the
processing loop.

A ``Module`` can be called in any order in a processing stream after the
``Source``.  Its role is to operate on data in the ``Bag`` and add new results
as needed.  Nearly all user reconstruction and analysis code is implemented as
a `subclass
<http://en.wikipedia.org/wiki/Inheritance_(object-oriented_programming)#Subclasses_and_superclasses>`_
of ``Module``.

Source Interface
----------------

Like all :ref:`hawcnest_services`, a ``Source`` can be configured using the
``DefaultConfiguration`` and ``Initialize`` functions, which must be
implemented.  A ``Finish`` function is also provided in case some cleanup is
needed at the end of the event loop.

The Next Function
^^^^^^^^^^^^^^^^^

The main function in a ``Source`` is called ``Next``, which creates a new
``Bag`` for the data stream or returns a ``NULL`` pointer when no data are
available.

Example: C++
^^^^^^^^^^^^

In this example, we set up a source that pushes ``BaggableInt`` objects into a
``Bag``.

.. code-block:: c++

   #include <hawcnest/HAWCNest.h>
   ...

   class CountInserter : public Source {
     public:
       typedef Source Interface;

       Configuration DefaultConfiguration() {
         // Set up Source configuration parameters
         Configuration c;
         c.Parameter<int>("counter", 0);
         c.Parameter<int>("maxCount", 10);
         c.Parameter<int>("stepCount", 1);
         return c;
       }

       void Initialize(const Configuration& c) {
         // Initialize the Source configuration
         c.GetParameter("counter", counter_);
         c.GetParameter("maxCount", maxCount_);
         c.GetParameter("stepCount", step_);
       }

       BagPtr Next() {
         // Return an integer and increment it below some maximum
         if (counter_ <= maxCount_) {
           BaggableIntPtr c = boost::make_shared<BaggableInt>(counter_++);
           BagPtr bag = boost::make_shared<Bag>();
           bag->Put(c, "count");
           return bag;
         }

         // Above the maximum, return an empty pointer
         return BagPtr();
       }

       void Finish() { }

     private:
       int counter_;
       int maxCount_;
       int step_;
   };

   REGISTER_SERVICE(Source);

The class defines ``Interface`` as a `typedef
<http://en.wikipedia.org/wiki/Typedef>`_ for ``Source`` so that the framework
knows to treat it as a ``Source``.  Its job is to start with an integer
("counter") and push a stream of integers into the ``Bag``.  Each new integer
is incremented by a step size.  When a maximum value is reached, the class
returns an empty ``BagPtr``, causing the framework to halt the processing loop
(see :ref:`hawcnest_loops`).

Hence, the ``Source`` is essentially a kind of `while loop
<http://en.wikipedia.org/wiki/While_loop>`_, returning values as long as some
condition is true.  You can easily imagine this extended to file I/O, in which
data are read from disk and pushed into the ``Bag`` until EOF is reached.

Example: Python
^^^^^^^^^^^^^^^

The same example above can be easily implemented in Python:

.. code-block:: python

   from hawc import hawcnest
   ...

   class CountInserter(hawcnest.Source):
       """Generate integers up to some max value and stuff them into the Bag.
          Keyword arguments are used to set up the counter.
       """
       def __init__(self, **kwargs):
           hawcnest.Source.__init__(self)
           self.counter = kwargs.get("startCount", 0)
           self.maxCount = kwargs.get("maxCount", 10)
           self.step = kwargs.get("stepCount", 1)
   
       def Next(self):
           if self.counter <= self.maxCount:
               x = hawcnest.BaggableInt(self.counter)
               self.counter += self.step
               bag = hawcnest.Bag()
               bag["count"] = x
               return bag
           else:
               return None

This version of CountInserter works exactly the same way as its C++
counterpart, using inheritance to implement the ``Source`` functionality.
Because it is so easy in Python to set parameters of the class using keyword
arguments (or kwargs), it is not necessary to define ``DefaultConfiguration``
and ``Initialize`` member functions.

Module Interface
----------------

Like all :ref:`hawcnest_services`, a ``Module`` can be configured using the
``DefaultConfiguration`` and ``Initialize`` functions, which must be
implemented.  A ``Finish`` function is also provided in case some cleanup is
needed at the end of the event loop.

The Process Function
^^^^^^^^^^^^^^^^^^^^

The main function in a ``Module`` is called ``Process``, which takes a
``shared_ptr<Bag>`` argument and operates on data expected to be inside the
``Bag``. New data can be inserted into the ``Bag`` if desired.  At the end of
each processing step, the function returns a ``Module::Result`` enum which can
take on one of three values:

========= ===== ==============================================================
Result    Value Meaning
========= ===== ==============================================================
Continue  0     Processing went normally, pass event to subsequent Modules
Filter    1     Stop processing this event, restart loop on next event
Terminate 2     Serious issue encountered: completely halt the processing loop
========= ===== ==============================================================

Example: C++
^^^^^^^^^^^^

To illustrate how AERIE Modules work, we implement a class that reads in the
counter created by our CountInserter ``Source``, filters out any odd-numbered
counts, and scales what remains by three:

.. code-block:: c++

   class ScaleEvenBy3 : public Module {
     public:
       typedef Module Interface;

       Configuration DefaultConfiguration() {
         Configuration c;
         return c;
       }

       void Initialize(const Configuration& c) { }

       Module::Result Process(BagPtr bag) {
         if (bag->Exists("count")) {
           int count = bag->Get<BaggableInt>("count").value;
           if (count % 2 != 0)
             return Filter;

           BaggableIntPtr countX3 = boost::make_shared<BaggableInt>(3*count);
           bag->Put(countX3, "countX3");
           return Continue;
         }
         return Filter;
       }

       void Finish() { }
   };

Example: Python
^^^^^^^^^^^^^^^

In Python, we can define classes that inherit from ``hawcnest.Module``, the
Python version of the C++ ``Module`` class.  However, the Python bindings allow
Modules to be defined as regular Python functions or `lambdas
<https://docs.python.org/2/reference/expressions.html#lambda>`_:

If a Python function or lambda is used in place of a class that inherits from
``Module``, the function must return boolean values: ``True`` to continue
processing events (equivalent of ``Module::Continue``) or ``False`` to skip to
the next event (equivalent of ``Module::Filter``).  There is no equivalent to
``Module::Terminate``, but this is rarely used in any case.

.. code-block:: python

   # The Python equivalent of the C++ class ScaleEvenBy3
   def scaleEvenBy3(bag):
      if "count" in bag:
         if bag["count"].value % 2 != 0:
            return False
         countX3 = hawcnest.BaggableInt(bag["count"].value * 3)
         bag["countX3"] = countX3
         return True
      return False

    # Note: it is also trivial to implement simple filters and cuts using lambdas
    evenFilter = lambda bag: True if "count" in bag and bag["count"].value % 2 == 0 else False

We could have also implemented a ``ScaleEvenBy3`` class in Python in a similar
manner to the ``CountInserter`` source class above. However, the much shorter
syntax of functions and lambdas almost always make these forms a better
approach when writing AERIE modules in Python.
