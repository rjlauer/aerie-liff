/*!
 * @file Submodule.cc 
 * @brief Definition of the python bindings of the detector-service library.
 * @author Segev BenZvi 
 * @date 17 Dec 2011 
 * @version $Id: Submodule.cc 37840 2017-02-24 17:06:35Z criviere $
 */

#include <boost/python.hpp>

#include <hawcnest/LoadProject.h>
#include <hawcnest/Service.h>

using namespace boost::python;
using namespace std;

void pybind_liff_ModelInterface();
void pybind_liff_MultiTF1PointSource();
void pybind_liff_LikeHAWC();

BOOST_PYTHON_MODULE (liff) {
  // Shower user-defined and python docstrings; suppress C++ signatures
  docstring_options docOpts(true, true, false);

  load_project("liff", false);

  pybind_liff_ModelInterface();
  pybind_liff_MultiTF1PointSource();
  pybind_liff_LikeHAWC();
}

