/*!
 * @file Test.h 
 * @brief Define various useful predicate function objects for testing.
 * @author Lukas Nellen
 * @author Darko Veberic
 * @author Segev BenZvi 
 * @date 13 Jan 2012 
 * @version $Id: Test.h 18844 2014-02-18 01:42:07Z sybenzvi $
 */

#ifndef HAWCNEST_TEST_TEST_H_INCLUDED
#define HAWCNEST_TEST_TEST_H_INCLUDED

/*!
 * @file Test.h
 * @author Lukas Nellen
 * @author Darko Veberic
 * @date 8 Feb 2004
 */

#include <cmath>

/*!
 * @class IsSpace
 * @ingroup aerie_test
 * @brief Predicate useful for whitespace searching
 */
class IsSpace {
  public:
    bool operator()(const char x) const
    { return x == ' ' || x == '\r' || x == '\n' || x == '\t'; }
};

/*!
 * @class Equal
 * @ingroup aerie_test
 * @brief Predicate to search for equality between two values
 */
class Equal {
  public:
    template<typename T>
    bool operator()(const T& l, const T& r) const 
    { return l == r; }
};

/*!
 * @class Less
 * @ingroup aerie_test
 * @brief Predicate to search for less-than inequality between two values
 */
class Less {
  public:
    template<typename T>
    bool operator()(const T& l, const T& r) const 
    { return l < r; }
};

/*!
 * @class LessOrEqual
 * @ingroup aerie_test
 * @brief Predicate to search for less-than or equality between two values
 */
class LessOrEqual {
  public:
    template<typename T>
    bool operator()(const T& l, const T& r) const 
    { return l <= r; }
};

/*!
 * @class Greater
 * @ingroup aerie_test
 * @brief Predicate to search for greater-than inequality between two values
 */
class Greater {
  public:
    template<typename T>
    bool operator()(const T& l, const T& r) const 
    { return l > r; }
};

/*!
 * @class GreaterOrEqual
 * @ingroup aerie_test
 * @brief Predicate to search for greater-than or equality between two values
 */
class GreaterOrEqual {
  public:
    template<typename T>
    bool operator()(const T& l, const T& r) const 
    { return l >= r; }
};

/*!
 * @class CloseTo
 * @ingroup aerie_test
 * @brief Predicate for approximate inequality between floating point values
 *        with a tolerance epsilon
 *
 * This class returns true if the absolute or relative difference between two
 * quantities is less than some epsilon.
 */
class CloseTo {

  public:

    CloseTo(const double eps = 1e-6) : eps_(eps) { }

    template<typename T>
    bool operator()(const T& l, const T& r) const
    { return IsCloseTo(l, r); }

  protected:

    /// Calculate the absolute difference between two quantities
    template<typename T>
    double AbsDiff(const T& l, const T& r) const
    { return std::abs(double(l) - double(r)); }

    /// Check the absolute difference between the quantities
    template<typename T>
    bool IsCloseAbs(const T& l, const T& r) const
    { return AbsDiff<T>(l, r) < eps_; }

    /// Check the relative difference between the quantities
    template<typename T>
    bool IsCloseRel(const T& l, const T& r) const
    { return 2 * AbsDiff<T>(l, r) / std::abs(double(l) + double(r)) < eps_; }

    /// Check to see if the quantities are close in an absolute or relative
    /// sense
    template<typename T>
    bool IsCloseTo(const T& l, const T& r) const {
      if (IsCloseAbs(l, r))
        return true;
      return IsCloseRel(l, r);
    }

    double eps_;    ///< Tolerance for predicate calculation

};

/*!
 * @class CloseAbs
 * @ingroup aerie_test
 * @brief Return true if the absolute difference between two quantities is less
 *        than some epsilon
 */
class CloseAbs : public CloseTo {
  public:
    CloseAbs(const double eps = 1e-6) : CloseTo(eps) { }

    template<typename T>
    bool operator()(const T& l, const T& r) const
    { return IsCloseAbs(l, r); }
};

/*!
 * @class CloseRel
 * @ingroup aerie_test
 * @brief Return true if the relative difference between two quantities is less
 *        than some epsilon
 */
class CloseRel : public CloseTo {
  public:
    CloseRel(const double eps = 1e-6) : CloseTo(eps) { }

    template<typename T>
    bool operator()(const T& l, const T& r) const
    { return IsCloseRel(l, r); }
};

/*!
 * @class Not
 * @ingroup aerie_test
 * @brief Negation predicate for a comparison functor
 * @tparam Predicate Predicate functor that we would like to negate
 */
template<typename Predicate>
class Not : public Predicate {

  public:

    /// Negation of generic functors
    Not() : Predicate() { }

    /// Negation of "CloseTo" functors
    Not(const double eps) : Predicate(eps) { }

    /// Unary functor negation
    template<typename T>
    bool operator()(const T& x) const
    { return !Predicate::operator()(x); }

    /// Binary functor negation
    template<typename T, typename U>
    bool operator()(const T& x, const U& y) const
    { return !Predicate::operator(x, y); }

};

#endif // HAWCNEST_TEST_TEST_H_INCLUDED

