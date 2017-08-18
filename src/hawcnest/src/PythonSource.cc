/*!
 * @file PythonSource.cc 
 * @brief Implementation of the python AERIE source interface.
 * @author Segev BenZvi 
 * @date 31 Dec 2011 
 * @version $Id: PythonSource.cc 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#include <hawcnest/processing/PythonSource.h>

// Boilerplate base class config function; returns an empty configuration.
Configuration
PythonSource::DefaultConfiguration()
{
  Configuration c;
  return c;
}

