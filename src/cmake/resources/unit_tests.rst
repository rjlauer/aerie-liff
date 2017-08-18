Unit Testing
------------

.. contents::
   :local:
   :backlinks: top

Approaches to Testing
^^^^^^^^^^^^^^^^^^^^^

When testing software, there are four basic kinds of tests one can design:

#. **Unit Tests**: tests of functions, class interfaces, and software features.
#. **Regression Tests**: tests for the elimination (or reappearance) of known bugs.
#. **Integration Tests**: tests of the interaction between software units and with external resources (e.g., databases).
#. **Smoke Tests**: "activation tests" to see if programs will blow up when used.

Most of the tests in AERIE are of the first type.  The C++ modules are tested
using the :code:`boost::test` framework from the `Boost C++ Library
<http://www.boost.org/doc/libs/release/libs/test/>`_. Python scripts and
bindings don't require a testing framework.

The C++ and Python tests are stitched together with the `CTest
<http://www.cmake.org/Wiki/CMake/Testing_With_CTest>`_ tool available from
CMake. To run the tests, go to the AERIE build directory and execute

.. code-block:: bash

  $> make
  $> make test

For more verbose output, run CTest directly:

.. code-block:: bash

  $> make
  $> ctest -V

The unit tests are run after every AERIE commit by a `buildbot
<http://buildbot.hawc-observatory.org/>`_ hosted at the University of Maryland.
Developers should also get into the habit of running the tests after making
changes to projects.

Building a Test in C++
^^^^^^^^^^^^^^^^^^^^^^

AERIE tests are created at the project level.  In the project-level
:code:`CMakeLists.txt` file, there is a macro called :code:`HAWC_ADD_TEST` used
to set up tests.  Tests can include C++ unit tests, python scripts, and
configuration and data files.  An example of a call to the macro is shown below:

.. code-block:: CMake

  HAWC_ADD_TEST (detector-configurations
   SOURCES src/test/src1.cc src/test/src2.cc
   CONFIGS src/test/conf.xml.in
   src/test/test-config
   SCRIPTS src/test/script1.py src/test/script2.py
   USE_PROJECTS hawcnest data-structures detector-service
   USE_PACKAGES Boost Root
   NO_PREFIX)

To write a unit test in C++, developers will need to use the
:code:`boost::test` framework.  It is not very hard if you copy and edit an
existing test to create a new test.  For example, one of the unit tests in the
:ref:`detector_service` project looks like this:

.. code-block:: C++

  BOOST_AUTO_TEST_SUITE(StdDetectorServiceTest)
  // ___________________________________________________________________________
  // Check that exceptions are raised when configuring with an empty param list
  BOOST_AUTO_TEST_CASE(EmptyParameterList)
  {
    HAWCNest nest;
    nest.Service("StdDetectorService", "detS");
    BOOST_CHECK_THROW(nest.Configure(), runtime_error);
  }
  BOOST_AUTO_TEST_SUITE_END

Building a Test in Python
^^^^^^^^^^^^^^^^^^^^^^^^^

Making a test in python is even easier. Just add assert statements for
quantities that need to be checked:

.. code-block:: python

  from hawc import hawcnest, data_structures, detector_service
  from hawc.data_structures import *
  from HAWCNest import HAWCNest
  import os

  # Module setup here:
  nest = HAWCNest()
  nest.Service("StdDetectorService", "det",
      configFile="/".join([os.environ["TEST_DIR"], "testHAWCConfig.xml"]),
      validateConfigs=False)
  nest.Configure()

  ds = detector_service.GetService("det")
  det = ds.GetDetector(TimeStamp(1111111111))

  # Testing begins here:
  assert(det.name == "HAWC")
  assert(det.version == "1.0")
  ...

Integration Testing
^^^^^^^^^^^^^^^^^^^

The testing framework allows test programs to access databases, XML
configuration files, and XCD data files.

During test compilation, configuration files specified in the
:code:`HAWC_ADD_TEST` macro are copied to the folder :code:`libtest` in the
AERIE build directory.  This directory is exposed to the unit tests as the
environment variable :code:`TEST_DIR`.  See existing unit tests in the AERIE
projects for examples of how to access data and config files in the
:code:`$TEST_DIR` area.
