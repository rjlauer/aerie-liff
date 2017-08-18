#include <boost/python.hpp>

#include <astro-service/AstroService.h>
#include <astro-service/StdAstroService.h>
#include <astro-service/GeomagneticField.h>
#include <astro-service/GeoDipoleService.h>

#include <hawcnest/LoadProject.h>
#include <hawcnest/Service.h>

using namespace boost::python;
using namespace std;

void pybind_astro_service_AstroService();
void pybind_astro_service_StdAstroService();
void pybind_astro_service_Moon();
void pybind_astro_service_GeomagneticField();
void pybind_astro_service_GeoDipoleService();

BOOST_PYTHON_FUNCTION_OVERLOADS(GS_AstroService_Overloads,
  GetService<AstroService>, 1, 2);

BOOST_PYTHON_FUNCTION_OVERLOADS(GS_GeomagneticField_Overloads,
  GetService<GeomagneticField>, 1, 2);

BOOST_PYTHON_MODULE(astro_service)
{
  // Shower user-defined and python docstrings; suppress C++ signatures
  docstring_options docOpts(true, true, false);

  load_project("astro-service", false);

  pybind_astro_service_AstroService();
  pybind_astro_service_StdAstroService();
  pybind_astro_service_Moon();
  pybind_astro_service_GeomagneticField();
  pybind_astro_service_GeoDipoleService();

  def("GetService", GetService<AstroService>,
    GS_AstroService_Overloads()[return_value_policy<reference_existing_object>()]);

  def("GetField", GetService<GeomagneticField>,
    GS_GeomagneticField_Overloads()[return_value_policy<reference_existing_object>()]);
}

