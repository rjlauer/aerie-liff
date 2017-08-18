/*!
 * @file Bag.h 
 * @brief Declaration of the Bag object storage dictionary.
 * @author John Pretz 
 * @date 9 Sep 2008 
 * @version $Id: Bag.h 22869 2014-11-19 19:35:53Z criviere $
 */

#ifndef HAWCNEST_BAG_H_INCLUDED
#define HAWCNEST_BAG_H_INCLUDED

#include <hawcnest/impl/is_shared_ptr.h>
#include <hawcnest/impl/name_of.h>
#include <hawcnest/PointerTypedefs.h>
#include <hawcnest/Logging.h>

#include <boost/utility.hpp>
#include <boost/type_traits.hpp>
#include <boost/version.hpp>

#include <exception>
#include <iostream>
#include <string>
#include <map>

/*!
 * @author John Pretz
 * @ingroup hawcnest_api
 * @brief Base class for objects to be put in the bag
 */
class Baggable{
  public:
    virtual ~Baggable() { }
};

SHARED_POINTER_TYPEDEFS(Baggable);

/*!
 * @author John Pretz
 * @ingroup hawcnest_api
 * @brief Exception thrown when an error occurs with the Bag
 */
class bag_exception : public std::exception{
  public:
    virtual const char* what() const throw()
    { return "bag exception"; }
};

/*!
 * @author John Pretz
 * @ingroup hawcnest_api
 * @brief container for data passed Module to Module.
 *
 * The bag is read-only.  You use Bag::Put to fill the bag with objects which
 * inherit from Baggable.  Then Bag::Get can be used to retrieve objects.
 * There are two forms of Bag::Get.  One returns a const reference.  If the
 * object you're requesting doesn't exist or isn't the right type, 
 * a bag_exception is thrown.  The second form returns a const 
 * boost::shared_ptr.  In this case, no exception is thrown and the shared_ptr
 * is null if there is no such item in the bag.
 */
class Bag {

  public:

    /*!
     * @brief Adds an item to the bag with the specified name
     * @tparam An object which inherits from Baggable
     */
    template<class T>
    void
    Put(const std::string& name, T baggable);
  
    /*!
     * @brief Retrieves an item from the bag with the given name.  A null
     *        pointer is returned if the named object doesn't exist
     * @tparam A shared pointer to a const object
     */
    template<class T>
    T
    Get(const std::string& name,
        typename boost::enable_if<is_shared_ptr<T> >::type* = 0,
#if BOOST_VERSION < 105300
        typename boost::enable_if<boost::is_const<typename T::value_type> >::type* = 0
#else
        typename boost::enable_if<boost::is_const<typename T::element_type> >::type* = 0
#endif
        ) const;

    /*!
     * @brief Retrieves an item from the bag with the given name.  An exception
     *        is raised if the named object doesn't exist
     * @tparam A non-const Baggable type
     */
    template<class T>
    const T&
    Get(const std::string& name,
        typename boost::disable_if<is_shared_ptr<T> >::type* = 0) const;

    /*!
     * @brief Delete an object in the bag using its name key
     */
    void
    Delete(const std::string& name);

    /*!
     * @brief Clear the bag/key contents
     */
    void
    Clear() {
      bag_.clear();
      bagTypes_.clear();
    }

    /*!
     * @brief Return 'true' if an object with the given name exists, regardless
     *        of type
     */
    bool
    Exists(const std::string& name) const
    { return bag_.find(name) != bag_.end(); }

    /*!
     * @brief Returns 'true' if an object with the given name (and type)
     *        exists in the bag.  Redundant with Get-by-shared-pointer function
     * @tparam A shared pointer to a const object
     */
    template <class T>
    bool
    Exists(const std::string& name,
           typename boost::disable_if<is_shared_ptr<T> >::type* = 0) const {
      return bool(Get<boost::shared_ptr<const T> >(name));
    }

    /*!
     * @brief For pretty printing
     */
    void dump(std::ostream& o) const;

    typedef std::map<std::string, BaggableConstPtr> BagType;
    typedef BagType::const_iterator ConstBagIterator;

    /// Read-only iterator to the start of the Bag instance key/object container
    ConstBagIterator BagBegin() const { return bag_.begin(); }

    /// Read-only iterator to the end of the Bag instance key/object container
    ConstBagIterator BagEnd() const { return bag_.end(); }

    typedef std::map<std::string, std::string> KeyType;
    typedef KeyType::const_iterator ConstTypeIterator;

    /// Read-only iterator to the start of the Bag instance key/type dictionary
    ConstTypeIterator TypesBegin() const { return bagTypes_.begin(); }

    /// Read-only iterator to the end of the Bag instance key/type dictionary
    ConstTypeIterator TypesEnd() const { return bagTypes_.end(); }

    typedef BagType::size_type size_type;

    /// Output the size of the Bag
    size_type GetSize() const { return bag_.size(); }

  private:

    BagType bag_;       ///< List of key-Baggable pointer pairs (the Bag)
    KeyType bagTypes_;  ///< Dictionary of key-Baggable type pairs

};

#include <hawcnest/processing/Bag-inl.h>

SHARED_POINTER_TYPEDEFS(Bag);

#endif // HAWCNEST_BAG_H_INCLUDED

