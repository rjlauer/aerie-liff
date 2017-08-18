Introduction
============

The HAWC software, called the Analysis and Event Reconstruction Integrated
Environment (AERIE) provides a framework intended for processing of HAWC events
and for subsequent analysis.

The software is structured as a set of interdependent C++ projects glued
together by a central core or "framework." The core provides a run loop for
analyzing batches of data, hooks for physicists to plug in their own
algorithms, data classes to store simulated and reconstructed data, and
libraries to handle common tasks such as geometry, astronomical coordinates, or
time conversions.  Other projects are provided to handle specialized
tasks like disk I/O, track reconstruction, and map making.

The AERIE run loop can be driven with C++ "main" programs or python scripts.
Templates for running popular applications can be found in the examples folders
inside various projects.

Software Components
-------------------

There are several major components to AERIE:

1. **The HAWCNest Framework** - A central object that registers and initializes
services. It does not edit data.

2. **The Data Structures** - An in-memory representation of the data that can
be edited by services.

3. **Services** - User code which can be used to edit data in a processing loop
("modules") or provide stand-alone calculations, like random number generation
or astronomical transformations.

4. **MainLoop** - A special service that defines the flow of control for data
processing.

5. **Applications** - A suite of programs used for basic analysis of HAWC data,
like map-making or estimation of energy spectra.

Beginning users will typically be most interested in applications.
Intermediate users will be interested in data structures and data I/O.
Advanced users write their own services and, for specialized tasks, edit the
HAWCNest framework.
