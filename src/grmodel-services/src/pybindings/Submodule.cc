#include <boost/python.hpp>

#include <grmodel-services/ebl/EBLAbsorptionService.h>
#include <grmodel-services/spectra/CosmicRayService.h>
#include <grmodel-services/spectra/CosmicRaySource.h>
#include <grmodel-services/spectra/PointSource.h>

#include <hawcnest/LoadProject.h>
#include <hawcnest/Service.h>

using namespace boost::python;
using namespace std;

void pybind_grmodel_services_ebl_EBLAbsorptionService();
void pybind_grmodel_services_ebl_Gilmore09EBLModel();
void pybind_grmodel_services_ebl_Dominguez11EBLModel();
void pybind_grmodel_services_ebl_Gilmore12FiducialEBLModel();
void pybind_grmodel_services_ebl_Gilmore12FixedEBLModel();

void pybind_grmodel_services_spectra_CosmicRayService();
void pybind_grmodel_services_spectra_GenericSpectrum();
void pybind_grmodel_services_spectra_GenericBrokenSpectrum();
void pybind_grmodel_services_spectra_GenericCutoffSpectrum();
void pybind_grmodel_services_spectra_TabulatedSpectrum();

void pybind_grmodel_services_spectra_CosmicRaySource();
void pybind_grmodel_services_spectra_IsotropicCosmicRaySource();
void pybind_grmodel_services_spectra_CRCatalog();

void pybind_grmodel_services_spectra_PointSource();
void pybind_grmodel_services_spectra_PSCatalog();

void pybind_grmodel_services_diffuse_GALPROPMapTable();
void pybind_grmodel_services_diffuse_CosmicRayAnisotropyTable();

// Overload GetService so that the second parameter is treated like a default
// argument in python
BOOST_PYTHON_FUNCTION_OVERLOADS(GS_EBLAbsorptionService_Overloads,
  GetService<EBLAbsorptionService>, 1, 2);

// Dummy class for defining the scope of GetService for EBL services
class EBLScope { };

BOOST_PYTHON_FUNCTION_OVERLOADS(GS_CosmicRayService_Overloads,
  GetService<CosmicRayService>, 1, 2);

BOOST_PYTHON_FUNCTION_OVERLOADS(GS_CosmicRaySource_Overloads,
  GetService<CosmicRaySource>, 1, 2);

// Dummy class for defining the scope of GetService for cosmic services
class CRScope { };

// Overloaded GetService to access PointSource objects
BOOST_PYTHON_FUNCTION_OVERLOADS(GS_PointSource_Overloads,
  GetService<PointSource>, 1, 2);

BOOST_PYTHON_MODULE(grmodel_services)
{
  // Shower user-defined and python docstrings; suppress C++ signatures
  docstring_options docOpts(true, true, false);

  load_project("grmodel-services", false);

  // EBL models
  pybind_grmodel_services_ebl_EBLAbsorptionService();
  pybind_grmodel_services_ebl_Gilmore09EBLModel();
  pybind_grmodel_services_ebl_Dominguez11EBLModel();
  pybind_grmodel_services_ebl_Gilmore12FiducialEBLModel();
  pybind_grmodel_services_ebl_Gilmore12FixedEBLModel();

  // GetService has to be called "GetEBLAbsorptionService" in python
  def("GetEBLAbsorptionService", GetService<EBLAbsorptionService>,
    GS_EBLAbsorptionService_Overloads()[
      return_value_policy<reference_existing_object>()]);

  // Cosmic ray and gamma ray spectral models
  pybind_grmodel_services_spectra_CosmicRayService();
  pybind_grmodel_services_spectra_GenericSpectrum();
  pybind_grmodel_services_spectra_GenericBrokenSpectrum();
  pybind_grmodel_services_spectra_GenericCutoffSpectrum();
  pybind_grmodel_services_spectra_TabulatedSpectrum();

  pybind_grmodel_services_spectra_CosmicRaySource();
  pybind_grmodel_services_spectra_IsotropicCosmicRaySource();
  pybind_grmodel_services_spectra_CRCatalog();

  pybind_grmodel_services_spectra_PointSource();
  pybind_grmodel_services_spectra_PSCatalog();

  // Get PointSource service
  def("GetPointSource", GetService<PointSource>,
    GS_PointSource_Overloads()[
      return_value_policy<reference_existing_object>()]);

  // GetService has to be called "GetCosmicRayService" in python
  def("GetCosmicRayService", GetService<CosmicRayService>,
    GS_CosmicRayService_Overloads()[
      return_value_policy<reference_existing_object>()]);

  def("GetCosmicRaySource", GetService<CosmicRaySource>,
    GS_CosmicRaySource_Overloads()[
      return_value_policy<reference_existing_object>()]);

  // Diffuse classes
  pybind_grmodel_services_diffuse_GALPROPMapTable();
  pybind_grmodel_services_diffuse_CosmicRayAnisotropyTable();
}

