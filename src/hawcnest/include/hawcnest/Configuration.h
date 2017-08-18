/*!
 * @file Configuration.h
 * @brief Configuration classes for the HAWCNest framework.
 * @author John Pretz
 * @date 25 Aug 2008
 * @version $Id: Configuration.h 40083 2017-08-07 17:40:11Z pretz $
 */

#ifndef HAWCNEST_CONFIGURATION_H_INCLUDED
#define HAWCNEST_CONFIGURATION_H_INCLUDED

#include <hawcnest/impl/conversion_allowed.h>
#include <hawcnest/impl/name_of.h>
#include <hawcnest/Logging.h>
#include <hawcnest/ConfigurationUtil.h>

#include <boost/variant.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <exception>

/*!
 * @class configuration_exception
 * @author John Pretz
 * @date 11 Nov 2009
 * @ingroup hawcnest_api
 * @brief Configuration exception, so that configuration exceptions can be
 *        identified
 */
class configuration_exception : public std::exception{
 public:
  virtual const char* what() const throw() {
    return "hawcnest configuration exception";
  }
};

/*!
 * @class Configuration
 * @author John Pretz
 * @date 11 Nov 2009
 * @ingroup hawcnest_api
 * @brief Big class for managing a service/module's configuration
 */
class Configuration {

  public:

    // just some convenience typedefs
    typedef std::string string;
    typedef std::ostream ostream;
    typedef std::pair<int, int> RangeI;
    typedef std::pair<double, double> RangeD;
    typedef std::vector<int> vecI;
    typedef std::vector<double> vecD;
    typedef std::vector<string> vecS;
    typedef boost::variant<int, double, string, RangeI,
                           RangeD, vecI, vecD, vecS, bool> param;
    typedef std::map<std::string, param> ParamMap;
    typedef std::set<std::string> ParamSet;

    /// Add a parameter.  The type must be specified, but the default value
    /// isn't.  You'll get problems if that parameter isn't set
    template <class T>
    void Parameter(const string& name);

    /// Add a parameter to the configuration.  Specify a default value to 
    /// be used in case one isn't given by the user
    template <class T>
    void Parameter(const string& name, const T& defaultValue);

    /// Sets the parameter to the given value.  Problems occur if the
    /// value you supply cannot be converted to the original type
    /// specified for this configuration parameter
    template <class T>
    void SetParameter(const string& name, const T& value);

    // Set a parameter where the value must be decoded from the
    // string. The format for vector parameters is, e.g. [1,2,3]
    void SetParameter_decode(const string& name, const string& value);
 
    /// Retrieve the parameter with the given name.  The raw type of the
    /// parameter must be convertible to the target
    template <class T>
    void GetParameter(const string& name, T& target) const;
 
    /// Dump the configuration to the given ostream
    void dump(ostream& out) const;

    /// Dump the configuration to the given ostream (ini format)
    void dump_ini(ostream& out) const;

    /// Does the configuration contain the given parameter name?
    bool HasParameter(const string& name) const
    { return (parameters_.find(name) != parameters_.end()); }

    /// Has a parameter's value been set, by default or otherwise?
    bool IsParameterSet(const string& name) const
    { return (parametersSet_.find(name) != parametersSet_.end()); }
 
  private:

    /// Supply the default initialization for a parameter
    template <class T>
    T InitializeParameter() const;

    // visitor voodoo for setting the variant value without losing it's type
    template <class target>
    class set_value : public boost::static_visitor<> {
      public:
        set_value(target t) : t_(t){}
      
        template <class T>
        void operator()(T& t,
                        typename 
                        boost::enable_if<conversion_allowed<target,T> >::type* 
                        d = 0)
        const {
          t = t_;
        }
      
        template <class T>
        void operator()(T& t,
                        typename 
                        boost::disable_if<conversion_allowed<target,T> >::type* 
                        d = 0) 
        const {
          log_error("bad argument to configuration in converting '"
                    << name_of<target>() <<"' to '"<< name_of<T>() <<"'" );
          throw configuration_exception();
        }
      
        target t_;
    };
  
    // visitor voodoo for dumping variant value into an ostream
    class print : public boost::static_visitor<> {
      public:
        print(ostream& o) : out_(o){}
        
        void operator()(const int& t) const { out_ << t; }
        void operator()(const double& t) const { out_ << t; }
        void operator()(const bool& t) const { 
	  t ? out_ << "true" : out_<<"false"; }
        void operator()(const std::string& t) const { out_ << t;}

        void operator()(const RangeI& t) const {
          out_ << "[" << t.first << ", " << t.second << "]";
        }

        void operator()(const RangeD& t) const {
          out_ << "[" << t.first << ", " << t.second << "]";
        }

        void operator()(const vecD& t) const {
          out_ << "[";
          for (vecD::const_iterator iter = t.begin(); iter != t.end(); iter++){
	    out_ << *iter << " ";
	    if(iter+1 != t.end())
	      out_<<",";
	  }
          out_ << "]";
        }

        void operator()(const vecI& t) const {
          out_ << "[";
          for (vecI::const_iterator iter = t.begin() ;iter != t.end() ;iter++){
            out_ << *iter;
	    if(iter+1 != t.end())
	      out_<<",";
	  }
          out_ << "]";
        }

        void operator()(const vecS& t) const {
          out_ << "[";
          for (vecS::const_iterator iter = t.begin(); iter != t.end(); iter++){
            out_ << *iter;
	    if(iter+1 != t.end())
	      out_<<",";
	  }
          out_ << "]";
        }
        
        ostream& out_;
    };

   // visitor voodoo for dumping variant type into an ostream
    class print_type : public boost::static_visitor<> {
      public:
        print_type(ostream& o) : out_(o){}
        
        void operator()(const int& t) const {out_ << "(int)";}
        void operator()(const double& t) const {out_ << "(double)";}
        void operator()(const bool& t) const {out_ << "(bool)";}
        void operator()(const std::string& t) const {out_ << "(string)";}
        void operator()(const RangeI& t) const {out_ << "(pair<int, int>)";}
        void operator()(const RangeD& t) const {out_<< "(pair<double, double>)";}
        void operator()(const vecD& t) const {out_ << "(vector<double>)";}
        void operator()(const vecI& t) const {out_ << "(vector<int>)";}
        void operator()(const vecS& t) const {out_ << "(vector<string>)";}
        
        ostream& out_;
    };
  
    // visitor voodoo for retrieving the value and giving an error if it
    // can't be converted to the requested type
    template<class target>
    class retrieve : public boost::static_visitor<> {
      public:
        retrieve(target& t) : t_(t) { }
        
        template <class T>
        void operator()(T& t,
          typename 
          boost::enable_if<conversion_allowed<T, target> >::type* = 0) const {
          t_ = t;
        }
        
        template <class T>
        void operator()(T& t, 
                        typename 
                        boost::disable_if<conversion_allowed<T, 
                                          target> >::type* = 0) const {
          log_error("bad argument retrieving configuration. Can't convert '"
                    <<name_of<T>()<<"' to '"<<name_of<target>()<<"'");
          throw configuration_exception();
        }
        
        target& t_;
    };

    // visitor voodoo for setting a parameter from a string
    class set_via_string : public boost::static_visitor<> {
      public:
      

    set_via_string(const string& s) : data_(s){}
      
      void operator()(int& t) const { t=ConfigurationUtil::parse<int>(data_);}
      void operator()(double& t) const { t=ConfigurationUtil::parse<double>(data_); }
      void operator()(bool& t) const { t=ConfigurationUtil::parse<bool>(data_); }
        void operator()(std::string& t) const { t=ConfigurationUtil::parse<string>(data_);}

        void operator()(RangeI& t) const { }
        void operator()(RangeD& t) const { }

        void operator()(vecD& t) const {t=ConfigurationUtil::parse<vecD>(data_);}
        void operator()(vecI& t) const {t=ConfigurationUtil::parse<vecI>(data_);}
        void operator()(vecS& t) const {t=ConfigurationUtil::parse<vecS>(data_);}
        
        string data_;
    };

  
  ParamMap parameters_;
  ParamSet parametersSet_;

 public:
  friend Configuration compose(const Configuration& lhs,
                               const Configuration& rhs);
};

Configuration compose(const Configuration& lhs, const Configuration& rhs);

template <class T>
void
Configuration::Parameter(const string& name)
{
  if (parameters_.find(name) != parameters_.end()) {
    log_error("duplicate parameters '" << name << "' when adding a parameter"
              " of type '"<<name_of<T>()<<"'" );
    throw configuration_exception();
  }

  T t = InitializeParameter<T>();
  parameters_[name] = param(t);
}

template <class T>
T
Configuration::InitializeParameter() const
{
  return T();
}

template <>
double Configuration::InitializeParameter<double>() const;

template <>
int Configuration::InitializeParameter<int>() const;

template <>
bool Configuration::InitializeParameter<bool>() const;

template <class T>
void
Configuration::Parameter(const string& name, const T& defaultValue)
{
  if (parameters_.find(name) != parameters_.end()) {
    log_error("duplicate parameters '" << name << "' when adding a parameter"
              " of type '"<<name_of<T>()<<"'" );
    throw configuration_exception();
  }

  parameters_[name] = defaultValue;
  parametersSet_.insert(name);
}

template <class T>
void
Configuration::SetParameter(const string& name, const T& value)
{
  ParamMap::iterator found = parameters_.find(name);

  if (found == parameters_.end()) {
    log_error("parameter '" << name << "' not found when setting a parameter "
              << "of type '"<<name_of<T>()<<"'");
    throw configuration_exception();
  }

  try {
    apply_visitor(set_value<T>(value), found->second);
  }
  catch (const configuration_exception& e) {
    log_error("could not set value of parameter \"" << name << "\"");
    throw;
  }

  parametersSet_.insert(name);
}

template <class T>
void
Configuration::GetParameter(const string& name, T& target) const
{
  ParamMap::const_iterator found = parameters_.find(name);

  if(found == parameters_.end()) {
    log_error("parameter " << name << " not found when getting a parameter "
              << "of type '"<<name_of<T>()<<"'");
    throw configuration_exception();
  }

  if(parametersSet_.find(name) == parametersSet_.end()) {
    log_error("parameter " << name << " has no default value and wasn't set"
              << ".  Looking for param of type '"<<name_of<T>()<<"'");
    throw configuration_exception();
  }

  apply_visitor(retrieve<T>(target),found->second);    
}

// overload operator << so we can 'cout' the Configuration objects
inline std::ostream&
operator<<(std::ostream& o,const Configuration& conf)
{
  conf.dump(o);
  return o;
}

#endif // HAWCNEST_CONFIGURATION_H_INCLUDED

