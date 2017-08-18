/*!
 * @file XMLBranch.cc
 * @author T. Paul
 * @author P. Cattaneo
 * @author D. Veberic
 * @author Javier Gonzalez
 * @date 25 Nov 2007
 * @version $Id: XMLBranch.cc 18422 2014-01-15 16:00:30Z sybenzvi $
 */

#include <hawcnest/xml/XMLReader.h>
#include <hawcnest/xml/XMLBranch.h>
#include <hawcnest/xml/XMLErrorHandler.h>
#include <hawcnest/Logging.h>
#include <hawcnest/HAWCUnits.h>
#include <hawcnest/Parser.h>

#include <boost/tokenizer.hpp>

#include <xercesc/util/XercesVersion.hpp>
#if _XERCES_VERSION >= 30000
 #define HAVE_XERCES3
 #include <xercesc/dom/DOMLSSerializer.hpp>
#else
 #include <xercesc/framework/MemBufFormatTarget.hpp>
#endif
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>

// this is here only for the StrX class
#define Str2X(str) XStr(str).XmlString()

using namespace HAWCUnits;
using namespace xercesc;
using namespace std;

namespace {
  
  class IsSpace {
    public:
      bool operator()(const char x) const
      { return x == ' ' || x == '\r' || x == '\n' || x == '\t'; }
  };

  class NotSpace {
    public:
      bool operator()(const char x) const
      { return !(x == ' ' || x == '\r' || x == '\n' || x == '\t'); }
  };

}

XMLBranch
XMLBranch::GetParent()
  const
{
  if (!domNode_ || domNode_->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
    return XMLBranch();

  DOMNode* parentNode =
    dynamic_cast<xercesc::DOMElement*>(domNode_)->getParentNode();

  if (!parentNode || parentNode->getNodeType() != DOMNode::ELEMENT_NODE)
    return XMLBranch();

  return XMLBranch(parentNode, owner_);
}

/// Get the first branch child
/// @return First child branch
XMLBranch
XMLBranch::GetFirstChild()
  const
{
  if (!domNode_) {
    log_warn(warning_);
    log_fatal("Getting first child in a null-XMLBranch. " << warning_);
  }

  DOMNode* childNode = domNode_->getFirstChild();
  while (childNode && childNode->getNodeType() != DOMNode::ELEMENT_NODE)
    childNode = childNode->getNextSibling();

  XMLBranch b(childNode, owner_);
  if (!childNode) {
    ostringstream warn;
    warn << "First child in branch '" << GetXMLBranchNameString()
         << "' not found";
    b.SetWarning(warn.str());
  }

  return b;
}

/// Get branch child
/// @param requestedName Name requested in the branch
/// @param requestedAttributeMap Map of the attributes attached to the branch
/// @return Child branch
XMLBranch
XMLBranch::GetChild(const string& requestedName,
                    AttributeMap requestedAttributeMap)
  const
{
  if (!domNode_) {
    log_warn(warning_);
    log_fatal("Getting child '" << requestedName << "' on a null-XMLBranch. "
              << warning_);
  }

  requestedAttributeMap.erase("unit");
  requestedAttributeMap.erase("UNIT");

  for (DOMNode* childNode = domNode_->getFirstChild(); childNode;
       childNode = childNode->getNextSibling())
  {
    if (childNode->getNodeType() == DOMNode::ELEMENT_NODE) {

      const string foundName = StrX(childNode->getNodeName()).localForm();

      if (foundName == requestedName) {

        // Get all the attributes into a map.
        AttributeMap foundAttributeMap;
        const DOMNamedNodeMap* const attributes = childNode->getAttributes();

        for (unsigned int j = 0; j < attributes->getLength(); ++j) {

          const DOMNode* const attribute = attributes->item(j);

          foundAttributeMap.insert(
            make_pair(StrX(attribute->getNodeName()).localForm(),
                      StrX(attribute->getNodeValue()).localForm()));

        } // filled up foundAttributeMap

        // First, discard any unit attributes in either the found or requested
        // maps.  Units are treated as a special case.
        foundAttributeMap.erase("unit");
        foundAttributeMap.erase("UNIT");

        // Check for exact match between remaining attributes and attribute
        // values in the requested and found maps
        if (foundAttributeMap.size() == requestedAttributeMap.size()) {

          bool foundIt = true;

          // try to disqualify equality
          for (AttributeMap::iterator foundIter = foundAttributeMap.begin();
               foundIter != foundAttributeMap.end(); ++foundIter) {

            const AttributeMap::iterator requestedIter =
              requestedAttributeMap.find(foundIter->first);
            if (requestedIter == requestedAttributeMap.end() ||
                foundIter->second != requestedIter->second) {
              foundIt = false;
              break;
            }
          }

          if (foundIt)
            return XMLBranch(childNode, owner_);
        }
      } // same name
    } // condition ELEMENT_NODE
  }

  XMLBranch b;
  ostringstream warn;
  warn << "Child '" << requestedName << "' in branch '"
       << GetXMLBranchNameString() << "' not found";
  b.SetWarning(warn.str());
  return b; // null-branch
}

/// Get child from the branch
/// @param requestedName Name requested in the branch
/// @return Child branch
XMLBranch
XMLBranch::GetChild(const string& requestedName)
  const
{
  AttributeMap dummy;
  return GetChild(requestedName, dummy);
}

XMLBranch
XMLBranch::GetChild(const string& requestedName, const string& att)
  const
{
  using namespace boost;

  if (att.empty())
    return GetChild(requestedName);

  char_separator<char> aSep(" ");
  char_separator<char> kvSep("=");
  typedef tokenizer<char_separator<char> > Tokenizer;
  Tokenizer aTok(att, aSep);
  AttributeMap attMap;
  for (Tokenizer::const_iterator aIt = aTok.begin(); aIt != aTok.end(); ++aIt) {
    string key;
    string value;
    Tokenizer kvTok(*aIt, kvSep);
    Tokenizer::const_iterator kvIt = kvTok.begin();
    if (kvIt != kvTok.end()) {
      key = *kvIt;
      ++kvIt;
      if (kvIt != kvTok.end())
        value = *kvIt;
    }
    attMap[key] = value;
  }

  return GetChild(requestedName, attMap);
}

/// Get the map of the branch attibutes
AttributeMap
XMLBranch::GetAttributes()
  const
{
  if (!domNode_) {
    log_warn(warning_);
    log_fatal("Getting attributes of a null-XMLBranch. " << warning_);
  }

  AttributeMap attMap;

  const DOMNamedNodeMap* const attributes = domNode_->getAttributes();
  for (unsigned int j = 0; j < attributes->getLength(); ++j) {
    const DOMNode* const attribute = attributes->item(j);

    attMap[StrX(attribute->getNodeName()).localForm()] =
      StrX(attribute->getNodeValue()).localForm();
  }
  return attMap;
}

string
XMLBranch::GetXMLBranchNameString()
  const
{
  if (!domNode_) {
    log_warn(warning_);
    log_fatal("Getting the name of a null-XMLBranch. " << warning_);
  }
  domNode_->getNodeName();
  return StrX(domNode_->getNodeName()).localForm();
}

XMLBranch
XMLBranch::GetNextSibling()
  const
{
  if (!domNode_) {
    log_warn(warning_);
    log_fatal("Getting next sibling in a null-XMLBranch. " << warning_);
  }

  DOMNode* siblingNode = domNode_->getNextSibling();

  while (siblingNode && siblingNode->getNodeType() != DOMNode::ELEMENT_NODE)
    siblingNode = siblingNode->getNextSibling();

  XMLBranch b(siblingNode, owner_);
  if (!siblingNode) {
    ostringstream warn;
    warn << "Next sibling of branch '" << GetXMLBranchNameString()
         << "' not found";
    b.SetWarning(warn.str());
  }
  return b;
}

XMLBranch
XMLBranch::GetSibling(const string& requestedName, AttributeMap& attributeMap)
  const
{
  // check that current node is valid.
  if (!domNode_) {
    log_warn(warning_);
    log_fatal("Getting sibling of a null-XMLBranch. " << warning_);
  }

  // back up to the parent.
  DOMNode* const parentNode = domNode_->getParentNode();

  XMLBranch parentXMLBranch(parentNode, owner_);

  if (!parentNode) {
    ostringstream warn;
    warn << "Parent of branch '" << GetXMLBranchNameString() << "' not found";
    parentXMLBranch.SetWarning(warn.str());
  }

  return parentXMLBranch.GetChild(requestedName, attributeMap);
}

XMLBranch
XMLBranch::GetSibling(const string& requestedName)
  const
{
  AttributeMap dummy;
  return GetSibling(requestedName, dummy);
}

/// Get sibling
/// @param requestedName Requested name
/// @param id Attribute name
/// @return Sibling branch
XMLBranch
XMLBranch::GetSibling(const string& requestedName, const string& id)
  const
{
  AttributeMap idMap;
  idMap["id"] = id;
  return GetSibling(requestedName, idMap);
}

string
XMLBranch::GetName()
  const
{
  return StrX(domNode_->getNodeName()).localForm();
}

// -------------------- GetData methods ------------------------------

/// Get Data string
/// @return String with data attached
string
XMLBranch::GetDataString()
  const
{
  if (!domNode_) {
    log_warn(warning_);
    log_fatal("Getting data from a null-XMLBranch. " << warning_);
  }

  // Find the data residing beneath this tag
  const DOMNodeList* const dataNodes = domNode_->getChildNodes();

  if (!dataNodes)
    return string();

  string dataString;

  for (unsigned int k = 0; k < dataNodes->getLength(); ++k) {

    const DOMNode* const currentNode = dataNodes->item(k);

    // For TEXT nodes, postpend the node onto the dataString. NB if
    // there are multiple TEXT nodes beneath this element, and they
    // are, for example, separated by one or more sub- elements,
    // these text nodes will be concatenated into a single data
    // string.
    if (currentNode->getNodeType() == DOMNode::TEXT_NODE)
      dataString += StrX(currentNode->getNodeValue()).localForm();

  }

  // Remove any leading and/or trailing whitespace (and CR's) from the string
  const string::const_iterator start =
    find_if(dataString.begin(), dataString.end(), NotSpace());
  const string::const_reverse_iterator stop =
    find_if(dataString.rbegin(), dataString.rend(), NotSpace());

  return distance(start, stop.base()) > 0 ?
    string(start, stop.base()) : string();
}

/// Get the unit of the token
/// @return Unit of the token
double
XMLBranch::GetUnit()
  const
{
  if (!domNode_) {
    log_warn(warning_);
    log_fatal("Getting unit from null-XMLBranch. " << warning_);
  }

  double unit = 1.;

  // Check for a unit attribute
  const DOMNamedNodeMap* const attributes = domNode_->getAttributes();

  if (attributes) {

    string unitString;
    for (unsigned int j = 0; j < attributes->getLength(); ++j) {
      const DOMNode* const attribute = attributes->item(j);
      const string name = StrX(attribute->getNodeName()).localForm();
      if (name == "UNIT" || name == "unit") {
        unitString = StrX(attribute->getNodeValue()).localForm();
        break;
      }
    }

    // If found, convert the unit to the appropriate scale factor,
    // otherwise set scale factor to 1.
    if (!unitString.empty()) {
      HAWCUnits::Parser p;
      unit = p.Evaluate(unitString);

      // The unit value is 0 if the name is not in the dictionary (hackish...)
      if (unit == 0.) {
        log_fatal("The unit '" << unitString 
                  << "' was not in the HAWCUnits dictionary");
      }
    }
  }

  return unit;
}

XMLBranch&
XMLBranch::operator=(const XMLBranch& b)
{
  domNode_ = b.domNode_;
  owner_ = b.owner_;
  warning_ = b.warning_;
  return *this;
}

/// Getting a std::string
void
XMLBranch::GetData(string& s)
  const
{
  // do not use castData, as in this case
  // we want to return the ENTIRE string, including
  // possible white space.
  s = GetDataString();
}


// DV considered dangerous since user has to call free()
/// Getting a char*
void
XMLBranch::GetData(char*& /*c*/)
  const
{
  // do not use castData, as in this case
  // we want to return the ENTIRE string, including
  // possible white space. Do not forget to call free.

  /*const string s = GetDataString();
  c = strdup(s.c_str());*/
  log_fatal("This method requires user to call free() after "
            "done with the char*! Disabled for the moment.");
}

/// Get a tm struct from an XML datetime value (yyyy-mm-ddThh:mm:ssZ)
void
XMLBranch::GetData(tm& t)
  const
{
  stringstream ss;
  ss << GetDataString() << endl;

  char minus = '?';
  char minus2 = '?';
  char T = '?';
  char colon = '?';
  char colon2 = '?';
  char zone = '?';

  if ((ss >> t.tm_year >> minus >> t.tm_mon >> minus2 >> t.tm_mday >> T
          >> t.tm_hour >> colon >> t.tm_min >> colon2 >> t.tm_sec >> zone) &&
       minus == '-' && minus2 == '-' && colon == ':' && colon2 == ':' &&
       zone == 'Z')
  {
    t.tm_year -= 1900;
    t.tm_mon -= 1;
  }
  else {
    log_fatal("Invalid datetime "
             << t.tm_year << minus << t.tm_mon << minus2 << t.tm_mday << T
             << t.tm_hour << colon << t.tm_min << colon2 << t.tm_sec << zone);
  }
}

XMLBranch
XMLBranch::Clone()
  const
{
  if (domNode_) {
    const XMLReaderStringInput readerInput(String());
    const XMLReader r(readerInput);
    return r.GetTopBranch();
  }
  return XMLBranch();
}

std::string
XMLBranch::String()
  const
{
  XMLCh* tempStr = XMLString::transcode("LS");
  DOMImplementation* const impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
#ifdef HAVE_XERCES3
  DOMLSSerializer* const theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
  char *Dom = xercesc::XMLString::transcode(theSerializer->writeToString(domNode_));
  std::string res(Dom);
  xercesc::XMLString::release(&Dom);
  return res;
#else
  DOMWriter* const theSerializer = ((DOMImplementationLS*)impl)->createDOMWriter();

  MemBufFormatTarget memForm;
  theSerializer->writeNode(&memForm, *domNode_);
  XMLString::release(&tempStr);
  return string((const char*)memForm.getRawBuffer());
#endif
}

#define XMLBRANCH_GETDATA_WITH_CAST_ONLY(_Type_...) \
void                                                 \
XMLBranch::GetData(_Type_& t)                           \
  const                                              \
{                                                    \
  CastData(t);                                       \
}

XMLBRANCH_GETDATA_WITH_CAST_ONLY(bool)
XMLBRANCH_GETDATA_WITH_CAST_ONLY(vector<bool>)
XMLBRANCH_GETDATA_WITH_CAST_ONLY(list<bool>)
XMLBRANCH_GETDATA_WITH_CAST_ONLY(vector<string>)
XMLBRANCH_GETDATA_WITH_CAST_ONLY(list<string>)

#undef XMLBRANCH_GETDATA_WITH_CAST_ONLY

