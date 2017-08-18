/*!
 * @file RegisterService.h
 * @brief HAWCNest Service registration interface.
 * @author John Pretz
 * @date 22 Mar 2010
 * @version $Id: RegisterService.h 14858 2013-04-27 14:12:23Z sybenzvi $
 */

#ifndef HAWCNEST_REGISTER_SERVICE_H_INCLUDED
#define HAWCNEST_REGISTER_SERVICE_H_INCLUDED

#include <hawcnest/impl/ServiceRegistry.h>

/*!
 * @def REGISTER_SERVICE
 * @author John Pretz
 * @date 22 Mar 2010
 * @ingroup hawcnest_api
 * @brief registers a service, so that the non-typed 'AddService' of
 *        HAWCNest can be used.  Creates the needed mapping between string and
 *        type 
 */
#define REGISTER_SERVICE(ServiceType) \
  namespace { DoServiceRegistration<ServiceType> registration ##ServiceType; }

#endif // HAWCNEST_REGISTER_SERVICE_H_INCLUDED

