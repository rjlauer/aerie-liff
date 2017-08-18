/*!
 * @file SystemOfUnitsTest.cc 
 * @brief Unit test of HAWCUnits and unit parsing/lexing.
 * @author Segev BenZvi 
 * @date 18 Jan 2012 
 * @ingroup unit_test
 * @version $Id: SystemOfUnitsTest.cc 18844 2014-02-18 01:42:07Z sybenzvi $
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/Parser.h>

#include <vector>

using namespace HAWCUnits;

// Create the "SystemOfUnitsTest" testing suite
BOOST_AUTO_TEST_SUITE(SystemOfUnitsTest)

  // ___________________________________________________________________________
  // Test the IsDefined function in the unit evaluator
  BOOST_AUTO_TEST_CASE(UnitNames)
  {
    const HAWCUnits::Evaluator& e = HAWCUnits::Evaluator::GetInstance();

    // Tests that should pass
    #define X(name, value) BOOST_CHECK(e.IsDefined(#name));
    #include <hawcnest/impl/unit_defs.h>
    #undef X

    // Tests that should fail
    BOOST_CHECK(!e.IsDefined("spongebob"));
    BOOST_CHECK(!e.IsDefined("squarepants"));
  }

  // ___________________________________________________________________________
  // Test the GetUnit function in the unit evaluator
  BOOST_AUTO_TEST_CASE(UnitValues)
  {
    const HAWCUnits::Evaluator& e = HAWCUnits::Evaluator::GetInstance();

    // Tests that should pass
    #define X(name, value) BOOST_CHECK_EQUAL(e.GetUnit(#name), value);
    #include <hawcnest/impl/unit_defs.h>
    #undef X

    // Tests that should fail
    BOOST_CHECK_THROW(e.GetUnit("spongebob"), std::runtime_error);
    BOOST_CHECK_THROW(e.GetUnit("squarepants"), std::runtime_error);
  }

  // ___________________________________________________________________________
  // Check the unit expression parser
  BOOST_AUTO_TEST_CASE(UnitParser)
  {
    HAWCUnits::Parser p;
    BOOST_CHECK_EQUAL(p.Evaluate("km/m"), 1e3);
    BOOST_CHECK_EQUAL(p.Evaluate("sqrt(km/mm)"), 1e3);
  }

BOOST_AUTO_TEST_SUITE_END()

