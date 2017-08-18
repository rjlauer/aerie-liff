/*!
 * @file Submodule.cc 
 * @brief HAWCNest python module definition.
 * @author John Pretz 
 * @date 19 Mar 2010 
 * @version $Id: Submodule.cc 18669 2014-02-02 04:15:20Z sybenzvi $
 */
#include <boost/python.hpp>
#include <string>
#include <hawcnest/LoadProject.h>


using namespace boost::python;

void pybind_hawcnest_Baggable();
void pybind_hawcnest_Bag();
void pybind_hawcnest_Configuration();
void pybind_hawcnest_PythonModule();
void pybind_hawcnest_PythonSource();
void pybind_hawcnest_HAWCNest();
void pybind_hawcnest_HAWCUnits();
void pybind_hawcnest_Logging();
void pybind_hawcnest_MainLoop();
void pybind_hawcnest_SoftwareVersion();

/// Library loading function exposed to boost::python (called "load")
void
load_impl(const std::string& name, bool doVerbose = false)
{
  load_project(name, doVerbose);
}

BOOST_PYTHON_FUNCTION_OVERLOADS(load_overloads, load_impl, 1, 2);

BOOST_PYTHON_MODULE(hawcnest)
{
  // Shower user-defined and python docstrings; suppress C++ signatures
  docstring_options docOpts(true, true, false);

  load_project("hawcnest", false);

  def("load", load_impl,
      load_overloads(args("name", "doVerbose"),
        "Load a DLL by name, omitting the lib prefix and .so/.dylib suffix."));

  pybind_hawcnest_Baggable();
  pybind_hawcnest_Bag();
  pybind_hawcnest_Logging();
  pybind_hawcnest_Configuration();
  pybind_hawcnest_MainLoop();
  pybind_hawcnest_PythonModule();
  pybind_hawcnest_PythonSource();
  pybind_hawcnest_HAWCNest();
  pybind_hawcnest_HAWCUnits();
  pybind_hawcnest_SoftwareVersion();
}

