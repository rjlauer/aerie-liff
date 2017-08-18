.. _hawcnest_bag:

The Bag
=======

.. contents::
   :local:
   :backlinks: top

Description
-----------

The `Bag <../../doxygen/html/classBag.html>`_ is a generic container for event
data.  Objects which inherit from the class `Baggable
<../../doxygen/html/classBaggable.html>`_ can be placed into the ``Bag`` and
accessed with a unique string key, much like a ``std::map`` in C++ or a
dictionary in Python.

The ``Bag`` is an add-only structure.  New data can be added dynamically to the
``Bag``, but once they are added they cannot be retrieved except by ``const``
reference or pointer, making them *immutable*. This ensures that any step in
the event processing chain cannot inadvertently modify or overwrite the output
from previous steps.

Putting Data into the Bag
-------------------------

Putting objects into the ``Bag`` is relatively straightforward.  The object
must inherit from ``Baggable``; then a ``boost::shared_ptr`` containing the
data is put into the ``Bag``. For your convenience, a macro called
``SHARED_POINTER_TYPEDEFS`` is provided in the project to simplify the syntax
of ``shared_ptr``.

C++ Example
^^^^^^^^^^^

Here is an example of putting some ``Baggable`` data into a bag in C++

.. code-block:: c++

   #include <hawcnest/processing/Bag.h>
   ...

   class BaggableInt : public Baggable {
     public:
       BaggableInt(int v) : value(v) { }
       int value;
   };
   SHARED_POINTER_TYPEDEFS(BaggableInt);  // Declare shared_ptr to BaggableInt

   int main()
   {
     // Create a shared pointer to a BaggableInt
     BaggableIntPtr x = boost::make_shared<BaggableInt>(11);

     // Stuff the shared pointer into the Bag
     Bag b;
     b.Put("x", x);

     return 0;
   }

Python Example
^^^^^^^^^^^^^^

In Python, the ``Bag`` is set up to look like a dictionary, so you can use the
bracket operator to define keys and insert data.  For example,

.. code-block:: python

   from hawc import hawcnest
   from hawc.hawcnest import Bag as Bag
   from hawc.hawcnest import BaggableInt as BaggableInt

   bag = Bag()              # Create the Bag
   x = BaggableInt(11)      # Create a BaggableInt x
   bag["x"] = x             # Stuff x into the Bag, keyed by the string "x"

Fetching Data from the Bag
--------------------------

Fetching data from the ``Bag`` requires knowledge of the object key and the
object type in C++.  In Python, only the key is required.

C++ Example
^^^^^^^^^^^

Fetching an integer pushed into the ``Bag``:

.. code-block:: c++

   // Get a const reference
   const BaggableInt& xr = b.Get<BaggableInt>("x");

   // Get a const shared_ptr, which stores a reference count. The name
   // BaggableIntConstPtr has been defined by SHARED_POINTER_TYPEDEFS
   BaggableIntConstPtr xp = b.Get<BaggableIntConstPtr>("x");

Python Example
^^^^^^^^^^^^^^

Fetching an integer pushed into the ``Bag``:

.. code-block:: python

   # Get the integer value directly from the Bag, first checking that the key
   # is found:
   if "x" in bag:
       xv = bag["count"].value

