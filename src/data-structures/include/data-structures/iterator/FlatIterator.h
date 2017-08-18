/*!
 * @file FlatIterator.h
 * @brief Iterator that collapses a nested structure into a flat structure.
 * @author Segev BenZvi
 * @version $Id: FlatIterator.h 25538 2015-05-31 01:13:31Z sybenzvi $
 */

#ifndef DATACLASSES_ITERATOR_FLATITERATOR_H_INCLUDED
#define DATACLASSES_ITERATOR_FLATITERATOR_H_INCLUDED

#include <cassert>
#include <cstddef>
#include <iterator>

/*!
 * @class flat_iterator
 * @author Segev BenZvi
 * @ingroup design_patterns
 * @brief Make a flattening iterator over a two-level nested container such as
 *        vector<vector<int> >
 *
 * This class defines a forward iterator over the bottom level of a nested
 * structure.  This has some syntactic advantages.  For example, in a class
 * hierarchy composed of a structure like Detector->Tank->PMT, a flat_iterator
 * over PMTs can be used to avoid writing nested loops over Tanks and PMTs 
 * while accessing the bottom level.
 *
 * Construction of a flat_iterator requires iterators to the beginning and end
 * of the outer container, and a defined policy for accessing the beginning and
 * end of the inner container type from an outer container object.
 *
 * @tparam OIter Iterator type for outer container
 * @tparam IIter Iterator type for inner container
 * @tparam AccessPolicy Tells flat_iterator how to access the start and end of
 *         the inner container (defined per inner iterator)
 */
template <typename OIter, typename IIter, typename AccessPolicy>
class flat_iterator : 
  public std::iterator<std::forward_iterator_tag, ptrdiff_t>
{
  public:

    typedef typename IIter::reference reference;
    typedef typename IIter::pointer pointer;

    flat_iterator() { }

    flat_iterator(OIter oBegin, OIter oEnd);

    reference operator*() { return *iInner_; }
    const reference operator*() const { return *iInner_; }

    pointer operator->() { return &(*iInner_); }
    const pointer operator->() const { return &(*iInner_); }

    bool operator==(const flat_iterator& fi) const;
    bool operator!=(const flat_iterator& fi) const { return !(*this == fi); }

    flat_iterator& operator++();        ///< Pre-increment
    flat_iterator operator++(int);      ///< Post-increment

  private:

    void NextValid();

    OIter iOuter_;
    OIter endOuter_;
    IIter iInner_;

};

/* flat_iterator must exist in one of two well-defined states:
 *   1. iInner_ points to the current valid element; iOuter != endOuter_.
 *   2. iOuter_ == endOuter_; iInner_ is meaningless.
 */
template <typename OIter, typename IIter, typename AccessPolicy>
flat_iterator<OIter, IIter, AccessPolicy>::flat_iterator(
  OIter oBegin, OIter oEnd) :
  iOuter_(oBegin),
  endOuter_(oEnd)
{
  // Check that outer container is not empty before dereferencing iOuter_
  if (iOuter_ != endOuter_) {
    // Starting from an indeterminate state, find the next valid element
    // or reach iOuter_ == endOuter_
    iInner_ = AccessPolicy::begin(*iOuter_);
    NextValid();
  }
}

template <typename OIter, typename IIter, typename AccessPolicy>
bool
flat_iterator<OIter, IIter, AccessPolicy>::operator==(
  const flat_iterator<OIter, IIter, AccessPolicy>& fi)
  const
{
  // If both outer containers are empty, the iterators are automatically equal
  if (iOuter_ == fi.iOuter_ &&
      endOuter_ == fi.endOuter_ &&
      iOuter_ == endOuter_)
    return true;

  // Otherwise, check if the state iterators are equal
  return iOuter_ == fi.iOuter_ &&
         endOuter_ == fi.endOuter_ &&
         iInner_ == fi.iInner_;
}

template <typename OIter, typename IIter, typename AccessPolicy>
flat_iterator<OIter, IIter, AccessPolicy>&
flat_iterator<OIter, IIter, AccessPolicy>::operator++()
{
  // Check out-of-bounds increments (debugging only)
  assert(iOuter_ != endOuter_);

  // iInner currently is not at the end of its container. Put iInner_ in an
  // indeterminate state.
  ++iInner_;

  // Starting from an indeterminate state, find the next valid element
  // or reach iOuter_ == endOuter_
  NextValid();

  return *this;
}

template <typename OIter, typename IIter, typename AccessPolicy>
flat_iterator<OIter, IIter, AccessPolicy>
flat_iterator<OIter, IIter, AccessPolicy>::operator++(int)
{
  // Post-fix: return a copy of the iterator, then increment it.
  flat_iterator<OIter, IIter, AccessPolicy> cpy(*this);
  ++*this;
  return cpy;
}

template <typename OIter, typename IIter, typename AccessPolicy>
void
flat_iterator<OIter, IIter, AccessPolicy>::NextValid() {

  // In case the inner container is empty, jump to the next one
  while (iInner_ == AccessPolicy::end(*iOuter_)) {

    ++iOuter_;

    // Avoid dereferencing endOuter_
    if (iOuter_ == endOuter_) {
      break;
    }

    iInner_ = AccessPolicy::begin(*iOuter_);
  }
}

/**
 * @class STLAccessPolicy
 * @brief Policy class that tells flat_iterator how to access the start/end
 *        of a writable STL container (list, vector, etc.)
 */
template <typename Container>
class STLAccessPolicy {
  public:
    static typename Container::iterator begin(Container& c)
    { return c.begin(); }
    static typename Container::iterator end(Container& c)
    { return c.end(); }
};

/**
 * @class ConstSTLAccessPolicy
 * @brief Policy class that tells flat_iterator how to access the start/end
 *        of a read-only STL container (list, vector, etc.)
 */
template <typename Container>
class ConstSTLAccessPolicy {
  public:
    static typename Container::const_iterator begin(const Container& c)
    { return c.begin(); }
    static typename Container::const_iterator end(const Container& c)
    { return c.end(); }
};

#endif // DATACLASSES_ITERATOR_FLATITERATOR_H_INCLUDED

