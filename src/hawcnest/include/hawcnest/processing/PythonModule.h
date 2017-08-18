/*!
 * @file PythonModule.h 
 * @brief Expose C++ Module calls to python (and vice-versa).
 * @author Segev BenZvi 
 * @date 28 Dec 2011 
 * @version $Id: PythonModule.h 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#ifndef HAWCNEST_PROCESSING_PYTHONMODULE_H_INCLUDED
#define HAWCNEST_PROCESSING_PYTHONMODULE_H_INCLUDED

#include <boost/python.hpp>

#include <hawcnest/processing/Module.h>
#include <hawcnest/Configuration.h>


/*!
 * @class PythonModule
 * @author Segev BenZvi
 * @date 30 Dec 2011
 * @ingroup hawcnest_api
 * @brief Define the interface needed by python classes inheriting from Module
 *
 * This is a base class that inherits the Process function from Module and
 * defines the additional members required by the framework during
 * configuration and event processing: DefaultConfiguration, Initialize, and
 * Finish.  It's role is just to define this interface with some dummy
 * functions.
 */
class PythonModule : public Module {

  public:

    typedef Module Interface;

    virtual ~PythonModule() { }

    virtual Configuration DefaultConfiguration();
    virtual void Initialize(const Configuration& config) { }
    virtual Module::Result Process(BagPtr bag) { return Module::Continue; }
    virtual void Finish() { }

};

/*!
 * @class PythonModuleCallback
 * @author Segev BenZvi
 * @date 30 Dec 2011
 * @ingroup hawcnest_api
 * @brief A subclass of PythonModule that dispatches function calls to python
 *
 * This class is derived from PythonModule.  It contains a PyObject pointer
 * that corresponds to a C++ instance of PythonModule.  It wraps PythonModule
 * in such a way that subclasses of PythonModuleCallback defined in python can
 * override the base class functions inside C++.  In this way, we can derive
 * Module instances in python which override the Process function and plug
 * directly into the AERIE framework.
 *
 * PythonModuleCallback is exposed to python using the name "Module," so
 * subclassing in python looks something like this:
 *
 * @code
 * from hawc import hawcnest
 *
 * class MyModule(hawcnest.Module):
 *     def __init__(self):
 *         hawcnest.Module.__init__(self)
 *         # Override construction here...
 *     def Process(self, bagptr):
 *         # Override Process here...
 *     def Finish(self):
 *         # Override Finish here...
 * @endcode
 *
 * Note that the boost::python documentation on the subject of callbacks isn't
 * very helpful, but the topic is described in detail in the Python Wiki:
 * http://wiki.python.org/moin/boost.python/OverridableVirtualFunctions
 */
class PythonModuleCallback : public PythonModule {

  public:

    PythonModuleCallback(PyObject* p)
      : PythonModule(), self_(p) { }

    PythonModuleCallback(PyObject* p, const PythonModule& m)
      : PythonModule(m), self_(p) { }

    /// Override DefaultConfiguration to call back into python
    Configuration DefaultConfiguration()
    { return boost::python::call_method<Configuration>(self_, "DefaultConfiguration"); }

    /// The default implementation of DefaultConfiguration
    static Configuration DefaultConfiguration(const PythonModule& m)
    { return const_cast<PythonModule&>(m).PythonModule::DefaultConfiguration(); }

    /// Override Initialize to call back into python
    void Initialize(const Configuration& c)
    { return boost::python::call_method<void>(self_, "Initialize", c); }

    /// The default implementation of Initialize
    static void Initialize(const PythonModule& m, const Configuration& c)
    { return const_cast<PythonModule&>(m).PythonModule::Initialize(c); }

    /// Override Process to call back into python
    Module::Result Process(BagPtr bp)
    { return boost::python::call_method<Module::Result>(self_, "Process", bp); }

    /// The default implementation of Process
    static Module::Result Process(const PythonModule& m, BagPtr bp)
    { return const_cast<PythonModule&>(m).PythonModule::Process(bp); }

    /// Override Finish to call back into python
    void Finish()
    { return boost::python::call_method<void>(self_, "Finish"); }

    /// The default implementation of Finish
    static void Finish(const PythonModule& m)
    { return const_cast<PythonModule&>(m).PythonModule::Finish(); }

  private:

    /// Pointer to a Module subclass derived inside of python
    PyObject* self_;

};

#endif // HAWCNEST_PROCESSING_PYTHONMODULE_H_INCLUDED

