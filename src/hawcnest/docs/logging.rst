.. _hawcnest_logging:

Logging
=======

.. contents::
   :local:
   :backlinks: top

Motivation
----------

A C++ `logging system <../../doxygen/html/classLogger.html>`_ is provided in
the `hawcnest <../../doxygen/html/group__hawcnest__api.html>`_ project to
ensure that basic information, warning messages, and errors are handled in a
consistent way.  Any time you would call ``printf`` or ``cout`` in your
personal code, you should use these logging functions instead.

The logger defines seven functions (see below) for printing out messages with
different kinds of priority.  If you want to hide or show some of these
messages, you can turn the verbosity of the logger up or down from the command
line.  The logging functions send log messages to the standard error stream of
your console.  Simply redirect standard error to write the logs to a file.

How to Use the Logging System
-----------------------------

There are several logging levels or "priorities" available:

===================== ===============================================================================
Function              Priority/Purpose
===================== ===============================================================================
``log_trace``         Very low-level logging that prints everything the framework is doing.
``log_debug``         Print statements for debugging (for experts, normally hidden).
``log_info``          General information that prints by default during normal running.
``log_warn``          Warnings to indicate possible problems with calculations.
``log_error``         Recoverable error encountered. Indicates a serious issue that needs attention.
``log_fatal``         Non-recoverable error, resulting in an exception and a halt to data processing.
``log_fatal_nothrow`` Non-recoverable error that calls ``std::exit(EXIT_FAILURE)``.
===================== ===============================================================================

The default logging level is **INFO**.  Users should be cautious about
overusing ``log_info`` or the AERIE logs will be filled with useless messages.
As a general rule, if you are putting ``printf``/``cout`` statements into your
code to see what it is doing, you should probably be using ``log_debug``.

Using Standard C++ I/O Manipulators
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The logging functions are essentially wrappers for ``std::ostream``, so you can
use the typical C++ syntax (including I/O manipulators) when writing your log
messages.  For example,

.. code-block:: c++

   #include <iostream>
   #include <iomanip>
   #include <hawcnest/Logging.h>

   int main()
   {
     // Print a message to stdout
     int x = 5;
     std::cout << "Sorry, x = " << x << " is invalid" << std::endl;

     // Print the same message to stderr using log_error
     log_error("Sorry, x = " << x << " is invalid");

     return 0;
   }

As you can see, when using the AERIE logger the syntax is identical to using
``std::cout`` and ``std::cerr``, except that you don't need to specify the I/O
object or carriage return character.  The logger ensures that carriage returns
are included and the I/O buffer is flushed after each message. Note that you
can include any I/O manipulator such as ``std::setfill`` in these messages.

Using Boost Format
^^^^^^^^^^^^^^^^^^

For users who prefer compact ``printf``-style formatting, we suggest you use
the ``boost::format`` library, which is available as a dependency of AERIE.
For example,

.. code-block:: c++

   #include <cstdio>
   #include <hawcnest/Logging.h>
   #include <boost/format.hpp>

   using boost::format;

   int main()
   {
     int x = 5;

     // Print a formatted message with printf
     printf("Sorry, x = %d is invalid\n", x);

     // Print a formatted log using boost::format
     int x = 5;
     log_error(format("Sorry, x = %d is invalid") % x);

     return 0;
   }

Setting Logging Thresholds
--------------------------

The verbosity of the logger can be set from the C++ class `Logger
<../../doxygen/html/group__hawcnest__api.html>`_.  There are six thresholds
that you can set in the logging system:

========== ===== =====================
Level Enum Value Minimum Logging Level
========== ===== =====================
TRACE      0     ``log_trace``
DEBUG      1     ``log_debug``
INFO       2     ``log_info``
WARN       3     ``log_warn``
ERROR      4     ``log_error``
FATAL      5     ``log_fatal``
========== ===== =====================

For a given threshold, only logging messages of that level/priority or higher
will be printed.  Thus, it is easy for users to turn off all messages except
for errors:

.. code-block:: c++

   Logger::GetInstance().SetDefaultLogLevel(Logger::ERROR);

By default, the log level is set to **INFO**, so trace and debug messages are
not printed.  Users interested in viewing debug messages can manually reset the
logging level via the call

.. code-block:: c++

   Logger::GetInstance().SetDefaultLogLevel(Logger::DEBUG);

However, we recommend you do not hardcode the logging level into your projects
with this function call.  Instead, the :ref:`hawcnest_cmdline` provides a
default command-line option to reset the logging level at runtime. We strongly
suggest you use this option instead of directly setting the log verbosity in
your code.
