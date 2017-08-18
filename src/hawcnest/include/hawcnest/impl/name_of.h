/*!
 * @file name_of.h 
 * @brief Obtain the string name of a class or object.
 * @author John Pretz 
 * @date 25 Aug 2008 
 * @version $Id: name_of.h 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#ifndef HAWCNEST_IMPL_NAME_OF_H_INCLUDED
#define HAWCNEST_IMPL_NAME_OF_H_INCLUDED

#include <boost/preprocessor/stringize.hpp>

#include <cxxabi.h>

#include <typeinfo>
#include <cstdlib>
#include <string>

/*!
 * @function name_of
 * @author John Pretz
 * @date 25 Aug 2008
 * @ingroup hawcnest_impl
 * @tparam T Class type that we want to stringify
 * @brief Take a typename and return a (hopefully) identical string
 *
 * Note: name_of<string>() returns "std::string."
 */
template<typename T>
inline static
std::string name_of()
{
  int status;
  char* realname;
  std::string to_return;
  const static char* const notavailable = "N/A";
  const std::type_info &ti = typeid(T);
  const char* mangled = ti.name();
  if (!mangled)
    to_return = notavailable;
  else {
    realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
    if (realname) {
      to_return = realname;
      free(realname);
    }
    else
      to_return = notavailable;
  }
  return to_return;
}

/*!
 * @function name_of
 * @author John Pretz
 * @date 25 Aug 2008
 * @ingroup hawcnest_impl
 * @param ti Type info of a type that we want to stringify
 * @brief Take a typename and return a (hopefully) identical string
 */
inline static
std::string
name_of(const std::type_info &ti)
{
  int status;
  char* realname;
  std::string to_return;
  
  realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
  to_return = realname;
  free(realname);
  return to_return;
}

#endif // HAWCNEST_IMPL_NAME_OF_H_INCLUDED

