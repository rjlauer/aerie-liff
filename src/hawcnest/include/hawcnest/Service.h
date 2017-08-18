/*!
 * @file Service.h
 * @author John Pretz
 * @date 25 Aug 2008
 * @brief Interface to registered Service instances.
 * @version $Id: Service.h 18422 2014-01-15 16:00:30Z sybenzvi $
 */

#ifndef HAWCNEST_SERVICE_H_INCLUDED
#define HAWCNEST_SERVICE_H_INCLUDED

#include <hawcnest/impl/ServiceImpl.h>
#include <hawcnest/impl/name_of.h>

#include <boost/version.hpp>

#include <string>
#include <exception>

/*!
 * @class service_exception
 * @author John Pretz
 * @date 25 Aug 2008
 * @ingroup hawcnest_api
 * @brief Service exception, so that service exceptions can be identified
 */
class service_exception : public std::exception {
  public:
    virtual const char* what() const throw() { return "service exception"; }
};

/*!
 * @author John Pretz
 * @ingroup hawcnest_api
 * @date 25 Aug 2008
 * @brief Function used to provide global access to services.  This
 * version returns a reference to the service and throws an exception if
 * the service cannot be found
 */
template <class Interface>
Interface& 
GetService(const std::string& name,
           typename boost::disable_if<is_shared_ptr<Interface> >::type* = 0)
{
  typedef typename ServiceLifetimeControl<Interface>::InterfaceMap InterfaceMap;

  InterfaceMap& services = ServiceLifetimeControl<Interface>::GetServices();
  typename InterfaceMap::iterator found = services.find(name);

  if (found == services.end()) {
    log_error("couldn't find service to satisfy type '" 
               << name_of<Interface>() << "' and named '" << name << "'");
    throw service_exception();
  }
  return *found->second;
}

/*!
 * @author John Pretz
 * @date 25 Aug 2008
 * @ingroup hawcnest_api
 * @brief Function used to provide global access to services.  This
 * version returns a shared_ptr to the service and returns null if 
 * the service cannot be found
 */
template <class Interface>
Interface
GetService(const std::string& name,
           typename boost::enable_if<is_shared_ptr<Interface> >::type* = 0)
{
  typedef typename std::map<std::string, Interface> IMap;

  IMap& services =
#if BOOST_VERSION < 105300
    ServiceLifetimeControl<typename Interface::value_type>::GetServices();
#else
    ServiceLifetimeControl<typename Interface::element_type>::GetServices();
#endif

  typename IMap::iterator found = services.find(name);

  if (found == services.end()) {
    log_info("couldn't find service to satisfy type '" 
#if BOOST_VERSION < 105300
             << name_of<typename Interface::value_type>() << "' and named '"
#else
             << name_of<typename Interface::element_type>() << "' and named '"
#endif
             << name << "'.  Returning NULL Ptr");
    return Interface();
  }
  return found->second;
}

#endif // HAWCNEST_SERVICE_H_INCLUDED

