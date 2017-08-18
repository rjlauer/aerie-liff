/*!
 * @file BaggableVector.h 
 * @brief A vector that extends Baggable.
 * @author Jim Braun 
 * @date 13 Jul 2012 
 * @version $Id: BaggableVector.h 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#ifndef HAWCNEST_BAGGABLE_VECTOR_H_INCLUDED
#define HAWCNEST_BAGGABLE_VECTOR_H_INCLUDED

#include <hawcnest/processing/Bag.h>

#include <vector>

/*!
 * @class BaggableVector
 * @author Jim Braun
 * @ingroup hawcnest_api
 * @brief std::vector wrapper that extends Baggable
 */

template <typename T, typename Allocator = std::allocator<T> >
class BaggableVector : public Baggable {

  public:

    typedef typename std::vector<T, Allocator>::pointer pointer;
    typedef typename std::vector<T, Allocator>::const_pointer const_pointer;
    typedef typename std::vector<T, Allocator>::reference reference;
    typedef typename std::vector<T, Allocator>::const_reference
                                                           const_reference;
    typedef typename std::vector<T, Allocator>::iterator iterator;
    typedef typename std::vector<T, Allocator>::const_iterator const_iterator;
    typedef typename std::vector<T, Allocator>::reverse_iterator
                                                          reverse_iterator;
    typedef typename std::vector<T, Allocator>::const_reverse_iterator
                                                       const_reverse_iterator;
    typedef typename std::vector<T, Allocator>::allocator_type
                                                            allocator_type;


    BaggableVector(const Allocator& alloc = Allocator()) : data_(alloc) { }

    BaggableVector(const size_t size,
                   const T& value = T(),
                   const Allocator& alloc = Allocator()) :
                                        data_(size, value, alloc) { }

    template <class InputIterator>
    BaggableVector(InputIterator first,
                   InputIterator last,
                   const Allocator& alloc = Allocator()) :
                                        data_(first, last, alloc) { }

    BaggableVector(const std::vector<T, Allocator>& data) : data_(data) { }

    BaggableVector(const BaggableVector<T, Allocator>& vec) :
                                                  data_(vec.data_) { }

    virtual ~BaggableVector() { }

    size_t size() const {return data_.size();}

    void reserve(const size_t size) {data_.reserve(size);}

    void clear() {data_.clear();}

    reference at(const size_t pos) {return data_[pos];}
    const_reference at(const size_t pos) const {return data_[pos];}

    reference operator[](const size_t pos) {return at(pos);}
    const_reference operator[](const size_t pos) const {return at(pos);}

    iterator begin() {return data_.begin();}
    const_iterator begin() const {return data_.begin();}

    iterator end() {return data_.end();}
    const_iterator end() const {return data_.end();}

    reverse_iterator rbegin() {return data_.rbegin();}
    const_reverse_iterator rbegin() const {return data_.rbegin();}

    reverse_iterator rend() {return data_.rend();}
    const_reverse_iterator rend() const {return data_.rend();}

    void push_back(const T& t) {data_.push_back(t);}
    void pop_back() {data_.pop_back();}

    size_t capacity() const {return data_.capacity();}

    BaggableVector<T, Allocator>&
    operator=(const BaggableVector<T, Allocator>& vec) {
      data_ = vec.data_;
      return *this;
    }

    void resize(const size_t n, T t = T()) {data_.resize(n, t);}

    void swap(BaggableVector& vec) {data_.swap(vec.data_);}

    size_t max_size() const {return data_.max_size();}

    iterator insert(iterator pos, const T& t) {data_.insert(pos, t);}
    void insert(iterator pos, const size_t n, const T& t ) {
      data_.insert(pos, n, t);
    }

    template <class InputIterator>
    void insert(iterator pos, InputIterator first, InputIterator last) {
      data_.insert(pos, first, last);
    }

    allocator_type get_allocator() const {return data_.get_allocator();}

    reference front() {return data_.front();}
    const_reference front() const {return data_.front();}

    reference back() {return data_.back();}
    const_reference back() const {return data_.back();}

    iterator erase(iterator pos) {return data_.erase(pos);}
    iterator erase(iterator first, iterator last) {
      return data_.erase(first, last);
    }

    bool empty() const {return data_.empty();}

    template <class InputIterator>
    void assign(InputIterator first, InputIterator last) {
      data_.assign(first, last);
    }
    void assign(const size_t n, const T& t) {data_.assign(n, t);}

  protected:

    std::vector<T, Allocator> data_;
};

#endif // HAWCNEST_BAGGABLE_VECTOR_H_INCLUDED
