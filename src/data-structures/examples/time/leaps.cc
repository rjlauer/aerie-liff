/*!
 * @file leaps.cc
 * @author Tom Weisgarber
 * @date 25 Jul 2013
 * @brief Simple test of the leap second calculator in the time structures.
 * @version $Id: leaps.cc 16442 2013-07-30 12:41:04Z sybenzvi $
 */

#include <hawcnest/HAWCNest.h>
#include <data-structures/time/LeapSeconds.h>

using namespace std;

int main()
{
  unsigned int gpsSec1 = 1055178059;
  time_t unixTime;
  unsigned int gpsSec2;

  LeapSeconds::GetInstance().ConvertGPSToUnix(gpsSec1, unixTime);
  LeapSeconds::GetInstance().ConvertUnixToGPS(unixTime, gpsSec2);

  cout << "Before conversion: " << gpsSec1 << endl;
  cout << "After conversion:  " << gpsSec2 << endl;

  return 0;
}

