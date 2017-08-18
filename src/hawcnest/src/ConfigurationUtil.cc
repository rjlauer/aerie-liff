/*!
 * @file ConfigurationUtil.cc 
 * @brief Utilities for checking Module configurations.
 * @author Jim Braun 
 * @date 28 Jun 2010 
 * @version $Id: ConfigurationUtil.cc 40083 2017-08-07 17:40:11Z pretz $
 */

#include <hawcnest/ConfigurationUtil.h>
#include <hawcnest/Logging.h>

#include <errno.h>

#include <cmath>
#include <cstdlib>
#include <cstring>

#include <boost/lexical_cast.hpp>

using namespace std;

void
ConfigurationUtil::ImportCSVParameter(const string in, vector<double>* v)
{
  double val;
  errno = 0;
  char cstr[in.size()+1];
  strcpy(cstr, in.c_str());
  char *current = strtok(cstr, ",");
  for (; current != 0; current = strtok(0, ",")) {
    val = strtod(current, (char**)0);
    if (errno || std::isnan(val) || std::isinf(val))
      log_fatal("Error parsing double: " << val << " from input string " << in);
    v->push_back(val);
  }
}

void
ConfigurationUtil::ImportCSVParameter(const string in,
                                      vector<double>* v, unsigned int size)
{
  ImportCSVParameter(in, v);
  if (v->size() != size)
    log_fatal("Expected " << size << " parameters in input string " << in
                          << ", only found " << v->size());
}

bool
ConfigurationUtil::ImportBool(const string in) 
{
  if (!in.compare("true"))
    return true;
  else if (!in.compare("false"))
    return false;
  log_fatal("String " << in << " is not a boolean \"true\" or \"false\"");

  return false; ///< Will never be reached
}

vector<string> ConfigurationUtil::tokenize(string s,char separator){
  vector<string> returned;
  string::iterator start = s.begin();
  string::iterator here = s.begin();
  while(here != s.end()){
    if(*here == separator){
      string tokenized = string(start,here);
      if(tokenized == "")
	throw parse_exception();
      returned.push_back(tokenized);
      start = here+1;
      here = here+1;
    }
    else{
      here++;
    }
  }
  string trailing(start,here);
  if(trailing == "")
    return returned;
  else
    returned.push_back(trailing);
 
  return returned;
}


std::string ConfigurationUtil::bookend(std::string input){
  string::iterator start = input.begin();
  string::iterator end = input.end();
  while(start != input.end()){
    if(!isspace(*start)){
      if(*start == '['){
        break;
      }
    }
    start++;
  }
  if(start == input.end())
    throw ConfigurationUtil::parse_exception();
  while(end != input.begin()){
    if(!isspace(*end)){
      if(*end == ']'){
        break;
      }
    }
    end--;
  }

  return string(start+1,end);
}

std::string ConfigurationUtil::stripWhite(std::string input){
  string::iterator start = input.begin();
  string::iterator end = input.end();

  while(start != input.end()){
    if(!isspace(*start)){
        break;
    }
    start++;
  }
  if(start == input.end()){
    return "";
  }

  end--; // end() is _past_ the end
  while(end != input.begin()){
    if(!isspace(*end)){
      break;
    }
    end--;
  }
  return string(start,end+1);
}

template <>
string ConfigurationUtil::parse<string>(const std::string& parseme){
  return stripWhite(parseme);
}

template <>
int ConfigurationUtil::parse<int>(const std::string& parseme){
  try{
    return boost::lexical_cast<int>(stripWhite(parseme));
  }
  catch(...){
    throw parse_exception();
  }
}

template <>
double ConfigurationUtil::parse<double>(const std::string& parseme){
  try{
    return boost::lexical_cast<double>(stripWhite(parseme));
  }
  catch(...){
    throw parse_exception();
  }
}

template <>
bool ConfigurationUtil::parse<bool>(const std::string& parseme){
  try{
    return ImportBool(stripWhite(parseme));
  }
  catch(...){
    throw parse_exception();
  }
}

template <>
vector<int> ConfigurationUtil::parse<vector<int> >(const std::string& parseme){
  std::vector<int> ints;
  vector<string> tokens = tokenize(bookend(parseme),',');
  std::transform(tokens.begin(), tokens.end(), back_inserter(ints), parse<int>);
  return ints;
}

template <>
vector<string> 
ConfigurationUtil::parse<vector<string> >(const std::string& parseme){
  std::vector<string> strings;
  vector<string> tokens = tokenize(bookend(parseme),',');
  std::transform(tokens.begin(), tokens.end(), 
		 back_inserter(strings), parse<string>);
  return strings;
}

template <>
vector<double> 
ConfigurationUtil::parse<vector<double> >(const std::string& parseme){
  std::vector<double> doubles;
  vector<string> tokens = tokenize(bookend(parseme),',');
  std::transform(tokens.begin(), tokens.end(), 
		 back_inserter(doubles), parse<double>);
  return doubles;
}
