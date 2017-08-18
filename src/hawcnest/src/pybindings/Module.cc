/*!
 * @file Module.cc 
 * @brief Python bindings for the C++ Module base class.
 * @author Segev BenZvi 
 * @date 3 Jan 2012 
 * @version $Id: Module.cc 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#include <hawcnest/processing/PythonModule.h>

using namespace boost::python;

// Pointers to virtual member functions
namespace {
  Configuration (*DefaultConfigurationFcn)(const PythonModule&) =
    &PythonModuleCallback::DefaultConfiguration;

  void (*InitializeFcn)(const PythonModule&, const Configuration&) =
    &PythonModuleCallback::Initialize;

  Module::Result (*ProcessFcn)(const PythonModule&, BagPtr) =
    &PythonModuleCallback::Process;

  void (*FinishFcn)(const PythonModule&) =
    &PythonModuleCallback::Finish;
}

void
pybind_hawcnest_PythonModule()
{
  scope inModule = class_<PythonModule, PythonModuleCallback>
    ("Module",
     "Base class for modules that can be inserted into the processing stream.")
    .def("DefaultConfiguration", DefaultConfigurationFcn) 
    .def("Initialize", InitializeFcn) 
    .def("Process", ProcessFcn) 
    .def("Finish", FinishFcn) 
    ;

  enum_<Module::Result>("ModuleResult")
    .value("CONTINUE", Module::Continue)
    .value("FILTER", Module::Filter)
    .value("TERMINATE", Module::Terminate)
    .export_values()
    ;

}

