/*!
 * @file GalPoint.cc 
 * @brief Implementation of a galactic coordinate class.
 * @author Segev BenZvi 
 * @date 28 May 2010 
 * @version $Id: GalPoint.cc 19006 2014-03-08 17:10:22Z sybenzvi $
 */

#include <data-structures/astronomy/GalPoint.h>

using namespace HAWCUnits;
using namespace std;

ostream&
operator<<(ostream& os, const GalPoint& s)
{
  os << "(B = " << s.GetB() / degree << "°,"
     << " L = " << s.GetL() / degree << "°)";
  return os;
}

