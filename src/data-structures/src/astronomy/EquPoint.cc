/*!
 * @file EquPoint.cc 
 * @brief Implemenation of an equatorial coordinate class.
 * @author Segev BenZvi 
 * @date 28 May 2010 
 * @version $Id: EquPoint.cc 35401 2016-10-25 17:47:40Z imc $
 */

#include <data-structures/astronomy/EquPoint.h>
#include <data-structures/astronomy/AstroCoords.h>

using namespace HAWCUnits;
using namespace std;

ostream&
operator<<(ostream& os, const EquPoint& s)
{
  os << '(' << HrMinSec(s.GetRA()) << ", " << DegMinSec(s.GetDec()) << ')';
  return os;
}

void EquPoint::GetRATraditional(int& h, int& m, double& s){

  double hours = GetRA()*24/twopi;

  h = int(hours);
  m = int( (hours-h) * 60);
  s = (hours - h - m/60.) * 3600;
    
}

void EquPoint::GetDecTraditional(int&d, int& m, double& s){

  double dec = GetDec()/degree;

  int sign = (dec > 0) - (dec < 0);

  dec = abs(dec);
  
  d = int(dec);
  m = int( (dec-d) * 60. );
  s = (dec - d - m/60.) * 3600.;

  d = sign*d;
  
}
