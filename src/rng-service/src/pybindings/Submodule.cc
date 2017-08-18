
#include <boost/python.hpp>

#include <rng-service/RNGService.h>
#include <rng-service/StdRNGService.h>

#include <hawcnest/LoadProject.h>
#include <hawcnest/Service.h>

using namespace boost::python;
using namespace std;

void pybind_rng_service_RNGService();
void pybind_rng_service_StdRNGService();

BOOST_PYTHON_FUNCTION_OVERLOADS(GS_RNGService_Overloads,
  GetService<RNGService>, 1, 2);

BOOST_PYTHON_MODULE(rng_service)
{
  // Shower user-defined and python docstrings; suppress C++ signatures
  docstring_options docOpts(true, true, false);

  load_project("rng-service", false);

  pybind_rng_service_RNGService();
  pybind_rng_service_StdRNGService();

  def("GetService", GetService<RNGService>,
    GS_RNGService_Overloads()[return_value_policy<reference_existing_object>()]);
}

