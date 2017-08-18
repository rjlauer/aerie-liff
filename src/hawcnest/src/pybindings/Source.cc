/*!
 * @file Source.cc 
 * @brief Python bindings for the data Source class.
 * @author Segev BenZvi 
 * @date 3 Jan 2012 
 * @version $Id: Source.cc 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#include <hawcnest/processing/PythonSource.h>

using namespace boost::python;

// Pointers to virtual member functions
namespace {
  Configuration (*DefaultConfigurationFcn)(const PythonSource&) =
    &PythonSourceCallback::DefaultConfiguration;

  void (*InitializeFcn)(const PythonSource&, const Configuration&) =
    &PythonSourceCallback::Initialize;

  BagPtr (*NextFcn)(const PythonSource&) =
    &PythonSourceCallback::Next;

  void (*FinishFcn)(const PythonSource&) =
    &PythonSourceCallback::Finish;
}

void
pybind_hawcnest_PythonSource()
{
  class_<PythonSource, PythonSourceCallback>
    ("Source",
     "Base class for Sources that inject data into the processing stream.")
    .def("DefaultConfiguration", DefaultConfigurationFcn) 
    .def("Initialize", InitializeFcn) 
    .def("Next", NextFcn) 
    .def("Finish", FinishFcn) 
    ;
}

