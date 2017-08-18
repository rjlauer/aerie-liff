/*!
 * @file CoaxialCable.cc 
 * @brief Implementation of a coaxial transmission cable (for simulations).
 * @author Segev BenZvi
 * @date 8 Feb 2012 
 * @version $Id: CoaxialCable.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <data-structures/detector/CoaxialCable.h>

using namespace det;
using namespace std;

CoaxialCable::CoaxialCable() :
  TransmissionLine(),
  C_L_(0),
  vel_(0),
  ndb100_(0),
  ndb100Freq_(0)
{
}

CoaxialCable::CoaxialCable(const string& type,
                           const double l, const double Z,
                           const double C, const double v,
                           const double a, const double af) :
  TransmissionLine(type, l, Z),
  C_L_(C),
  vel_(v),
  ndb100_(a),
  ndb100Freq_(af)
{
}

