"""Python front-end to the HAWCNest class used to configure the AERIE
framework.

.. moduleauthor:: John Pretz
.. moduleauthor:: Segev BenZvi
"""

import warnings
import inspect

import hawc.hawcnest as hawcnest

HAWCUnits = hawcnest.HAWCUnits

def load(library):
    """
    Load a library for use by the framework.  This is only for libraries that
    do not have python bindings.  Eventually all services and modules should
    have bindings so this function is deprecated.

    :param library:
      A string naming the library to be loaded, without a prefix or a suffix.
      E.g., for libPhysics.so or libPhysics.dylib, use the call load(Physics).
    """
    warnings.warn(
        "load() is for libraries without python bindings, and is deprecated.",
        DeprecationWarning)
    hawcnest.load(library)

class PythonFunction(hawcnest.Module):
    """A Module subclass container of python functions."""

    def __init__(self, func):
        """Wrap a function inside a Module so it can be used to initialize a
        Service.  The function implements the Process method of the Service.

        :param func:
          A function which returns a boolean value.
        """
        hawcnest.Module.__init__(self)
        self.process_ = func

    def Process(self, bag):
        """If function evaluates to True, keep processing the Bag.  Else, drop
        the current Bag out of the processing loop."""
        if self.process_(bag):
            return hawcnest.Module.CONTINUE
        return hawcnest.Module.FILTER

class HAWCNest:
    def __init__(self):
        """A wrapper around the C++ class hawcnest.HAWCNest.  This class
        provides access to the framework and allows users to initialize
        services with key-value parameter pairs.
        """
        self.hawcnest_ = hawcnest.HAWCNest()
        self.pyServices_ = []

    def Service(self, servicetype, servicename, **kwargs):
        """Add a service to the HAWCNest framework.

        :param servicetype:
          * A string with the name of a registered service
          * A python class, subclassed from hawcnest.Source or hawcnest.Module
          * A python object, instance of a Source or Module subclass
          * A python function which returns True or False

        :param servicename:
          A name for the instance of the service.

        :param kwargs:
          Pass parameters to the service using a list of keyword arguments.
        """
        # Initialize python service; append to service list to keep instances
        # in scope during program lifetime
        if inspect.isclass(servicetype):
            serviceInstance = servicetype(**kwargs)
            self.hawcnest_.Service(serviceInstance, servicename)
            self.pyServices_.append(serviceInstance)
        # Initialize python function as a Module
        elif inspect.isfunction(servicetype):
            serviceInstance = PythonFunction(servicetype)
            self.hawcnest_.Service(serviceInstance, servicename)
            self.pyServices_.append(serviceInstance)
        # Initialize a registered Source/Module by string name
        else:
            self.hawcnest_.Service(servicetype, servicename)
            for k,v in kwargs.items():
                self.hawcnest_.SetParameter(servicename, k, v)
        return self

    def SetParameter(self, servicename, name, value):
        """Pass configuration parameters to the HAWCNest framework.

        :param servicename:
          A name for the instance of a service.

        :param name:
          The name of the configuration parameter.

        :param value:
          The value of the configuration parameter.  Acceptable types are int,
          float, string... and lists of these.  The functions for each type are
          overloaded by boost::python.
        """
        self.hawcnest_.SetParameter(servicename, name, value)
        return self

    def Configure(self):
        """Configure services added to the framework processing stream.
        """
        self.hawcnest_.Configure()

    def ExecuteMainLoop(self,mainloop):
        """Run the framework processing stream.
        """
        self.hawcnest_.ExecuteMainLoop(mainloop);

    def Finish(self):
        """End event processing and clean up services.
        """
        self.hawcnest_.Finish()

# Function to get the MainLoop of processing modules
GetService_MainLoop = hawcnest.GetService_MainLoop

# Function to set the logging level of the framework
SetLoggingLevel = hawcnest.SetLoggingLevel

