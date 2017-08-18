/*!
 * @file TabulatedFunction.h
 * @brief Tables of x-y data, with basic interpolation
 * @author Segev BenZvi
 * @date 25 Dec 2010
 * @version $Id: TabulatedFunction.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_MATH_TABULATEDFUNCTION_H_INCLUDED
#define DATACLASSES_MATH_TABULATEDFUNCTION_H_INCLUDED

#include <algorithm>
#include <functional>
#include <vector>

/*!
 * @class XYPair
 * @ingroup math
 * @author Segev BenZvi
 * @date 25 Dec 2010
 * @brief Storage for a coordinate pair with abscissa x and ordinate y
 *
 * Note: XYPairs are sortable by the abscissa via the implementation of the
 * operator<() member function.
 */
template<class T>
class XYPair {

  public:

    XYPair(): x_(0), y_(0) { }
    XYPair(const T& x, const T& y) : x_(x), y_(y) { }
    XYPair(const XYPair& p) : x_(p.x_), y_(p.y_) { }
   ~XYPair() { }

    const T& GetX() const { return x_; }
    const T& GetY() const { return y_; }

    bool operator<(const XYPair& c) const { return x_ < c.x_; }

  private:

    T x_;
    T y_;

};

/*!
 * @class CompareOrdinate
 * @ingroup math
 * @author Segev BenZvi
 * @date 25 Dec 2010
 * @brief Allow comparison of XYPairs via the ordinate y rather than the
 *        abscissa x.  Useful for sorting.
 */
template<class T>
class CompareOrdinate : public std::binary_function<XYPair<T>, XYPair<T>, bool> {

  public:

    bool operator()(const XYPair<T>& c1, const XYPair<T>& c2) const {
      return c1.GetY() < c2.GetY();
    }

};

/*!
 * @class TabulatedFunction
 * @ingroup math
 * @date 25 Dec 2010
 * @author Luis Prado, Jr
 * @author Segev BenZvi
 * @brief Storage table for x-y data, with simple interpolation functions
 *
 * Based on the TabulatedFunction class written by Luis Prado for the Auger
 * Offline software.  As of this writing, the Auger version implements two
 * vectors of x and y data.  This class implements a single vector of
 * XYPairs to simplify sorting and searching.
 *
 * @todo Implement polynomial/cubic spline interpolation
 */
template<class T>
class TabulatedFunction {

  private:

    typedef typename std::vector<XYPair<T> > Table;

  public:

    typedef typename Table::iterator Iterator;
    typedef typename Table::const_iterator ConstIterator;

    TabulatedFunction() { }

    TabulatedFunction(const TabulatedFunction<T>& tf) : coords_(tf.coords_) { }

    virtual ~TabulatedFunction() { }

    /// Remove all XYPairs from the storage table
    void Clear() { coords_.clear(); }

    /// Push a new XYPair into the storage table
    void PushBack(const XYPair<T>& c)
    { coords_.push_back(c); }

    /// Push an abscissa x and an ordinate y into the table
    void PushBack(const T& x, const T& y)
    { coords_.push_back(XYPair<T>(x, y)); }

    /// Read-only access to the first element in the table
    const XYPair<T>& Front() const
    { return coords_.front(); }

    /// Read-only access to the last element in the table
    const XYPair<T>& Back() const
    { return coords_.back(); }

    /// Sort the table of XYPairs according to the abscissa (x)
    void Sort()
    { std::sort(coords_.begin(), coords_.end()); }

    /// Sort the table of XYPairs according to the ordinate (y)
    void SortByOrdinate()
    { std::sort(coords_.begin(), coords_.end(), CompareOrdinate<T>()); }

    /// Check to see if the table is empty
    bool IsEmpty() const { return coords_.empty(); }

    /// Return the number of XYPairs in the table
    size_t GetN() const { return coords_.size(); }

    /// Read-write iterator to the beginning of the XYPair table
    Iterator Begin() { return coords_.begin(); }
    Iterator End() { return coords_.end(); }

    /// Read-only iterator to the beginning of the XYPair table
    ConstIterator Begin() const { return coords_.begin(); }
    ConstIterator End() const { return coords_.end(); }

    /// Return the first position where an XYPair with abscissa x could be
    /// inserted in the table without violating the ordering of the XYPairs
    ConstIterator LowerBound(const T& x) const;

    /// Return the last position where an XYPair with abscissa x could be
    /// inserted in the table without violating the ordering of the XYPairs
    ConstIterator UpperBound(const T& x) const;

    /// Evaluate the function at position x using linear interpolation, with 
    /// simple range checking
    T Evaluate(const T& x) const;

  private:

    Table coords_;

};

#include <data-structures/math/TabulatedFunction-inl.h>

#endif // DATACLASSES_MATH_TABULATEDFUNCTION_H_INCLUDED

