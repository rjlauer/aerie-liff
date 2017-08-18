/*!
 * @file Logging.cc 
 * @brief Implementation of color-coded logging.
 * @author Segev BenZvi 
 * @date 30 Jan 2012 
 * @version $Id: Logging.cc 14897 2013-04-29 17:04:50Z sybenzvi $
 */

#include <hawcnest/Logging.h>

#include <ctime>
#include <iomanip>

using namespace std;

namespace {

  const ANSIColorCode blueBold("\033[1;34m");
  const ANSIColorCode blueNorm("\033[0;34m");

  const ANSIColorCode cyanBold("\033[1;36m");
  const ANSIColorCode cyanNorm("\033[0;36m");

  const ANSIColorCode greenBold("\033[1;32m");
  const ANSIColorCode greenNorm("\033[0;32m");

  const ANSIColorCode magentaBold("\033[1;35m");
  const ANSIColorCode magentaNorm("\033[0;35m");

  const ANSIColorCode redBold("\033[1;31m");
  const ANSIColorCode redNorm("\033[0;31m");

  const ANSIColorCode noColor("\033[0m");

}

void
Logger::Write(const Logger::LoggingLevel& level,
              const std::string& fileName,
              const std::string& fileExt,
              const std::string& funcName,
              const int lineNumber,
              const std::string& message)
  const
{
  stringstream timeStr;
  if (printTime_) {
    time_t currentTime = time(0);
    tm* currentTimePtr = gmtime(&currentTime);
    timeStr << setfill('0') 
            << '('
            << currentTimePtr->tm_year + 1900 << '-'
            << setw(2) << currentTimePtr->tm_mon + 1 << '-'
            << setw(2) << currentTimePtr->tm_mday << ' '
            << setw(2) << currentTimePtr->tm_hour << ':'
            << setw(2) << currentTimePtr->tm_min << ':'
            << setw(2) << currentTimePtr->tm_sec << ") "
            << setfill(' ');
  }

  switch (level) {
    case (Logger::TRACE): {
      cerr << blueBold << "TRACE " << timeStr.str()
           << blueNorm << "["  << fileName << fileExt
                      << ", " << funcName << ":" << lineNumber << "]: "
           << noColor << message
           << endl;
      break;
    }
    case (Logger::DEBUG): {
      cerr << cyanBold << "DEBUG " << timeStr.str()
           << cyanNorm << "["  << fileName << fileExt
                      << ", " << funcName << ":" << lineNumber << "]: "
           << noColor << message
           << endl;
      break;
    }
    case (Logger::INFO): {
      cerr << greenBold << "INFO " << timeStr.str()
           << greenNorm << "["  << fileName << fileExt
                      << ", " << funcName << ":" << lineNumber << "]: "
           << noColor << message
           << endl;
      break;
    }
    case (Logger::WARN): {
      cerr << magentaBold << "WARNING " << timeStr.str()
           << magentaNorm << "["  << fileName << fileExt
                      << ", " << funcName << ":" << lineNumber << "]: "
           << noColor << message
           << endl;
      break;
    }
    case (Logger::ERROR): {
      cerr << redBold << "ERROR " << timeStr.str()
           << redNorm << "["  << fileName << fileExt
                      << ", " << funcName << ":" << lineNumber << "]: "
           << noColor << message
           << endl;
      break;
    }
    case (Logger::FATAL): {
      cerr << redBold << "FATAL ERROR " << timeStr.str()
           << redNorm << "["  << fileName << fileExt
                      << ", " << funcName << ":" << lineNumber << "]: "
           << noColor << message
           << endl;
      break;
    }
    default:
      break;
  }
}

