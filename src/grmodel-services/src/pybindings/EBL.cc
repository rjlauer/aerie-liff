#include <boost/python.hpp>

#include <grmodel-services/ebl/Gilmore09EBLModel.h>
#include <grmodel-services/ebl/Dominguez11EBLModel.h>
#include <grmodel-services/ebl/Gilmore12FiducialEBLModel.h>
#include <grmodel-services/ebl/Gilmore12FixedEBLModel.h>

using namespace boost::python;

/*!
 * @class EBLAbsorptionServiceWrapper
 * @brief Expose EBLAbsorptionService abstract base class to boost::python
 */
class EBLAbsorptionServiceWrapper : public EBLAbsorptionService,
                                    public wrapper<EBLAbsorptionService> {

  public:

    double GetOpticalDepth(const double E, const double z,
                           const ErrorContour e=CENTRAL) const
    { return this->get_override("GetOpticalDepth")(E, z, e); }

    double GetAttenuation(const double E, const double z,
                          const ErrorContour e=CENTRAL) const
    { return this->get_override("GetAttenuation")(E, z, e); }

};

// Overload the optical depth getter to allow for "default" arguments in python
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(
  EBLOptDepthOverloads, GetOpticalDepth, 2, 3);

// Overload the attenuation getter to allow for "default" arguments in python
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(
  EBLAttenOverloads, GetAttenuation, 2, 3);

/// Expose EBLAbsorptionService abstract interface to python
void
pybind_grmodel_services_ebl_EBLAbsorptionService()
{
  scope in_EBLAbsorption =
  class_<EBLAbsorptionServiceWrapper, boost::noncopyable>
    ("EBLAbsorptionService",
     "Abstract interface for extragalactic background light absorption models.",
     no_init)
    .def("GetOpticalDepth",
         &EBLAbsorptionService::GetOpticalDepth,
         EBLOptDepthOverloads())
    .def("GetAttenuation",
         &EBLAbsorptionService::GetAttenuation,
         EBLAttenOverloads())
    ;

  enum_<EBLAbsorptionService::ErrorContour>("ErrorContour")
    .value("LOWER", EBLAbsorptionService::LOWER)
    .value("CENTRAL", EBLAbsorptionService::CENTRAL)
    .value("UPPER", EBLAbsorptionService::UPPER)
    .export_values()
    ;
}

/// Expose Gilmore09EBLModel to python
void
pybind_grmodel_services_ebl_Gilmore09EBLModel()
{
  class_<Gilmore09EBLModel, bases<EBLAbsorptionService> >
    ("Gilmore09EBLModel",
     "WMAP3 EBL model of R.C. Gilmore et al., MNRAS 399 (2009) 1694.")
    ;
}

/// Expose Dominguez11EBLModel to python
void
pybind_grmodel_services_ebl_Dominguez11EBLModel()
{
  class_<Dominguez11EBLModel, bases<EBLAbsorptionService> >
    ("Dominguez11EBLModel",
     "EBL model of A. Dominguez et al., MNRAS 410 (2011) 2556.")
    ;
}

/// Expose Gilmore12FiducialEBLModel to python
void
pybind_grmodel_services_ebl_Gilmore12FiducialEBLModel()
{
  class_<Gilmore12FiducialEBLModel, bases<EBLAbsorptionService> >
    ("Gilmore12FiducialEBLModel",
     "WMAP5 fiducial EBL model of R.C. Gilmore et al., MNRAS 422 (2012) 3189.")
    ;
}

/// Expose Gilmore12FixedEBLModel to python
void
pybind_grmodel_services_ebl_Gilmore12FixedEBLModel()
{
  class_<Gilmore12FixedEBLModel, bases<EBLAbsorptionService> >
    ("Gilmore12FixedEBLModel",
     "WMAP5 fixed EBL model of R.C. Gilmore et al., MNRAS 422 (2012) 3189.")
    ;
}

