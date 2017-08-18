/*!
 * @file Logging.h
 * @brief A simple logger for the HAWCNest framework.
 * @author John Pretz
 * @date 26 Sep 2008
 * @version $Id: Logging.h 27390 2015-10-26 15:48:24Z sybenzvi $
 */

#ifndef HAWCNEST_LOGGING_H_INCLUDED
#define HAWCNEST_LOGGING_H_INCLUDED

#include <hawcnest/impl/ANSIColorCode.h>

#if BOOST_VERSION > 103301
#include <boost/filesystem.hpp>
#else
#include <boost/filesystem/convenience.hpp>
#endif

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <stdexcept>

/*!
 * @class Logger
 * @author John Pretz
 * @author Segev BenZvi
 * @ingroup hawcnest_api
 * @brief Top-level configuration of logging.  Sets the run-time logging level
 *        and handles output to the console
 * @todo Allow redirection of logs to a file?
 * @todo Create internal stream buffers to reduce overhead in logging macros?
 *
 * This class creates a program-wide instance of an error logger using the
 * Meyers singleton pattern.  It is guaranteed to have thread-safe
 * initialization (in gcc), but is not necessarily thread-safe during
 * execution.
 *
 * It stores an internal LoggingLevel to control output (e.g., for suppressing
 * or allowing debug messages).
 */
class Logger {

  public:

    /// Logging levels for top-level configuration
    enum LoggingLevel {
      TRACE,
      DEBUG,
      INFO,
      WARN,
      ERROR,
      FATAL
    };

    /// Public access point to the logger object
    static Logger& GetInstance() {
      static Logger* const log_ = new Logger;
      return *log_;
    }

    /// Set the minimum log level for printing log messages
    void SetDefaultLogLevel(LoggingLevel level) { defaultLevel_ = level; }

    /// Print messages with the current time (UT)
    void SetTimeStamping(bool doIt=true) { printTime_ = doIt; }

    /// Prevent logging of messages with priority below the default level
    bool DoLogging(LoggingLevel level) const { return level >= defaultLevel_; }

    /// Write log messages to an output stream
    void Write(const LoggingLevel& level,
               const std::string& fileName,
               const std::string& fileExt,
               const std::string& funcName,
               const int lineNumber,
               const std::string& message) const;

    /// Write log messages (syntactic sugar using a stringstream)
    void Write(const LoggingLevel& level,
               const std::string& fileName,
               const std::string& fileExt,
               const std::string& funcName,
               const int lineNumber,
               const std::stringstream& s) const
    {
      Write(level, fileName, fileExt, funcName, lineNumber, s.str());
    }

  private:

    LoggingLevel defaultLevel_;
    bool printTime_;

    // Prevent local construction or copying of the Logger
    Logger() : defaultLevel_(INFO), printTime_(false) { }
    Logger(const Logger&);
   ~Logger() { }

    Logger& operator=(const Logger&);

};

#ifndef NDEBUG

/*!
 * @def log_trace
 * @author John Pretz
 * @author Segev BenZvi
 * @ingroup hawcnest_api
 * @brief Log a message with 'trace' priority
 *
 * This macro logs tracing messages (start, stop, etc.), and can handle
 * user-defined types, C++-style stream extraction, and boost formatting
 * strings.  Example calls are:
 * \code
 * log_trace("Module start");
 * log_trace("Module " << moduleName << " has ended.");
 * log_trace(boost::format("Module has run for %d seconds") % dt);
 * \endcode
 */
#define log_trace(m) {                                                        \
  const Logger& logger = Logger::GetInstance();                               \
  if (logger.DoLogging(Logger::TRACE)) {                                      \
    do {                                                                      \
      std::stringstream sstr;                                                 \
      sstr << m;                                                              \
      logger.Write(Logger::TRACE,                                             \
                   boost::filesystem::basename(__FILE__),                     \
                   boost::filesystem::extension(__FILE__),                    \
                   __FUNCTION__,                                              \
                   __LINE__,                                                  \
                   sstr);                                                     \
    } while (false);                                                          \
  }                                                                           \
}

#else  // NDEBUG

#define log_trace(m)

#endif // NDEBUG

/*!
 * @def log_debug
 * @author John Pretz
 * @author Segev BenZvi
 * @ingroup hawcnest_api
 * @brief Log a message with 'debug' priority
 *
 * This macro logs debug messages, and can handle user-defined types, C++-style
 * stream extraction, and boost formatting strings.  Example calls are:
 * \code
 * log_debug("Allocating 50k floats");
 * log_debug("Area = " << pi*r*r / (cm*cm) << " cm^2");
 * log_debug(boost::format("Time: %02d:%02d:%02d") % hour % minute % second);
 * \endcode
 */
#define log_debug(m) {                                                        \
  const Logger& logger = Logger::GetInstance();                               \
  if (logger.DoLogging(Logger::DEBUG)) {                                      \
    do {                                                                      \
      std::stringstream sstr;                                                 \
      sstr << m;                                                              \
      logger.Write(Logger::DEBUG,                                             \
                   boost::filesystem::basename(__FILE__),                     \
                   boost::filesystem::extension(__FILE__),                    \
                   __FUNCTION__,                                              \
                   __LINE__,                                                  \
                   sstr);                                                     \
    } while (false);                                                          \
  }                                                                           \
}

/*!
 * @def log_info
 * @author John Pretz
 * @author Segev BenZvi
 * @ingroup hawcnest_api
 * @brief Log a message with 'info' priority
 *
 * This macro logs informational messages, and can handle user-defined types,
 * C++-style stream extraction, and boost formatting strings.  Example calls
 * are:
 * \code
 * log_info("Starting run loop");
 * log_info("nHit = " << nFit << ", nFit = " << nFit);
 * log_info(boost::format("CxPE = %.8f") % CxPE);
 * \endcode
 */
#define log_info(m) {                                                         \
  const Logger& logger = Logger::GetInstance();                               \
  if (logger.DoLogging(Logger::INFO)) {                                       \
    do {                                                                      \
      std::stringstream sstr;                                                 \
      sstr << m;                                                              \
      logger.Write(Logger::INFO,                                              \
                   boost::filesystem::basename(__FILE__),                     \
                   boost::filesystem::extension(__FILE__),                    \
                   __FUNCTION__,                                              \
                   __LINE__,                                                  \
                   sstr);                                                     \
    } while (false);                                                          \
  }                                                                           \
}

/*!
 * @def log_warn
 * @author John Pretz
 * @author Segev BenZvi
 * @ingroup hawcnest_api
 * @brief Log a message with 'warn' priority
 *
 * This macro logs warning messages, and can handle user-defined types,
 * C++-style stream extraction, and boost formatting strings.  Example calls
 * are:
 * \code
 * log_warn("This function is deprecated");
 * log_warn((!isBadEvent && nHit < 0 ? "nHit < 0" : ""));
 * log_warn(boost::format("Time difference %s is small") % TimeInterval());
 * \endcode
 */
#define log_warn(m) {                                                         \
  const Logger& logger = Logger::GetInstance();                               \
  if (logger.DoLogging(Logger::WARN)) {                                       \
    do {                                                                      \
      std::stringstream sstr;                                                 \
      sstr << m;                                                              \
      logger.Write(Logger::WARN,                                              \
                   boost::filesystem::basename(__FILE__),                     \
                   boost::filesystem::extension(__FILE__),                    \
                   __FUNCTION__,                                              \
                   __LINE__,                                                  \
                   sstr);                                                     \
    } while (false);                                                          \
  }                                                                           \
}

/*!
 * @def log_error
 * @author John Pretz
 * @author Segev BenZvi
 * @ingroup hawcnest_api
 * @brief Log a message with 'error' priority
 *
 * This macro logs error messages, and can handle user-defined types,
 * C++-style stream extraction, and boost formatting strings.  Example calls
 * are:
 * \code
 * log_error("Division by zero");
 * log_error("Event " << event << " was not reconstructed");
 * log_error(boost::format("Event %s was not reconstructed") % event);
 * \endcode
 */
#define log_error(m) {                                                        \
  const Logger& logger = Logger::GetInstance();                               \
  if (logger.DoLogging(Logger::ERROR)) {                                      \
    do {                                                                      \
      std::stringstream sstr;                                                 \
      sstr << m;                                                              \
      logger.Write(Logger::ERROR,                                             \
                   boost::filesystem::basename(__FILE__),                     \
                   boost::filesystem::extension(__FILE__),                    \
                   __FUNCTION__,                                              \
                   __LINE__,                                                  \
                   sstr);                                                     \
    } while (false);                                                          \
  }                                                                           \
}

/*!
 * @def log_fatal
 * @author John Pretz
 * @author Segev BenZvi
 * @ingroup hawcnest_api
 * @brief Log a message with 'fatal' priority (throws an exception)
 *
 * This macro logs fatal error messages, throwing an exception because the
 * framework has encountered a critical error.  It can handle user-defined 
 * types, C++-style stream extraction, and boost formatting strings.  Example
 * calls * are:
 * \code
 * log_fatal("Out of memory");
 * log_fatal("Could not connect to database " << dbName);
 * log_fatal(boost::format("Survey file %s not found") % filename);
 * \endcode
 */
#define log_fatal(m) {                                                        \
  const Logger& logger = Logger::GetInstance();                               \
  if (logger.DoLogging(Logger::FATAL)) {                                      \
    do {                                                                      \
      std::stringstream sstr;                                                 \
      sstr << m;                                                              \
      logger.Write(Logger::FATAL,                                             \
                   boost::filesystem::basename(__FILE__),                     \
                   boost::filesystem::extension(__FILE__),                    \
                   __FUNCTION__,                                              \
                   __LINE__,                                                  \
                   sstr);                                                     \
      throw std::runtime_error(sstr.str());                                   \
    } while (false);                                                          \
  }                                                                           \
}

/*!
 * @def log_fatal_nothrow
 * @author Segev BenZvi
 * @ingroup hawcnest_api
 * @brief Log a message with 'fatal' priority; terminate program rather than
 *        throwing an exception.
 *
 * This macro logs fatal error messages, but it does not throw an exception.
 * Instead it simply quits execution with a call to exit.  This is for cases
 * when a program needs to be killed due to a fatal error but when the user
 * doesn't want to handle the resulting exception.  It is technically not the
 * best way to handle runtime errors and should be used with caution.
 *
 * The macro can handle user-defined types, C++-style stream extraction, and
 * boost formatting strings.  Example calls are:
 * \code
 * log_fatal_nothrow("Out of memory");
 * log_fatal_nothrow("Could not connect to database " << dbName);
 * log_fatal_nothrow(boost::format("Survey file %s not found") % filename);
 * \endcode
 */
#define log_fatal_nothrow(m) {                                                \
  const Logger& logger = Logger::GetInstance();                               \
  if (logger.DoLogging(Logger::FATAL)) {                                      \
    do {                                                                      \
      std::stringstream sstr;                                                 \
      sstr << m;                                                              \
      logger.Write(Logger::FATAL,                                             \
                   boost::filesystem::basename(__FILE__),                     \
                   boost::filesystem::extension(__FILE__),                    \
                   __FUNCTION__,                                              \
                   __LINE__,                                                  \
                   sstr);                                                     \
      std::exit(EXIT_FAILURE);                                                \
    } while (false);                                                          \
  }                                                                           \
}

#endif // HAWCNEST_LOGGING_H_INCLUDED

