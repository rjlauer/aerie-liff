/*!
 * @file CommandLineConfigurator.cc 
 * @brief Implementation of a wrapper of the Boost program option tool.
 * @author Segev BenZvi 
 * @date 19 Sep 2010 
 * @version $Id: CommandLineConfigurator.cc 27839 2015-11-18 22:52:11Z sybenzvi $
 */

#include <hawcnest/CommandLineConfigurator.h>
#include <hawcnest/SoftwareVersion.h>

#ifdef __GNUC__
#ifdef __APPLE__
#include <xmmintrin.h>
#else
#include <fenv.h>
#endif
#endif // __GNUC__

#include <boost/format.hpp>

using namespace std;
using namespace boost::program_options;

typedef unsigned int fpexcept_t;

namespace {
  void enableFPExceptions(const fpexcept_t& mask);
}

/*
 * OptionGroup: constructor and various member functions
 */
OptionGroup::OptionGroup(const string& name) :
  opts_(name),
  hasPositionalOption_(false),
  hasMultiOption_(false)
{
}

void
OptionGroup::AddFlag(const string& name, const string& desc)
{
  opts_.add_options()
    (name.c_str(), desc.c_str());
}

/*
 * CommandLineConfigurator: constructor and various member functions
 */
CommandLineConfigurator::CommandLineConfigurator(const string& dscr) :
  progDscr_(dscr),
  gopts_("Generic options"),
  copts_("Configuration"),
  hasPositionalOption_(false),
  hasMultiOption_(false),
  verbosity_(3),
  fpExceptMask_(0)
{
  // These are generic options for the execution of the framework.  Settings
  // for modules should be passed via config files or user-provided options
  gopts_.add_options()
    ("help,h", "Write this message")
    ("version,v", "Print Aerie version info")
    ("verbosity,V", value<int>(&verbosity_)->default_value(2),
                    "Set the verbosity of the logging system: "\
                    "0=trace, 1=debug, 2=info, 3=warn, 4=error, 5=fatal")
    ("fpexcept,x", value<unsigned int>(&fpExceptMask_)->default_value(0), 
                   "Enable floating-point exceptions for debugging: " \
                   "1=invalid-arg, 4=div-by-zero, 8=overflow, 13=all")
    ("timelog", "Include current UT time in AERIE logs.");

  // Create a default options group for users to fill in
  AddOptionGroup("Configuration");
}

CommandLineConfigurator::~CommandLineConfigurator()
{
  if (!groups_.empty()) {
    for (GroupMapIterator iO = groups_.begin(); iO != groups_.end(); ++iO)
      delete iO->second;
  }
}

OptionGroup&
CommandLineConfigurator::AddOptionGroup(const string& name)
{
  if (groups_.find(name) == groups_.end()) {
    groupNames_.push_back(name);
    groups_[name] = new OptionGroup(name);
  }
  return *groups_[name];
}

bool
CommandLineConfigurator::ParseCommandLine(int argc, char* argv[])
{
  // Group the generic options
  options_description myoptions("Allowed options");
  myoptions.add(gopts_);

  // Group the user-specified options, including any extra option groups
  if (!copts_.options().empty())
    myoptions.add(copts_);

  if (!groups_.empty()) {
    vector<string>::const_iterator iN;
    for (iN = groupNames_.begin(); iN != groupNames_.end(); ++iN) {
      po::options_description& opts = groups_[*iN]->GetOptionsDescription();
      if (!opts.options().empty())
        myoptions.add(opts);
    }
  }

  try {
    // Parse options and store data in the variables map
    store(command_line_parser(argc, argv).
          options(myoptions).positional(popts_).run(), vm_);
    notify(vm_);

    // If help flag is specified, return false (user should print help & exit)
    if (vm_.count("help")) {
      if (progDscr_.empty()) {
        log_info('\n' << myoptions);
      }
      else {
        log_info("\n\nProgram description:\n\n" << progDscr_ << "\n\n"
                 << myoptions);
      }
      return false;
    }

    // If version flag is specified, return false (user prints version & exits)
    if (vm_.count("version")) {
      log_info(boost::format("\n\n %s\n Aerie version: %s\n Build type: %s\n")
               % argv[0] % SoftwareVersion(AERIE_VERSION_CODE)
               % AERIE_BUILD_TYPENAME);
      return false;
    }

    // Set up the verbosity of the logging system
    if (vm_.count("verbosity"))
      Logger::GetInstance().SetDefaultLogLevel(Logger::LoggingLevel(verbosity_));

    if (vm_.count("timelog"))
      Logger::GetInstance().SetTimeStamping();

    // Set up floating point exceptions
    if (vm_.count("fpexcept"))
      enableFPExceptions(fpExceptMask_);
  }
  catch (const error& e) {
    log_error("Command line error: " << e.what() << '\n' << myoptions);
    return false;
  }

  return true;
}

void
CommandLineConfigurator::AddFlag(const string& name, const string& desc)
{
  copts_.add_options()
    (name.c_str(), desc.c_str());
}

bool
CommandLineConfigurator::HasFlag(const std::string& name)
  const
{
  return vm_.count(name.c_str());
}

namespace {

  void
  enableFPExceptions(const fpexcept_t& mask)
  {
    #ifdef __GNUC__
      // On the Intel Mac, this will only work if the SSE instructions are used.
      // This should be OK for everything except extended double-precision
      // calculations (e.g., long double), where the 387 is used
      #ifdef __APPLE__ 
      if (mask & 0x1) {
        log_info("Enabling invalid floating-point exception, e.g., sqrt(-1)");
        _MM_SET_EXCEPTION_MASK(_MM_GET_EXCEPTION_MASK() & _MM_EXCEPT_INVALID);
      }
      if (mask & 0x4) {
        log_info("Enabling divide-by-zero floating-point exception");
        _MM_SET_EXCEPTION_MASK(_MM_GET_EXCEPTION_MASK() & _MM_EXCEPT_DIV_ZERO);
      }
      if (mask & 0x8) {
        log_info("Enabling overflow floating-point exception");
        _MM_SET_EXCEPTION_MASK(_MM_GET_EXCEPTION_MASK() & _MM_EXCEPT_OVERFLOW);
      }
      #elif __linux
      if (mask & 0x1) {
        log_info("Enabling invalid floating-point exception, e.g., sqrt(-1)");
        feenableexcept(FE_INVALID);
      }
      if (mask & 0x4) {
        log_info("Enabling divide-by-zero floating-point exception");
        feenableexcept(FE_DIVBYZERO);
      }
      if (mask & 0x8) {
        log_info("Enabling overflow floating-point exception");
        feenableexcept(FE_OVERFLOW);
      }
      #else
      log_error("Floating point exceptions will not be enabled on this" \
                "architecture");
      #endif
    #endif // __GNUC__
  }

}

