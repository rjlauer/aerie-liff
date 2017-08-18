/*!
 * @file ServiceRegistryTest.cc 
 * @brief Unit test of the HAWCNest service registry.
 * @author Segev BenZvi 
 * @date 18 Jan 2012 
 * @ingroup unit_test
 * @version $Id: ServiceRegistryTest.cc 18844 2014-02-18 01:42:07Z sybenzvi $
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <hawcnest/impl/ServiceRegistry.h>
#include <hawcnest/RegisterService.h>
#include <hawcnest/HAWCNest.h>

using namespace std;

// Dummy class for testing service parameter initialization
class TestService {

  public:

    typedef TestService Interface;

    Configuration DefaultConfiguration() {
      Configuration config;
      config.Parameter<int>("intparam");
      return config;
    }

    void Initialize(const Configuration& config) {
      config.GetParameter("intparam", intparam);
    }

    int intparam;

};

// A dummy service class that is automatically registered with the framework
class AnotherTestService {
  public:
    typedef AnotherTestService Interface;
};

REGISTER_SERVICE(AnotherTestService);


// Create the "ServiceRegistryTest" testing suite
BOOST_AUTO_TEST_SUITE(ServiceRegistryTest)

  // ___________________________________________________________________________
  // Example of manually adding a service to the global registry
  BOOST_AUTO_TEST_CASE(RegisterServices)
  {
    GlobalServiceRegistry().RegisterService<TestService>();

    HAWCNest nest;

    GlobalServiceRegistry().AddService("TestService", nest, "testService")
                                      ("intparam", 5);

    nest.Configure();

    TestService& serv = GetService<TestService>("testService");
    BOOST_CHECK(serv.intparam == 5);
  }

  // ___________________________________________________________________________
  // Example of the automatic registration macro
  BOOST_AUTO_TEST_CASE(AutomaticRegistration)
  {
    HAWCNest nest;

    BOOST_CHECK_NO_THROW(
      GlobalServiceRegistry().AddService("AnotherTestService",
                                         nest,
                                         "anotherTestService"));
  }

BOOST_AUTO_TEST_SUITE_END()

