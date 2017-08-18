/*!
 * @filename CommandLineConfigurator-inl.h
 * @brief Implementation of the command-line parser.
 * @author Segev BenZvi
 * @date 17 Sep 2010
 * @version $Id: CommandLineConfigurator-inl.h 27430 2015-10-29 19:40:54Z sybenzvi $
 */

#include <hawcnest/Logging.h>

#include <sstream>

template<class T>
void
OptionGroup::AddOption(
  const std::string& name, const std::string& desc)
{
  // Save the option
  opts_.add_options()
    (name.c_str(), boost::program_options::value< T >(), desc.c_str());
}

template<class T>
void
OptionGroup::AddOption(
  const std::string& name, const T& init, const std::string& desc)
{
  // Save the option
  boost::shared_ptr<T> optPtr;
  opts_.add_options()
    (name.c_str(),
     boost::program_options::value<T>(optPtr.get())->default_value(init),
     desc.c_str());
}

template<class T>
void
OptionGroup::AddMultiOption(
  const std::string& name, const std::string& desc)
{
  // To avoid nasty parsing issues, don't allow mixing with positional option
  if (hasPositionalOption_) {
    log_fatal("Could not add option \"" << name
              << ";\" cannot mix multi-option with a positional option");
  }

  boost::shared_ptr<T> optPtr;
  opts_.add_options()
    (name.c_str(),
     boost::program_options::value<T>(optPtr.get())->multitoken(),
     desc.c_str());

  hasMultiOption_ = true;
}

template<class T>
void
CommandLineConfigurator::AddOption(
  const std::string& name, const std::string& desc)
{
  // Save the option
  copts_.add_options()
    (name.c_str(), boost::program_options::value< T >(), desc.c_str());
}

template<class T>
void
CommandLineConfigurator::AddOption(
  const std::string& name, const T& init, const std::string& desc)
{
  // Save the option
  boost::shared_ptr<T> optPtr;
  copts_.add_options()
    (name.c_str(),
     boost::program_options::value<T>(optPtr.get())->default_value(init),
     desc.c_str());
}

template<class T>
void
CommandLineConfigurator::AddMultiOption(
  const std::string& name, const std::string& desc)
{
  // To avoid nasty parsing issues, don't allow mixing with positional option
  if (hasPositionalOption_) {
    log_fatal("Could not add option \"" << name
              << ";\" cannot mix multi-option with a positional option");
  }

  boost::shared_ptr<T> optPtr;
  copts_.add_options()
    (name.c_str(),
     boost::program_options::value<T>(optPtr.get())->multitoken(),
     desc.c_str());

  hasMultiOption_ = true;
}

template<class T>
void
CommandLineConfigurator::AddPositionalOption(
  const std::string& name, const std::string& desc)
{
  // To avoid nasty parsing issues, allow only one positional option
  if (hasPositionalOption_) {
    log_fatal("Could not add option \"" << name
              << ";\" already have a positional option");
  }

  // To avoid nasty parsing issues, allow only one positional option
  if (hasMultiOption_) {
    log_fatal("Could not add option \"" << name
              << ";\" cannot mix a positional option with multi-options");
  }

  // Positional options should not have shortcuts in the name (e.g., -i):
  size_t pos;
  if ((pos = name.find(",")) != std::string::npos) {
    log_warn("In positional options, changing \"" << name
              << "\" to \"" << name.substr(0, pos) << "\"");
    std::string& newName = const_cast<std::string&>(name);
    newName = newName.substr(0, pos);
  }

  // Add as a regular option in the option list
  copts_.add_options()
    (name.c_str(), boost::program_options::value< T >(), desc.c_str());

  // Add as a positional option
  popts_.add(name.c_str(), -1);

  // Do not allow more than one positional option
  hasPositionalOption_ = true;
}

template<class T>
const T&
CommandLineConfigurator::GetArgument(const std::string& name)
  const
{
  if (!vm_.count(name.c_str())) {
    std::ostringstream s;
    s << "\nArgument " << name << " not found";
    log_fatal_nothrow(s.str());
  }

  try{
    return vm_[name].as< T >();
  }
  catch(const std::exception& e){
    log_error("Error converting command line argument ''"<<name<<"'' : "<<e.what());
    throw;
  }
}

