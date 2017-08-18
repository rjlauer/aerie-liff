/*!
 * @file IteratorTypedefs.h
 * @brief Access policy typedefs.
 * @author Segev BenZvi
 * @version $Id: IteratorTypedefs.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_ITERATOR_ITERATORTYPDEFS_H_INCLUDED
#define DATACLASSES_ITERATOR_ITERATORTYPDEFS_H_INCLUDED

/*!
 * @def STL_CONTAINER_ITERATOR_TYPEDEFS
 * @ingroup design_patterns
 * @brief Given an STL container storing some type, define iterator typedefs.
 *        E.g., if Type=BigInt and Container=#std::vector, this will define 
 *        BigIntIterator and ConstBigIntIterator typedefs.
 */
#define STL_CONTAINER_ITERATOR_TYPEDEFS(Type, Container)         \
  typedef Container<Type >::iterator Type##Iterator;             \
  typedef Container<Type >::const_iterator Const##Type##Iterator;

/*!
 * @def STL_CONTAINER_WRAPPER_FUNCTIONS
 * @ingroup design_patterns
 * @brief Create iterator access wrappers for classes with private container
 *        members (e.g., BigIntsBegin, BigIntsEnd).
 */
#define STL_CONTAINER_WRAPPER_FUNCTIONS(Type, memberName) \
  Type##Iterator Type##sBegin()                           \
  { return memberName.begin(); }                          \
  Type##Iterator Type##sEnd()                             \
  { return memberName.end(); }                            \
  Const##Type##Iterator Type##sBegin() const              \
  { return memberName.begin(); }                          \
  Const##Type##Iterator Type##sEnd() const                \
  { return memberName.end(); }                            \
  void Add##Type(const Type& t)                           \
  { memberName.push_back(t); }

#endif // DATACLASSES_ITERATOR_ITERATORTYPDEFS_H_INCLUDED

