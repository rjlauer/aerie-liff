/*!
 * @file Time.cc 
 * @brief Python bindings for time data structures.
 * @author Segev BenZvi 
 * @date 17 Dec 2011 
 * @version $Id: Time.cc 19194 2014-03-26 20:33:16Z sybenzvi $
 */

#include <boost/python.hpp>

#include <data-structures/time/TimeStamp.h>
#include <data-structures/time/TimeInterval.h>
#include <data-structures/time/TimeRange.h>
#include <data-structures/time/UTCDateTime.h>
#include <data-structures/time/ModifiedJulianDate.h>

#include <hawcnest/impl/pybindings.h>

using namespace boost::python;

typedef return_value_policy<copy_const_reference> RVPCCR;

// -----------------------------------------------------------------------------
/// Expose TimeInterval to boost::python
void
pybind_time_TimeInterval()
{
  class_<TimeInterval, bases<Baggable>, boost::shared_ptr<TimeInterval> >
    ("TimeInterval",
     "The elapsed time between two events (up to 62 days with 1 ns precision)")
    .def(init<optional<const double> >())
    .add_property("second", &TimeInterval::GetSecond,
                  "Seconds in time interval.")
    .add_property("nanosecond", &TimeInterval::GetNanoSecond,
                  "Nanoseconds in time interval.")
    .add_property("interval", &TimeInterval::GetInterval,
                  "Interval length in base HAWC units.")
    .def(self + self)
    .def(self - self)
    .def(-self)
    .def(self * other<double>())
    .def(other<double>() * self)
    .def(self / other<double>())
    .def(self / self)
    .def(self == self)
    .def(self != self)
    .def(self > self)
    .def(self >= self)
    .def(self < self)
    .def(self <= self)
    ;

  register_pointer_conversions<TimeInterval>();
}

// -----------------------------------------------------------------------------
namespace {

  /// Convert a TimeStamp to a python (sec, ns) tuple
  tuple
  time_TimeStamp_to_tuple(const TimeStamp& ti)
  {
    return make_tuple(ti.GetGPSSecond(), ti.GetGPSNanoSecond());
  }

  /// Convert a python (sec, ns) tuple to a TimeStamp
  void
  time_tuple_to_TimeStamp(TimeStamp& ti, const tuple& tu)
  {
    ti.SetGPSTime(extract<unsigned int>(tu[0]),
                  extract<unsigned int>(tu[1]));
  }

}

/// Expose TimeStamp to boost::python
void
pybind_time_TimeStamp()
{
  class_<TimeStamp, bases<Baggable>, boost::shared_ptr<TimeStamp> >
    ("TimeStamp",
     "A TimeStamp holds a GPS second and nanosecond")
    .def(init<const unsigned int, optional<const unsigned int> >())
    .def(init<const int, optional<const int> >())
    .add_property("second", &TimeStamp::GetGPSSecond,
                  "Number of seconds in GPS time.")
    .add_property("nanosecond", &TimeStamp::GetGPSNanoSecond,
                  "Number of nanoseconds in GPS time.")
    .add_property("gpstime", time_TimeStamp_to_tuple, time_tuple_to_TimeStamp,
                  "Tuple access (GPS sec, GPS ns)")
    .def(self + other<TimeInterval>())
    .def(self - self)
    .def(self == self)
    .def(self != self)
    .def(self > self)
    .def(self >= self)
    .def(self < self)
    .def(self <= self)
    .def(self_ns::str(self))
    ;

  register_pointer_conversions<TimeStamp>();
}

// -----------------------------------------------------------------------------
/// Expose TimeRange to boost::python
void
pybind_time_TimeRange()
{
  class_<TimeRange, bases<Baggable>, boost::shared_ptr<TimeRange> >
    ("TimeRange",
     "Define a range of time between two TimeStamps")
    .def(init<const TimeStamp&, const TimeStamp&>())
    .def(init<const TimeStamp&, const TimeInterval&>())
    .add_property("start", 
                  make_function(&TimeRange::GetStartTime, RVPCCR()),
                  "Starting TimeStamp t0 of the range.")
    .add_property("stop", 
                  make_function(&TimeRange::GetStopTime, RVPCCR()),
                  "Stopping TimeStamp t1 of the range.")
    .add_property("duration", &TimeRange::GetDuration,
                  "TimeInterval size of range.")
    .def("in_range", &TimeRange::IsInRange,
         "Check for TimeStamp within range [t0, t1).")
    .def("in_range_inclusive", &TimeRange::IsInRangeInclusive,
         "Check for TimeStamp within range [t0, t1].")
    .def("has_common_range", &TimeRange::HasCommonTime,
         "Check for overlap with another TimeRange.")
    .def(self == self)
    .def(self != self)
    .def(self > self)
    .def(self < self)
    .def(self == other<TimeStamp>())
    .def(other<TimeStamp>() == self)
    .def(other<TimeStamp>() != self)
    .def(other<TimeStamp>() > self)
    .def(other<TimeStamp>() < self)
    ;

  register_pointer_conversions<TimeRange>();
}

// -----------------------------------------------------------------------------
namespace {

  /// Convert a UTCDate to a python (year, month, day) tuple
  tuple
  time_UTCDate_to_tuple(const UTCDate& d)
  {
    return make_tuple(d.GetYear(), d.GetMonth(), d.GetDay());
  }

  /// Convert a python (year, month, day) tuple to a UTCDate
  void
  time_tuple_to_UTCDate(UTCDate& d, const tuple& tu)
  {
    d.SetDate(extract<int>(tu[0]),
              extract<int>(tu[1]),
              extract<int>(tu[2]));
  }

}

/// Expose UTCDate to boost::python
void
pybind_time_UTCDate()
{
  class_<UTCDate, bases<Baggable>, boost::shared_ptr<UTCDate> >
    ("UTCDate",
     "Storage of a date by calendar year, month, and day")
    .def(init<const int, const int, const int>())
    .add_property("year", &UTCDate::GetYear, "The year.")
    .add_property("month", &UTCDate::GetMonth, "The month [1-12].")
    .add_property("day", &UTCDate::GetDay, "The day of month [1-31].")
    .add_property("date", time_UTCDate_to_tuple, time_tuple_to_UTCDate,
                  "Tuple access (year, month, day of month).")
    .def(self == self)
    .def(self != self)
    ;

  register_pointer_conversions<UTCDate>();

  enum_<UTCDate::Month>("Month")
    .value("JAN", UTCDate::JAN)
    .value("FEB", UTCDate::FEB)
    .value("MAR", UTCDate::MAR)
    .value("APR", UTCDate::APR)
    .value("MAY", UTCDate::MAY)
    .value("JUN", UTCDate::JUN)
    .value("JUL", UTCDate::JUL)
    .value("AUG", UTCDate::AUG)
    .value("SEP", UTCDate::SEP)
    .value("OCT", UTCDate::OCT)
    .value("NOV", UTCDate::NOV)
    .value("DEC", UTCDate::DEC)
    .export_values()
    ;
}

// -----------------------------------------------------------------------------
namespace {

  /// Convert a UTCDateTime to a python (year, month, day, hr, min, sec) tuple
  tuple
  time_UTCDateTime_to_tuple(const UTCDateTime& d)
  {
    return make_tuple(d.GetYear(), d.GetMonth(), d.GetDay(),
                      d.GetHour(), d.GetMinute(), d.GetSecond());
  }

  /// Convert a python (year, month, day, hr, min, sec) tuple to a UTCDateTime
  void
  time_tuple_to_UTCDateTime(UTCDateTime& d, const tuple& tu)
  {
    d.SetDateTime(extract<int>(tu[0]),
                  extract<int>(tu[1]),
                  extract<int>(tu[2]),
                  extract<int>(tu[3]),
                  extract<int>(tu[4]),
                  extract<int>(tu[5]));
  }

}

/// Expose UTCDateTime to boost::python
void
pybind_time_UTCDateTime()
{
  class_<UTCDateTime, bases<UTCDate>, boost::shared_ptr<UTCDateTime> >
    ("UTCDateTime",
     "Storage of a date and time: YYYY-MM-DDThh:mm:ss")
    .def(init<const UTCDate&, 
             optional<const int, const int, const int, const int> >())
    .def(init<const int, const int, const int, 
             optional<const int, const int, const int, const int> >())
    .def(init<const TimeStamp&>())
    .def(init<const time_t&>())
    .def(init<const tm&>())
    .def(init<const std::string&>())
    .add_property("hour", &UTCDateTime::GetHour)
    .add_property("minute", &UTCDateTime::GetMinute)
    .add_property("second", &UTCDateTime::GetSecond)
    .add_property("nanosecond", &UTCDateTime::GetNanoSecond)
    .add_property("datetime",
                  time_UTCDateTime_to_tuple, time_tuple_to_UTCDateTime,
                  "Tuple access (year, month, day, hour, minute, second)")
    .add_property("timestamp", &UTCDateTime::GetTimeStamp)
    .add_property("unixsecond", &UTCDateTime::GetUnixSecond)
    .def(self_ns::str(self))
    ;

  register_pointer_conversions<UTCDateTime>();

  def("GetCurrentTime", GetCurrentTime,
      "Get the current UTC date and time using the system clock.");
}

// -----------------------------------------------------------------------------
// Overload default time system argument in GetDate function
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(MJD_gd_ol, GetDate, 0, 1);

// Overload the SetDate function for different argument signatures
void (ModifiedJulianDate::*f1)(const UTCDateTime&) = &ModifiedJulianDate::SetDate;
void (ModifiedJulianDate::*f2)(const TimeStamp&) = &ModifiedJulianDate::SetDate;
void (ModifiedJulianDate::*f3)(const double&) = &ModifiedJulianDate::SetDate;

/// Expose ModifiedJulianDate to boost::python
void
pybind_time_ModifiedJulianDate()
{
  enum_<TimeScale>
    ("TimeScale",
     "Enums for standard time scales (atomic time, universal time, etc.)")
    .value("UTC", UTC) 
    .value("TAI", TAI) 
    .value("TT", TT) 
    .export_values()
    ;

  class_<ModifiedJulianDate, bases<Baggable>, boost::shared_ptr<ModifiedJulianDate> >
    ("ModifiedJulianDate",
     "Modified Julian Date: number of days since midnight of 17 Nov 1858",
                           init<const double&>())
    .def(init<const TimeStamp&>())
    .def(init<const UTCDateTime&>())
    .def("set_date", f1, "Set MJD with a UTC date and time.")
    .def("set_date", f2, "Set MJD with a GPS time stamp.")
    .def("set_date", f3, "Set MJD using decimal days.")
    .def("get_date", &ModifiedJulianDate::GetDate, MJD_gd_ol())

    .add_property("datetime", &ModifiedJulianDate::GetUTCDateTime,
                  "Get UTC date and time.")
    .add_property("timestamp", &ModifiedJulianDate::GetTimeStamp,
                  "Get TimeStamp (GPS sec and ns).")
    .def(self_ns::str(self))
    ;

  register_pointer_conversions<ModifiedJulianDate>();
}

