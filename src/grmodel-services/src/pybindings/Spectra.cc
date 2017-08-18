
#include <boost/python.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>

#include <grmodel-services/spectra/CosmicRayService.h>
#include <grmodel-services/spectra/GenericSpectrum.h>
#include <grmodel-services/spectra/GenericBrokenSpectrum.h>
#include <grmodel-services/spectra/GenericCutoffSpectrum.h>
#include <grmodel-services/spectra/TabulatedSpectrum.h>
#include <grmodel-services/spectra/CRCatalog.h>

#include <grmodel-services/spectra/CosmicRaySource.h>
#include <grmodel-services/spectra/IsotropicCosmicRaySource.h>

#include <grmodel-services/spectra/PointSource.h>

#include <data-structures/time/ModifiedJulianDate.h>
#include <data-structures/math/PowerLaw.h>

#include <hawcnest/HAWCNest.h>

using namespace boost::python;

/*!
 * @class CosmicRayServiceWrapper
 * @brief Expose CosmicRayService abstract base class to boost::python
 */
class CosmicRayServiceWrapper : public CosmicRayService,
                                public wrapper<CosmicRayService> {

  public:

    double GetFlux(const double E,
                   const ModifiedJulianDate& mjd,
                   const ParticleType& type) const
    { return this->get_override("GetFlux")(E, mjd, type); }

    double GetFluxWeight(const double E,
                         const ModifiedJulianDate& mjd,
                         const PowerLaw& pl,
                         const ParticleType& type) const
    { return this->get_override("GetFluxWeight")(E, mjd, pl, type); }

    double GetMinEnergy(const ParticleType& type) const
    { return this->get_override("GetMinEnergy")(type); }

    double GetMaxEnergy(const ParticleType& type) const
    { return this->get_override("GetMaxEnergy")(type); }

    double GetRandomEnergy(const RNGService& rng,
                           const double E0, const double E1,
                           const ParticleType& pt) const
    { return this->get_override("GetRandomEnergy")(rng, E0, E1, pt); }

};

/// Expose CosmicRayService abstract interface to python
void
pybind_grmodel_services_spectra_CosmicRayService()
{
  class_<CosmicRayServiceWrapper, boost::noncopyable>
    ("CosmicRayService",
     "Abstract interface for extragalactic background light absorption models.",
     no_init)
    .def("GetFlux",
         pure_virtual(&CosmicRayService::GetFlux),
         "Calculate the flux of a particle species at a given energy E.")
    .def("GetFluxWeight",
         pure_virtual(&CosmicRayService::GetFluxWeight),
         "Weight to convert from a power law into another spectrum at energy E.")
    .def("GetMinEnergy",
         pure_virtual(&CosmicRayService::GetMinEnergy),
         "Minimum valid energy for the model.")
    .def("GetMaxEnergy",
         pure_virtual(&CosmicRayService::GetMaxEnergy),
         "Maximum valid energy for the model.")
    .def("GetRandomEnergy",
         pure_virtual(&CosmicRayService::GetRandomEnergy),
         "Sample a random energy from the spectrum, given a ParticleType.")
    ;
}

/// Expose GenericSpectrum to python
void
pybind_grmodel_services_spectra_GenericSpectrum()
{
  class_<GenericSpectrum, bases<CosmicRayService> >
    ("GenericSpectrum",
     "A generic power law spectrum.")
    ;
}

/// Expose GenericBrokenSpectrum to python
void
pybind_grmodel_services_spectra_GenericBrokenSpectrum()
{
  class_<GenericBrokenSpectrum, bases<CosmicRayService> >
    ("GenericBrokenSpectrum",
     "A generic broken power law spectrum.")
    ;
}

/// Expose GenericCutoffSpectrum to python
void
pybind_grmodel_services_spectra_GenericCutoffSpectrum()
{
  class_<GenericCutoffSpectrum, bases<CosmicRayService> >
    ("GenericCutoffSpectrum",
     "A generic power law spectrum with an exponential cutoff.")
    ;
}

/// Expose TabulatedSpectrum to python
void
pybind_grmodel_services_spectra_TabulatedSpectrum()
{
  class_<TabulatedSpectrum, bases<CosmicRayService> >
    ("TabulatedSpectrum",
     "A spectrum created from an ASCII flux table.")
    ;
}

/*!
 * @class CosmicRaySourceWrapper
 * @brief Expose CosmicRaySource abstract base class to boost::python
 */
class CosmicRaySourceWrapper : public CosmicRaySource,
                               public wrapper<CosmicRaySource> {

  public:
    
    double GetFlux(const double E, const ModifiedJulianDate& mjd) const
    { return this->get_override("GetFlux")(E, mjd); }

    double GetFluxWeight(const double E, const ModifiedJulianDate& mjd, const PowerLaw& p) const
    { return this->get_override("GetFluxWeight")(E, mjd, p); }

    double GetMinEnergy() const
    { return this->get_override("GetMinEnergy")(); }

    double GetMaxEnergy() const
    { return this->get_override("GetMaxEnergy")(); }

    double GetRandomEnergy(const RNGService& rng,
                           const double E0, const double E1) const
    { return this->get_override("GetRandomEnergy")(rng, E0, E1); }

    double Integrate(const double E0, const double E1,
                     const ModifiedJulianDate& mjd) const
    { return this->get_override("Integrate")(E0, E1, mjd); }

    const ParticleType& GetParticleType() const
    { return this->get_override("GetParticleType")(); }

};

/// Expose CosmicRaySource abstract interface to python
void
pybind_grmodel_services_spectra_CosmicRaySource()
{
  class_<CosmicRaySourceWrapper, boost::noncopyable>
    ("CosmicRaySource",
     "Abstract interface for a source of cosmic rays (of one particle type)",
     no_init)
  .def("GetFlux",
       pure_virtual(&CosmicRaySource::GetFlux),
       "Calculate the flux at a given energy E and MJD.")
  .def("GetFluxWeight",
       pure_virtual(&CosmicRaySource::GetFluxWeight),
       "Calculate the weight to convert a PowerLaw to this spectrum.")
  .def("GetMinEnergy",
       pure_virtual(&CosmicRaySource::GetMinEnergy),
       "Minimum valid energy for the spectrum.")
  .def("GetMaxEnergy",
       pure_virtual(&CosmicRaySource::GetMaxEnergy),
       "Maximum valid energy for the spectrum.")
  .def("GetRandomEnergy",
       pure_virtual(&CosmicRaySource::GetRandomEnergy),
       "Sample a random energy from the spectrum between E0 and E1.")
  .def("Integrate",
       pure_virtual(&CosmicRaySource::Integrate),
       "Integrate the spectrum between energies E0 and E1.")
  .def("GetParticleType",
       make_function(&CosmicRaySource::GetParticleType,
                     return_value_policy<copy_const_reference>()),
       "Get the particle type of the CosmicRaySource spectrum.")
  ;
}

/// Expose IsotropicCosmicRaySource to python
void
pybind_grmodel_services_spectra_IsotropicCosmicRaySource()
{
  class_<IsotropicCosmicRaySource, bases<CosmicRaySource> >
    ("IsotropicCosmicRaySource",
     "An isotropic source of cosmic rays of some particle type.")
    ;
}

/// Expose cosmic ray catalog builder to python
void
pybind_grmodel_services_spectra_CRCatalog()
{
  class_<CRCatalog::SourceMap>("CRSourceMap")
    .def(map_indexing_suite<CRCatalog::SourceMap, true>());

  def("BuildCRCatalog", &CRCatalog::Build,
      "Initialize a catalog of point sources from an input XML file.");
}

// -----------------------------------------------------------------------------

/*!
 * @class PointSourceWrap
 * @brief Expose the PointSource abstract service class to boost::python
 */
class PointSourceWrap : 
  public PointSource, public wrapper<PointSource> {
  
  public:

    double GetFlux(const double E, const ModifiedJulianDate& mjd) const
    { return this->get_override("GetFlux")(E, mjd); }

    double GetFluxWeight(const double E, const ModifiedJulianDate& mjd, const PowerLaw& p) const
    { return this->get_override("GetFluxWeight")(E, mjd, p); }

    double GetMinEnergy() const
    { return this->get_override("GetMinEnergy")(); }

    double GetMaxEnergy() const
    { return this->get_override("GetMaxEnergy")(); }

    double GetRandomEnergy(const RNGService& rng,
                           const double E0, const double E1) const
    { return this->get_override("GetRandomEnergy")(rng, E0, E1); }

    double Integrate(const double E0, const double E1,
                     const ModifiedJulianDate& mjd) const
    { return this->get_override("GetFlux")(E0, E1, mjd); }

    const ParticleType& GetParticleType() const
    { return this->get_override("GetParticleType")(); }

    const EquPoint& GetLocation() const
    { return this->get_override("GetLocation")(); }

    double GetRedshift() const
    { return this->get_override("GetRedshift")(); }

};

void
pybind_grmodel_services_spectra_PointSource()
{
  class_<PointSourceWrap, boost::noncopyable>
    ("PointSource",
     "Interface for accessing point source data (spectrum, location, etc.).")

    .def("GetFlux",
         pure_virtual(&PointSource::GetFlux),
         "Get the source flux at a given energy.")
    .def("GetFluxWeight",
         pure_virtual(&PointSource::GetFluxWeight),
         "Get the weight from a PowerLaw to this source flux.")
    .def("GetMinEnergy",
         pure_virtual(&PointSource::GetMinEnergy),
         "Get minimum energy (model or measured) of the source spectrum.")
    .def("GetMaxEnergy",
         pure_virtual(&PointSource::GetMaxEnergy),
         "Get maximum energy (model or measured) of the source spectrum.")
    .def("GetRandomEnergy",
         pure_virtual(&PointSource::GetRandomEnergy),
         "Sample a random energy from the source spectrum.")
    .def("Integrate",
         pure_virtual(&PointSource::Integrate),
         "Integrate the source flux.")
    .def("GetParticleType",
          &PointSource::GetParticleType,
          return_value_policy<copy_const_reference>(),
          "Get the particle type generated by the source.")
    .def("GetLocation",
          &PointSource::GetLocation,
          return_value_policy<copy_const_reference>(),
          "Get the location of the source (RA,Dec).")
    .def("GetRedshift",
          &PointSource::GetRedshift,
          "Get the redshift of the source.")
     ; 
}

