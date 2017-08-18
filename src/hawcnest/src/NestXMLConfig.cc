/*!
 * @file NestXMLConfig.cc 
 * @brief Configuration of the framework using an XML file.
 * @author John Pretz 
 * @date 7 Sep 2010 
 * @version $Id: NestXMLConfig.cc 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#include <hawcnest/NestXMLConfig.h>
#include <hawcnest/xml/XMLReader.h>
#include <hawcnest/Logging.h>
#include <hawcnest/HAWCNest.h>

#include <boost/preprocessor/stringize.hpp>

#include <dlfcn.h>

using namespace std;

void
load_library_xml(const std::string& libname_in)
{
  string libname = libname_in + BOOST_PP_STRINGIZE(SHARED_LIBRARY_SUFFIX); 
  log_info("loading library.... " << libname);
  void* v = dlopen(libname.c_str(), RTLD_NOW | RTLD_GLOBAL);
  if (!v)
    log_fatal("error opening shared library " << libname);
}

void
NestXMLConfig(HAWCNest& nest, const std::string& configFile)
{
  // Parse and validate the XML file
  XMLReader r(configFile);
  XMLBranch topB = r.GetTopBranch();

  // Read and load the list of libraries
  XMLBranch listB = topB.GetChild("librarylist"); 
  if (listB) {
    string libraryName;
    for (XMLBranch lB = listB.GetFirstChild(); lB; lB = lB.GetNextSibling()) {
      lB.GetData(libraryName);
      load_library_xml(libraryName);
    }
  }

  // Read and instantiate the list of services
  XMLBranch servB = topB.GetChild("servicelist");
  if (servB) {
    string serviceName;
    string serviceType;
    for (XMLBranch sB = servB.GetFirstChild(); sB; sB = sB.GetNextSibling()) {
      sB.GetChild("name").GetData(serviceName);
      sB.GetChild("type").GetData(serviceType);

      // Create a service
      nest.Service(serviceType, serviceName);

      // Loop through the list of params
      int nParams = 0;
      for (XMLBranch pB = sB.GetFirstChild(); pB; pB = pB.GetNextSibling()) {
        if (pB.GetName() == "param") {
          string paramName;
          string paramType;
          pB.GetChild("name").GetData(paramName);
          pB.GetChild("type").GetData(paramType);

          if (paramType == "bool") {
            string value;
            pB.GetChild("value").GetData(value);
            if (value == "true")
              nest.SetParameter(serviceName, paramName, true);
            else
              nest.SetParameter(serviceName, paramName, false);
          }
          if (paramType == "string") {
            string value;
            pB.GetChild("value").GetData(value);
            nest.SetParameter(serviceName, paramName, value);
          }
          if (paramType == "int") {
            int value;
            pB.GetChild("value").GetData(value);
            nest.SetParameter(serviceName, paramName, value);
          }
          if (paramType == "float") {
            double value;
            pB.GetChild("value").GetData(value);
            nest.SetParameter(serviceName, paramName, value);
          }
          if (paramType == "vector_string") {
            vector<string> values;
            string val;

            // Loop through vector items.  To be replaced by list elements
            XMLBranch vB = pB.GetChild("value");
            for (XMLBranch i = vB.GetFirstChild(); i; i = i.GetNextSibling()) {
              i.GetData(val);
              values.push_back(val);
            }
            nest.SetParameter(serviceName, paramName, values);
          }
          if (paramType == "vector_int") {
            vector<int> values;
            int val;

            // Loop through vector items.  To be replaced by list elements
            XMLBranch vB = pB.GetChild("value");
            for (XMLBranch i = vB.GetFirstChild(); i; i = i.GetNextSibling()) {
              i.GetData(val);
              values.push_back(val);
            }
            nest.SetParameter(serviceName, paramName, values);
          }
          if (paramType == "vector_float") {
            vector<double> values;
            float val;

            // Loop through vector items.  To be replaced by list elements
            XMLBranch vB = pB.GetChild("value");
            for (XMLBranch i = vB.GetFirstChild(); i; i = i.GetNextSibling()) {
              i.GetData(val);
              values.push_back(val);
            }
            nest.SetParameter(serviceName, paramName, values);
          }

          ++nParams;
        }
      }
      if (nParams == 0)
        log_info("No parameters given for service " << serviceName);
    }
  }
}

