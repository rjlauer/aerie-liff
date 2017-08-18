/*!
 * @file UnitTestFormatter.cc 
 * @brief Project-level formatting of the unit test output.
 * @author Segev BenZvi 
 * @date 26 Jan 2012 
 * @version $Id: UnitTestFormatter.cc 26970 2015-09-23 15:12:21Z sybenzvi $
 */

#include <hawcnest/test/UnitTestFormatter.h>

#include <boost/version.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/framework.hpp>
#include <boost/test/utils/basic_cstring/io.hpp>
#include <boost/test/utils/lazy_ostream.hpp>
#include <boost/test/execution_monitor.hpp>

#include <iostream>
#include <iomanip>

using namespace boost;
using namespace boost::unit_test;
using namespace std;

namespace {

  const_string
  test_phase_identifier()
  {
  #if BOOST_VERSION < 105900
    return framework::is_initialized()
           ? const_string(framework::current_test_case().p_name.get())
           : BOOST_TEST_L("Test setup");
  #else
    return const_string(framework::current_test_case().p_name.get());
  #endif
  }

}

UnitTestFormatter::UnitTestFormatter() :
  failures_(0),
  firstLog_(true)
{
}

void 
UnitTestFormatter::log_start(ostream& os,
  counter_t ncases)
{
  if (ncases > 0) {
    os << "Running " << ncases << " test " 
       << (ncases > 1 ? "cases..." : "case...")
       << endl;
  }
}

void 
UnitTestFormatter::log_finish(ostream& os)
{
}

void 
UnitTestFormatter::log_build_info(ostream& os)
{
}

void 
UnitTestFormatter::test_unit_start(ostream& os,
  test_unit const& tu)
{
  failures_ = 0;
  firstLog_ = true;

  const_string type(tu.p_type_name);

  if (type == "suite") {
    os << tu.p_name << " Suite:" << endl;
  }
  else if (type == "case") {
    os.setf(ios::left);
    os << "  + " 
       << setw(50) << setfill('.') << tu.p_name << setfill(' ');
    os.unsetf(ios::left);
  }
}

void
UnitTestFormatter::test_unit_finish(ostream& os,
  test_unit const& tu, unsigned long elapsed)
{
  if (elapsed > 0) {
    if (!firstLog_)
      os << "    " << tu.p_name << ":";

    if (elapsed % 1000 == 0)
      os << (failures_ > 0 ? " FAILED" : " ok")
         << " (" << elapsed / 1000 << " ms)" << endl;
    else
      os << (failures_ > 0 ? " FAILED" : " ok")
         << " (" << elapsed << " us)" << endl;
  }
}

void
UnitTestFormatter::test_unit_skipped(ostream& os,
  test_unit const& tu)
{
  os << "    SKIPPING " << tu.p_type_name << " \"" << tu.p_name << "\""
     << endl;
}

void 
UnitTestFormatter::log_exception(ostream& os,
  log_checkpoint_data const& cd, execution_exception const& e)
{
  execution_exception::location const& loc = e.where();
  os << (firstLog_ ? "\n" : "") << "    * EXCEPTION: ";
  print_prefix(os, loc.m_file_name, loc.m_line_num);

  os << "\n      "
     << (loc.m_function.is_empty() ? test_phase_identifier() : loc.m_function)
     << ": " << e.what();

  if (!cd.m_file_name.is_empty()) {
    os << '\n';
    print_prefix(os, cd.m_file_name, cd.m_line_num);
    os << "last checkpoint";
    if (!cd.m_message.empty())
      os << ": " << cd.m_message;
  }

  os << endl;

  ++failures_;
  firstLog_ = false;
}

void 
UnitTestFormatter::log_exception_start(ostream& os,
  log_checkpoint_data const& cd, execution_exception const& e)
{
  log_exception(os, cd, e);
}

void 
UnitTestFormatter::log_exception_finish(ostream& os)
{
}

void
UnitTestFormatter::log_entry_start(ostream& os,
  log_entry_data const& l, log_entry_types let)
{
  switch (let) {
    case BOOST_UTL_ET_INFO:
      os << (firstLog_ ? "\n" : "") << "      o ";
      break;
    case BOOST_UTL_ET_MESSAGE:
      os << (firstLog_ ? "\n" : "") << "      o ";
      break;
    case BOOST_UTL_ET_WARNING:
      os << (firstLog_ ? "\n" : "") << "      o ";
      break;
    case BOOST_UTL_ET_ERROR:
      ++failures_;
      os << (firstLog_ ? "\n" : "") << "    * ERROR: ";
      print_prefix(os, l.m_file_name, l.m_line_num);
      os << "\n      - ";
      break;
    case BOOST_UTL_ET_FATAL_ERROR:
      ++failures_;
      os << (firstLog_ ? "\n" : "") << "    * FATAL: ";
      print_prefix(os, l.m_file_name, l.m_line_num);
      os << "\n      - ";
      break;
    default:
      break;
  }
  firstLog_ = false;
}

void
UnitTestFormatter::log_entry_value(ostream& os, const_string value)
{
  os << value;
}

void
UnitTestFormatter::log_entry_value(ostream& os,
  lazy_ostream const& value)
{
  os << value;
}

void
UnitTestFormatter::log_entry_finish(ostream& os)
{
  os << endl;
}

void
UnitTestFormatter::print_prefix(ostream& os, const_string file, size_t line)
{
#ifdef __APPLE_CC__
  os << file << ':' << line << ": ";
#else
  os << file << '(' << line << "): ";
#endif
}

