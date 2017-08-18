/*!
 * @file Bag-inl.h 
 * @brief Definition of the Bag object storage dictionary.
 * @author John Pretz 
 * @date 9 Sep 2008 
 * @version $Id: Bag-inl.h 18422 2014-01-15 16:00:30Z sybenzvi $
 */

#ifndef HAWCNEST_BAG_INL_H_INCLUDED
#define HAWCNEST_BAG_INL_H_INCLUDED

template <class T>
void
Bag::Put(const std::string& name, T baggable) 
{
  if (!baggable) {
    log_error("cannot put empty objects in the bag. Called with type '" 
              <<name_of<T>() <<"' and name '" << name << "'");
    throw bag_exception();
  }
  if (bag_.find(name) == bag_.end()) {
    bag_[name] = baggable;
#if BOOST_VERSION < 105300
    bagTypes_[name] = name_of<typename T::value_type>();
#else
    bagTypes_[name] = name_of<typename T::element_type>();
#endif
  }
  else {
    log_error("bag member '" << name << "' already exists. Cannot put type '"
              << name_of<T>() << "' with that key.");
    throw bag_exception();
  }
}

// version of Get if T is a const shared_ptr
template <class T>
T
Bag::Get(const std::string& name,
  typename boost::enable_if<is_shared_ptr<T> >::type*,
#if BOOST_VERSION < 105300
  typename boost::enable_if<boost::is_const<typename T::value_type> >::type*
#else
  typename boost::enable_if<boost::is_const<typename T::element_type> >::type*
#endif
  )
const
{
  typename BagType::const_iterator found = bag_.find(name);
  if(found == bag_.end()){
    return T();
  }
  BaggableConstPtr toreturn = found->second;
#if BOOST_VERSION < 105300
  return boost::dynamic_pointer_cast<typename T::value_type>(toreturn);
#else
  return boost::dynamic_pointer_cast<typename T::element_type>(toreturn);
#endif
}

// version of Get if T is a reference
template <class T>
const T&
Bag::Get(const std::string& name,
  typename boost::disable_if<is_shared_ptr<T> >::type*)
const
{
  boost::shared_ptr<const T> returnme =
    Get<boost::shared_ptr<const T> >(name);
  if (!returnme) {
    if (bag_.count(name) > 0) {
      log_error("Found bag member " << name
                << " but it isn't the requested type of " << name_of<T>());
      throw bag_exception();
    }
    else {
      log_error("Nothing in the bag with the name '" << name << "'"
                << " when looking for type '" << name_of<T>() << "'");
      throw bag_exception();
    }
  }
  return *returnme;
}

// for pretty-printing bag members
inline std::ostream&
operator<<(std::ostream& o, const Bag& b)
{
  b.dump(o);
  return o;
}

#endif // HAWCNEST_BAG_INL_H_INCLUDED

