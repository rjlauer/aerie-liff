.. _hawcnest_services:

Services
========

.. contents::
   :local:
   :backlinks: top

Definition of Terms
-------------------

In the `HAWCNest <../../doxygen/html/classHAWCNest.html>`_ framework, the term
*Service* refers to pieces of code which define an *Interface* for some
calculation or task and then implement the calculation using `subclasses
<http://en.wikipedia.org/wiki/Inheritance_(object-oriented_programming)#Subclasses_and_superclasses>`_
of the interface.  This is standard `object-oriented programming
<http://en.wikipedia.org/wiki/Inheritance_(object-oriented_programming)>`_, a
design style now used in most high-level programming languages.

Object-Oriented Programming: Polymorphism
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For those not familiar with object-oriented design, the concept is best
illustrated with a simple example from [Pre2010]_:

.. code-block:: c++

   // Define a class interface with a member DoSomething.
   // The class is "abstract" (you can't create Interface objects)
   class Interface {
     public:
       virtual void DoSomething() = 0;
   };

   // A subclass of Interface. DoSomething must be defined.
   class Something : public Interface {
     public:
       virtual void DoSomething() { printf("Hello, world!\n"); }
   };

   // Another subclass of Interface, where DoSomething is a little different
   class Algo : public Interface {
     public:
       virtual void DoSomething() { printf("¡Hola mundo!"); }
   };

   // A function that takes a pointer to the Interface base class and uses
   // run-time type information to figure out if it's looking at the Something
   // child class or the Algo child class
   void do_something(Interface* i)
   {
     i->DoSomething();
   }

   int main()
   {
     Interface* s = new Something();      // Create a Something object
     Interface* a = new Algo();           // Create an Algo object

     do_something(s);                     // Prints "Hello, world!"
     do_something(a);                     // Prints "¡Hola mundo!"
   }

In this C++ example, ``Interface`` defines a class with an empty member
function called ``DoSomething()``.  The classes ``Something`` and ``Algo``
inherit from ``Interface`` and implement different versions of
``DoSomething()``.  Finally, the function ``do_something`` takes an an
``Interface`` pointer argument and calls ``DoSomething()`` using the pointer.
When we pass a ``Something`` object or an ``Algo`` object to ``do_something``,
the program knows the ``Interface*`` argument is pointing to a ``Something`` or
an ``Algo`` and calls the appropriate version of ``DoSomething()``.

This example exhibits four crucial features of object-oriented programming in
C++ used by Services:

#. In places where we anticipate having multiple implementations or ways of doing things, an interface is defined in a base class (``Interface``).
#. The various approaches are implemented via inheritance of the base class.
#. Code which calls this interface (``do_something``) is ignorant of the implementation or algorithm used in the subclasses.
#. The actual behavior is selected by choosing which of the implementations of the interface are used.

In computer science, the technique of defining a single interface for objects
of different types is called `polymorphism
<http://en.wikipedia.org/wiki/Polymorphism_(computer_science)>`_. The mechanism
by which C++ determines if ``Interface*`` points to a ``Something`` or an
``Algo`` object is known as `run-time type identification
<http://en.wikipedia.org/wiki/Run-time_type_information>`_. The ``HAWCNest``
framework uses this technology extensively in the Service concept.

Services in HAWCNest
--------------------

In the HAWCNest framework, all user code is implemented as a Service.  That is,
users define an *interface* with some list of functions. They then define one
or more subclasses of the interface and register the subclasses with the base
`HAWCNest <../../doxygen/html/classHAWCNest.html>`_ class.

Services are used to implement important core functionality in AERIE, such as
random number generation (see :ref:`rng_service`) or astronomical
transformations (see :ref:`astro_service`).  For data processing, the framework
has a set of specialized services known as :ref:`hawcnest_modules`.  Since the
average user is mostly interested in event reconstruction and analysis, most
user code is implemented as a subclass of the ``Module`` interface.

After creating a subclass of a Service interface, the subclass can be
*registered* in the `HAWCNest <../../doxygen/html/classHAWCNest.html>`_ class
using a macro called ``REGISTER_SERVICE``.  Objects of the service subclass are
instantiated using the ``HAWCNest`` class, and they can be accessed directly
with a global function called ``GetService``.

All of these features are illustrated below with a simple example.

Defining an Interface
^^^^^^^^^^^^^^^^^^^^^

To demonstrate how services are used in the ``HAWCNest`` framework, let's
define an interface for a random number generator.  We'll work in C++.  Our
random number generator interface will let us produce uniform and exponential
random numbers.

.. code-block:: c++

   // Define an "abstract" interface that random number services must have
   class RandomNumberService {
     public:
       virtual double Uniform() const = 0;
       virtual double Exponential() const = 0;
   };

It's that simple.  Just define the interface desired. The interface is meant to
be `abstract
<http://en.wikipedia.org/wiki/Virtual_function#Abstract_classes_and_pure_virtual_functions>`_,
so we don't need to define the bodies of the ``Uniform()`` and
``Exponential()`` functions at this stage.

Defining and Registering Services
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Now we want to actually create working random number generators. To do this, we
subclass ``RandomNumberService`` and register the subclass with the
framework:

.. code-block:: c++

   #include <hawcnest/Service.h>
   #include <cstdint>

   // Implement a linear congruential generator
   class LCGRandomService : public RandomNumberService {
     public:
       typedef RandomNumberService Interface; 

       // Configuration function for default parameters
       Configuration DefaultConfiguration();

       // Initialization of parameters
       void Initialize(const Configuration& config);

       double Uniform() const
       { 
         xn_ = (a_*xn_ + c_) % m_;
         return double(xn_) / m_;
       }

       double Exponential() const
       { return -std::log(Uniform()); }

     private:
       uint64_t m_;    ///< Modulus of the LCG
       uint64_t a_;    ///< Multiplier of the LCG
       uint64_t c_;    ///< Increment of the LCG
       uint64_t xn_;   ///< Nth value in the LCG random number sequence
   };

   REGISTER_SERVICE(LCGRandomService);

In this example we defined a C++ implementation of a `linear congruential
generator (LCG) <http://en.wikipedia.org/wiki/Linear_congruential_generator>`_
of random numbers.  The class has four private *state variables* ``m_``,
``a_``, ``c_``, and ``xn_`` to calculate the random number sequence.  It
inherits publicly from ``RandomNumberService`` and contains concrete
definitions of the functions ``Uniform()`` and ``Exponential()``.

There are several additional features that need to be explained here:

``typedef RandomNumberService Interface``
   This line tells the HAWCNest framework that the ``LCGRandomService`` class
   implements the ``RandomNumberService`` interface. It is not required for C++
   inheritance but is used in the service registry, which we describe below.

``Configuration DefaultConfiguration()``
   This is a function that allows us to pass *configuration parameters* to the
   ``LCGRandomService`` using the HAWCNest framework. We describe how this is
   done below.

``void Initialize(const Configuration& config)``
   This member function allows the HAWCNest framework to initialize the
   service.

``REGISTER_SERVICE(LCGRandomService)``
   This is a macro that registers the class name ``LCGRandomService`` with a
   global service registry in the framework.  Users can print a list of all
   registered services and their configuration parameters in AERIE.

Service Parameters
^^^^^^^^^^^^^^^^^^

Once a service is registered with the ``HAWCNest`` framework, we can use an
instance of the ``HAWCNest`` class to initialize the service.  This is
described in the next section.  Here, we show how users can create default
configuration parameters for their service and use them to initialize their
classes. We use a class called `Configuration
<../../doxygen/html/classConfiguration.html>`_  to pass service parameters back
and forth between the framework and the classes.

.. code-block:: c++

   Configuration
   LCGRandomService::DefaultConfiguration()
   {
     Configuration c;

     c.Parameter<uint64_t>("modulus", 1<<32);
     c.Parameter<uint64_t>("multiplier", 1664525);
     c.Parameter<uint64_t>("increment", 1013904223);
     c.Parameter<uint64_t>("seed", 12345);

     return c;
   }

   void
   LCGRandomService::Initialize(const Configuration& c)
   {
     c.GetParameter("modulus", m_);       // modulus of LCG
     c.GetParameter("multiplier", a_);    // multiplier of LCG
     c.GetParameter("increment", c_);     // increment of LCG
     c.GetParameter("seed", xn_);         // seed (first value in sequence)
   }

The function ``DefaultConfiguration()`` allows us to define parameters for the
LCG random number generator and can include default values.  The function
``Initialize()`` takes these values and uses them to initialize the private
state variables in the generator.

The utility of doing things this way is probably not obvious.  You might wonder
why we don't define the default parameters in a C++ constructor and have
getter/setter functions to change them. The answer is that these two functions
help provide a uniform interface for setting up **all** services via the
``HAWCNest`` framework.  We describe how this is done next.

Creating Service Instances
^^^^^^^^^^^^^^^^^^^^^^^^^^

Now that we have our service defined, we want to create an instance and use it.
This is done using the ``HAWCNest::Service`` function:

.. code-block:: c++

   #include <hawcnest/HAWCNest.h>
   ...

   int main()
   {
     // Create an instance of the framework
     HAWCNest nest;

     // Create an instance of the LCGRandomService. We can use the string
     // "LCGRandomService" because the class was registered with the framework.
     // The instance will be created with default parameters.
     nest.Service("LCGRandomService", "rng1");

     // Create a second RNG with a different random number seed
     nest.Service("LCGRandomService", "rng2")
       ("seed", 41);

     // Create a third RNG using a template version of the Service function.
     // Keep the default seed but use different magic numbers for the LCG
     nest.Service<LCGRandomService>("rng3")
       ("modulus", 1<<32)
       ("multiplier", 22695477)
       ("increment", 1);

     // Configure the LCG instances
     nest.Configure();
     ...
   }

We created three different instances of the ``LCGRandomService`` and
initialized its parameters using the ``Service`` function.  All services can be
initialized in this way.

Note how in the call to ``Service`` we specify the name of the class (either as
a string or as a template argument) and a name for the instance.  The instance
name is used later when we want to access a particular object.  We discuss this
next.

Accessing Service Instances
^^^^^^^^^^^^^^^^^^^^^^^^^^^

To access a service and use it, we get a constant reference to its base class
through a global function called ``GetService``.  Any particular object can be
accessed using the name given to the object in the call to ``Service``.  For
example,

.. code-block:: c++

   #include <hawcnest/HAWCNest.h>
   ...

   int main()
   {
     // Create an instance of the framework
     HAWCNest nest;

     // "rng1", "rng2", "rng3" are created and configured, etc., etc.
     ...

     nest.Configure();

     // Access "rng1" and get a uniform number:
     const RandomNumberService& rng1 = GetService<RandomNumberService>("rng1");
     printf("%g", rng1.Uniform());

     // Access "rng2" and get another uniform number:
     const RandomNumberService& rng2 = GetService<RandomNumberService>("rng2");
     printf("%g", rng2.Uniform());

     // Access "rng3" and get an exponential number:
     const RandomNumberService& rng3 = GetService<RandomNumberService>("rng3");
     printf("%g", rng3.Exponential());

     ...
   }

In this example, we access our ``LCGRandomService`` objects via a ``const``
reference to the ``RandomNumberService`` base class. Because of run-time type
identification, the calls to ``Uniform`` and ``Exponential`` are evaluated
correctly.

Because the service objects are registered with the ``HAWCNest`` framework and
can be called using the global ``GetService`` function, we can access any named
service instance this way in any piece of user code.

