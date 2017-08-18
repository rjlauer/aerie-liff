/*!
 * @file BaggableSharedPtr.h 
 * @brief Declaration of a Baggable object containing a shared_ptr.
 * @author Jim Braun 
 * @date 13 Nov 2012 
 * @version $Id: BaggableSharedPtr.h 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#ifndef HAWCNEST_BAGGABLE_SHARED_PTR_H_INCLUDED
#define HAWCNEST_BAGGABLE_SHARED_PTR_H_INCLUDED

#include <hawcnest/processing/Bag.h>

#include <boost/shared_ptr.hpp>

/*!
 * @class BaggableSharedPtr
 * @author Jim Braun
 * @ingroup hawcnest_api
 * @brief Baggable object containing a shared_ptr, as a shared_ptr
 * itself cannot be Baggable.  Only most of the shared_ptr interface
 * is implemented.
 */

template <typename T>
class BaggableSharedPtr : public Baggable {

  public:

    typedef T element_type;

    BaggableSharedPtr() { }
    BaggableSharedPtr(T* t) : ptr_(t) { }
    BaggableSharedPtr(const boost::shared_ptr<T>& ptr) : ptr_(ptr) { }

    template <typename Y>
    BaggableSharedPtr(Y* p) : ptr_(p) { }

    template <typename Y, typename D>
    BaggableSharedPtr(Y* p, D d) : ptr_(p, d) { }

    template <typename Y, typename D, typename A>
    BaggableSharedPtr(Y* p, D d, A a) : ptr_(p, d, a) { }

    template <typename Y>
    BaggableSharedPtr(const boost::shared_ptr<Y>& r) : ptr_(r) { }

    template <typename Y>
    BaggableSharedPtr(const boost::shared_ptr<Y>& r, T* p) : ptr_(r, p) { }

    BaggableSharedPtr(const BaggableSharedPtr& b) : ptr_(b.ptr_) { }

    template <typename Y>
    BaggableSharedPtr(const BaggableSharedPtr<Y>& r) : ptr_(r.ptr_) { }

    template <typename Y>
    BaggableSharedPtr(const BaggableSharedPtr<Y>& r, T* p) :
                                                       ptr_(r.ptr_, p) { }

    BaggableSharedPtr& operator=(const boost::shared_ptr<T>& r) {
      ptr_ = r;
      return *this;
    }

    template <typename Y>
    BaggableSharedPtr& operator=(const boost::shared_ptr<Y>& r) {
      ptr_ = r;
      return *this;
    }

    void Reset() {ptr_.reset();}

    template <typename Y>
    void Reset(Y* p) {ptr_.reset(p);}

    template <typename Y, typename D>
    void Reset(Y* p, D d) {ptr_.reset(p, d);}

    template <typename Y, typename D, typename A>
    void Reset(Y* p, D d, A a) {ptr_.reset(p, d, a);}

    template <typename Y>
    void Reset(const boost::shared_ptr<Y>& r, T* p) {ptr_.reset(r, p);}

    template <typename Y>
    void Reset(const BaggableSharedPtr<Y>& r, T* p) {ptr_.reset(r.ptr_, p);}

    T& operator*() const {return *ptr_;}
    T* operator->() const {return Get();}
    T* Get() const {return &(*ptr_);}

    bool Unique() const {return ptr_.unique();}
    bool UseCount() const {return ptr_.use_count();}

    void Swap(boost::shared_ptr<T>& b) {ptr_.swap(b);}
    void Swap(BaggableSharedPtr<T>& b) {ptr_.swap(b.ptr_);}

    // For the operations that are not implemented
    const boost::shared_ptr<T>& Ptr() const {return ptr_;}
    boost::shared_ptr<T>& Ptr() {return ptr_;}

    template <typename U>
    bool operator==(const BaggableSharedPtr<U>& p) {return ptr_ == p.ptr_;}

    template <typename U>
    bool operator!=(const BaggableSharedPtr<U>& p) {return ptr_ != p.ptr_;}

    template <typename U>
    bool operator<(const BaggableSharedPtr<U>& p) {return ptr_ < p.ptr_;}

  private:

    template <typename Y>
    friend class BaggableSharedPtr;

    boost::shared_ptr<T> ptr_;
};

template<class E, class T, class Y>
inline std::basic_ostream<E, T>&
operator<<(std::basic_ostream<E, T> & os, const BaggableSharedPtr<Y>& p) {
  os << p.Ptr();
  return os;
}

#endif // HAWCNEST_BAGGABLE_SHARED_PTR_H_INCLUDED
