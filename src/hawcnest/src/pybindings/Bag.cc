/*!
 * @file Bag.cc 
 * @brief Python bindings to the Bag object dictionary.
 * @author Segev BenZvi 
 * @date 23 Jul 2009 
 * @version $Id: Bag.cc 18520 2014-01-25 15:24:24Z sybenzvi $
 */

#include <boost/python.hpp>

#include <hawcnest/processing/Bag.h>
#include <hawcnest/impl/pybindings.h>

using namespace boost::python;

/// Class for storing primitive baggable values, for unit testing only
template<class T>
class BaggablePrimitive : public Baggable {
  public:
    BaggablePrimitive() : value(T(0)) { }
    BaggablePrimitive(const T& v) : value(v) { }
    T value;
};

typedef BaggablePrimitive<int> BaggableInt;

/// Expose Baggable objects to boost::python
void
pybind_hawcnest_Baggable()
{
  class_<Baggable, boost::shared_ptr<Baggable> >
    ("Baggable",
     "Base class for objects handled by the HAWCNest framework.")
     ;

  class_<BaggableInt, bases<Baggable>, boost::shared_ptr<BaggableInt> >
    ("BaggableInt",
     "A baggable integer for python testing.")
    .def(init<const int>())
    .add_property("value", &BaggableInt::value, &BaggableInt::value,
                  "Integer value stored in object.")
     ;

  register_pointer_conversions<BaggableInt>();
}

// Function pointer to the simple Bag::Exists member
bool (Bag::*ExistsFcn)(const std::string&) const = &Bag::Exists;

// Return a shared pointer of an object from the Bag
template<typename T>
inline static
boost::shared_ptr<T>
bag_get(const Bag* b, const std::string& name)
{
  if (!b->Exists(name)) {
    PyErr_SetString(PyExc_KeyError, name.c_str());
    throw "blah";
    return boost::shared_ptr<T>();
  }

  try {
    boost::shared_ptr<const T> p = b->Get<boost::shared_ptr<const T> >(name);
    return boost::const_pointer_cast<T>(p);
  }
  catch (const std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return boost::shared_ptr<T>();
  }
}

// Put a const shared pointer to a Baggable object into the Bag
static void
bag_put(Bag& b, const std::string& s, BaggableConstPtr bp)
{
  b.Put(s, bp);
}

// Get a list of the instance name keys in the Bag
static boost::python::list
bag_keys(Bag const& b)
{
  boost::python::list keys;
  for (Bag::ConstBagIterator iT = b.BagBegin(); iT != b.BagEnd(); ++iT)
    keys.append(iT->first);
  return keys;
}

// Get a list of the object instance shared pointers in the Bag
static boost::python::list
bag_values(Bag const& b)
{
  boost::python::list values;
  for (Bag::ConstBagIterator iT = b.BagBegin(); iT != b.BagEnd(); ++iT)
    values.append(bag_get<Baggable>(&b, iT->first));
  return values; 
}

// Get a list of the key/instance pairs in the Bag
static boost::python::list
bag_items(Bag const& b)
{
  boost::python::list items;
  for (Bag::ConstBagIterator iT = b.BagBegin(); iT != b.BagEnd(); ++iT)
    items.append(boost::python::make_tuple(iT->first,
                                           bag_get<Baggable>(&b, iT->first)));
  return items; 
}

/// Expose Bag objects to boost::python
void
pybind_hawcnest_Bag()
{
  void (Bag::*putPtr)(const std::string&, BaggableConstPtr) = &Bag::Put;

  class_<Bag, boost::shared_ptr<Bag> >
    ("Bag",
     "Container for data passed between processing modules.")

    // Put an item into the bag
    .def("Put", bag_put,
         "Put a Baggable object into the Bag with an instance name.")
    .def("__setitem__", putPtr,
         "Put a Baggable object into the Bag with an instance name.")

    // Get an item from the Bag
    .def("Get", &bag_get<Baggable>,
         "Retrieve an item from the Bag by instance name.")
    .def("__getitem__", &bag_get<Baggable>,
         "Retrieve an item from the Bag by instance name.")

     // Determine whether or not an item exists in the Bag
    .def("Exists", ExistsFcn,
         "Return 'true' if an object with the given name is in the Bag.")
    .def("__contains__", ExistsFcn,
         "Return 'true' if an object with the given name is in the Bag.")

    // Access elements inside the Bag as python lists
    .def("keys", &bag_keys, "List of instance keys in the Bag.")
    .def("values", &bag_values, "List of object instances in the Bag.")
    .def("items", &bag_items, "List of key/object pairs in the Bag.")

    .def("size", (Bag::size_type (Bag::*)() const)&Bag::GetSize)
    .def("__len__", (Bag::size_type (Bag::*)() const)&Bag::GetSize)

    .def(self_ns::str(self))
    ;
}

