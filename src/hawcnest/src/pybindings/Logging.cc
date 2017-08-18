/*!
 * @file Logging.cc 
 * @brief Pyhton bindings for the AERIE logger.
 * @author John Pretz 
 * @date 19 Mar 2010 
 * @version $Id: Logging.cc 19158 2014-03-24 18:20:05Z aimran $
 */

#include <boost/python.hpp>

#include <hawcnest/Logging.h>

#include <iostream>

using namespace boost::python;
using namespace std;

void
SetLoggingLevel(int level, bool printTime=false)
{
  Logger& logger = Logger::GetInstance();
  logger.SetTimeStamping(printTime);

  switch (level) {
    case 0: {
      cout << "Set logging level to TRACE" << endl; 
      logger.SetDefaultLogLevel(Logger::TRACE);
      break;
    }
    case 1: {
      cout << "Set logging level to DEBUG" << endl; 
      logger.SetDefaultLogLevel(Logger::DEBUG);
      break;
    }
    case 2: {
      cout << "Set logging level to INFO" << endl; 
      logger.SetDefaultLogLevel(Logger::INFO);
      break;
    }
    case 3: {
      cout << "Set logging level to WARN" << endl; 
      logger.SetDefaultLogLevel(Logger::WARN);
      break;
    }
    case 4: {
      cout << "Set logging level to ERROR" << endl; 
      logger.SetDefaultLogLevel(Logger::ERROR);
      break;
    }
    case 5: {
      cout << "Set logging level to FATAL" << endl;
      logger.SetDefaultLogLevel(Logger::FATAL);
      break;
    }
    default: {
      cout << "Acceptible log levels: [0-5].  Setting level to WARN (3)"
           << endl;
      logger.SetDefaultLogLevel(Logger::WARN);
      break;
    }
  }
}

/// Define boost::python bindings for the error logger
void
pybind_hawcnest_Logging()
{
  def("SetLoggingLevel", SetLoggingLevel,
      "Set log level and print time option (int loglevel, bool printtime)");
}

