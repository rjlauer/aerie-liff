/*!
 * @file AstroCoords.cc 
 * @brief Implemenation of astronomical coordinate conversion classes.
 * @author Segev BenZvi 
 * @date 28 May 2010 
 * @version $Id: AstroCoords.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <cmath>

#include <data-structures/astronomy/AstroCoords.h>

using namespace HAWCUnits;
using namespace std;

HrMinSec::HrMinSec(double hms)
{
  double sign = 1;
  if (hms < 0.) {
    sign = -1;
    hms *= sign;
  }
  hms /= 15 * degree;
  hr_ = floor(hms);
  hms  = 60 * (hms - hr_);
  min_ = floor(hms);
  sec_ = 60 * (hms - min_);
  hr_ *= sign * hour;
  min_ *= sign * minute;
  sec_ *= sign * second;
}

ostream&
operator<<(ostream& os, const HrMinSec& hms)
{
  os << hms.hr_ / hour << "h "
     << (hms.min_ < 0. ? -hms.min_ : hms.min_) / minute << "m "
     << (hms.sec_ < 0. ? -hms.sec_ : hms.sec_) / second << 's';

  return os;
}

DegMinSec::DegMinSec(double dms)
{
  double sign = 1;
  if (dms < 0.) {
    sign = -1;
    dms *= sign;
  }
  dms /= degree;
  deg_ = floor(dms);
  dms  = 60 * (dms - deg_);
  min_ = floor(dms);
  sec_ = 60 * (dms - min_);
  deg_ *= sign * degree;
  min_ *= sign * arcmin;
  sec_ *= sign * arcsec;
}

ostream&
operator<<(ostream& os, const DegMinSec& dms)
{
  os << dms.deg_ / degree << "° "
     << (dms.min_ < 0. ? -dms.min_ : dms.min_) / arcmin << "\' "
     << (dms.sec_ < 0. ? -dms.sec_ : dms.sec_) / arcsec << '\"';

  return os;
}

