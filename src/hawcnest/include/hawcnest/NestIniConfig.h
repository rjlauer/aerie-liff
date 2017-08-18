/*!
 * @file NestIniConfig.h
 * @author John Pretz
 * @brief Configuration of the framework from an Ini file.
 * @date 7 Sep 2010
 * @version $Id: NestIniConfig.h 14858 2013-04-27 14:12:23Z sybenzvi $
 */

#ifndef HAWCNEST_NESTINICONFIG_H_INCLUDED
#define HAWCNEST_NESTINICONFIG_H_INCLUDED

#include <string>
#include <iostream>

class HAWCNest;

/*!
 * @fn NestIniConfig(HAWCNest& nest,const std::string& configfile)
 * @author John Pretz
 * @date 7 Sep 2010
 * @ingroup hawcnest_api
 * @brief A function which configures a HAWCNest instance from a 
 * specified config file
 */
void NestIniConfig(HAWCNest& nest,std::istream& configFile);

void NestIniConfig(HAWCNest& nest,std::string configFile);

#endif
