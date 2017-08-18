/*!
 * @file MainLoop.cc 
 * @brief Python bindings for the MainLoop base class.
 * @author John Pretz 
 * @date 19 Mar 2010 
 * @version $Id: MainLoop.cc 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#include <boost/python.hpp>

#include <hawcnest/Service.h>
#include <hawcnest/processing/MainLoop.h>

using namespace boost::python;

/// Structure used to expose the MainLoop service to python
struct __mainloop__ {
  __mainloop__(MainLoop& loop) : loop_(loop){}

  void Execute(){loop_.Execute();}

  MainLoop& loop_;
};

/// GetService wrapper for python
__mainloop__
GetService_MainLoop(const std::string& name)
{
  return __mainloop__(GetService<MainLoop>(name));
}

/// Define boost::python bindings for the MainLoop class
void
pybind_hawcnest_MainLoop()
{
  class_<__mainloop__>("MainLoop",init<MainLoop&>())
    .def("Execute", &__mainloop__::Execute);

  def("GetService_MainLoop",GetService_MainLoop);
}

