/*!
 * @file TabulatedFunction-inl.h
 * @brief Implementation of TabulatedFunction calculations.
 * @author Segev BenZvi
 * @date 25 Dec 2010
 * @version $Id: TabulatedFunction-inl.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_MATH_TABULATEDFUNCTION_INL_H_INCLUDED
#define DATACLASSES_MATH_TABULATEDFUNCTION_INL_H_INCLUDED

#include <hawcnest/Logging.h>

template<class T>
typename TabulatedFunction<T>::ConstIterator
TabulatedFunction<T>::LowerBound(const T& x) const
{
  size_t len = std::distance(Begin(), End());
  size_t half;

  ConstIterator iFirst = Begin();
  ConstIterator iMid;

  while (len > 0) {
    half = len >> 1;
    iMid = iFirst;
    advance(iMid, half);
    if (x >= iMid->GetX()) {
      iFirst = iMid;
      ++iFirst;
      len = len - half - 1;
    }
    else 
      len = half;
  }
  return iFirst;
}

template<class T>
typename TabulatedFunction<T>::ConstIterator
TabulatedFunction<T>::UpperBound(const T& x) const
{
  size_t len = std::distance(Begin(), End());
  size_t half;

  ConstIterator iFirst = Begin();
  ConstIterator iMid;

  while (len > 0) {
    half = len >> 1;
    iMid = iFirst;
    advance(iMid, half);
    if (x < iMid->GetX())
      len = half;
    else {
      iFirst = iMid;
      ++iFirst;
      len = len - half - 1;
    }
  }
  return iFirst;
}

template<class T>
T
TabulatedFunction<T>::Evaluate(const T& x) const
{
  if (IsEmpty())
    log_fatal("Tabulated function is empty");

  ConstIterator iF = Begin();
  ConstIterator iB = End() - 1;

  if (GetN() == 1)
    return iF->GetY();

  // Handle cases at the boundaries
  if (x < iF->GetX() || x > iB->GetX()) {
    if (x < iF->GetX()) {
      ConstIterator i0 = iF;
      ConstIterator i1 = iF + 1;
      if (float(i0->GetX() - x) / (i1->GetX() - i0->GetX()) < 1e-3)
        return i0->GetY();
    }
    else {
      ConstIterator i0 = iB;
      ConstIterator i1 = iB - 1;
      if (float(x - i0->GetX()) / (i0->GetX() - i1->GetX()) < 1e-3)
        return i0->GetY();
    }
    log_fatal("Argument " << x << " is out of range ["
                          << iF->GetX() << ", " << iB->GetX() << "]");
  }

  ConstIterator iC = UpperBound(x);
  --iC;
  if (iC->GetX() == x)
    return iC->GetY();

  const T& x1 = iC->GetX();
  const T& y1 = iC->GetY();
  ++iC;
  const T& x2 = iC->GetX();
  const T& y2 = iC->GetY();

  return y1 + (y2-y1)/(x2-x1) * (x-x1);
}

#endif // DATACLASSES_MATH_TABULATEDFUNCTION_INL_H_INCLUDED

