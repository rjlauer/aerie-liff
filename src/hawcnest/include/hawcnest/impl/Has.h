/*!
 * @file Has.h
 * @author John Pretz
 * @brief Define compile time checks for the existence of certain functions
 *        inside a class
 * @date 25 Aug 2008 
 * @version $Id: Has.h 14881 2013-04-28 14:02:42Z sybenzvi $
 *
 * This code provides a compile-time check that a certain class has or doesn't
 * have certain methods. This is used so that services don't have to implement
 * Initialize, Finish or DefaultConfiguration methods and so that we don't have
 * to have all services inherit from a common base class. Template
 * meta-programming at it's best
 *
 * It also supplies, for a method XYZ a CallXYZ function which defines the
 * default implementation in the case that there is nothing in the service
 * class or forwards the call to the class if that method exists.
 *
 * There might be a way to clean this up, but it's so buried in the framework,
 * there's really no really significant advantage
 */

#ifndef HAWCNEST_HAS_H_INCLUDED
#define HAWCNEST_HAS_H_INCLUDED

#include <boost/mpl/bool.hpp>
#include <hawcnest/Configuration.h>

typedef char NotFound;
struct Found {
  char x[2];
};

// testing that the class has a Finish method
template <class T, void (T::*)() >
struct test_for_member_finish
{
};

template<class T>
static Found
test_for_finish(test_for_member_finish<T,&T::Finish>*);

template<class T>
static NotFound
test_for_finish( ... );

template<class T>
struct has_finish {
  static const bool value =
    (sizeof(test_for_finish<T>(0)) == sizeof(Found));
  typedef boost::mpl::bool_<value> type;
};

// The two 'CallFinish' overloads. One if the WrappedType has a Finish
// method and one if it doesn't
template <class WrappedType>
void 
CallFinish(WrappedType& wrapped,
           typename boost::enable_if<has_finish<WrappedType> >::type* = 0)
{
  wrapped.Finish();
}

template <class WrappedType>
void 
CallFinish(WrappedType& wrapped,
           typename boost::disable_if<has_finish<WrappedType> >::type* = 0)
{
}

// testing for Initialize
template <class T, void (T::*)(const Configuration&)> 
struct test_for_member_initialize
{
};

template<class T>
static Found
test_for_initialize(test_for_member_initialize<T,&T::Initialize>*);

template<class T>
static NotFound
test_for_initialize( ... );

template<class T>
struct has_initialize {
  static const bool value =
    (sizeof( test_for_initialize<T>( 0 ) ) == sizeof(Found));
  typedef boost::mpl::bool_<value> type;
};

// The two 'CallInitialize' overloads. One if the WrappedType has an Intialize
// method and one if it doesn't
template <class WrappedType>
void 
CallInitialize(WrappedType& wrapped,
               const Configuration& conf,
               typename 
               boost::enable_if<has_initialize<WrappedType> >::type* = 0)
{
  wrapped.Initialize(conf);
}

template <class WrappedType>
void 
CallInitialize(WrappedType& wrapped,
               const Configuration& conf,
               typename 
               boost::disable_if<has_initialize<WrappedType> >::type* = 0)
{
}

// testing for DefaultConfiguration
template <class T, Configuration (T::*)()> 
struct test_for_member_defaultconfiguration
{
};

template<class T>
static Found
test_for_defaultconfiguration(test_for_member_defaultconfiguration<T,
                              &T::DefaultConfiguration>*);

template<class T>
static NotFound
test_for_defaultconfiguration( ... );

template<class T>
struct has_defaultconfiguration {
  static const bool value =
    (sizeof( test_for_defaultconfiguration<T>( 0 ) ) == sizeof(Found));
  typedef boost::mpl::bool_<value> type;
};

// The two 'CallDefaultConfiguration' overloads. One if the WrappedType has a 
// DefaultConfiguration method and one if it doesn't.
template <class WrappedType>
Configuration
CallDefaultConfiguration(WrappedType& wrapped,
                         typename 
                         boost::enable_if<has_defaultconfiguration<
                                          WrappedType> >::type* = 0)
{
  return wrapped.DefaultConfiguration();
}

template <class WrappedType>
Configuration
CallDefaultConfiguration(WrappedType& wrapped,
                         typename 
                         boost::disable_if<has_defaultconfiguration<
                                           WrappedType> >::type* = 0)
{
  Configuration config;
  return config;
}

#endif // HAWCNEST_HAS_H_INCLUDED

