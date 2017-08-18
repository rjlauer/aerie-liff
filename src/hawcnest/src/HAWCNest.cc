/*!
 * @file HAWCNest.cc 
 * @brief Definition of the main framework.
 * @author John Pretz 
 * @date 16 May 2009 
 * @version $Id: HAWCNest.cc 40056 2017-08-03 16:41:54Z pretz $
 */

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>

#include <hawcnest/HAWCNest.h>
#include <hawcnest/impl/ServiceRegistry.h>
#include <hawcnest/processing/MainLoop.h>
#include <hawcnest/processing/PythonModule.h>
#include <hawcnest/processing/PythonSource.h>

using namespace std;
namespace bp = boost::python;

int HAWCNest::instance_ = 0;

HAWCNest::HAWCNest()
{
  instance_ = instance_ + 1;
  if(instance_ > 1)
    throw hawcnest_exception();
}

HAWCNest::~HAWCNest()
{
  std::vector<boost::function<void ()> >::iterator iter;
  for (iter = atDestruction_.begin(); iter != atDestruction_.end() ; iter++)
    (*iter)();
  instance_ = instance_ -1;
}

void
HAWCNest::Configure()
{
  for (unsigned i = 0 ; i < services_.size() ; i++) {
    {
      try {
        services_[i]->Initialize();
        log_info("Configuring " << services_[i]->GetName());
      }
      // Handle configuration exceptions, then re-throw
      catch (configuration_exception& e) {
        log_error("Configuration exception for service "
                  << services_[i]->GetName());
        throw;
      }
      // Handle runtime exceptions, then re-throw
      catch (std::exception& e) {
        log_error("Standard exception for service "
                  << services_[i]->GetName());
        throw;
      }
      // Handle things not anticipated, then throw runtime_error with log_fatal
      catch (...) {
        log_fatal("Unknown exception for service " << services_[i]->GetName());
      }

      std::ostringstream printedconfig;
      printers_[i](printedconfig);
      log_info("Configured with:\n"<<printedconfig.str());
    }
  }
}

HAWCNest::configurator
HAWCNest::Service(const string& type, const string& name)
{
  return GlobalServiceRegistry().AddService(type, *this, name);
}

HAWCNest::configurator
HAWCNest::ServiceIfExists(const string& type, const string& name)
{
  typedef ServiceRegistry::RegistrationList RegistrationList;
  RegistrationList rlist = 
    GlobalServiceRegistry().GetRegisteredServiceTypes();
  bool serviceTypeExists = false;
  for(RegistrationList::const_iterator iter = rlist.begin() ;
      iter!= rlist.end() ; 
      iter++){
    if(type==iter->first)
      serviceTypeExists = true;
  }
  if(serviceTypeExists)
    return GlobalServiceRegistry().AddService(type, *this, name);
  else
    return HAWCNest::configurator();
}

bool HAWCNest::ServiceExists(const string& name){
  if (configurators_.find(name) != configurators_.end())
    return true;
  return false;
}

HAWCNest::configurator
HAWCNest::Service(const bp::object& obj, const string& name)
{
  typedef boost::shared_ptr<configurator> CfgPtr;
  CfgPtr config;

  // If input is a string, assume it's the name of a registered service
  if (PyString_Check(obj.ptr())) {
    config = CfgPtr(new configurator(Service(bp::extract<string>(obj), name)));
  }
  // Otherwise we have an overloaded Module or Source from python:
  else {
    static bp::object hawcnest = bp::import("hawc.hawcnest");
    static bp::object module = hawcnest.attr("Module");
    static bp::object source = hawcnest.attr("Source");

    // We could be adding a Module... 
    if (PyObject_IsInstance(obj.ptr(), module.ptr())) {
      typedef PythonModuleCallback PyModule;
      typedef ServiceWrapper<PyModule> PyService;
      typedef PyModule::Interface Interface;
      SHARED_POINTER_TYPEDEFS(Interface);
      typedef map<string, InterfacePtr> Services;

      // Make sure that the list of services is reset when HAWCNest is destroyed
      Services& services = ServiceLifetimeControl<Interface>::GetServices();
      atDestruction_.push_back(reset_map<Services>(services));

      // Error out if another instance of this service has been registered
      if (services.find(name) != services.end())
        log_fatal("service with name '" << name
                  << "' already exists when adding service of type '"
                  << name_of<PyModule>() << "'");

      // Create a service instance with the ServiceWrapper using the returned
      // python object
      PyModule pm(obj.ptr());
      boost::shared_ptr<PyService> wrappedServicePtr(new PyService(name, pm));

      // Register the instance name and service with the framework
      services_.push_back(wrappedServicePtr);
      services[name] = wrappedServicePtr->wrappedService_;
      printers_.push_back(printconfig(wrappedServicePtr->configuration));

      // Set up the configuration and return
      config = CfgPtr(new configurator(wrappedServicePtr->configuration));
      configurators_.insert(std::make_pair(name, *config));
    }
    // ...or we could be adding a Source...
    else if (PyObject_IsInstance(obj.ptr(), source.ptr())) {
      typedef PythonSourceCallback PySource;
      typedef ServiceWrapper<PySource> PyService;
      typedef PySource::Interface Interface;
      SHARED_POINTER_TYPEDEFS(Interface);
      typedef map<string, InterfacePtr> Services;

      // Make sure that the list of services is reset when HAWCNest is destroyed
      Services& services = ServiceLifetimeControl<Interface>::GetServices();
      atDestruction_.push_back(reset_map<Services>(services));

      // Error out if another instance of this service has been registered
      if (services.find(name) != services.end())
        log_fatal("service with name '" << name
                  << "' already exists when adding service of type '"
                  << name_of<PySource>() << "'");

      // Create a service instance with the ServiceWrapper using the returned
      // python object
      PySource pm(obj.ptr());
      boost::shared_ptr<PyService> wrappedServicePtr(new PyService(name, pm));

      // Register the instance name and service with the framework
      services_.push_back(wrappedServicePtr);
      services[name] = wrappedServicePtr->wrappedService_;
      printers_.push_back(printconfig(wrappedServicePtr->configuration));

      // Set up the configuration and return
      config = CfgPtr(new configurator(wrappedServicePtr->configuration));
      configurators_.insert(std::make_pair(name, *config));
    }
    // ...or the object is not recognized and we raise an exception.
    else {
      log_fatal("Service '" << name << "' is not a registered Service, " 
                            << "python Source, or python Module");
    }
  }

  return *config;
}

void
HAWCNest::SetParameter(const std::string& name, const std::string& parname,
                       const bp::object& obj)
{
  if (obj.ptr() != Py_None) {
    // Set a string parameter
    if (PyString_Check(obj.ptr())) {
      string value = bp::extract<string>(obj);
      SetParameter(name, parname, value);
    }
    // Set a boolean parameter
    else if (PyBool_Check(obj.ptr())) {
      bool value = bp::extract<bool>(obj);
      SetParameter(name, parname, value);
    }
    // Set an integer parameter
    else if (PyInt_Check(obj.ptr())) {
      int value = bp::extract<int>(obj);
      SetParameter(name, parname, value);
    }
    // Set a floating-point parameter
    else if (PyFloat_Check(obj.ptr())) {
      double value = bp::extract<double>(obj);
      SetParameter(name, parname, value);
    }
    // Set a tuple pair<T> parameter
    else if (PyTuple_Check(obj.ptr())) {
      if (PyTuple_Size(obj.ptr()) == 2) {
        PyObject* item = PyTuple_GetItem(obj.ptr(), 0);
        // T = int
        if (PyInt_Check(item)) {
          bp::stl_input_iterator<int> iter(obj);
          int first = *iter;
          int second = *(++iter);
          SetParameter(name, parname, make_pair(first, second));
        }
        // T = double
        else if (PyFloat_Check(item)) {
          bp::stl_input_iterator<double> iter(obj);
          double first = *iter;
          double second = *(++iter);
          SetParameter(name, parname, make_pair(first, second));
        }
        else
          log_fatal(parname << ": pair params can be int or float");
      }
      else
        log_fatal(parname << " is NOT a pair; too many items!");
    }
    // Set a list (vector<T>) parameter
    else if (PyList_Check(obj.ptr())) {
      if (PyList_Size(obj.ptr()) > 0) {
        PyObject* item = PyList_GetItem(obj.ptr(), 0);
        // T = string
        if (PyString_Check(item)) {
          vector<string> list;
          bp::stl_input_iterator<string> begin(obj);
          bp::stl_input_iterator<string> end;
          list.insert(list.end(), begin, end);
          SetParameter(name, parname, list);
        }
        // T = int
        else if (PyInt_Check(item)) {
          vector<int> list;
          bp::stl_input_iterator<int> begin(obj);
          bp::stl_input_iterator<int> end;
          list.insert(list.end(), begin, end);
          SetParameter(name, parname, list);
        }
        // T = double
        else if (PyFloat_Check(item)) {
          vector<double> list;
          bp::stl_input_iterator<double> begin(obj);
          bp::stl_input_iterator<double> end;
          list.insert(list.end(), begin, end);
          SetParameter(name, parname, list);
        }
        else
          log_fatal(parname << ": vector params can be string, int, or float");
      }
      else
        log_fatal("Passed an empty list to parameter " << parname);
    }
    else
      log_fatal("Type of " << parname << " is unknown");
  }
}

void 
HAWCNest::SetParameter_decode(const std::string& name, 
			      const std::string& parname,
			      const std::string value){
  // find appropriate configurator and call it
  std::map<std::string, configurator>::iterator found = 
    configurators_.find(name);
  if (found == configurators_.end())
    log_fatal("service with name " << name << " not found when setting"
              " parameter " << parname);
  found->second.config_->SetParameter_decode(parname,value);
}


void
HAWCNest::ExecuteMainLoop(const std::string& mainloop)
{
  MainLoop& loop = GetService<MainLoop>(mainloop);
  loop.Execute();
}

