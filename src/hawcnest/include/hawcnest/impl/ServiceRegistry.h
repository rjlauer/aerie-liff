/*!
 * @file ServiceRegistry.h 
 * @brief Registration dictionary of services, by name.
 * @author John Pretz 
 * @date 23 Aug 2009 
 * @version $Id: ServiceRegistry.h 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#ifndef HAWCNEST_SERVICEREGISTRY_H_INCLUDED
#define HAWCNEST_SERVICEREGISTRY_H_INCLUDED

#include <hawcnest/impl/name_of.h>
#include <hawcnest/HAWCNest.h>
#include <hawcnest/Logging.h>

#include <boost/function.hpp>
#include <boost/utility.hpp>

#include <string>
#include <map>
#include <vector>
#include <stdexcept>

/*!
 * @class ServiceRegistry
 * @author John Pretz
 * @ingroup hawcnest_impl
 * @date 23 Aug 2009
 * @brief This is the class that manages the mapping of string->typename
 *        so that the python and XML configuration of the framework works.
 */
class ServiceRegistry {

  public:

    typedef HAWCNest::configurator configurator;
    typedef std::pair<std::string, std::string> Registration;
    typedef std::vector<Registration> RegistrationList;

  private:

    typedef boost::function<configurator(HAWCNest&, const std::string&)> 
      registration_function;

    typedef std::map<std::string, registration_function>
      RegistrationMap;

    typedef std::map<std::string, std::string> InterfaceMap;

  public:

    /// Take a ServiceType and register it
    template <class ServiceType>
    void 
    RegisterService()
    {
      registration_function reg = service_registration<ServiceType>();
      registry_[name_of<ServiceType>()] = reg;
      interfaces_[name_of<ServiceType>()] =
        name_of<typename ServiceType::Interface>();
    }

    /// Return a dictionary of registered Services and interface types
    RegistrationList
    GetRegisteredServiceTypes()
    {
      RegistrationList registered;
      for (RegistrationMap::const_iterator iter = registry_.begin(); 
           iter != registry_.end(); ++iter)
      {
        registered.push_back(
          std::make_pair(iter->first, interfaces_[iter->first]));
      }
      return registered;
    }

    /// Add a Service of type 'type' to an instance of HAWCNest with a given
    /// instance name
    configurator
    AddService(const std::string& type, HAWCNest& nest, const std::string& name)
    {
      RegistrationMap::iterator iter = registry_.find(type);
      if (iter == registry_.end())
        log_fatal("Attempt to add service of unregistered type " << type);
      return iter->second(nest, name);
    }
                                    
  private:

    RegistrationMap registry_;    ///< Map of service type and configurator
    InterfaceMap interfaces_;     ///< Interface list for Services

    ServiceRegistry() { }

    template <class ServiceType>
    class service_registration{

      public:

        HAWCNest::configurator
        operator()(HAWCNest& nest, const std::string& name)
        { return nest.Service<ServiceType>(name); }    

    };

  private:

    friend class ServiceRegistry& GlobalServiceRegistry();

};

/// A simple function that provides access to the global service registry
ServiceRegistry& GlobalServiceRegistry();

/// The constructor of the servie registry class
template <class ServiceType>
class DoServiceRegistration {
  public:

    DoServiceRegistration()
    { GlobalServiceRegistry().RegisterService<ServiceType>(); }

};

#endif // HAWCNEST_SERVICEREGISTRY_H_INCLUDED

