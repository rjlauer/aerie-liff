/*!
 * @file NestXMLConfig.cc 
 * @brief Configuration of the framework using an XML file.
 * @author John Pretz 
 * @date 7 Sep 2010 
 * @version $Id: NestXMLConfig.cc 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#include <hawcnest/NestIniConfig.h>
#include <hawcnest/Logging.h>
#include <hawcnest/HAWCNest.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

using namespace std;
using boost::property_tree::ptree; 

void
NestIniConfig(HAWCNest& nest, std::istream& configFile){
 boost::property_tree::ptree pt;
 boost::property_tree::ini_parser::read_ini(configFile, pt);

 for (ptree::iterator services = pt.begin(); services != pt.end(); services++){
   string service_name = services->first;
   string service_type;
   boost::property_tree::ptree& parameterList = services->second;
   map<string,string> serviceParameters;
   for (ptree::iterator param = parameterList.begin(); 
 	param != parameterList.end(); 
 	param++){
     if(param->first == "service_type"){
       service_type = param->second.data();
     }
     else{
       serviceParameters[param->first] = param->second.data();
     }
   }
   nest.Service(service_type,service_name);
   for(map<string,string>::iterator iter = serviceParameters.begin() ; 
       iter != serviceParameters.end() ; 
       iter++){
     nest.SetParameter_decode(service_name,iter->first,iter->second);
     
   }

 }
 // for (ptree::iterator services = pt.begin(); services != pt.end(); services++){
 //   map<string,string> serviceParameters;
 //   string type;
 //   string name;
 //   for (ptree::iterator services = pt.begin(); 
 // 	services != pt.end(); 
 // 	services++){
 //     name = services->first;
 //     ptree& parameters = services->second;
 //     for(ptree::iterator param = parameters.begin() ; 
 // 	 param != parameters.end() ;
 // 	 param++){
 //       if(param->first == "service_type")
 // 	 type = param->second.data();
 //       else{
 // 	 serviceParameters[param->first] = param->second.data();
 //       }
 //     }
 //   }
 //   if(type == "")
 //     log_fatal("ini file configuration failed. "
 // 	       "each service needs a 'service_type'");
 //   if(name == "")
 //     log_fatal("ini file configuration failed. "
 // 	       "each service needs a name");
 //   //   nest.Service(type,name);
 //   for(map<string,string>::iterator iter = serviceParameters.begin() ;
 //       iter != serviceParameters.end() ; 
 //       iter++){
 //     //     nest.SetParameter_decode(name,iter->first,iter->second);
 //   }
 // }
}



void NestIniConfig(HAWCNest& nest,std::string configFile){
  ifstream fin(configFile.c_str());
  if(fin.good())
    NestIniConfig(nest,fin);
  else
    log_fatal("couldn't open file "<<configFile);
}
