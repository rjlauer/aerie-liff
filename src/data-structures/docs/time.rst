.. _data_structures_time:

Time Classes
============

.. contents::
   :local:
   :backlinks: top

Timekeeping Standards
---------------------

.. figure:: timesys.png
   :align: center
   :height: 240px
   :figclass: align-center

   Relationship between coordinated universal time (UTC), international atomic
   time (TAI), and passed and present time systems related to the rotation of
   the Earth: ephemeris time (ET), terrestrial dynamical time (TDT), and
   terrestrial time (TT). The global positioning system (GPS) is offset from
   atomic time by 19 s.  UTC is periodically adjusted by leap seconds.

Different time scales are in common use and the data structures in AERIE
provide conversions between these systems.  HAWC events are recorded in terms
of the Global Position System (GPS) timescale and can be converted to
the civilian Coordinated Universal Time (UTC) timescale.  The basic time
classes in AERIE are:

TimeStamp
^^^^^^^^^

A `TimeStamp <../../doxygen/html/classTimeStamp.html>`_ is used to represent
GPS time in seconds and nanoseconds since the start of the GPS epoch (6 Jan
1980 00:00:00UT).  TimeStamp is the central time class in AERIE because the
event data are provided in the GPS system.

Two utility classes are provided for managing TimeStamps:

#. `TimeInterval <../../doxygen/html/classTimeInterval.html>`_: the difference between two TimeStamps.  A TimeInterval can store an interval as long as 62 days to a precision of 1 ns.
#. `TimeRange <../../doxygen/html/classTimeRange.html>`_: another way to define an interval using a start and stop time.

Coordinated Universal Time
^^^^^^^^^^^^^^^^^^^^^^^^^^

GPS time is fixed to the international atomic time standard by a fixed offset
of 19 s (see figure above).  Due to changes in the rotational velocity of the
Earth, the GPS standard drifts with respect to day/night transitions.

To correct for the drift, `LeapSeconds
<../../doxygen/html/classLeapSeconds.html>`_ are periodically defined to convert
between atomic time and GPS and the standard civilian timescale, known as
Coordinated Universal Time (UTC).  The UTC system is represented in AERIE by
the `UTCDateTime <../../doxygen/html/classUTCDateTime.html>`_ class, which can
be initalized using a standard calendar data (e.g., 6 September 2015, 3:14:16).

A table of leap seconds introduced since the start of the GPS epoch (as of
mid-2015) is given below:

==== ===== ===
Year Month Day
==== ===== ===
1981 Jul   1
1982 Jul   1
1983 Jul   1
1985 Jul   1
1988 Jan   1
1990 Jan   1
1991 Jan   1
1992 Jul   1
1993 Jul   1
1994 Jul   1
1996 Jan   1
1997 Jul   1
1999 Jan   1
2006 Jan   1
2009 Jan   1
2012 Jul   1
2015 Jul   1
==== ===== ===

Astronomical Time Systems
-------------------------

The TimeStamp and UTCDateTime classes are sufficient for representing data from
all hardware and calendar dates.  However, a separate time system is needed for
astronomical transformations.

An astronomical time base known as Ephemeris Time (ET) was introduced in 1960
to account for fluctuations in the rotation of the Earth. The ET standard has
been superceded by Terrestrial Time (TT), an idealized time system defined by
the IAU.  It is fixed (to the nearest millisecond) to atomic time (TAI) with an
offset of 32.184 s.  Due to the 19 s offset of GPS from TAI, TT is offset from
GPS by 32.184 s + 19 s = 51.184 s.

Most astronomical algorithms convert location and time observations on Earth's
surface to a celestial coordinate using modified Julian date (MJD), or the time
in days that has elapsed since 17 Nov 1858 00:00:00.  The `ModifiedJulianDate
<../../doxygen/html/classModifiedJulianDate.html>`_ class handles conversions
between TimeStamp (GPS) and UTCDateTime (calendar dates) to MJD and accounts
for the UTC-TT offset, including leap seconds.

