/*!
 * @file CommandLine.cc 
 * @brief Unit test for the command line configuration.
 * @author Segev BenZvi 
 * @date 18 Jan 2012 
 * @ingroup unit_test
 * @version $Id: CommandLine.cc 18844 2014-02-18 01:42:07Z sybenzvi $
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <hawcnest/CommandLineConfigurator.h>

#include <vector>

using namespace std;

// Create the "CommandLineTest" testing suite
BOOST_AUTO_TEST_SUITE(CommandLineTest)

  // ___________________________________________________________________________
  // Test simple command line flags which take no arguments
  BOOST_AUTO_TEST_CASE(Flags)
  {
    CommandLineConfigurator cl;
    cl.AddFlag("blue,b", "Set the blue flag.");
    cl.AddFlag("red,r", "Set the red flag.");

    // Check simulated command line input
    const int argc = 3;
    const char* argv[argc] = { "prog_name",
                               "-b", "--red" };

    BOOST_CHECK(cl.ParseCommandLine(argc, const_cast<char**>(argv)));
    BOOST_CHECK(cl.HasFlag("blue"));
    BOOST_CHECK(cl.HasFlag("red"));
    BOOST_CHECK(!cl.HasFlag("green"));
  }

  // ___________________________________________________________________________
  // Test options with default values
  BOOST_AUTO_TEST_CASE(DefaultOptions)
  {
    CommandLineConfigurator cl;
    cl.AddOption<int>("number,n", 5, "This could be any number, really.");
    cl.AddOption<string>("string,s", "Hello!", "Just a string, nothing more.");
    cl.AddOption<double>("double,d", 0., "A double-precision float");

    // Check simulated command line input.  Overwrite one default option.
    const int argc = 3;
    const char* argv[argc] = { "prog_name",
                               "-d", "3.14159" };

    BOOST_CHECK(cl.ParseCommandLine(argc, const_cast<char**>(argv)));
    BOOST_CHECK_EQUAL(cl.GetArgument<int>("number"), 5);
    BOOST_CHECK_EQUAL(cl.GetArgument<string>("string"), "Hello!");
    BOOST_CHECK_EQUAL(cl.GetArgument<double>("double"), 3.14159);
  }

  // ___________________________________________________________________________
  // Test a few standard options available to all AERIE programs
  BOOST_AUTO_TEST_CASE(StandardOptions)
  {
    CommandLineConfigurator cl;

    // Check simulated command line input
    const int argc = 4;
    const char* argv[argc] = { "prog_name",
                               "--verbosity=4",   // program verbosity
                               "-x", "0" };       // floating-point exceptions

    BOOST_CHECK(cl.ParseCommandLine(argc, const_cast<char**>(argv)));
    BOOST_CHECK_EQUAL(cl.GetArgument<int>("verbosity"), 4);
    BOOST_CHECK_EQUAL(cl.GetArgument<unsigned int>("fpexcept"), 0u);

    // SYB (Jan 2012): setting fp exceptions to a non-zero value seems to cause
    // SEGFAULTS on my Mac due to a weird interaction with Boost Test.
    // Will investigate this... eventually.
  }

  // ___________________________________________________________________________
  // Test initialization of multi-options ("-arg" string followed by a list)
  BOOST_AUTO_TEST_CASE(MultiOptions)
  {
    typedef vector<string> Files;

    CommandLineConfigurator cl;
    cl.AddMultiOption<Files>("input", "Input name(s)");
    cl.AddMultiOption<Files>("output", "Output name(s)");

    // Cannot add a positional option once we defined a multi-option
    BOOST_CHECK_THROW(
      cl.AddPositionalOption<Files>("more-input", "More input(s)"),
      runtime_error);

    // Check simulated command line input
    const int argc = 9;
    const char* argv[argc] = { "prog_name",
                               "--input", "i1", "i2", "i3",
                               "--output", "o1", "o2", "o3" };

    BOOST_CHECK(cl.ParseCommandLine(argc, const_cast<char**>(argv)));

    // Check multi-option lists
    Files inputs = cl.GetArgument<Files>("input");
    BOOST_CHECK_EQUAL_COLLECTIONS(inputs.begin(), inputs.end(),
                                  argv + 2, argv + 5);

    Files outputs = cl.GetArgument<Files>("output");
    BOOST_CHECK_EQUAL_COLLECTIONS(outputs.begin(), outputs.end(),
                                  argv + 6, argv + 9);
  }

  // ___________________________________________________________________________
  // Test initialization of command line positional options (no "-arg" required)
  BOOST_AUTO_TEST_CASE(PositionalOptions)
  {
    typedef vector<string> Files;

    CommandLineConfigurator cl;
    cl.AddPositionalOption<Files>("input", "Input name(s)");

    // Cannot add multiple positional options
    BOOST_CHECK_THROW(
      cl.AddPositionalOption<Files>("more-input", "More input(s)"),
      runtime_error);

    // Cannot add a multi-option once a positional option is defined
    BOOST_CHECK_THROW(
      cl.AddMultiOption<Files>("even-more-input", "Even more input(s)"),
      runtime_error);

    // Check simulated command line input
    const int argc = 6;
    const char* argv[argc] = { "prog_name",
                               "one", "two", "three", "four", "five" };

    BOOST_CHECK(cl.ParseCommandLine(argc, const_cast<char**>(argv)));

    // Check inputs
    Files files = cl.GetArgument<Files>("input");
    BOOST_CHECK_EQUAL_COLLECTIONS(files.begin(), files.end(),
                                  argv + 1, argv + argc);
  }

BOOST_AUTO_TEST_SUITE_END()

