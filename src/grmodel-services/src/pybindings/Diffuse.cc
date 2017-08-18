#include <boost/python.hpp>

#include <grmodel-services/diffuse/GALPROPMapTable.h>
#include <grmodel-services/diffuse/CosmicRayAnisotropyTable.h>

#include <rng-service/RNGService.h>

#include <data-structures/astronomy/GalPoint.h>
#include <data-structures/astronomy/EquPoint.h>

#include <hawcnest/HAWCUnits.h>

#include <cmath>

using namespace HAWCUnits;
using namespace boost::python;

/// Expose MapCube class to python
void
pybind_grmodel_services_diffuse_GALPROPMapTable()
{
  class_<GALPROPMapTable, boost::shared_ptr<GALPROPMapTable> >
    ("GALPROPMapTable",
     "GALPROP FITS fluxes as a function of energy and Galactic coordinates.",
     init<std::string>())

    .def("GetFlux", &GALPROPMapTable::GetFlux,
         "Get flux at a given energy and Galactic position.")
    .def("GetMaxFlux", &GALPROPMapTable::GetMaxFlux,
         "Get maximum flux at a given energy.")
    .def("GetRandomEnergy", &GALPROPMapTable::GetRandomEnergy,
         "Sample a random energy from a Galactic position.")
    .def("Integrate", &GALPROPMapTable::Integrate,
         "Integrate flux between E0 and E1 at a given Galactic Position.")
    ;
}

void
pybind_grmodel_services_diffuse_CosmicRayAnisotropyTable()
{
  class_<CosmicRayAnisotropyTable, boost::shared_ptr<CosmicRayAnisotropyTable> >
    ("CosmicRayAnisotropyTable",
     "Cosmic ray intensity PDF versus energy and equatorial coordinates.",
     init<std::string>())

    .def("GetPDF", &CosmicRayAnisotropyTable::GetPDF,
         "Get flux PDF at a given energy and equatorial position.")
    ;
}

