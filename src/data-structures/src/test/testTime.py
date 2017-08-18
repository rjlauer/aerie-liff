#!/usr/bin/env python
"""Test python access to the time classes in the AERIE python bindings.

.. codeauthor:: Segev BenZvi
"""

__version__ = "$Id"

from hawc import hawcnest, data_structures
from hawc.data_structures import *
from hawc.hawcnest import HAWCUnits as U

# Start of the GPS epoch
t = TimeStamp(0)
u = UTCDateTime(t)
assert(u.year == 1980 and u.month == 1  and u.day == 6 and
       u.hour == 0    and u.minute == 0 and u.second == 0)

# Just before the July 1981 leap second
t = TimeStamp(46828799)
u = UTCDateTime(t)
assert(u.year == 1981 and u.month == 6  and u.day == 30 and
       u.hour == 23   and u.minute == 59 and u.second == 59)

# During the July 1981 leap second
t = TimeStamp(46828800)
u = UTCDateTime(t)
assert(u.year == 1981 and u.month == 6  and u.day == 30 and
       u.hour == 23   and u.minute == 59 and u.second == 60)

# After the July 1981 leap second
t = TimeStamp(46828801)
u = UTCDateTime(t)
assert(u.year == 1981 and u.month == 7  and u.day == 1 and
       u.hour == 0    and u.minute == 0 and u.second == 0)

# The billionth GPS second
t = TimeStamp(1000000000)
u = UTCDateTime(t)
assert(u.year == 2011 and u.month == 9  and u.day == 14 and
       u.hour == 1    and u.minute == 46 and u.second == 25)

# Test string conversion
assert(("%s" % t) == "1000000000 s")

# Check time intervals between TimeStamps
t1 = TimeStamp(900000000, 999999900)
t2 = TimeStamp(900000001, 10)
dt = t2 - t1
assert(dt.second == 0)
assert(dt.nanosecond == 110)
assert(dt.interval == 110*U.ns)

dt = TimeInterval(10*U.day)
t2 += dt
assert(t2.second == 900864001)

ti = TimeInterval(2*U.day)
assert(ti < dt)
assert(dt > ti)
assert(dt >= dt)
assert(ti*5 == dt)
assert(ti == dt/5)

dt -= ti
assert(dt.interval == 8*U.day)
dt += ti
assert(dt.interval == 10*U.day)

# Check conversion of MJD to and from different time systems
u = UTCDateTime(2010,6,21, 2,24,0)
mjd = ModifiedJulianDate(u)

assert(mjd.get_date() == 55368.1*U.day)
fu = mjd.datetime
assert(fu.year == 2010 and fu.month == 6  and fu.day == 21 and
       fu.hour == 2    and fu.minute == 24 and fu.second == 0)
assert(mjd.get_date(TimeScale.TAI) == 55368.1*U.day + (19+15)*U.second)
assert(mjd.get_date(TimeScale.TT) == 55368.1*U.day + (32.184+19+15)*U.second)

t = TimeStamp(1015718415)
mjd = ModifiedJulianDate(t)
assert(mjd.get_date() == 56000*U.day)
assert(mjd.timestamp == t)

