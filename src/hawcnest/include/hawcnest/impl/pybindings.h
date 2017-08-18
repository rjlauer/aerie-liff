/*!
 * @file pybindings.h
 * @author Troy Straszheim
 * @author Erik Blaufuss
 * @author Alex Olivas
 * @author Segev BenZvi
 * @date 24 Jul 2009
 * @ingroup hawcnest_impl
 * @brief Some handy utilities for exposing classes to python.  Adapted with 
 *        few changes from the IceTray build system.
 * @version $Id: pybindings.h 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#ifndef HAWCNEST_PYBINDINGS_H_INCLUDED
#define HAWCNEST_PYBINDINGS_H_INCLUDED

#include <hawcnest/processing/Bag.h>

#include <boost/python.hpp>
#include <boost/python/implicit.hpp>
#include <boost/shared_ptr.hpp>

/*!
 * @function register_pointer_conversions
 * @brief Tell python about implicit conversions to const and Baggable pointers
 * @tparam A class which inherits from Baggable
 */
template<typename T>
void
register_pointer_conversions()
{
  using boost::python::implicitly_convertible;
  using boost::shared_ptr;

  implicitly_convertible<shared_ptr<T>, shared_ptr<Baggable> >();
  implicitly_convertible<shared_ptr<T>, shared_ptr<const T> >();
  implicitly_convertible<shared_ptr<T>, shared_ptr<const Baggable> >();
}

#endif // HAWCNEST_PYBINDINGS_H_INCLUDED

