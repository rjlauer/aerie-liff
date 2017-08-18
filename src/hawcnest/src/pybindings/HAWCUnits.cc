/*!
 * @file HAWCUnits.cc 
 * @brief Python bindings for HAWCUnits.
 * @author Segev BenZvi 
 * @date 19 Jun 2011 
 * @version $Id: HAWCUnits.cc 29992 2016-02-22 18:03:56Z sybenzvi $
 */

#include <boost/python.hpp>
#include <boost/preprocessor.hpp>

#include <hawcnest/HAWCUnits.h>

using namespace boost::python;

/// Structure used to expose HAWC units to python
struct __hawcunits__ {

  #define X(name, value) \
  static double BOOST_PP_CAT(hawcunits_return_,name)() \
  { return HAWCUnits::name; }

  #include <hawcnest/impl/unit_defs.h>
  #undef X

};

double
getUnit(const std::string& name)
{
  const HAWCUnits::Evaluator& e = HAWCUnits::Evaluator::GetInstance();
  return e.GetUnit(name);
}

/// Define boost::python bindings for the HAWCUnits 
void
pybind_hawcnest_HAWCUnits()
{
  // The following definition ensures that the units are read-only.
  // The units will be available using the usual import statements, e.g.,
  //
  // >>> from libhawcnest import HAWCUnits as U
  // >>> c = 0.2999 * U.meter / U.ns

  class_<__hawcunits__, boost::noncopyable>
    ("HAWCUnits", "'Namespace' holding values for HawcUnits", no_init)
    #define X(name, value) \
    .add_static_property(#name, &__hawcunits__::BOOST_PP_CAT(hawcunits_return_,name))
    #include <hawcnest/impl/unit_defs.h>
    #undef X

    ;

  def("getUnit", getUnit, arg("name"));
}

