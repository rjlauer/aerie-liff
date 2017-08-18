Building with CMake
-------------------

.. contents::
   :local:
   :backlinks: top

`CMake <http://www.cmake.org/>`_ is a cross-platform build system that lets
users write build scripts (i.e., instructions for compiling and linking
libraries and programs) in a single scripting language.  CMake runs those
scripts and creates Makefiles customized for your particular computer
architecture and operating system.  We use CMake in AERIE because it is
relatively easy to learn and maintain.

Configuration and Compilation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When you download the AERIE software (see :ref:`downloading_AERIE`), you will
see that the source code has two directories:

.. code-block:: bash

  build
  src

The :code:`src` directory contains the AERIE source code, while the
:code:`build` directory will be empty at first. Building AERIE requires two
steps: configuration with CMake followed by building/compilation with Make. In
practice the steps are quite simple:

#. Make sure your user environment is set up to use the external dependencies installed with :code:`ape` (see :ref:`downloading_AERIE`).
#. Move into the build directory.
#. Configure the build by passing various options to CMake (or using the default options).
#. Call :code:`make` to compile the code, and if desired, :code:`make install` to install it.

In a shell, your command sequence will look something like this:

.. code-block:: bash

  $> cd build
  $> cmake -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_BUILD_TYPE=Release ../src
  $> make -j2
  $> make install

Configuration Options
^^^^^^^^^^^^^^^^^^^^^

CMake allows for many possible command line options so you can customize the
AERIE build for your system.  However, most users will care about only two
options:

#. :code:`CMAKE_INSTALL_PREFIX`: a *relative* or *absolute* path to the directory where you wish to install AERIE
#. :code:`CMAKE_BUILD_TYPE`: the kind of build you require, which depends on whether or not you wish to develop and debug the code or run in production mode.

Five build types are supported in AERIE:

#. **Debug**: compile the source code with no optimization and debugging symbols enabled (:code:`-O0 -g`).
#. **Release**: debugging is disabled and optimization is set to the highest value that allows strict compatibility with C/C++ language standards (:code:`-O3 -DNDEBUG`).
#. **RelWithDebInfo**: enable debugging symbols but disable asserts.  Optimization is set to the second-highest value (:code:`-O2 -g -DNDEBUG`).
#. **MinSizeRel**: debugging is disabled and the code is optimized for size (:code:`-Os -DNDEBUG`).
#. **FastRel**: the most aggressive optimization of the code possible (e.g., :code:`-Ofast -DNDEBUG` in :code:`clang`). The resulting machine code will run fast, but may violate the strict C/C++ standard and lead to unexpected floating-point round-offs (among other side effects).

Developers will likely want to compile AERIE in **Debug** or **RelWithDebInfo**
mode to take advantage of debugging capability.  Average users and
simulation/data production will want to run in **Release** mode, which should
execute in about 50%-60% of the time required by the software when compiler
optimization is turned off.

The **FastRel** option is reserved for situations where speed is absolutely
critical but a bit of floating-point accuracy can be sacrificed. Handling of
floating-point exceptions is also turned off. A good example of this case is
the online reconstruction.

The **MinSizeRel** option optimizes the AERIE programs and libraries for size.
There is not a clear use for this in AERIE but it is a standard option in CMake
and is included here for completeness.
