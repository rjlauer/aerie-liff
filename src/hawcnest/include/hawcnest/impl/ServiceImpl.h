/*!
 * @file ServiceImpl.h 
 * @brief Definition of the interface to services in the HAWCNest framework.
 * @author John Pretz 
 * @date 25 Aug 2008 
 * @version $Id: ServiceImpl.h 40074 2017-08-04 20:44:52Z pretz $
 */

#ifndef HAWCNEST_SERVICEIMPL_H_INCLUDED
#define HAWCNEST_SERVICEIMPL_H_INCLUDED

#include <boost/shared_ptr.hpp>
#include <hawcnest/Configuration.h>
#include <hawcnest/impl/Has.h>
#include <hawcnest/impl/is_shared_ptr.h>
#include <boost/utility.hpp>
#include <map>
#include <exception>
#include <string>

// ServiceWrapperBase and ServiceWrapper avoid the need to 
// have services subclass a common base class

/*!
 * @class ServiceWrapperBase
 * @author John Pretz
 * @ingroup hawcnest_impl
 * @brief Base wrapper for services, defining the functions needed by a given
 *        Service to operate in the framework
 */
class ServiceWrapperBase {

  public:

 ServiceWrapperBase(const std::string& name,
		    const std::string& service_type) 
   : name_(name),service_type_(service_type) { }
    virtual ~ServiceWrapperBase(){}

    // All services must have an Initialize and Finish function
    virtual void Initialize() = 0;
    virtual void Finish() = 0;

    const std::string& GetName() { return name_; }
    const std::string& GetType() { return service_type_; }

  private:

    std::string name_;
    std::string service_type_;

};

/*!
 * @class ServiceWrapper
 * @author John Pretz
 * @ingroup hawcnest_impl
 * @brief Wrapper for services.  So that the calling of service life-cycle
 *        functions can be done by virtual function
 */
template <class WrappedServiceType>
class ServiceWrapper : public ServiceWrapperBase
{
  public:

 ServiceWrapper(const std::string& name) 
   : ServiceWrapperBase(name,name_of<WrappedServiceType>()) {
      log_trace("constructing service '" << name << "'")
      wrappedService_ = boost::shared_ptr<WrappedServiceType>(
                          new WrappedServiceType());
      configuration = CallDefaultConfiguration(*wrappedService_);
    }

    ServiceWrapper(const std::string& name, const WrappedServiceType& obj)
      : ServiceWrapperBase(name,name_of<WrappedServiceType>())
    {
      log_trace("constructing service '" << name << "'");
      wrappedService_ = boost::shared_ptr<WrappedServiceType>(
                          new WrappedServiceType(obj));
      configuration = CallDefaultConfiguration(*wrappedService_);
    }

    virtual ~ServiceWrapper() { }

    void Initialize() {
      log_trace("initializing service "<<this->GetName());
      CallInitialize(*wrappedService_, configuration);
    }

    void Finish() {
      log_trace("finishing service "<<this->GetName());
      CallFinish(*wrappedService_);
    }

    Configuration configuration;

  private:

    boost::shared_ptr<WrappedServiceType> wrappedService_;

  friend class HAWCNest;
  
};

/*!
 * @class ServiceLifetimeControl
 * @author John Pretz
 * @ingroup hawcnest_impl
 * @brief Singleton which returns a list of Service interfaces and Service
 *        instance names
 */
template <class Interface>
class ServiceLifetimeControl {

  public:

    typedef typename std::map<std::string, boost::shared_ptr<Interface> >
      InterfaceMap;

    static InterfaceMap& GetServices() {
      static InterfaceMap services;
      return services;
    }

};

#endif // HAWCNEST_SERVICEIMPL_H_INCLUDED

