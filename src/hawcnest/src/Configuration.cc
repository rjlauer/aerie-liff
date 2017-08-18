/*!
 * @file Configuration.cc 
 * @brief HAWCNest Module configuration class.
 * @author John Pretz
 * @date 23 Jul 2009 
 * @version $Id: Configuration.cc 40060 2017-08-03 20:19:56Z pretz $
 */

#include <hawcnest/Configuration.h>

using namespace std;

void 
Configuration::SetParameter_decode(const string& name, const string& value){
  ParamMap::iterator found = parameters_.find(name);
  if (found == parameters_.end()) {
    log_error("parameter '" << name << "' not found when setting a parameter ");
    throw configuration_exception();
  }
  try {
    apply_visitor(set_via_string(value), found->second);
  }
  catch (const configuration_exception& e) {
    log_error("could not set value of parameter \"" << name << "\"");
    throw;
  }

  parametersSet_.insert(name);
}


/// Dump the configuration to the given ostream
void
Configuration::dump(std::ostream& out)
  const
{
  for (ParamMap::const_iterator iter = parameters_.begin(); 
      iter != parameters_.end() ; 
      iter++)
  {
    out << "  - " << iter->first << " -> ";
    if(parametersSet_.find(iter->first) == parametersSet_.end()){
      out<<"NO DEFAULT";
    }
    else{
      apply_visitor(print(out),iter->second);
    }
    out<<" ";
    apply_visitor(print_type(out),iter->second);
    out << "\n";
  }
}

/// Dump the configuration to the given ostream
void
Configuration::dump_ini(std::ostream& out)
  const
{
  for (ParamMap::const_iterator iter = parameters_.begin(); 
      iter != parameters_.end() ; 
      iter++)
  {
    out << iter->first << " = ";
    if(parametersSet_.find(iter->first) == parametersSet_.end()){
      out<<"---NOT SET---";
    }
    else{
      apply_visitor(print(out),iter->second);
    }
    out << "\n";
  }
}

Configuration
compose(const Configuration& lhs, const Configuration& rhs)
{
  Configuration returned;
  
  for(Configuration::ParamMap::const_iterator iter = 
        lhs.parameters_.begin();
      iter != lhs.parameters_.end(); 
      ++iter){
    returned.parameters_[iter->first] = iter->second;
  }

  for(Configuration::ParamSet::const_iterator iter = rhs.parametersSet_.begin();
      iter != rhs.parametersSet_.end() ; 
      ++iter) {
    returned.parametersSet_.insert(*iter);
  }

  for(Configuration::ParamMap::const_iterator iter = 
        rhs.parameters_.begin();
      iter != rhs.parameters_.end(); 
      ++iter) {
    returned.parameters_[iter->first] = iter->second;
  }

  for(Configuration::ParamSet::const_iterator iter = lhs.parametersSet_.begin();
      iter != lhs.parametersSet_.end() ; 
      ++iter) {
    returned.parametersSet_.insert(*iter);
  }

  return returned;
}

template <>
double
Configuration::InitializeParameter<double>() const
{
  return 0.;
}

template <>
int
Configuration::InitializeParameter<int>() const
{
  return 0;
}

template <>
bool
Configuration::InitializeParameter<bool>() const
{
  return false;
}

