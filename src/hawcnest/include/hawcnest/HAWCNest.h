/*!
 * @file HAWCNest.h
 * @brief Central framework for service registration and event processing.
 * @author John Pretz
 * @date 25 Aug 2008
 * @version $Id: HAWCNest.h 40074 2017-08-04 20:44:52Z pretz $
 */

#ifndef HAWCNEST_HAWCNEST_H_INCLUDED
#define HAWCNEST_HAWCNEST_H_INCLUDED

#include <hawcnest/Service.h>
#include <hawcnest/processing/Module.h>
#include <hawcnest/processing/Source.h>
#include <hawcnest/Configuration.h>
#include <hawcnest/Logging.h>

#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <vector>
#include <cstdio>
#include <algorithm>
#include <sstream>

namespace boost {
  namespace python {
    namespace api {
      class object;
    }
    using api::object;
  }
}

/*!
 * @class hawcnest_exception
 * @author John Pretz
 * @date 25 Aug 2008
 * @ingroup hawcnest_api
 * @brief One HAWCNest exception class.  Separate class so that framework
 *        issues can be separated from exceptions in some client code.
 */
class hawcnest_exception : public std::exception {
  public:
    virtual const char* what() const throw() {
      return "hawcnest framework exception";
    }
};

// declaration so we can use it in the HAWCNest definition.
class HAWCNest;
std::ostream& operator<<(std::ostream& os, const HAWCNest& h);

/*!
 * @class HAWCNest
 * @author John Pretz
 * @ingroup hawcnest_api
 * @date 25 Aug 2008
 * @brief The HAWCNest is the central framework object.  All services
 *        are added to an instance of this class and the can be used.
 *
 * Services which implement the Module and Source interfaces are treated
 * slightly uniquely and are available after they've been added via GetSources
 * and GetModules.  Services are configured by specifying the Service instance
 * and giving a list of parameters, like so:
 *
 * @code
 * HAWCNest nest;
 *
 * // Services instances are named and configured below:
 * nest.Service("StdRNGService", "rng")
 *   ("seed", 0);
 *
 * nest.Service("MyModule", "mine")
 *   ("boolFlag", true)
 *   ("intParam", 3)
 *   ("doubleParam", 3.14159)
 *   ("stringParam", "I like string.")
 *
 * nest.Configure();
 * @endcode
 */
class HAWCNest {

  SHARED_POINTER_TYPEDEFS(ServiceWrapperBase);

  public:

    HAWCNest();
   ~HAWCNest();

    /*!
     * @class configurator
     * @brief A class used to configure modules with a nice interface.
     *
     * When Services are added to the HAWCNest instance, this class is
     * returned, allowing for a list of parameters to be specified. 
     */
    class configurator {
      public:
        configurator(Configuration& config) : config_(&config) { }
        configurator(Configuration* config) : config_(config) { }
        configurator() : config_(NULL) { }
        configurator(const configurator& origin) : config_(origin.config_) { }
        
        template<class ParameterType>
        configurator operator()(const std::string& name, ParameterType value) {
          if(config_){
            config_->SetParameter(name,value);
          }
          configurator config(config_);
          return config;
        }

      friend class HAWCNest;

      private:
        Configuration* config_;
      
    };

    /// Adding services with a string for the type and instance name
    configurator
    Service(const std::string& type, const std::string& name);

    /// Adding services with a string for the type and instance name
    /// Won't complain if the service type doesn't exist
    configurator
    ServiceIfExists(const std::string& type, const std::string& name);

    /// Adding services using a python object and instance name
    configurator
    Service(const boost::python::object& obj, const std::string& name);

    /// For adding services with full C++ type and instance name
    template <class ServiceType>
    configurator 
    Service(const std::string& name);

    /// Checks if a service with the given name has already been added
    bool ServiceExists(const std::string& name);

    /// Getting a parameter of an already-added service.  If there's
    /// no call to Configure() yet, it will overwrite the old parameter
    template <class T>
    void
    SetParameter(const std::string& srvname, const std::string& parname,
                 const T& par);

    /// Set parameters directly using objects passed from boost::python
    void
    SetParameter(const std::string& name, const std::string& parname,
                 const boost::python::object& obj);

    /// Set parameters directly using a decoded string
    void 
    SetParameter_decode(const std::string& name, 
                        const std::string& parname,
                        const std::string value);

    /// Getting a parameter of an already-added service
    template <class T>
    T GetParameter(const std::string& srvname, const std::string& parname);
  
    /// Configures all services 
    void Configure();

    /// Retrieve the named main loop and execute it.
    void ExecuteMainLoop(const std::string& mainloop);
    
    /// Calls Finish on all instantiated Services
    void Finish() {
      std::for_each(services_.begin(), services_.end(),
                    boost::bind(&ServiceWrapperBase::Finish,_1));
    }

    /// Just prints all the configurations
    void dump(std::ostream& o) const {
      for (std::vector<boost::function<void (std::ostream&)> >::const_iterator 
            iter = printers_.begin();
          iter != printers_.end(); 
          ++iter)
      {
        iter->operator()(o);
      }
    }

    /// Just prints all the configurations
    void dump_ini(std::ostream& o) const {
      for(std::vector<ServiceWrapperBasePtr>::const_iterator iter = services_.begin() ; 
	  iter != services_.end() ; 
	  iter++){
	o<<"["<<(*iter)->GetName()<<"]\n";
	o<<"service_type = "<<(*iter)->GetType()<<"\n";
	std::map<std::string,configurator>::const_iterator found = 
	  configurators_.find((*iter)->GetName());
	if(found == configurators_.end())
	  log_fatal("issue printing service named '"<<(*iter)->GetName());
	found->second.config_->dump_ini(o);
	o<<"\n";
      }
    }

  private:

    std::vector<ServiceWrapperBasePtr> services_;
    std::vector<boost::function<void ()> > atDestruction_;
    std::vector<boost::function<void (std::ostream&)> > printers_;
    std::map<std::string, configurator> configurators_;
    static int instance_;

    /// clears map (for use at destruction)
    template <class MapType>
    class reset_map {
      public:
        reset_map(MapType& maptype) : maptype_(maptype){}
        void operator()() { maptype_.clear(); }
        MapType& maptype_;
    };
 
    /// prints a single configuration. 
    class printconfig {
      public:
        printconfig(Configuration& config) : config_(config){}
        void operator()(std::ostream& o){config_.dump(o);}
        Configuration& config_;
    };

};

inline
std::ostream& operator<<(std::ostream& os, const HAWCNest& h)
{
  h.dump(os);
  return os;
}

template <class ServiceType>
HAWCNest::configurator 
HAWCNest::Service(const std::string& name)
{
  // Adding a service

  // typedef just to reduce typing
  typedef typename ServiceType::Interface Interface;
  SHARED_POINTER_TYPEDEFS(Interface);
  typedef typename std::map<std::string,InterfacePtr> Services;

  // making sure that when the HAWCNest instance is destroyed
  // that the list of services is reset
  Services& services = ServiceLifetimeControl<Interface>::GetServices();
  atDestruction_.push_back(reset_map<Services>(services));
  if (services.find(name) != services.end()) {
    log_fatal("service with name '" << name 
              << "' already exists when adding service of type '"
              << name_of<ServiceType>() << "'");
  }
  
  // instance of the new service is created by the ServiceWrapper
  // This below is just the bookeeping, keeping our list of services 
  // up to date
  boost::shared_ptr<ServiceWrapper<ServiceType> > 
    wrappedservice(new ServiceWrapper<ServiceType>(name));
  services_.push_back(wrappedservice);
  services[name] = wrappedservice->wrappedService_;
  printers_.push_back(printconfig(wrappedservice->configuration));

  // the thing that will configure the service. HAWCNest keeps
  // a copy in configurators_ for HAWCNest::SetParameter
  configurator config(wrappedservice->configuration);
  configurators_.insert(std::make_pair(name,config));

  return config;
}

template <class T>
void
HAWCNest::SetParameter(const std::string& servicename,
                       const std::string& parametername, const T& parameter)
{
  // find appropriate configurator and call it
  std::map<std::string, configurator>::iterator found = 
    configurators_.find(servicename);
  if (found == configurators_.end())
    log_fatal("service with name " << servicename << " not found when setting"
              " parameter " << parametername);
  found->second(parametername,parameter);
}

template <class T>
T
HAWCNest::GetParameter(const std::string& servicename,
                       const std::string& parametername)
{
  T returned;
  
  // find appropriate configurator and call it
  std::map<std::string, configurator>::iterator found = 
    configurators_.find(servicename);
  if (found == configurators_.end())
    log_fatal("service with name " << servicename << " not found when setting"
              " parameter " << parametername);

  (found->second.config_)->GetParameter(parametername,returned);
  return returned;
}

#endif // HAWCNEST_HAWCNEST_H_INCLUDED

