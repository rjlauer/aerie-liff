.. _rng_service:

RNG Service
===========

The `rng-service <../../doxygen/html/group__rndm__gen.html>`_ project provides
an interface to random number generation for user code in AERIE, as well as a
`standard implementation <../../doxygen/html/classStdRNGService.html>`_ of the
interface.  Details are provided below.

.. contents:: 
   :local:
   :backlinks: top

Motivation
----------

This project was written to avoid the proliferation of multiple random number
generators in AERIE.  Multiple external dependencies of AERIE, including GSL,
ROOT, and Boost, all supply functions or classes for random number generation.
In addition, the typical C++ library comes with uniform RNG functions such as
`rand <http://www.cplusplus.com/reference/cstdlib/rand/>`_ and `rand48
<http://man7.org/linux/man-pages/man3/drand48.3.html>`_ that many users have
probably used before.

Unfortunately, all of these options raise the possibility of proliferating RNGs
in the AERIE code, each with different seeding schemes and performance issues.
Allowing several different kinds of RNGs into the code is a recipe for chaos in
the simulation. Therefore, users are required to use random numbers generated
from the interface defined in this project.  **All other generators are
banned!**

Implementation
--------------

The implementation of ``rng-service`` defines a set of functions for different
kinds of random distributions useful for HAWC analyses, as well as a simple
scheme for choosing a random seed.

Interface
^^^^^^^^^

The RNG interface is meant to provide a list of standard distributions used
often in AERIE. These include:

*Uniform*
   Random uniform double-precision floating point numbers between a and b.

*Gaussian*
   Gaussian random numbers of mean mu and width sigma.

*Lognormal*
   A logarithmic Gaussian of mean mu and width sigma.

*Rician*
   A Rice distribution of mean nu and width sigmaa (for noise power in cables
   and radio signals).

*Exponential*
   An exponential distribution with a rate/decay constant.

*Poisson*
   Random integers drawn from a Poisson distribution

*Binomial*
   Random integers drawn from a binomial distribution of N Bernoulli trials
   with probability p

*Power Law*
   Draws from a power law with spectral index n on the interval [a,b]

*Cutoff Power Law*
   Draws from a power law with spectral index n on the interval [a,b], with an
   exponential cutoff characterized by a rate/decay constant

Seeding
^^^^^^^

In the `standard implementation <../../doxygen/html/classStdRNGService.html>`_
of the RNG interface, three kinds of seeds are allowed:

#. A number specified by the user. Useful to produce repeatable tests.
#. Zero (0), which generates a seed using the system clock via call to the C++ function `time <http://www.cplusplus.com/reference/ctime/time/>`_. To reduce the possibility of repeated seeds when running on a computing cluster, the process ID is added to the time via a call to the UNIX function `getpid <http://man7.org/linux/man-pages/man2/getpid.2.html>`_.
#. A negative number (e.g., -1), which obtains a seed from the "unlimited" system entropy pool in `/dev/urandom <http://en.wikipedia.org/?title=/dev/random>`_. The entropy pool is typically used for cryptographic random number generation, and so is probably overkill for AERIE... but it should help avoid repeated seeds when running many parallel jobs on a computing cluster.  

Examples
--------

C++ Example
^^^^^^^^^^^

The random number services are registered with the :ref:`hawcnest` framework
and can be instantiated as follows:

.. code-block:: c++

   #include <hawcnest/HAWCNest.h>
   #include <rng-service/RNGService.h>

   ...

   // Create the framework instance and initialize the RNG service
   HAWCNest nest;

   nest.Service("StdRNGService", "rng")
     ("seed", -1);

   nest.Configure();

   // Get a reference to the RNG service and make some numbers
   const RNGService& rng = GetService<RNGService>("rng");

   double u = rng.Uniform(10, 25);    // uniform number in [10,25]
   double e = rng.Exponential(0.5);   // exp(-x/2)
   ...

Python Example
^^^^^^^^^^^^^^

Python bindings to the RNG service are also available.  The syntax is almost
the same as in C++, except that service parameters like *seed* are python
keyword arguments:

.. code-block:: python

   from hawcnest import hawcnest, data_structures, rng_service
   from HAWCNest import HAWCNest

   # Create the framework instance and initialize the RNG service
   nest = HAWCNest()

   nest.Service("StdRNGService", "rng",
     seed=-1)

   nest.Configure()

   # Get a reference to the RNG service and make some numbers
   rng = rng_service_GetService("rng")

   u = rng.Uniform(10, 25)    # uniform number in [10,25]
   e = rng.Exponential(0.5)   # exp(-x/2)
   ...

