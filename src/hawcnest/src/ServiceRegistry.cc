/*!
 * @file ServiceRegistry.cc 
 * @brief Implementation of the string/object service registry.
 * @author John Pretz 
 * @date 23 Aug 2009 
 * @version $Id: ServiceRegistry.cc 14881 2013-04-28 14:02:42Z sybenzvi $
 */
#include <hawcnest/impl/ServiceRegistry.h>

ServiceRegistry& GlobalServiceRegistry(){
  static ServiceRegistry registry;
  return registry;
}


