/*!
 * @file OutputConfig.h 
 * @brief Configure screen output of Boost unit tests.
 * @author Segev BenZvi 
 * @date 26 Jan 2012 
 * @version $Id: OutputConfig.h 18844 2014-02-18 01:42:07Z sybenzvi $
 */

#ifndef HAWCNEST_HAWCNEST_TEST_OUTPUTCONFIG_H_INCLUDED
#define HAWCNEST_HAWCNEST_TEST_OUTPUTCONFIG_H_INCLUDED

#include <hawcnest/test/UnitTestFormatter.h>

#include <boost/test/output/plain_report_formatter.hpp>

#include <iostream>
#include <sstream>

/*!
 * @class OutputConfig
 * @author Segev BenZvi
 * @date 26 Jan 2012
 * @ingroup aerie_test
 * @brief This class configures the HAWCNest logger during unit tests
 *
 * This class is to be used as a global fixture for boost unit tests.  It shuts
 * down the AERIE logger to avoid clogging the test output and can also be used
 * to customize the test output.  For examples, see the boost documentation on
 * BOOST_GLOBAL_FIXTURE or look into the unit tests in each project.
 */
class OutputConfig {

  public:

    /// Setup: applied for all test suites in a framework
    OutputConfig()
    {
      // Redirect cerr (Logger::Write) to a dummy buffer
      cerr_buf_ = std::cerr.rdbuf();
      std::cerr.rdbuf(os_.rdbuf());

      // Set up a custom formatter for the unit test output
      boost::unit_test::unit_test_log.set_formatter(new UnitTestFormatter);
    }

    /// Teardown
   ~OutputConfig()
    {
      // Restor cerr (Logger::Write) to stderr
      std::cerr.rdbuf(cerr_buf_);
    }

  private:

    std::streambuf* cerr_buf_;    ///< Pointer to stderr buffer
    std::ostringstream os_;       ///< Dummy buffer

};

#endif // HAWCNEST_HAWCNEST_TEST_OUTPUTCONFIG_H_INCLUDED

