/*!
 * @file Configuration.cc 
 * @brief Python bindings to the Module configuration class.
 * @author John Pretz 
 * @date 19 Mar 2010 
 * @version $Id: Configuration.cc 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#include <boost/python.hpp>

#include <hawcnest/Configuration.h>

using namespace boost::python;
using namespace std;

//// Pointer to a version of HAWCNest::Service we want to expose to python:
//// take a registered Service name and instance name and construct a Service
//Configuration (Configuration::*ParameterFcn1)(const string&) =
//  &HAWCNest::Service;
//
//// Pointer to a version of HAWCNest::Service we want to expose to python:
//// take a python object and an instance name and construct a Service
//HAWCNest::configurator (HAWCNest::*Service2)(const object&, const string&) =
//  &HAWCNest::Service;

/// Define boost::python bindings for the Configuration class
void
pybind_hawcnest_Configuration()
{
  class_<Configuration>("Configuration")
    // Setters
    .def("SetParameter", &Configuration::SetParameter<double>)
    .def("SetParameter", &Configuration::SetParameter<int>)
    .def("SetParameter", &Configuration::SetParameter<string>)
    .def("SetParameter", &Configuration::SetParameter<vector<double> >)
    .def("SetParameter", &Configuration::SetParameter<vector<int> >)
    .def("SetParameter", &Configuration::SetParameter<vector<string> >)
    .def("SetParameter", &Configuration::SetParameter<pair<int,int> >)
    .def("SetParameter", &Configuration::SetParameter<pair<double,double> >)
    // Getters
    .def("GetParameter", &Configuration::GetParameter<double>)
    .def("GetParameter", &Configuration::GetParameter<int>)
    .def("GetParameter", &Configuration::GetParameter<string>)
    .def("GetParameter", &Configuration::GetParameter<vector<double> >)
    .def("GetParameter", &Configuration::GetParameter<vector<int> >)
    .def("GetParameter", &Configuration::GetParameter<vector<string> >)
    .def("GetParameter", &Configuration::GetParameter<pair<int,int> >)
    .def("GetParameter", &Configuration::GetParameter<pair<double,double> >)
    .def(self_ns::str(self))
    ;
}

