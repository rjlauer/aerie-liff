/*!
 * @file SoftwareVersion.cc 
 * @brief Python bindings for the software version class.
 * @author Segev BenZvi 
 * @date 17 Dec 2011 
 * @version $Id: SoftwareVersion.cc 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#include <boost/python.hpp>

#include <hawcnest/SoftwareVersion.h>

using namespace boost::python;

/// Expose SoftwareVersion to boost::python
void
pybind_hawcnest_SoftwareVersion()
{
  class_<SoftwareVersion>
    ("SoftwareVersion",
     "Software version storage and boolean comparisons.",
     init<SoftwareVersion::version_t,
          SoftwareVersion::version_t,
          SoftwareVersion::version_t>())
    .def(init<SoftwareVersion::version_t>())

    .add_property("major", &SoftwareVersion::GetMajorVersion,
                  "Software major version number.")
    .add_property("minor", &SoftwareVersion::GetMinorVersion,
                  "Software minor version number.")
    .add_property("patch", &SoftwareVersion::GetMinorVersion,
                  "Software patch version number.")

    .def(self > self)
    .def(self >= self)
    .def(self < self)
    .def(self <= self)
    .def(self == self)
    .def(self != self)
    .def(self_ns::str(self))
    ;
}

