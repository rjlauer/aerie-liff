/*!
 * @file XMLReader.cc 
 * @brief Implementation of the XML document reader.
 * @author Tom Paul
 * @author Segev BenZvi 
 * @date 20 Jun 2011 
 * @version $Id: XMLReader.cc 17766 2013-11-01 04:23:51Z sybenzvi $
 */

#include <hawcnest/xml/XMLReader.h>
#include <hawcnest/xml/XMLErrorHandler.h>
#include <hawcnest/Logging.h>
#include <hawcnest/HAWCUnits.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>

using namespace xercesc;
using namespace std;

XMLErrorHandlerPtr XMLReader::errHandler_ = XMLErrorHandlerPtr();
bool XMLReader::isInitialized_ = false;

typedef boost::shared_ptr<XercesDOMParser> const XercesDOMParserPtr;

/*!
 * @param name of the XML file
 * @param validationType validation option
 */
XMLReader::XMLReader(const string& name, const ValidationType vtype)
{
  topBranch_ = Parse(name, vtype, XMLReader::FROM_FILE);
}

/*!
 * @param inputString is input XML string.
 * @param validationType validation option
 */
XMLReader::XMLReader(const XMLReaderStringInput& inputString,
                     const ValidationType vt)
{
  topBranch_ = Parse(inputString.GetInputString(), vt, XMLReader::FROM_MEM_BUF);
}

string
XMLReader::GetUri()
  const
{
  string name;
  if (topBranch_) {
    DOMDocument* const doc = topBranch_.GetDOMNode()->getOwnerDocument();
    if (doc) {
      const XMLCh* uri = doc->getDocumentURI();
      if (uri) {
        char* n = XMLString::transcode(uri);
        name = n;
        XMLString::release(&n);
      }
    }
  }
  return name;
}

void
XMLReader::Initialize()
{
  if (isInitialized_)
    return;

  XMLPlatformUtils::Initialize();
  errHandler_ = boost::make_shared<XMLErrorHandler>();
  isInitialized_ = true;
}

/// Parse XML
XMLBranch
XMLReader::Parse(const string& input,
                 const ValidationType vtype,
                 const EInputType inputType)
{
  Initialize();

  //XercesDOMParser* const parser = new XercesDOMParser;
  XercesDOMParserPtr parser = boost::make_shared<XercesDOMParser>();

  parser->setDoSchema(false);
  parser->setValidationScheme(XercesDOMParser::Val_Never);
  parser->setCreateEntityReferenceNodes(false);

  switch (vtype) {
    case DTD:
      parser->setValidationScheme(XercesDOMParser::Val_Auto);
      break;
    case SCHEMA:
      parser->setDoSchema(true);
      parser->setValidationScheme(XercesDOMParser::Val_Always);
      parser->setValidationSchemaFullChecking(true);
      parser->setDoNamespaces(true);
      break;
    case NONE:
      parser->setValidationScheme(XercesDOMParser::Val_Never);
      break;
    default:
      log_fatal("You have selected an invalid validation mode");
      break;
  }

  parser->setErrorHandler(errHandler_.get());

  try {

    if (inputType == FROM_FILE)
      parser->parse(input.c_str());
    else {
      const MemBufInputSource memSource((const XMLByte*)input.c_str(),
                                        input.size(), "", false);
      parser->parse(memSource);
    }

    if (errHandler_->getSawErrors()) {

      const DOMNamedNodeMap* const topAtts =
        parser->getDocument()->getDocumentElement()->getAttributes();

      for (unsigned int i = 0; i < topAtts->getLength(); ++i) {

        const DOMNode* const attribute = topAtts->item(i);
        const string attName(StrX(attribute->getNodeName()).localForm());

        const string postNs = attName.substr(attName.find_first_of(":") + 1);

        if (postNs == "noNamespaceSchemaLocation") {

          const string validationFile = StrX(attribute->getNodeValue()).localForm();
          ostringstream msg;

          if (!boost::filesystem::exists(validationFile)) {
            msg << "Could not find the schema validation file \""
                << validationFile << "\" ";
            if (inputType == FROM_FILE)
              msg << "which was requested the by configuration file \"" << input << "\".";
          } else {
            if (inputType == FROM_FILE)
              msg << "Schema validation failed for file \"" << input << "\". ";
            else
              msg << "Schema validation failed after parameter replacement";

            msg << "The following errors were reported while parsing: "
                << errHandler_->getMessages().str() << '\n';
            if (inputType == FROM_MEM_BUF) {
              msg << "Here is the XML as it reads after parameter replacement:\n"
                  << input;
            }
          }
          log_fatal(msg.str());
        }
      }

      log_warn("No schema (.xsd) document is available for "
               << (inputType == FROM_FILE ? "file" : "in-memory string")
               << " \"" << input << "\". "
               << "It will not be validated!");

      errHandler_->resetErrors();
    }

  }
  catch (const XMLException& e) {
    const StrX message(e.getMessage());
    log_fatal("An error occurred during parsing, message: " << message);
  }

  // Initialize top branch to the document element, taking ownership
  // of the document.
  XMLBranch branch;
  //branch.owner_.reset(new XMLBranchOwner(parser->adoptDocument()));
  branch.owner_ = boost::make_shared<XMLBranchOwner>(parser->adoptDocument());
  branch.domNode_ = parser->getDocument()->getDocumentElement();
  return branch;
}

ostream&
operator<<(ostream& os, const XMLBranch& b)
{
  return os << b.String() << endl;
}

