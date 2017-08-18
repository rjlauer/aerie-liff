/*!
 * @file XMLErrorHandler.h 
 * @brief XML processing error reporter.
 * @author T. Paul
 * @date 20 Jun 2011 
 * @version $Id: XMLErrorHandler.h 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#ifndef HAWCNEST_XML_XMLERRORHANDLER_H_INCLUDED
#define HAWCNEST_XML_XMLERRORHANDLER_H_INCLUDED

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include <hawcnest/PointerTypedefs.h>

#include <iostream>
#include <sstream>

/*!
 * @class XMLErrorHandler
 * @author T. Paul
 * @ingroup hawcnest_api
 * @brief Reports errors encountered during XML parsing
 * 
 * This class was copied from the ReaderErrorReporter class in the Auger XML
 * Reader module.
 */
class XMLErrorHandler : public xercesc::ErrorHandler {

  public:

    XMLErrorHandler() : sawErrors_(false) { }

    void warning(const xercesc::SAXParseException& toCatch);
    void error(const xercesc::SAXParseException& toCatch);
    void fatalError(const xercesc::SAXParseException& toCatch);
    void resetErrors();

    bool getSawErrors() const { return sawErrors_; }
    const std::ostringstream& getMessages() const { return messages_; }

    bool sawErrors_;
    std::ostringstream messages_;

};

SHARED_POINTER_TYPEDEFS(XMLErrorHandler);

class StrX {

  public:

    StrX(const XMLCh* const toTranscode)
      : fLocalForm(0)
    {
      // Call the private transcoding method
      fLocalForm = xercesc::XMLString::transcode(toTranscode);
    }

    ~StrX()
    {
      if (fLocalForm)
        xercesc::XMLString::release(&fLocalForm);
    }

    const char* localForm() const { return fLocalForm; }

  private:

    // since we have pointer data-members we should be careful about copying:
    StrX(const StrX&);
    StrX& operator=(const StrX&);

    char* fLocalForm;

};

inline std::ostream&
operator<<(std::ostream& target, const StrX& toDump)
{ return target << toDump.localForm(); }

#endif // HAWCNEST_XML_XMLERRORHANDLER_H_INCLUDED

