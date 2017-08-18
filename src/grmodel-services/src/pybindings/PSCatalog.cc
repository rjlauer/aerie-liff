
#include <boost/python.hpp>

#include <grmodel-services/spectra/PSCatalog.h>

#include <hawcnest/HAWCNest.h>

using namespace boost::python;

void
pybind_grmodel_services_spectra_PSCatalog()
{
  def("BuildPSCatalog", &PSCatalog::Build,
      "Initialize a catalog of point sources from an input XML file.");
}

