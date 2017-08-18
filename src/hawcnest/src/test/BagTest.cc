/*!
 * @file BagTest.cc 
 * @brief Unit test for the Bag class.
 * @author Segev BenZvi 
 * @date 18 Jan 2012 
 * @ingroup unit_test
 * @version $Id: BagTest.cc 18844 2014-02-18 01:42:07Z sybenzvi $
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

#include <hawcnest/processing/Bag.h>

using boost::test_tools::output_test_stream;
using namespace std;

// Dummy class for testing Baggable objects
class TestData : public Baggable {
  public:
    void go() const { }
};

SHARED_POINTER_TYPEDEFS(TestData);

// Create the "BagTest" testing suite
BOOST_AUTO_TEST_SUITE(BagTest)

  // ___________________________________________________________________________
  // Check shared_ptr Put and shared_ptr/const reference Get
  BOOST_AUTO_TEST_CASE(PutAndGet)
  {
    TestDataPtr testData = boost::make_shared<TestData>();
    TestData* testDataPtr = testData.get();

    Bag b;
    b.Put("testData", testData);

    TestDataConstPtr out = b.Get<TestDataConstPtr>("testData");
    BOOST_CHECK(out);

    const TestData& out2 = b.Get<TestData>("testData");
    BOOST_CHECK(&out2 == testDataPtr);
  }

  // ___________________________________________________________________________
  // Check if the Exists method works
  BOOST_AUTO_TEST_CASE(ExistMethod)
  {
    Bag b;

    BOOST_CHECK(!b.Exists<TestData>("testData"));
    BOOST_CHECK(!b.Exists("testData"));

    TestDataPtr testData = boost::make_shared<TestData>();
    b.Put("testData", testData);

    BOOST_CHECK(b.Exists<TestData>("testData"));
    BOOST_CHECK(b.Exists("testData"));
  }

  // ___________________________________________________________________________
  // Check that exceptions are raised when getting nonexistent data
  BOOST_AUTO_TEST_CASE(GetNonExistent)
  {
    Bag b;

    BOOST_CHECK_THROW(b.Get<TestData>("foo"), bag_exception);
    BOOST_CHECK(!b.Get<TestDataConstPtr>("foo"));
  }

  // ___________________________________________________________________________
  // Check that objects in the Bag have to have different names
  BOOST_AUTO_TEST_CASE(PutTwice)
  {
    Bag b;

    TestDataPtr testData = boost::make_shared<TestData>();

    BOOST_CHECK_NO_THROW(b.Put("testData", testData));
    BOOST_CHECK_THROW(b.Put("testData", boost::make_shared<TestData>()),
                      bag_exception);
  }

  // ___________________________________________________________________________
  // Check that contents in the Bag can be printed
  BOOST_AUTO_TEST_CASE(PrintBag)
  {
    Bag b;

    TestDataPtr testData = boost::make_shared<TestData>();

    b.Put("test_a", testData);
    b.Put("test_b", testData);
    b.Put("test_c", testData);
    b.Put("test_d", testData);

    output_test_stream output;
    output << b;

    // Check that stream insertion matches this pattern
    string ptrn("bag members:\n"
                "  \"test_a\" => <TestData>\n"
                "  \"test_b\" => <TestData>\n"
                "  \"test_c\" => <TestData>\n"
                "  \"test_d\" => <TestData>");

    BOOST_CHECK(output.is_equal(ptrn));
  }

BOOST_AUTO_TEST_SUITE_END()

