/*!
 * @file PythonModule.cc 
 * @brief Implementation of the python AERIE module interface.
 * @author Segev BenZvi 
 * @date 31 Dec 2011 
 * @version $Id: PythonModule.cc 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#include <hawcnest/processing/PythonModule.h>

// Boilerplate base class config function; returns an empty configuration.
Configuration
PythonModule::DefaultConfiguration()
{
  Configuration c;
  return c;
}

