/*!
 * @file HAWCNest.cc 
 * @brief Python bindings to the HAWCNest framework class.
 * @author John Pretz 
 * @date 19 Mar 2010 
 * @version $Id: HAWCNest.cc 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <hawcnest/HAWCNest.h>

using namespace boost::python;
using namespace std;

// Pointer to a version of HAWCNest::Service we want to expose to python:
// take a registered Service name and instance name and construct a Service
HAWCNest::configurator (HAWCNest::*Service1)(const string&, const string&) =
  &HAWCNest::Service;

// Pointer to a version of HAWCNest::Service we want to expose to python:
// take a python object and an instance name and construct a Service
HAWCNest::configurator (HAWCNest::*Service2)(const object&, const string&) =
  &HAWCNest::Service;

// Python tuple/pair conversion
template<typename M, typename N>
struct PairToTupleConverter {
  static PyObject* convert(std::pair<M,N> const& pair)
  { return incref(make_tuple(pair.first, pair.second).ptr()); }

  static PyTypeObject const* get_pytype() 
  { return &PyTuple_Type; }
};

/// Define boost::python bindings for the HAWCNest class
void
pybind_hawcnest_HAWCNest()
{
  class_<vector<int> >("vector_int")
    .def(vector_indexing_suite<vector<int> >());

  class_<vector<double> >("vector_double")
    .def(vector_indexing_suite<vector<double> >());

  class_<vector<string> >("vector_string")
    .def(vector_indexing_suite<vector<string> >());

  to_python_converter<std::pair<int,int>,
                      PairToTupleConverter<int,int> >();

  to_python_converter<std::pair<double,double>,
                      PairToTupleConverter<double,double> >();

  void (HAWCNest::*SetParam)(const string&, const string&, const object&)
    = &HAWCNest::SetParameter;

  scope hawcnest_class(
    class_<HAWCNest>("HAWCNest")
      .def("SetParameter", SetParam)
      .def("Service", Service2)
      .def("Configure", &HAWCNest::Configure)
      .def("ExecuteMainLoop", &HAWCNest::ExecuteMainLoop)
      .def("Finish", &HAWCNest::Finish)
      .def(self_ns::str(self))
  );

  // Nested configurator class inside the scope of HAWCNest
  class_<HAWCNest::configurator>
    ("configurator",
     "A class used to configure modules with a nice interface.",
     init<Configuration&>())
    ;
}

