/*!
 * @file TimeStamp.cc 
 * @brief Implementation of a class which stores GPS seconds and nanoseconds.
 * @author Tom Paul
 * @author Segev BenZvi 
 * @date 17 Feb 2010 
 * @version $Id: TimeStamp.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <hawcnest/Logging.h>

#include <data-structures/time/TimeStamp.h>

using namespace std;

void
TimeStamp::SetNormalized(int sec, int nsec)
{
  const int nsPerSec = int(HAWCUnits::s / HAWCUnits::ns);
  while (nsec < 0) {
    --sec;
    nsec += nsPerSec;
  }
  while (nsec >= nsPerSec) {
    ++sec;
    nsec -= nsPerSec;
  }

  if (sec >= 0) {
    sGPS_ = sec;
    nsGPS_ = nsec;
  }
  else
    log_fatal("Operation for sec = " << sec << ", nsec = " << nsec
              << "results in time before GPS epoch");
}

ostream&
operator<<(ostream& os, const TimeStamp& ts)
{
  os << ts.GetGPSSecond() << " s";
  if (ts.GetGPSNanoSecond() != 0)
    os << " " << ts.GetGPSNanoSecond() << " ns";
  return os;
}

