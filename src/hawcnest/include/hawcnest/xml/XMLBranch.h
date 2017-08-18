/*!
 * @file XMLBranch.h 
 * @brief Representation of XML document branches.  This file was copied from
          Branch.h in the Auger Offline utility package.
 * @author T. Paul
 * @author P. Cattaneo
 * @author D. Veberic
 * @author Javier Gonzalez
 * @author Segev BenZvi 
 * @date 25 Nov 2007
 * @version $Id: XMLBranch.h 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#ifndef HAWCNEST_XML_XMLBRANCH_H_INCLUDED
#define HAWCNEST_XML_XMLBRANCH_H_INCLUDED

#include <hawcnest/PointerTypedefs.h>
#include <hawcnest/impl/SafeBoolCast.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

// stuff we want to be able to GetData()
#include <vector>
#include <list>
#include <map>

#include <sstream>

class XMLReader;

typedef std::map<std::string, std::string> AttributeMap;

namespace {
  std::istream& operator>>(std::istream& is, tm& t);
};
/*!
 * @class XMLBranchOwner
 * @ingroup hawcnest_api
 * @brief Class to handle memory related to Xerces
 *
 * To guarantee the owner remains valid until the last copy of a branch
 * dissapears we use a shared pointer to a XMLBranchOwner which in turn holds
 * a pointer to the actual owner. This class is responsible for calling
 * the release method on the owner so Xerces can deallocate the memory.
 */
class XMLBranchOwner {

  public:

    XMLBranchOwner(xercesc::DOMDocument* const doc) : fDocument(doc) { }
   ~XMLBranchOwner() { if (fDocument) fDocument->release(); }

  private:

    xercesc::DOMDocument* fDocument;

};

SHARED_POINTER_TYPEDEFS(XMLBranchOwner);


/**
 * @class XMLBranch
 * @ingroup hawcnest_api
 * @brief Class representing a document branch.
 *
 * This class provides the methods for getting data from branches.
 *
 * For an introduction to using XMLBranches, see the documentation for the
 * XMLReader.
 *
 * A branch is essentially a DOM element node (a subset of what the DOM
 * considers to be a node). That is, DOM nodes such as comment or text nodes
 * which may not have children do not qualify to be branches. The idea of the
 * XMLBranch is to provide a simple tool for navigating trees of data that
 * might be more palatable for Shine applications (ie those which do not
 * require especially sophisticated document traversal.)
 *
 * In any case, if you do not want to use the XMLBranch class, you are free to
 * retrieve the DOM document element via Reader::GetDocument() and apply
 * standard DOM traversal tools.  Thus the reader allows the full power of DOM2
 * should the user need it.
 *
 * <h3>GetData methods</h3>
 * These methods find data in the current branch and attempt to cast it
 * according to the type of argument in the GetData argument list. Note that,
 * if you look at the actual implementation of GetData methods, most of them
 * simply invoke the (private) castData template function. The reason for the
 * intermediate GetData method is that is some cases, one might want to deal
 * with special cases that are not dealt with by a simple template.  For
 * example, suppose one has an XML element: \<someData\> 13 14 15 16
 * \</someData\> If the user requests to retrieve \<someData\> as an int,
 * probably one should return just 13.  If one asks for a string, probably one
 * should return the whole list of numbers.  These two different
 * interpretations can be dealt with using the intermediate GetData methods to
 * compliment the templated castData method.  Note that, currently, scaling by
 * the appropriate unit factor is handled in the GetData methods, not the
 * castData method. (In principle this could change, but keep in mind that for
 * the case of strings you don't want to try to multiply by a scale factor.)
 */
class XMLBranch : public SafeBoolCast<XMLBranch> {

  public:

    XMLBranch() :
      domNode_(0) { }

    XMLBranch(const XMLBranch& branch) :
      owner_(branch.owner_),
      domNode_(branch.domNode_),
      warning_(branch.warning_) { }

    XMLBranch& operator=(const XMLBranch& b);

  protected:

    XMLBranch(xercesc::DOMNode* const dom, XMLBranchOwnerPtr owner) :
      owner_(owner),
      domNode_(dom) { }

  public:

    XMLBranch GetParent() const;

    /// Get first child of this XMLBranch
    /// @return NullXMLBranch if no child found
    XMLBranch GetFirstChild() const;

    /// Get child of this XMLBranch by child name
    /// @return NullXMLBranch if no child found
    XMLBranch GetChild(const std::string& childName) const;

    /// Get child of this XMLBranch by child name and attributes (in string)
    /*!
     * Format of the attribute string: the ' ' (space) character separates the
     * attributes, the '=' is used between key and value.
     * Examples "id=123", "id=123 use=yes" etc.
     * @param childName name of child branch
     * @param attributes attribute string
     * @return NullXMLBranch if no child found
     */
    XMLBranch GetChild(const std::string& childName,
                       const std::string& attributes) const;

    /// Get child of this XMLBranch by child name and any number of
    /// attribute-value pairs (in map)
    /*!
     * This method allows the user to specify values for any number of
     * attributes. A (non-null) XMLBranch is returned only if the requested
     * name is found in the XML file and <b>all</b> the attributes are found
     * <b>with</b> their specified value, <b>with the single exception of the
     * unit attribute</b>.  Unit is handled as a special case by the reader,
     * and any unit attribute which may be present, either in the XML file or
     * the second argument of this method, will be ignored by this method. The
     * requested attributes are specified in a map\<string, string\> where the
     * first string is the attribute name and the second string is the
     * attribute value.
     */
    XMLBranch GetChild(const std::string& childName,
                       AttributeMap attributeMap) const;

    /// Get next sibling of this branch
    /// @return Null-XMLBranch if no child found
    XMLBranch GetNextSibling() const;

    /// Get sibling by name
    /// @return Null-XMLBranch if no sibling found
    XMLBranch GetSibling(const std::string& childName) const;

    /// Get sibling by name and ID
    /// @return Null-XMLBranch if no sibling found */
    XMLBranch GetSibling(const std::string& childName,
                         const std::string& multiID) const;

    /// Get sibling of this XMLBranch by child name and any number of
    /// attribute-value pairs.
    /// @return Null-XMLBranchImpl if no sibling found
    XMLBranch GetSibling(const std::string& childName,
                         AttributeMap& attributeMap) const;

    /// Get a map\<string, string\> containing all the attributes of this
    /// XMLBranch
    /// Unlike the GetData() method, GetAttributes() makes no attempt
    /// to cast the attributes
    AttributeMap GetAttributes() const;

    // Overloads of the GetData member template function
    // for cases that do not need unit conversion

    // bool
    void GetData(bool& b) const;
    void GetData(std::vector<bool>& b) const;
    void GetData(std::list<bool>& b) const;
    // string
    void GetData(std::string& s) const;
    void GetData(std::vector<std::string>& s) const;
    void GetData(std::list<std::string>& s) const;
    // char*
    void GetData(char*& c) const;
    // tm struct from ctime
    void GetData(tm& t) const;

    /// Get data in the current branch into an atomic type.
    /// Data in the XMLBranch gets cast to type T
    template<typename T> void GetData(T& a) const
    { CastData(a); a *= static_cast<T>(GetUnit()); }

    /// Get data in the current XMLBranch into an STL list or vector.
    /*! Data are loaded into an STL container of type W. Atomic types
     *  in the XML XMLBranch can be space, tab, or CR delimited. For
     *  example, data can be provided in an XML file like so:
     *
     *  \code
     *  <someData> 12.0 16.1 18.4 </someData>
     *  \endcode
     *
     * Since there are 3 space delimited floating point numbers
     * between the \<someData\> tags, one could read these data with
     * something like:
     *
     * \code
     * vector<double> someData;
     * someDataXMLBranch.GetData(someData);
     * // someDataXMLBranch assumed to point to <someData> element
     * assert(someData.size() == 3);
     * // the vector should have three numbers in it
     * \endcode
     */
    template<typename T, class A, template<typename, typename> class W>
    void
    GetData(W<T, A>& a)
      const
    {
      // a = W<T, A>(); <-- at some point we should make sure that
      // the argument is cleared
      CastData(a);
      T u = static_cast<T>(GetUnit());
      for (typename W<T, A>::iterator it = a.begin(); it != a.end(); ++it)
        *it *= u;
    }

    /// Get data in the current branch into a pair\<\>
    template<typename T1, typename T2>
    void
    GetData(std::pair<T1, T2>& p)
      const
    {
      std::istringstream is(GetDataString());
      is >> p.first >> p.second;
      p.first  *= static_cast<T1>(GetUnit());
      p.second *= static_cast<T2>(GetUnit());
    }

    /// Retrieve branch name as a string
    std::string GetXMLBranchNameString() const;

    /// returns a clone of this branch.
    XMLBranch Clone() const;

    /// Dump the branch into a string.
    std::string String() const;

    void SetWarning(const std::string& wrn) { warning_ = wrn; }

    bool BoolCast() const { return domNode_; }

    /// function to get the XMLBranch name
    std::string GetName() const;

    /// function to get the data inside an element as one big string
    std::string GetDataString() const;

    bool operator==(const XMLBranch& a) const
    { return a.GetDOMNode() == GetDOMNode(); }

    bool operator!=(const XMLBranch& a) const
    { return !operator==(a); }

    xercesc::DOMNode* GetDOMNode() const { return domNode_; }

  protected:

    // this here to guaranteee that the pointed data is still allocated when
    // the Reader goes out of scope.
    XMLBranchOwnerPtr owner_;

    mutable xercesc::DOMNode* domNode_;

    std::string warning_;

    /// Return data for the XMLBranch, attempt to cast it as the requested type,
    /// and multiply by the unit multiplier.  These methods use the
    /// getDataAndUnit method to find the data string and unit multiplier
    template<typename T>
    void
    CastData(T& dataT)
      const
    {
      std::istringstream is(GetDataString());
      is >> dataT;
    }

    template<typename T, class A, template<typename, typename> class W>
    void
    CastData(W<T, A>& v)
      const
    {
      const std::string dataString = GetDataString();
      std::istringstream is(dataString);
      T value;
      while (!is.eof())
        if (is >> value)
          v.push_back(value);
    }

    // Helper function to the (optional) unit attribute and return the
    // appropriate scale factor.
    double GetUnit() const;

    friend class XMLReader;

};

#endif // HAWCNEST_XML_XMLBRANCH_H_INCLUDED

