/*!
 * @file Submodule.cc 
 * @brief Definition of the python bindings of the detector-service library.
 * @author Segev BenZvi 
 * @date 17 Dec 2011 
 * @version $Id: Submodule_3ML.cc 31179 2016-04-09 20:26:44Z sybenzvi $
 */

#include <boost/python.hpp>

#include <hawcnest/LoadProject.h>
#include <hawcnest/Service.h>

using namespace boost::python;
using namespace std;

void pybind_liff_LikeHAWC_3ML();

BOOST_PYTHON_MODULE (liff_3ML) {
  // Shower user-defined and python docstrings; suppress C++ signatures
  docstring_options docOpts(true, true, false);

  load_project("liff", false);

  pybind_liff_LikeHAWC_3ML();
}

