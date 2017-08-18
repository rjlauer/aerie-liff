/*!
 * @file NestXMLConfig.h
 * @author John Pretz
 * @brief Configuration of the framework from an XML file.
 * @date 7 Sep 2010
 * @version $Id: NestXMLConfig.h 14858 2013-04-27 14:12:23Z sybenzvi $
 */

#ifndef HAWCNEST_NESTXMLCONFIG_H_INCLUDED
#define HAWCNEST_NESTXMLCONFIG_H_INCLUDED

#include <string>

class HAWCNest;

/*!
 * @fn NestXMLConfig(HAWCNest& nest,const std::string& configfile)
 * @author John Pretz
 * @date 7 Sep 2010
 * @ingroup hawcnest_api
 * @brief A function which configures a HAWCNest instance from a 
 * specified config file
 */
void NestXMLConfig(HAWCNest& nest,const std::string& configfile);

#endif
