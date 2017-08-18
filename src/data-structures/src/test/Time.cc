/*!
 * @file Time.cc 
 * @brief Unit tests for the time-based data structures.
 * @author Segev BenZvi 
 * @date 27 Jan 2012 
 * @ingroup unit_test
 * @version $Id: Time.cc 19479 2014-04-12 17:04:53Z sybenzvi $
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/output_test_stream.hpp>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/test/OutputConfig.h>

#include <data-structures/time/TimeStamp.h>
#include <data-structures/time/UTCDateTime.h>
#include <data-structures/time/ModifiedJulianDate.h>
#include <data-structures/time/LeapSeconds.h>

#include <cstdlib>

using namespace HAWCUnits;
using boost::test_tools::output_test_stream;
using namespace std;


BOOST_AUTO_TEST_SUITE(TimeTest)

  // Global logger settings.
  // Note: this will affect all test suites in the data-structures project
  BOOST_GLOBAL_FIXTURE(OutputConfig);

  //____________________________________________________________________________
  // Check leap second calculations
  BOOST_AUTO_TEST_CASE(LeapSecond)
  {
    UTCDateTime u;
    int nl;

    // Before first GPS leap second
    u.SetDateTime(1981,6,30, 23,59,59);
    nl = LeapSeconds::GetInstance().GetLeapSeconds(u.GetUnixSecond());
    BOOST_CHECK_EQUAL(nl, 0); 

    // During first GPS leap second
    u.SetDateTime(1981,6,30, 23,59,60);
    nl = LeapSeconds::GetInstance().GetLeapSeconds(u.GetUnixSecond());
    BOOST_CHECK_EQUAL(nl, 1); 

    // After first GPS leap second
    u.SetDateTime(1981,7,1, 0,0,0);
    nl = LeapSeconds::GetInstance().GetLeapSeconds(u.GetUnixSecond());
    BOOST_CHECK_EQUAL(nl, 1); 

    // Valentine's Day 1992
    u.SetDateTime(1992,2,14, 3,14,15);
    nl = LeapSeconds::GetInstance().GetLeapSeconds(u.GetUnixSecond());
    BOOST_CHECK_EQUAL(nl, 7); 

    // Bastille Day 1997
    u.SetDateTime(1997,7,14, 15,30,20);
    nl = LeapSeconds::GetInstance().GetLeapSeconds(u.GetUnixSecond());
    BOOST_CHECK_EQUAL(nl, 12); 

    // Dia de la Independencia Mexicana 2001
    u.SetDateTime(2001,9,16, 6,10,11);
    nl = LeapSeconds::GetInstance().GetLeapSeconds(u.GetUnixSecond());
    BOOST_CHECK_EQUAL(nl, 13); 

    // Dahlia's birthday
    u.SetDateTime(2009,3,30, 22,17,0);
    nl = LeapSeconds::GetInstance().GetLeapSeconds(u.GetUnixSecond());
    BOOST_CHECK_EQUAL(nl, 15); 

    // USA Independence Day, 2012
    u.SetDateTime(2012,7,4, 9,0,0);
    nl = LeapSeconds::GetInstance().GetLeapSeconds(u.GetUnixSecond());
    BOOST_CHECK_EQUAL(nl, 16); 
  }

  //____________________________________________________________________________
  // Check conversion of MJD to and from different time systems
  BOOST_AUTO_TEST_CASE(GPSTime)
  {
    TimeStamp t;

    // Start of the GPS epoch
    t.SetGPSTime(0);

    UTCDateTime u(t);
    BOOST_CHECK(u.GetYear() == 1980 &&
                u.GetMonth() == 1 &&
                u.GetDay() == 6 &&
                u.GetHour() == 0 &&
                u.GetMinute() == 0 &&
                u.GetSecond() == 0);

    // Just before the July 1981 leap second: 1981-06-30 23:59:59
    t.SetGPSTime(46828799);
    u = UTCDateTime(t);

    BOOST_CHECK(u.GetYear() == 1981 &&
                u.GetMonth() == 6 &&
                u.GetDay() == 30 &&
                u.GetHour() == 23 &&
                u.GetMinute() == 59 &&
                u.GetSecond() == 59);

    // During the July 1981 leap second: 1981-06-30 23:59:60
    t.SetGPSTime(46828800);
    u = UTCDateTime(t);

    BOOST_CHECK(u.GetYear() == 1981 &&
                u.GetMonth() == 6 &&
                u.GetDay() == 30 &&
                u.GetHour() == 23 &&
                u.GetMinute() == 59 &&
                u.GetSecond() == 60);

    // After the July 1981 leap second: 1981-07-01 00:00:00
    t.SetGPSTime(46828801);
    u = UTCDateTime(t);

    BOOST_CHECK(u.GetYear() == 1981 &&
                u.GetMonth() == 7 &&
                u.GetDay() == 1 &&
                u.GetHour() == 0 &&
                u.GetMinute() == 0 &&
                u.GetSecond() == 0);

    // The billionth GPS second:
    t.SetGPSTime(1000000000);
    u = UTCDateTime(t);

    BOOST_CHECK(u.GetYear() == 2011 &&
                u.GetMonth() == 9 &&
                u.GetDay() == 14 &&
                u.GetHour() == 1 &&
                u.GetMinute() == 46 &&
                u.GetSecond() == 25);

    // Streaming
    output_test_stream output;
    output << t;

    string ptrn("1000000000 s");
    BOOST_CHECK(output.is_equal(ptrn));
  }

  //____________________________________________________________________________
  // Check time intervals between TimeStamp objects
  BOOST_AUTO_TEST_CASE(TimeIntervals)
  {
    TimeStamp t1;
    TimeStamp t2;

    t1.SetGPSTime(900000000, 999999900);
    t2.SetGPSTime(900000001, 10);

    TimeInterval dt = t2 - t1;
    BOOST_CHECK_EQUAL(dt.GetSecond(), 0);
    BOOST_CHECK_EQUAL(dt.GetNanoSecond(), 110u);
    BOOST_CHECK_EQUAL(dt.GetInterval(), 110*nanosecond);

    dt = TimeInterval(10*day);
    t2 += dt;
    BOOST_CHECK_EQUAL(t2.GetGPSSecond(), 900864001u);

    TimeInterval ti(2*day);
    BOOST_CHECK(ti < dt);
    BOOST_CHECK(dt > ti);
    BOOST_CHECK(dt >= dt);
    BOOST_CHECK(ti*5. == dt);
    BOOST_CHECK(ti == dt/5.);

    dt -= ti;
    BOOST_CHECK_EQUAL(dt.GetInterval(), 8*day);
    dt += ti;
    BOOST_CHECK_EQUAL(dt.GetInterval(), 10*day);
  }

  //____________________________________________________________________________
  // Check conversion of MJD to and from different time systems
  BOOST_AUTO_TEST_CASE(MJDConversions)
  {
    // UTC to MJD
    UTCDateTime utc(2010,6,21, 2,24,0);
    ModifiedJulianDate mjd(utc);

    BOOST_CHECK_EQUAL(mjd.GetDate(), 55368.1*day);

    // MJD back to UTC
    UTCDateTime fromMJD = mjd.GetUTCDateTime();

    BOOST_CHECK(fromMJD.GetYear() == 2010 &&
                fromMJD.GetMonth() == 6 &&
                fromMJD.GetDay() == 21 &&
                fromMJD.GetHour() == 2 &&
                fromMJD.GetMinute() == 24 &&
                fromMJD.GetSecond() == 0 &&
                fromMJD.GetNanoSecond() == 0);

    // MJD expressed in atomic time (TAI)
    // TAI is offset from UTC by 19s (GPS offset) + leap seconds since 1980
    int nl = LeapSeconds::GetInstance().GetLeapSeconds(fromMJD.GetUnixSecond());
    BOOST_CHECK_EQUAL(nl, 15);
    BOOST_CHECK_EQUAL(mjd.GetDate(TAI), 55368.1*day + (19 + nl)*second);

    // MJD expressed in ephemeris/terrestrial time (TT)
    // TT is offset from UTC by 32.184s + 19s (GPS offset) + leaps since 1980
    BOOST_CHECK_EQUAL(mjd.GetDate(TT), 55368.1*day + (32.184 + 19 + nl)*second);

    // GPS to MJD
    TimeStamp ts(1015718415);
    mjd = ModifiedJulianDate(ts);

    BOOST_CHECK_EQUAL(mjd.GetDate(), 56000*day);

    // MJD to GPS
    BOOST_CHECK_EQUAL(mjd.GetTimeStamp(), ts);

    // Generate 100000 random TimeStamps between 1 and 1700000000 and check that
    // conversions from/to TimeStamp preserve the time (helps find roundoffs)
    srand48(12345);

    for (int i = 0; i < 100000; ++i) {
      int gpsSec = lround(1 + 1699999999*drand48());
      TimeStamp t(gpsSec);
      UTCDateTime u(t);
      ModifiedJulianDate m(t);

      BOOST_CHECK_EQUAL(m.GetTimeStamp(), t);
    }
  }

BOOST_AUTO_TEST_SUITE_END()

