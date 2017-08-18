/*!
 * @file NestTest.cc 
 * @brief Unit test of the HAWCNest class.
 * @author Segev BenZvi 
 * @date 18 Jan 2012 
 * @ingroup unit_test
 * @version $Id: NestTest.cc 40083 2017-08-07 17:40:11Z pretz $
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <hawcnest/HAWCNest.h>
#include <hawcnest/NestIniConfig.h>
#include <hawcnest/RegisterService.h>
#include <hawcnest/test/OutputConfig.h>

using namespace std;

// Dummy class for testing service parameter initialization
class TestParam {
  
public:
  
  typedef TestParam Interface;
  
  Configuration DefaultConfiguration() {
    Configuration config;
    config.Parameter<int>("i", 5);
    config.Parameter<string>("s", "foo");
    config.Parameter<string>("blank", "");
    config.Parameter<string>("nodefault");
    
    vector<int> vi;
    config.Parameter<vector<int> >("vi",vi);

    vector<string> vs;
    config.Parameter<vector<string> >("vs",vs);
    
    return config;
  }
  
  void Initialize(const Configuration& config) {
    config.GetParameter("i", i_);
    config.GetParameter("s", s_);
    config.GetParameter("nodefault", nodefault_);
    config.GetParameter("blank", blank_);
    config.GetParameter("vi", vi_);
    config.GetParameter("vs", vs_);
  }
  
  int i_;
  string s_;
  string nodefault_;
  string blank_;
  vector<int> vi_;
  vector<string> vs_;
};

SHARED_POINTER_TYPEDEFS(TestParam);
REGISTER_SERVICE(TestParam);

// Dummy class for testing module initialization
class TestModule : public Module {

  public:

    typedef Module Interface;

    Module::Result Process(BagPtr bag) { return Continue; }

};

// Create the "NestTest" testing suite
BOOST_AUTO_TEST_SUITE(NestTest)

  // Global output setting: shut off the HAWCNest logger.
  // Note: this will affect all test suites in the hawcnest project
  BOOST_GLOBAL_FIXTURE(OutputConfig);

  // ___________________________________________________________________________
  // Set parameters using a parameter list
  BOOST_AUTO_TEST_CASE(SetParams)
  {
    HAWCNest nest;

    nest.Service<TestParam>("test")
      ("i", 134)
      ("s", "bar")
      ("nodefault", "somethingspecial");

    nest.Service<TestParam>("test_2")
      ("nodefault", "somethingspecial");

    // Retrieve parameter value
    BOOST_CHECK(nest.GetParameter<string>("test", "s") == "bar");
    BOOST_CHECK(nest.GetParameter<int>("test", "i") == 134);

    nest.Configure();

    // Retrieve by reference
    BOOST_CHECK(GetService<TestParam>("test").i_ == 134);
    BOOST_CHECK(GetService<TestParam>("test").s_ == "bar");

    BOOST_CHECK(GetService<TestParam>("test_2").i_ == 5);
    BOOST_CHECK(GetService<TestParam>("test_2").s_ == "foo");

    // Retrieve by pointer
    BOOST_CHECK(GetService<TestParamPtr>("test"));

    BOOST_CHECK(GetService<TestParamPtr>("test")->i_ == 134);
    BOOST_CHECK(GetService<TestParamPtr>("test")->s_ == "bar");

    BOOST_CHECK(GetService<TestParamPtr>("test_2")->i_ == 5);
    BOOST_CHECK(GetService<TestParamPtr>("test_2")->s_ == "foo");
  }

  // ___________________________________________________________________________
  // Set parameters using a parameter list
  BOOST_AUTO_TEST_CASE(SetParams_stringadd)
  {
    HAWCNest nest;

    nest.Service("TestParam","test")
      ("i", 134)
      ("s", "bar")
      ("nodefault", "somethingspecial");

    nest.Service<TestParam>("test_2")
      ("nodefault", "somethingspecial");

    // Retrieve parameter value
    BOOST_CHECK(nest.GetParameter<string>("test", "s") == "bar");
    BOOST_CHECK(nest.GetParameter<int>("test", "i") == 134);

    nest.Configure();

    // Retrieve by reference
    BOOST_CHECK(GetService<TestParam>("test").i_ == 134);
    BOOST_CHECK(GetService<TestParam>("test").s_ == "bar");

    BOOST_CHECK(GetService<TestParam>("test_2").i_ == 5);
    BOOST_CHECK(GetService<TestParam>("test_2").s_ == "foo");

    // Retrieve by pointer
    BOOST_CHECK(GetService<TestParamPtr>("test"));

    BOOST_CHECK(GetService<TestParamPtr>("test")->i_ == 134);
    BOOST_CHECK(GetService<TestParamPtr>("test")->s_ == "bar");

    BOOST_CHECK(GetService<TestParamPtr>("test_2")->i_ == 5);
    BOOST_CHECK(GetService<TestParamPtr>("test_2")->s_ == "foo");
  }


  // ___________________________________________________________________________
  // Set parameters using the SetParameter method
  BOOST_AUTO_TEST_CASE(SetParamsByMethod)
  {
    HAWCNest nest;

    nest.Service<TestParam>("test");
    nest.SetParameter("test","i",134);
    nest.SetParameter("test","s","bar");
    nest.SetParameter("test","nodefault","somethingspecial");

    nest.Service<TestParam>("test_2");
    nest.SetParameter("test_2","nodefault","soemthingspecial");

    nest.Configure();

    // retrieve by reference
    BOOST_CHECK(GetService<TestParam>("test").i_ == 134);
    BOOST_CHECK(GetService<TestParam>("test").s_ == "bar");

    BOOST_CHECK(GetService<TestParam>("test_2").i_ == 5);
    BOOST_CHECK(GetService<TestParam>("test_2").s_ == "foo");

    // retrieve by pointer
    BOOST_CHECK(GetService<TestParamPtr>("test"));

    BOOST_CHECK(GetService<TestParamPtr>("test")->i_ == 134);
    BOOST_CHECK(GetService<TestParamPtr>("test")->s_ == "bar");

    BOOST_CHECK(GetService<TestParamPtr>("test_2")->i_ == 5);
    BOOST_CHECK(GetService<TestParamPtr>("test_2")->s_ == "foo");
  }

  // ___________________________________________________________________________
  // Set parameters and configure, then reset parameters and reconfigure
  BOOST_AUTO_TEST_CASE(NestReconfigure)
  {
    HAWCNest nest;

    nest.Service<TestParam>("test")
      ("i", 134)
      ("s", "bar")
      ("nodefault", "somethingspecial");

    nest.Service<TestParam>("test_2")
      ("nodefault", "whatever");

    nest.Configure();

    // Retrieve parameter values
    BOOST_CHECK(nest.GetParameter<string>("test", "s") == "bar");
    BOOST_CHECK(nest.GetParameter<int>("test", "i") == 134);

    // Reset a parameter and reconfigure the framework
    nest.SetParameter("test", "i", 567);

    nest.Configure();

    BOOST_CHECK(nest.GetParameter<int>("test", "i") == 567);
    BOOST_CHECK(GetService<TestParam>("test_2").i_ == 5);
    BOOST_CHECK(GetService<TestParam>("test_2").s_ == "foo");
  }

  // ___________________________________________________________________________
  // Attempting to configure without setting a parameter raises an exception
  BOOST_AUTO_TEST_CASE(SetParamsNodefault)
  {
    HAWCNest nest;

    nest.Service<TestParam>("test")
      ("i", 134)
      ("s", "bar");

    BOOST_CHECK_THROW(nest.Configure(), configuration_exception);
  }

  // ___________________________________________________________________________
  // Attempting to create a service with a duplicate name raises an exception
  BOOST_AUTO_TEST_CASE(DuplicateNames)
  {
    HAWCNest nest;
    nest.Service<TestParam>("test");

    BOOST_CHECK_THROW(nest.Service<TestParam>("test"), std::runtime_error);
  }

  // This is a good test, but it screws up global memory (intentionally)
  // and will screw up subsequent tests if it isn't cleaned up correctly.
  // So I'm commenting it out for now.
  // ___________________________________________________________________________
  // Trying to create two instances of the framework raises an exception
//  BOOST_AUTO_TEST_CASE(TwoNestsOneScope)
//  {
//    HAWCNest nest_1;
//    BOOST_CHECK_THROW(HAWCNest nest_2, hawcnest_exception);
//  }

  // ___________________________________________________________________________
  // Create two nest instances in two different scopes
  BOOST_AUTO_TEST_CASE(TwoNestsTwoScopes)
  {
    {
      HAWCNest nest1;

      nest1.Service<TestParam>("test1")
        ("i", 134)
        ("s", "bar")
        ("nodefault", "somethingspecial");

      nest1.Configure();

      // retrieve by reference
      BOOST_CHECK(GetService<TestParam>("test1").i_ == 134);
    }

    {
      HAWCNest nest2;

      nest2.Service<TestParam>("test2")
        ("i", 789)
        ("s", "foo")
        ("nodefault", "somethingspecial");

      nest2.Configure();

      // retrieve by reference
      BOOST_CHECK(GetService<TestParam>("test2").i_ == 789);
    }
  }

  // ___________________________________________________________________________
  // Create a HAWCNest instance on the heap, deallocate it, and do it again
  BOOST_AUTO_TEST_CASE(HeapOfNests)
  {
    // Create the framework instance
    HAWCNest* nest;
    nest = new HAWCNest();
    nest->Service<TestParam>("test1")
      ("i", 134)
      ("s", "bar")
      ("nodefault", "somethingspecial");

    nest->Configure();

    // retrieve by reference
    BOOST_CHECK(GetService<TestParam>("test1").i_ == 134);

    delete nest;

    // Recreate the framework instance
    nest = new HAWCNest();

    nest->Service<TestParam>("test2")
      ("i", 789)
      ("s", "foo")
      ("nodefault", "somethingspecial");

    nest->Configure();

    // retrieve by reference
    BOOST_CHECK(GetService<TestParam>("test2").i_ == 789);

    delete nest;
  }

  // ___________________________________________________________________________
  // Create a HAWCNest instance on the heap, deallocate it, and do it again
  BOOST_AUTO_TEST_CASE(NestIniConfig_test)
  {
    HAWCNest nest;

    string config="[test]\n"
      "service_type = TestParam\n"
      "i = 134\n"
      "s = bar\n"
      "vs = []\n"
      "blank =\n"
      "nodefault = somethingspecial\n";
    istringstream fin(config);

    NestIniConfig(nest,fin);

    nest.Configure();
    
    // retrieve by reference
    BOOST_CHECK(GetService<TestParam>("test").i_ == 134);
    BOOST_CHECK(GetService<TestParam>("test").s_ == "bar");
    BOOST_CHECK(GetService<TestParam>("test").nodefault_ == "somethingspecial");
    vector<string> vs;
    BOOST_CHECK(GetService<TestParam>("test").vs_.size() == 0);
    BOOST_CHECK(GetService<TestParam>("test").vs_ == vs);
    vector<int> vi;
    BOOST_CHECK(GetService<TestParam>("test").vi_.size() == 0);
    BOOST_CHECK(GetService<TestParam>("test").vi_ == vi);


  }

BOOST_AUTO_TEST_SUITE_END()

