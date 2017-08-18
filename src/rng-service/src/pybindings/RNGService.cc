
#include <boost/python.hpp>

#include <rng-service/RNGService.h>
#include <rng-service/StdRNGService.h>

using namespace boost::python;

/*!
 * @class RNGServiceWrap
 * @brief Expose RNGService abstract base class to boost::python
 */
class RNGServiceWrap : public RNGService, public wrapper<RNGService> {

  public:

    int Poisson(const double mu=1) const
    { return this->get_override("Poisson")(mu); }

    int Binomial(const int n=10, const double p=0.5) const
    { return this->get_override("Binomial")(n, p); }

    double Gaussian(const double mu=0, const double sigma=1) const
    { return this->get_override("Gaussian")(mu, sigma); }

    double LogNormal(const double mu=0, const double sigma=1) const
    { return this->get_override("LogNormal")(mu, sigma); }

    double Rician(const double mu=0, const double sigma=1) const
    { return this->get_override("Rician")(mu, sigma); }

    double Uniform(const double a=0, const double b=1) const
    { return this->get_override("Uniform")(a, b); }

    double Exponential(const double lambda=1) const
    { return this->get_override("Exponential")(lambda); }

    double PowerLaw(const double n, const double a, const double b) const
    { return this->get_override("PowerLaw")(n, a, b); }

    double CutoffPowerLaw(const double n, const double lambda,
                          const double a, const double b) const
    { return this->get_override("CutoffPowerLaw")(n, lambda, a, b); }

};

/// Define python bindings for RNGService pure virtual base class, with default
/// arguments for the functions
void
pybind_rng_service_RNGService()
{
  class_<RNGServiceWrap, boost::noncopyable>
    ("RNGService",
     "Abstract interface for random number generators",
     no_init)
     .def("Poisson", pure_virtual(&RNGService::Poisson),
          (arg("mu")=1.),
          "Generate Poisson random integers with mean mu.")
     .def("Binomial", pure_virtual(&RNGService::Binomial),
          (arg("n")=10, arg("p")=0.5),
          "Generate binomial random integers from n tries with success rate p.")
     .def("Gaussian", pure_virtual(&RNGService::Gaussian),
          (arg("mu")=0., arg("sigma")=1.),
          "Generate gaussian random numbers with mean mu and width sigma.")
     .def("LogNormal", pure_virtual(&RNGService::LogNormal),
          (arg("mu")=0., arg("sigma")=1.),
          "Generate log-normal random numbers with mean mu and width sigma.")
     .def("Rician", pure_virtual(&RNGService::Rician),
          (arg("mu")=0., arg("sigma")=1.),
          "Generate rician random numbers with mean mu and width sigma.")
     .def("Uniform", pure_virtual(&RNGService::Uniform),
          (arg("a")=0., arg("b")=1.),
          "Generate uniform random numbers in the range [a, b].")
     .def("Exponential", pure_virtual(&RNGService::Exponential),
          (arg("lambda")=1.),
          "Generate exponential random numbers with decay length lambda.")
     .def("PowerLaw", pure_virtual(&RNGService::PowerLaw),
          "Generate power law random numbers x^n on [a, b].")
     .def("CutoffPowerLaw", pure_virtual(&RNGService::CutoffPowerLaw),
          "Generate random numbers x^n * exp(-lambda*x) on [a, b].")
    ;
}

/// Define python bindings for the StdRNGService
void
pybind_rng_service_StdRNGService()
{
  class_<StdRNGService, bases<RNGService> >
    ("StdRNGService",
     "Default service for generating random numbers")
    ;
}

