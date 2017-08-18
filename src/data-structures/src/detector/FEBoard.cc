/*!
 * @file FEBoard.cc 
 * @brief Electronic properties of the Milagro front-end boards.
 * @author Segev BenZvi 
 * @date 23 Sep 2010 
 * @version $Id: FEBoard.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <data-structures/detector/FEBoard.h>

using namespace det;

ABChannel::ABChannel() :
  rInt_(0),
  cInt_(0),
  gm_(0),
  vThr_(0)
{
}

ABChannel::ABChannel(const double rInt, const double cInt,
                     const double gm, const double vThr) :
  rInt_(rInt),
  cInt_(cInt),
  gm_(gm),
  vThr_(vThr)
{
}

FEBoard::FEBoard() :
  cBlock_(0),
  rTerm_(0),
  vNoiseD_(0)
{
}

FEBoard::FEBoard(const double cb , const double rt, const double vRMS,
                 const AnalogCard& ac) :
  cBlock_(cb),
  rTerm_(rt),
  vNoiseD_(vRMS),
  ac_(ac)
{
}

