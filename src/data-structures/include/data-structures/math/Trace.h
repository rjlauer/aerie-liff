/*!
 * @file Trace-inl.h
 * @brief Representation of evenly-sampled 1D traces, e.g., a scope trace.
 * @author Segev BenZvi
 * @date 6 Feb 2012
 * @version $Id: Trace.h 13185 2012-11-10 15:46:03Z sybenzvi $
 */

#ifndef DATACLASSES_MATH_TRACE_H_INCLUDED
#define DATACLASSES_MATH_TRACE_H_INCLUDED

#include <hawcnest/processing/Bag.h>

#include <cmath>
#include <algorithm>
#include <functional>
#include <numeric>

/*!
 * @class Trace
 * @author Segev BenZvi
 * @date 6 Feb 2012
 * @tparam T Type of the container (float, double, int, etc.)
 * @tparam N Size of the container
 * @brief A simple container for signal traces with some overloaded arithmetic
 *        operations.
 *
 * This is a container meant to encapsulate evenly-sampled 1D traces.  Imagine
 * something like an FADC time series sampled with a constant rate.  The data
 * type and the size of the container are template parameters.
 *
 * Various arithmetic and transcendental operations are overloaded for the
 * Trace, e.g., pow, exp, log, etc.  Other convenience functions for
 * calculating integrals, minima, and maxima are provided.
 */
template<typename T, size_t N>
class Trace : public Baggable {

  public:

    Trace() : t0_(0.), dt_(0.)
    { Reset(); }

    Trace(const double dt) : t0_(0.), dt_(dt)
    { Reset(); }

    Trace(const double t0, const double dt) : t0_(t0), dt_(dt)
    { Reset(); }

    /// Number of points in the Trace
    size_t GetSize() const { return N; }

    /// Time binning
    double GetDt() const { return dt_; }

    /// Set the time binning
    void SetDt(const double dt) { dt_ = dt; }

    /// Get the start time
    double GetT0() const { return t0_; }

    /// Set the start time
    void SetT0(const double t0) { t0_ = t0; }

    /// Get the end time
    double GetT1() const { return t0_ + N*dt_; }

    /// Get the time of bin i
    double GetT(const int i) { return t0_ + i*dt_; }

    /// Get the minimum value
    T Minimum() const { return *std::min_element(signal_, signal_ + N); }

    /// Get the location of the minimum value
    size_t MinimumBin() const
    { return std::min_element(signal_, signal_ + N) - signal_; }

    /// Get the maximum value
    T Maximum() const { return *std::max_element(signal_, signal_ + N); }

    /// Get the location of the maximum value
    size_t MaximumBin() const
    { return std::max_element(signal_, signal_ + N) - signal_; }

    /// Set all the values in the Trace to some constant
    void Reset(const T& val=0)
    { std::fill_n(signal_, N, val); }

    /// L-value random access to the Trace
    T& operator[](const int i) { return signal_[i]; }

    /// R-value read-only access to the trace
    const T& operator[](const int i) const { return signal_[i]; }

    /// Integrate the trace (simple Riemann sum)
    T Integral() const
    { return std::accumulate(signal_, signal_ + N, T(0)) * dt_; }

    /// In-place multiplication by a constant
    Trace& operator*=(const T& x);

    /// In-place addition with a constant
    Trace& operator+=(const T& x);

    /// In-place subtraction of a constant
    Trace& operator-=(const T& x);

    /// In-place division by a constant
    Trace& operator/=(const T& x);

    typedef std::pair<double,T> PT;
    
    /// An internal iterator class that calculates (time,signal) pairs
    class iterator;
    class iterator : public std::iterator<std::forward_iterator_tag, PT> {

      private:

        const T* beg_;    ///< Pointer to the start of the Trace
        T* ptr_;          ///< Pointer to current position in the Trace
        double dt_;       ///< Time binning of the Trace
        PT tup_;          ///< A (time,signal) tuple used for output

        void set_tuple() {
          size_t i = ptr_ - beg_; 
          tup_.first = i*dt_;
          tup_.second = *ptr_;
        }
      
      public:

        iterator() : beg_(NULL), ptr_(NULL), dt_(0) { }
        iterator(T* b, T* p, double dt) : beg_(b), ptr_(p), dt_(dt) { }
        iterator(const iterator& i) : beg_(i.beg_), ptr_(i.ptr_), dt_(i.dt_) { }

        iterator& operator++() { ++ptr_; return *this; }
        iterator operator++(int) { iterator t(*this); operator++(); return t; }

        bool operator==(const iterator& i) { return ptr_ == i.ptr_; }
        bool operator!=(const iterator& i) { return !(*this == i); }

        PT& operator*() { set_tuple(); return tup_; }
        PT* operator->() { set_tuple(); return &tup_; }

    };

    /// Iterator to the start of the sequence (read-write)
    iterator begin() { return iterator(signal_, signal_, dt_); }

    /// Iterator to the end of the sequence (read-write)
    iterator end() { return iterator(signal_, signal_ + N, dt_); }

  private:

    double t0_;     ///< Start time
    double dt_;     ///< Time binning
    T signal_[N];   ///< Trace values

};

#include <data-structures/math/Trace-inl.h>

#endif // DATACLASSES_MATH_TRACE_H_INCLUDED

