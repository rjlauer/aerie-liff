/*!
 * @file SelectionIterator.h
 * @brief Iterator that applies a selection function (or functor)
 * @author Jim Braun
 * @version $Id: SelectionIterator.h 25538 2015-05-31 01:13:31Z sybenzvi $
 */

#ifndef DATACLASSES_ITERATOR_SELECTIONITERATOR_H_INCLUDED
#define DATACLASSES_ITERATOR_SELECTIONITERATOR_H_INCLUDED

#include <cassert>

/*!
 * @class SelectionIterator
 * @author Jim Braun
 * @ingroup design_patterns
 * @brief Iterate over only elements where a given comparison evaluates true
 *
 * @tparam InputIterator Iterator type
 * @tparam Compare Unary comparison function specific for iterator type
 */
template <typename InputIterator, typename Compare>
class selection_iterator :
  public std::iterator<std::input_iterator_tag, ptrdiff_t> {

  public:

    selection_iterator(const InputIterator start,
                       const InputIterator end,
                       const Compare compare) : iElement_(start),
                                                lastElement_(end),
                                                compare_(compare) {

      NextElement();
    }

    typename InputIterator::reference operator*() {return *iElement_;}
    const typename InputIterator::reference operator*() const {
      return *iElement_;
    }

    typename InputIterator::pointer operator->() {return &(*iElement_);}
    const typename InputIterator::pointer operator->() const {
      return &(*iElement_);
    }

    bool operator==(const selection_iterator& s) const {
      return s.lastElement_ ==  lastElement_ &&
             s.iElement_ == iElement_;
    }

    bool operator!=(const selection_iterator& s) const {return !(*this == s);}

    selection_iterator& operator++() {

      // Check bounds (debug mode only)
      assert(iElement_ != lastElement_);

      ++iElement_;
      NextElement();
      return *this;
    }

  private:

    void NextElement() {
      iElement_ = find_if(iElement_, lastElement_, compare_);
    }

    InputIterator  iElement_;
    InputIterator  lastElement_;
    const Compare  compare_;
};

#endif // DATACLASSES_ITERATOR_SELECTIONITERATOR_H_INCLUDED
