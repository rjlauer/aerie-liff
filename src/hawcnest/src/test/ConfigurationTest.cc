/*!
 * @file ConfigurationTest.cc 
 * @brief Unit test for framework configuration.
 * @author Segev BenZvi 
 * @date 18 Jan 2012 
 * @ingroup unit_test
 * @version $Id: ConfigurationTest.cc 40083 2017-08-07 17:40:11Z pretz $
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <hawcnest/Configuration.h>

#include <cstdio>

using namespace std;

// Create the "ConfigurationTest" testing suite
BOOST_AUTO_TEST_SUITE(ConfigurationTest)

  // ___________________________________________________________________________
  // Check parameter declarations, set, and get
  BOOST_AUTO_TEST_CASE(SetGetConfig)
  {
    Configuration config;
    config.Parameter<int>("i");
    config.Parameter<double>("d", 3.14159);
    config.Parameter<string>("s", "hello");

    vector<int> v;
    config.Parameter<vector<int> >("v", v);

    // Check that a floating point parameter is recovered (to 1e-3% precision)
    double dOut;
    config.GetParameter("d", dOut);
    BOOST_CHECK_CLOSE(dOut, 3.14159, 1e-3);

    // Check that a string parameter is recovered
    string sOut;
    config.GetParameter("s", sOut);
    BOOST_CHECK(sOut == "hello");

    // Check that an unset parameter cannot be recovered
    int iOut = 0;
    BOOST_CHECK_THROW(config.GetParameter("i", iOut), configuration_exception);

    // Try using the SetParameter method and then recover
    config.SetParameter("i", 6);
    iOut = 0;
    config.GetParameter("i", iOut);
    BOOST_CHECK(iOut == 6);

    // Check vector input/output
    vector<int> vIn;
    vIn.push_back(3);
    vIn.push_back(4);
    vIn.push_back(-699);

    config.SetParameter("v", vIn);

    vector<int> vOut;
    config.GetParameter("v", vOut);

    BOOST_CHECK_EQUAL_COLLECTIONS(vIn.begin(), vIn.end(),
                                  vOut.begin(), vOut.end());
  }

  // ___________________________________________________________________________
  // Check parameter declarations attempted more than once
  BOOST_AUTO_TEST_CASE(SetTwice)
  {
    Configuration config;
    config.Parameter<int>("i");
    config.Parameter<int>("j", 4);

    BOOST_CHECK_THROW(config.Parameter<int>("i"), configuration_exception);
    BOOST_CHECK_THROW(config.Parameter<int>("j"), configuration_exception);

    int j = 0;
    config.GetParameter("j", j);

    BOOST_CHECK(j == 4);
  }

  // ___________________________________________________________________________
  // Check string parameters
  BOOST_AUTO_TEST_CASE(NonConstString)
  {
    Configuration config;
    config.Parameter<string>("s");

    char value[1000];
    sprintf(value, "value");
    config.SetParameter<string>("s", value);

    string out;
    config.GetParameter("s", out);

    BOOST_CHECK(out == "value");
  }

  // ___________________________________________________________________________
  // Check failure to retrieve a nonexistent parameter
  BOOST_AUTO_TEST_CASE(NonexistentRetrieval)
  {
    Configuration config;
    int iOut = 0;

    BOOST_CHECK_THROW(config.GetParameter("i", iOut), configuration_exception);
  }

  // ___________________________________________________________________________
  // Check type mismatch on retrieval
  BOOST_AUTO_TEST_CASE(BadRetrieval)
  {
    Configuration config;
    config.Parameter<string>("s", "foo");
    config.Parameter<int>("i", 4);

    string s;
    int i;

    BOOST_CHECK_THROW(config.GetParameter("i", s), configuration_exception);
    BOOST_CHECK_THROW(config.GetParameter("s", i), configuration_exception);
  }

  // ___________________________________________________________________________
  // Check type mismatch on SetParameter
  BOOST_AUTO_TEST_CASE(BadSetParameter)
  {
    Configuration config;
    config.Parameter<string>("s", "foo");

    BOOST_CHECK_THROW(config.SetParameter("s", 5), configuration_exception);
  }

  // ___________________________________________________________________________
  // Check implicit parameter conversion from int to double
  BOOST_AUTO_TEST_CASE(SetDoubleWithInt)
  {
    Configuration config;
    config.Parameter<double>("d", 5.639);

    int dSet = 3;
    config.SetParameter("d", dSet);

    double dOut = 0.;
    config.GetParameter("d", dOut);

    BOOST_CHECK_CLOSE(dOut, 3, 1e-3);
  }

  // ___________________________________________________________________________
  // Compose two configurations into one
  BOOST_AUTO_TEST_CASE(Composition)
  {
    Configuration config1;
    config1.Parameter<int>("i1", 3);
    config1.Parameter<int>("j1", 18);
    config1.Parameter<double>("d1", 3.14);
    config1.Parameter<double>("unset");

    Configuration config2;
    config2.Parameter<int>("i2", 9);
    config2.Parameter<int>("j2", 45);
    config2.Parameter<double>("d2", 2.4);

    Configuration config = compose(config1, config2);

    int i1, i2, j1, j2;

    config.GetParameter("i1", i1);
    BOOST_CHECK(i1 == 3);

    config.GetParameter("i2", i2);
    BOOST_CHECK(i2 == 9);

    config.GetParameter("j1", j1);
    BOOST_CHECK(j1 == 18);

    config.GetParameter("j2", j2);
    BOOST_CHECK(j2 == 45);

    double d1, d2;

    config.GetParameter("d1", d1);
    BOOST_CHECK_CLOSE(d1, 3.14, 1e-3);

    config.GetParameter("d2", d2);
    BOOST_CHECK_CLOSE(d2, 2.4, 1e-3);

    // Unset parameters raise an exception if we try to get them
    double unset;
    BOOST_CHECK_THROW(config.GetParameter("unset", unset),
                      configuration_exception);
  }

  // ___________________________________________________________________________
  // Compose two configurations into one
  BOOST_AUTO_TEST_CASE(NutsAndBolts)
  {
    BOOST_CHECK_EQUAL(ConfigurationUtil::bookend("[a,b,c,d,e]"),
		      "a,b,c,d,e");

    BOOST_CHECK_EQUAL(ConfigurationUtil::bookend("  [a,b,c,d,e]  "),
		      "a,b,c,d,e");


    BOOST_CHECK_EQUAL(ConfigurationUtil::stripWhite("  [a,b,c,d,e]  "),
		      "[a,b,c,d,e]");

    BOOST_CHECK_EQUAL(ConfigurationUtil::stripWhite("1"),
		      "1");

    BOOST_CHECK_EQUAL(ConfigurationUtil::stripWhite("-1"),
		      "-1");


  }


  // ___________________________________________________________________________
  // Compose two configurations into one
  BOOST_AUTO_TEST_CASE(SetViaString)
  {
    Configuration config;
    config.Parameter<int>("i", 3);
    config.Parameter<int>("j", 18);
    config.Parameter<bool>("b", true);
    config.Parameter<double>("d", 3.14);
    config.Parameter<vector<double> >("vd");
    config.Parameter<vector<int> >("vi");
    config.Parameter<vector<string> >("vs");
    config.Parameter<double>("unset");
    config.Parameter<string>("s");

    vector<double> vd;
    vector<double> vd_check;
    vd_check.push_back(3.4);
    vd_check.push_back(3.2);
    vd_check.push_back(5.6);
    vd_check.push_back(-1.2);
    config.SetParameter_decode("vd","[3.4,3.2,5.6,-1.2]");
    config.GetParameter("vd",vd);
    BOOST_CHECK(vd.size() == 4);
    for(unsigned i = 0 ; i < 4 ; i++){
      BOOST_CHECK_CLOSE(vd[i],vd_check[i],0.01);
    }

    vector<int> vi;
    vector<int> vi_check;
    vi_check.push_back(3);
    vi_check.push_back(3);
    vi_check.push_back(5);
    vi_check.push_back(-1);
    config.SetParameter_decode("vi","[3, 3,5,-1]");
    config.GetParameter("vi",vi);
    BOOST_CHECK(vi.size() == 4);
    for(unsigned i = 0 ; i < 4 ; i++){
      BOOST_CHECK(vi[i]==vi_check[i]);
    }

    vector<string> vs;
    vector<string> vs_check;
    vs_check.push_back("hello");
    vs_check.push_back("world");
    vs_check.push_back("its");
    vs_check.push_back("me");
    config.SetParameter_decode("vs","[hello,world,its,me]");
    config.GetParameter("vs",vs);
    BOOST_CHECK(vs.size() == 4);
    for(unsigned i = 0 ; i < 4 ; i++){
      BOOST_CHECK(vs[i]==vs_check[i]);
    }

    config.SetParameter_decode("vs","   [  hello ,world ,  its, me] ");
    config.GetParameter("vs",vs);
    BOOST_CHECK(vs.size() == 4);
    for(unsigned i = 0 ; i < 4 ; i++){
      BOOST_CHECK(vs[i]==vs_check[i]);
    }


    config.SetParameter_decode("s","  string_here ");
    string s;
    config.GetParameter("s",s);
    BOOST_CHECK(s=="string_here");

    config.SetParameter_decode("s","string_goes_here");
    config.GetParameter("s",s);
    BOOST_CHECK(s=="string_goes_here");

    config.SetParameter_decode("i","10");
    int i;
    config.GetParameter("i",i);
    BOOST_CHECK(i==10);

    config.SetParameter_decode("d","5.16");
    double d;
    config.GetParameter("d",d);
    BOOST_CHECK_CLOSE(d,5.16,1e-3);


    config.SetParameter_decode("d","   2.34  ");
    config.GetParameter("d",d);
    BOOST_CHECK_CLOSE(d,2.34,1e-3);

    bool b;
    config.SetParameter_decode("b","false");
    config.GetParameter("b",b);
    BOOST_CHECK(!b);

    config.SetParameter_decode("b","  true  ");
    config.GetParameter("b",b);
    BOOST_CHECK(b);

    BOOST_CHECK_THROW(config.SetParameter_decode("b","  truuuue  "),
		      ConfigurationUtil::parse_exception);

  }

  BOOST_AUTO_TEST_CASE(TestParsing)
  {
    BOOST_CHECK(ConfigurationUtil::bookend("[]")=="");

    BOOST_CHECK(ConfigurationUtil::tokenize("",',').size() == 0);

    BOOST_CHECK_THROW(ConfigurationUtil::tokenize(",,",','),
		      ConfigurationUtil::parse_exception);

    BOOST_CHECK(ConfigurationUtil::tokenize("a,b,c",',').size()==3);

    BOOST_CHECK_THROW(ConfigurationUtil::tokenize(",",','),
		      ConfigurationUtil::parse_exception);

  }

BOOST_AUTO_TEST_SUITE_END()

