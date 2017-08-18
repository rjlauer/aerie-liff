/*!
 * @file TransmissionLine.cc 
 * @brief Implementation of a lossy transmission cable.
 * @author Segev BenZvi 
 * @date 8 Feb 2012 
 * @version $Id: TransmissionLine.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <data-structures/detector/TransmissionLine.h>

using namespace det;
using namespace std;

TransmissionLine::TransmissionLine() :
  type_(""),
  len_(0),
  Z_(0)
{
}

TransmissionLine::TransmissionLine(
  const string& type, const double l, const double Z) :
  type_(type),
  len_(l),
  Z_(Z)
{
}

