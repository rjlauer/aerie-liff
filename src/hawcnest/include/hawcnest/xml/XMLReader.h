/*!
 * @file XMLReader.h 
 * @author T. Paul
 * @author P. Cattaneo
 * @author D. Veberic
 * @author J. Gonzalez
 * @brief Utility for reading data from XML files.  Copied from Reader.h in
 *        Auger Offline utility package.
 * @date 20 Jun 2011 
 * @version $Id: XMLReader.h 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#ifndef HAWCNEST_XML_XMLREADER_H_INCLUDED
#define HAWCNEST_XML_XMLREADER_H_INCLUDED

#include <hawcnest/xml/XMLBranch.h>
#include <hawcnest/xml/XMLErrorHandler.h>

#include <xercesc/parsers/XercesDOMParser.hpp>

#include <sstream>

class XMLReaderStringInput;

/*!
 * @class XMLReader
 * @ingroup hawcnest_api
 */
class XMLReader {

  public:

    enum ValidationType {
      DTD, 
      SCHEMA,
      NONE
    };

    /// Constructor with validation options
    XMLReader(const std::string& name,
              const ValidationType vtype = XMLReader::SCHEMA);

    /// Constructor from an input string (as opposed to file)
    XMLReader(const XMLReaderStringInput& inputString,
              const ValidationType vtype = XMLReader::SCHEMA);

    XMLReader(const XMLBranch& branch)
    {
      if (branch.domNode_ && branch.domNode_->getOwnerDocument()) {
        topBranch_ = XMLBranch(
          branch.domNode_->getOwnerDocument()->getDocumentElement(),
          branch.owner_);
        topBranch_.owner_ = branch.owner_;
      }
    }

    std::string GetUri() const;

    /// Get the top Branch (represents same entity as document node)
    XMLBranch GetTopBranch() const { return topBranch_; }

  private:

    /// Branch at the top of the document tree
    XMLBranch topBranch_;

    // -------------------------------------------------------
    // The following are all the static methods and variables
    // -------------------------------------------------------

    enum EInputType {
      FROM_FILE = 0,      // take XML input from file
      FROM_MEM_BUF        // take XML input from memory
    };

    //static XMLErrorHandler* errHandler_;
    static XMLErrorHandlerPtr errHandler_;
    static bool isInitialized_;

    static void Initialize();

    static XMLBranch Parse(const std::string& input,
                           const ValidationType vtype,
                           const EInputType inputType);

};


/*!
 * @class XStr
 * @ingroup hawcnest_api
 * @brief Class to help converting from std::string to XMLCh*. It handles
 *        allocation and deallocation of the XML array.
 */
class XStr {

  public:

    // Call the private transcoding method
    XStr(const std::string toTranscode)
    { unicodeForm_ = xercesc::XMLString::transcode(toTranscode.c_str()); }

   ~XStr() { if (unicodeForm_) xercesc::XMLString::release(&unicodeForm_); }

    const XMLCh* XmlString() const { return unicodeForm_; }

  private:

    XStr(const XStr&);
    XStr& operator=(const XStr&);

    // This is the Unicode XMLCh format of the string.
    XMLCh* unicodeForm_;

};


/*!
 * @class XMLReaderStringInput
 * @ingroup hawcnest_api
 * @brief This just defines a type which holds some character data
 *        to be parsed by the Reader.
 *
 * This class is for use in cases where the Reader should parse a
 * string of XML information in memory instead of information in a
 * file.  It takes care of some of the pain of using the
 * MemBufInputSource class of Xerces.
 */
class XMLReaderStringInput {

  public:

    XMLReaderStringInput() { }

    XMLReaderStringInput(const std::string& inputString)
    { inputString_ = inputString; }

    XMLReaderStringInput(const std::ostringstream& inputStream)
    { inputString_ = inputStream.str(); }

   ~XMLReaderStringInput() { }

    std::string GetInputString() const { return inputString_; }

  private:

    std::string inputString_;

};

#endif // HAWCNEST_XML_XMLREADER_H_INCLUDED

