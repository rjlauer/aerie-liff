/*!
 * @file conversion_allowed.h 
 * @brief Template specializations for module parameter conversions.
 * @author John Pretz 
 * @date 20 Apr 2010 
 * @version $Id: conversion_allowed.h 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#ifndef HAWCNEST_CONVERSION_ALLOWED_H_INCLUDED
#define HAWCNEST_CONVERSION_ALLOWED_H_INCLUDED

#include <string>

/*!
 * @class conversion_allowed
 * @author John Pretz
 * @ingroup hawcnest_impl
 * @brief This structure and it's specializations define what is allowed to 
 * be converted to what when adapting parameters.
 */
template <class From, class To>
  struct conversion_allowed {
    static const bool value = false;
  };

/*! @ingroup hawcnest_impl */
template <class T>
struct conversion_allowed<T, T> {
  static const bool value = true;
};

/*! @ingroup hawcnest_impl */
template <class T>
struct conversion_allowed<const T, T> {
  static const bool value = true;
};

/*! @ingroup hawcnest_impl */
template <>
struct conversion_allowed<const char*, std::string> {
  static const bool value = true;
};

/*! @ingroup hawcnest_impl */
template <>
struct conversion_allowed<char*, std::string> {
  static const bool value = true;
};

/*! @ingroup hawcnest_impl */
template <>
struct conversion_allowed<int,double> {
  static const bool value = true;
};

#endif // HAWCNEST_CONVERSION_ALLOWED_H_INCLUDED

