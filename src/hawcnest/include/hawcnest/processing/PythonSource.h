/*!
 * @file PythonSource.h 
 * @brief Expose C++ Source objects to python (and vice-versa).
 * @author Segev BenZvi 
 * @date 28 Dec 2011 
 * @version $Id: PythonSource.h 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#ifndef HAWCNEST_PROCESSING_PYTHONSOURCE_H_INCLUDED
#define HAWCNEST_PROCESSING_PYTHONSOURCE_H_INCLUDED

#include <boost/python.hpp>

#include <hawcnest/processing/Source.h>
#include <hawcnest/Configuration.h>

/*!
 * @class PythonSource
 * @author Segev BenZvi
 * @date 2 Jan 2012
 * @ingroup hawcnest_api
 * @brief Define the interface needed by python classes inheriting from Source
 *
 * This is a base class that inherits the Next function from Source and
 * defines the additional members required by the framework during
 * configuration and event processing: DefaultConfiguration, Initialize, and
 * Finish.  It's role is just to define this interface with some dummy
 * functions.
 */
class PythonSource : public Source {

  public:

    typedef Source Interface;

    virtual ~PythonSource() { }

    virtual Configuration DefaultConfiguration();
    virtual void Initialize(const Configuration& config) { }
    virtual BagPtr Next() { return BagPtr(); }
    virtual void Finish() { }

};

/*!
 * @class PythonSourceCallback
 * @author Segev BenZvi
 * @date 2 Jan 2012
 * @ingroup hawcnest_api
 * @brief A subclass of PythonSource that dispatches function calls to python
 *
 * This class is derived from PythonSource.  It contains a PyObject pointer
 * that corresponds to a C++ instance of PythonSource.  It wraps PythonSource
 * in such a way that subclasses of PythonSourceCallback defined in python can
 * override the base class functions inside C++.  In this way, we can derive
 * Source instances in python which override the Next function and plug
 * directly into the AERIE framework.
 *
 * PythonSourceCallback is exposed to python using the name "Source," so
 * subclassing in python looks something like this:
 *
 * @code
 * from hawc import hawcnest
 *
 * class MySource(hawcnest.Source):
 *     def __init__(self):
 *         hawcnest.Source.__init__(self)
 *         # Override construction here...
 *     def Next(self):
 *         # Override Next here...
 *     def Finish(self):
 *         # Override Finish here...
 * @endcode
 *
 * Note that the boost::python documentation on the subject of callbacks isn't
 * very helpful, but the topic is described in detail in the Python Wiki:
 * http://wiki.python.org/moin/boost.python/OverridableVirtualFunctions
 */
class PythonSourceCallback : public PythonSource {

  public:

    PythonSourceCallback(PyObject* p)
      : PythonSource(), self_(p) { }

    PythonSourceCallback(PyObject* p, const PythonSource& m)
      : PythonSource(m), self_(p) { }

    /// Override DefaultConfiguration to call back into python
    Configuration DefaultConfiguration()
    { return boost::python::call_method<Configuration>(self_, "DefaultConfiguration"); }

    /// The default implementation of DefaultConfiguration
    static Configuration DefaultConfiguration(const PythonSource& m)
    { return const_cast<PythonSource&>(m).PythonSource::DefaultConfiguration(); }

    /// Override Initialize to call back into python
    void Initialize(const Configuration& c)
    { return boost::python::call_method<void>(self_, "Initialize", c); }

    /// The default implementation of Initialize
    static void Initialize(const PythonSource& m, const Configuration& c)
    { return const_cast<PythonSource&>(m).PythonSource::Initialize(c); }

    /// Override Process to call back into python
    BagPtr Next()
    { return boost::python::call_method<BagPtr>(self_, "Next"); }

    /// The default implementation of Process
    static BagPtr Next(const PythonSource& m)
    { return const_cast<PythonSource&>(m).PythonSource::Next(); }

    /// Override Finish to call back into python
    void Finish()
    { return boost::python::call_method<void>(self_, "Finish"); }

    /// The default implementation of Finish
    static void Finish(const PythonSource& m)
    { return const_cast<PythonSource&>(m).PythonSource::Finish(); }

  private:

    /// Pointer to a Source subclass derived inside of python
    PyObject* self_;

};

#endif // HAWCNEST_PROCESSING_PYTHONSOURCE_H_INCLUDED

