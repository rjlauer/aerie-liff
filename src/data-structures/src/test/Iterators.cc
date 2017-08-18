/*!
 * @file Iterators.cc 
 * @brief Unit test for the iterator data structures.
 * @author Segev BenZvi 
 * @date 5 Feb 2014
 * @ingroup unit_test
 * @version $Id: Iterators.cc 19006 2014-03-08 17:10:22Z sybenzvi $
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/output_test_stream.hpp>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/test/OutputConfig.h>

#include <data-structures/iterator/FlatIterator.h>

#include <iostream>
#include <vector>

using namespace HAWCUnits;
using boost::test_tools::output_test_stream;
using namespace std;

// Just a dummy structure that stores ints
class MyInt {
  public:
    MyInt()      : value_(0) { }
    MyInt(int v) : value_(v) { }
   ~MyInt() { }
    int GetValue() const { return value_; }
    operator int() const { return value_; }
    bool operator==(const MyInt& m) { return value_ == m.value_; }
    bool operator!=(const MyInt& m) { return !(*this == m); }
  private:
    int value_;
  friend ostream& operator<<(ostream& o, const MyInt& m) { o << m; return o; }
};

BOOST_AUTO_TEST_SUITE(IteratorTest)

  //____________________________________________________________________________
  // Check iteration over a nested structure of MyInt
  BOOST_AUTO_TEST_CASE(FlatIterator)
  {
    // Create a nested structure
    typedef vector<MyInt> Inner;
    typedef Inner::const_iterator ConstInnerIterator;

    typedef vector<Inner> Outer;
    typedef Outer::const_iterator ConstOuterIterator;

    int nInner = 5;
    int nOuter = 3;

    Outer vo;

    for (int i = 0; i < nOuter; ++i) {
      Inner vi;
      for (int j = 0; j < nInner; ++j)
        vi.push_back(i*nInner + j);
      vo.push_back(vi);
    }

    // Create a flat iterator
    typedef flat_iterator<ConstOuterIterator,
                          ConstInnerIterator,
                          ConstSTLAccessPolicy<vector<MyInt> > > FlatIterator;

    FlatIterator Begin(vo.begin(), vo.end());
    FlatIterator End(vo.end(), vo.end());

    FlatIterator iF = Begin;
    BOOST_CHECK(iF == Begin);

    iF = End;
    BOOST_CHECK(iF == End);

    // Test dereferencing with the * operator
    int k = 0;
    for (iF = Begin; iF != End; ++iF) {
      BOOST_CHECK_EQUAL(*iF, k++);
    }

    // Test dereferencing with the -> operator
    k = 0;
    for (iF = Begin; iF != End; ++iF) {
      BOOST_CHECK_EQUAL(iF->GetValue(), k++);
    }
  }

BOOST_AUTO_TEST_SUITE_END()

