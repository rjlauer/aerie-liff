/*!
 * @file ConfigurationUtil.h
 * @brief Utilities for importing bool and CSV parameters in the framework.
 * @author Jim Braun
 * @date 28 Jun 2010
 * @version $Id: ConfigurationUtil.h 40083 2017-08-07 17:40:11Z pretz $
 */

#ifndef HAWCNEST_CONFIGURATION_UTIL_H_INCLUDED
#define HAWCNEST_CONFIGURATION_UTIL_H_INCLUDED

#include <string>
#include <vector>

namespace ConfigurationUtil {

// Define some default functions to import vector parameters
void ImportCSVParameter(const std::string in,
                              std::vector<double> *v);

void ImportCSVParameter(const std::string in,
                              std::vector<double> *v, unsigned int size);

// Parse bool value from "true" or "false"
bool ImportBool(const std::string in);


class parse_exception : public std::exception{
 public:

  virtual const char* what() const throw() {
    return "parse_exception";
  }
};

 std::string bookend(std::string input);

 std::string stripWhite(std::string input);

 std::vector<std::string> tokenize(std::string,char separator);
 
 template <class T>
   T parse(const std::string& parse_me);
 
 template <>
   int parse<int>(const std::string& parseme);
 
 template <>
   std::string parse<std::string>(const std::string& parseme);
 
 template <>
   double parse<double>(const std::string& parseme);
 
 template <>
   std::string parse<std::string>(const std::string& parseme);
 
 template <>
   std::vector<int> parse<std::vector<int> >(const std::string& parseme);
 
 template <>
   std::vector<double> parse<std::vector<double> >(const std::string& parseme);
 
 template <>
   std::vector<std::string>
   parse<std::vector<std::string> >(const std::string& parseme);
 
 template <>
   bool parse<bool>(const std::string& parseme);



}

#endif // HAWCNEST_CONFIGURATION_UTIL_H_INCLUDED
