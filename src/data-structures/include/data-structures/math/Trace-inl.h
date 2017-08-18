/*!
 * @file Trace-inl.h
 * @brief Implementation of Trace functions.
 * @author Segev BenZvi
 * @date 6 Feb 2012
 * @version $Id: Trace-inl.h 13185 2012-11-10 15:46:03Z sybenzvi $
 */

#ifndef DATACLASSES_MATH_TRACE_INL_H_INCLUDED
#define DATACLASSES_MATH_TRACE_INL_H_INCLUDED

template<typename T, size_t N>
Trace<T,N>&
Trace<T,N>::operator*=(const T& x)
{
  std::transform(signal_, signal_ + N, signal_, 
                 std::bind2nd(std::multiplies<T>(), x));
  return *this;
}

template<typename T, size_t N>
Trace<T,N>&
Trace<T,N>::operator+=(const T& x)
{
  std::transform(signal_, signal_ + N, signal_, 
                 std::bind2nd(std::plus<T>(), x));
  return *this;
}

template<typename T, size_t N>
Trace<T,N>&
Trace<T,N>::operator-=(const T& x)
{
  std::transform(signal_, signal_ + N, signal_, 
                 std::bind2nd(std::minus<T>(), x));
  return *this;
}

template<typename T, size_t N>
Trace<T,N>&
Trace<T,N>::operator/=(const T& x)
{
  std::transform(signal_, signal_ + N, signal_, 
                 std::bind2nd(std::divides<T>(), x));
  return *this;
}

/// Calculate the absolute value of all quantities in a Trace
template<typename T, size_t N>
Trace<T,N>
abs(const Trace<T,N>& t)
{
  Trace<T,N> r = t;
  std::transform(&t[0], &t[0] + N, &r[0], std::ptr_fun<T,T>(std::abs));
  return r;
}

/// Exponentiate a Trace to some power e
template<typename T, size_t N>
Trace<T,N>
pow(const Trace<T,N>& t, const T& e)
{
  Trace<T,N> r = t;
  std::transform(&t[0], &t[0] + N, &r[0],
                 std::bind2nd(std::ptr_fun<T,T,T>(std::pow), e));
  return r;
}

/// Exponentiate a base b to all values inside the Trace
template<typename T, size_t N>
Trace<T,N>
pow(const T& b, const Trace<T,N>& t)
{
  Trace<T,N> r = t;
  std::transform(&t[0], &t[0] + N, &r[0],
                 std::bind1st(std::ptr_fun<T,T,T>(std::pow), b));
  return r;
}

/// Exponentiate a Trace
template<typename T, size_t N>
Trace<T,N>
exp(const Trace<T,N>& t)
{
  Trace<T,N> r = t;
  std::transform(&t[0], &t[0] + N, &r[0], std::ptr_fun<T,T>(std::exp));
  return r;
}

/// Find the natural logarithm of a trace
template<typename T, size_t N>
Trace<T,N>
log(const Trace<T,N>& t)
{
  Trace<T,N> r = t;
  std::transform(&t[0], &t[0] + N, &r[0], std::ptr_fun<T,T>(std::log));
  return r;
}

/// Find the base-10 logarithm of a trace
template<typename T, size_t N>
Trace<T,N>
log10(const Trace<T,N>& t)
{
  Trace<T,N> r = t;
  std::transform(&t[0], &t[0] + N, &r[0], std::ptr_fun<T,T>(std::log10));
  return r;
}

/// Find the square root of a trace
template<typename T, size_t N>
Trace<T,N>
sqrt(const Trace<T,N>& t)
{
  Trace<T,N> r = t;
  std::transform(&t[0], &t[0] + N, &r[0], std::ptr_fun<T,T>(std::sqrt));
  return r;
}

#endif // DATACLASSES_MATH_TRACE_INL_H_INCLUDED

