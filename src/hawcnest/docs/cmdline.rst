.. _hawcnest_cmdline:

Command Line Configuration
==========================

.. contents::
   :local:
   :backlinks: top

Motivation
----------

Handling command-line options and arguments (and printing a help/context menu
for these arguments)  is a critical part of any Linux program.  In AERIE, a
`CommandLineConfigurator
<../../doxygen/html/classCommandLineConfigurator.html>`_ class in the `hawcnest
<../../doxygen/html/group__hawcnest__api.html>`_ project is used to provide a
standard interface for defining command line options.  The class simplifies the
definition of command-line options by wrapping the ``boost::program_options``
library, making commands like

.. code-block:: sh

   $ progname -i -v --output=out.root in1.txt in2.txt in3.txt

very easy to implement. This interface hides some pitfalls inherent in
``boost::program_options`` and helps prevent the proliferation of many
heterogeneous command-line parsers in the AERIE code.

Note that the ``CommandLineConfigurator`` is used in C++ programs.  Users
running the framework from python scripts should just use the `argparse
<https://docs.python.org/3/library/argparse.html>`_ module.

Default Options
---------------

The command line configurator comes with five default options that are
relatively universal, and so are "pre-defined" and available to all programs in
AERIE.

Help
^^^^

By typing ``-h`` or ``--help`` users will print a help menu detailing the
default options and any additional program options and arguments specified by
the user.

Version
^^^^^^^

Typing ``-v`` or ``--version`` in any program that uses the command line
configurator will print the current version number of AERIE.

Verbosity
^^^^^^^^^

Users can turn the verbosity of the AERIE :ref:`hawcnest_logging` system up or
down by specifying ``-V`` or ``--verbosity`` on the command line, followed by a
numerical code that corresponds to one of the AERIE logging levels.  See the
help menu for a definition of the code.

Floating Point Exceptions
^^^^^^^^^^^^^^^^^^^^^^^^^

By specifying ``-x`` or ``--fpexcept`` followed by a numerical code, users can
allow the compiler to turn on one or more `floating point exceptions
<http://en.wikipedia.org/wiki/IEEE_floating_point#Exception_handling>`_ such as
overflow or divide-by-zero exceptions. This is useful for tracking down the
origin of `NaNs <http://en.wikipedia.org/wiki/NaN>`_ and `infs
<http://en.wikipedia.org/wiki/Infinity#Computing>`_ in reconstruction results.
It is mostly of use to experts.

Function Call Time Logging
^^^^^^^^^^^^^^^^^^^^^^^^^^

Specifying the option ``--timelog`` on the command line will cause the
:ref:`hawcnest_logging` system to include the date and time of function calls
in logged output. This is mainly useful for debugging of the online DAQ.

Adding Program Options
----------------------

The `CommandLineConfigurator
<../../doxygen/html/classCommandLineConfigurator.html>`_ class implements three
types of command line arguments:

#. Flags
#. Options
#. Positional options

The exact meaning of these types is defined below.

Flags
^^^^^

Flags are simple command-line arguments whose presence switches on features in
a program. For example, in the UNIX long directory listing

.. code-block:: sh

   $ ls -l

the option ``-l`` is a flag. To implement a flag in the C++ command line
parser, one writes something like this:

.. code-block:: c++

   #include <hawcnest/CommandLineConfigurator.h>
   ...

   int main(int argc, char* argv[])
   {
     CommandLineConfigurator cl;
     cl.AddFlag("blue,b", "Set the blue flag.");  // -b or --blue sets the flag
     if (!cl.ParseCommandLine(argc, argv))
       return 1;

     if (cl.HasFlag("blue")) {
       ...
     }
     return 0;
   }

Options
^^^^^^^

Options, as opposed to flags, are named command line arguments with values. The
variable type of the option is specified at compile time, and the value
actually passed via the command line is lexically cast to the desired type at
run time.  For example,

.. code-block:: c++

   #include <hawcnest/CommandLineConfigurator.h>
   ...

   int main(int argc, char* argv[])
   {
     // Define three options with default values
     CommandLineConfigurator cl;
     cl.AddOption<int>("number,n", 5, "This could be any number, really.");
     cl.AddOption<string>("string,s", "Hello!", "Just a string, nothing more.");
     cl.AddOption<double>("double,d", 0., "A double-precision float");
     if (!cl.ParseCommandLine(argc, argv))
       return 1;

     int n = cl.GetArgument<int>("number");
     double d = cl.GetArgument<double>("double");
     string s = cl.GetArgument<string>("string");
     ...

     return 0;
   }

This program defines three options with default values, so the options don't
have to be specified on the command line to be used later in the program. In
the usual POSIX style, all of the options are also given short and long names
so that

.. code-block:: sh

   $ progname -n 10 -d 3.14159 -s "Hello"

is equivalent to

.. code-block:: sh

   $ progname --number=10 --double=3.14159 --string="Hello"

Positional Options
^^^^^^^^^^^^^^^^^^

A positional option is an unnamed command line argument.  It is very convenient
for certain types of arguments, such as lists of input files.  For example, in
the command line

.. code-block:: sh

   $ progname -i -v --output=out.root in1.txt in2.txt in3.txt

the three text files are part of a positional option. This particular example
could have been implemented as follows:

.. code-block:: c++

   #include <hawcnest/CommandLineConfigurator.h>
   ...

   int main(int argc, char* argv[])
   {
     // Define a vector positional option which can take an arbitrarily large
     // number of inputs (ok, up to a point)
     CommandLineConfigurator cl;
     cl.AddPositionalOption<vector<string> >("input", "Input file name(s)");
     ...
     if (!cl.ParseCommandLine(argc, argv))
       return 1;

     vector<string> files = cl.GetArgument<vector<string> >("input");
     ...

     return 0;
   }

Due to the ambiguity of parsing positional options on the command line, **only
one such option is allowed per program**.
