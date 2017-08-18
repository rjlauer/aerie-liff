/*!
 * @file UnitTestFormatter.h 
 * @brief Control output formatting of Boost unit tests.
 * @author Segev BenZvi 
 * @date 26 Jan 2012 
 * @version $Id: UnitTestFormatter.h 26970 2015-09-23 15:12:21Z sybenzvi $
 */

#ifndef HAWCNEST_HAWCNEST_TEST_UNITTESTFORMATTER_H_INCLUDED
#define HAWCNEST_HAWCNEST_TEST_UNITTESTFORMATTER_H_INCLUDED

#include <boost/test/unit_test_log_formatter.hpp>

namespace but = boost::unit_test;

/*!
 * @class UnitTestFormatter
 * @author Segev BenZvi
 * @date 26 Jan 2012
 * @ingroup aerie_test
 * @brief A customization of the Boost unit test log formatter that controls
 *        how test output is written to output streams.
 */
class UnitTestFormatter : public but::unit_test_log_formatter {

  public:

    UnitTestFormatter();

    void log_start(std::ostream& os, but::counter_t ncases);

    void log_finish(std::ostream& os);

    void log_build_info(std::ostream& os);

    void test_unit_start(std::ostream& os, but::test_unit const& tu);

    void test_unit_finish(std::ostream& os, but::test_unit const& tu,
                                            unsigned long elapsed);

    void test_unit_skipped(std::ostream& os, but::test_unit const& tu);

    void log_exception(std::ostream& os, but::log_checkpoint_data const& cd,
                                         boost::execution_exception const& e);

    void log_exception_start(std::ostream& os,
                                         but::log_checkpoint_data const& cd,
                                         boost::execution_exception const& e);

    void log_exception_finish(std::ostream& os);

    void entry_context_start(std::ostream& os, but::log_level l) { }

    void log_entry_context(std::ostream& os, but::const_string value) { }

    void entry_context_finish(std::ostream& os) { }

    void log_entry_start(std::ostream& os, but::log_entry_data const& l,
                                           log_entry_types let);

    void log_entry_value(std::ostream& os, but::const_string value);

    void log_entry_value(std::ostream& os, but::lazy_ostream const& value);

    void log_entry_finish(std::ostream& os);

  private:

    int failures_;
    bool firstLog_;

    void print_prefix(std::ostream& os, but::const_string file, std::size_t ln);

};

#endif // HAWCNEST_HAWCNEST_TEST_UNITTESTFORMATTER_H_INCLUDED

