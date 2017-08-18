/*!
 * @file EclPoint.cc 
 * @brief Implementation of an ecliptic coordinate class.
 * @author Segev BenZvi 
 * @date 28 May 2010 
 * @version $Id: EclPoint.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <data-structures/astronomy/EclPoint.h>

using namespace HAWCUnits;
using namespace std;

ostream&
operator<<(ostream& os, const EclPoint& s)
{
  os << "(beta = " << s.GetBeta() / degree << ", lambda = "
     << s.GetLambda() / degree << ')';

  return os;
}


