/*!
 * @file XMLErrorHandler.cc 
 * @brief Logging of XML parsing errors.
 * @author Tom Paul
 * @author Segev BenZvi
 * @date 20 Jun 2011 
 * @version $Id: XMLErrorHandler.cc 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#include <hawcnest/xml/XMLErrorHandler.h>
#include <hawcnest/Logging.h>

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>

using namespace std;
using namespace xercesc;

void
XMLErrorHandler::warning(const SAXParseException&)
{
  // Ignore all warnings.
}

//! Collect errors during XML parsing.
/** Errors are gathered for each document, then
    evaluated by the Reader to decide what action
    to take. 
*/
void
XMLErrorHandler::error(const SAXParseException& toCatch)
{
  sawErrors_ = true;
  const StrX systemId(toCatch.getSystemId());
  const StrX message(toCatch.getMessage());
  messages_ << "Error at file \"" << systemId
      << "\", line " << toCatch.getLineNumber()
      << ", column " << toCatch.getColumnNumber()
      << ": " << message << "\n";
}

void
XMLErrorHandler::fatalError(const SAXParseException& toCatch)
{
  sawErrors_ = true;   
  ostringstream msg;
  const StrX systemId(toCatch.getSystemId());
  const StrX message(toCatch.getMessage());
  log_fatal("Fatal Error at file \"" << systemId
            << "\", line " << toCatch.getLineNumber()
            << ", column " << toCatch.getColumnNumber()
            << ": " << message);
}

void
XMLErrorHandler::resetErrors()
{
  sawErrors_ = false;
  messages_.str("");
}

