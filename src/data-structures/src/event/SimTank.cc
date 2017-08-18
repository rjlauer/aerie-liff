/*!
 * @file SimTank.cc 
 * @brief Tank-level data from simulated events.
 * @author Segev BenZvi 
 * @date 27 Apr 2010 
 * @version $Id: SimTank.cc 14880 2013-04-28 12:19:08Z sybenzvi $
 */

#include <data-structures/event/SimTank.h>

SimTank::SimTank() :
  ID_(0)
{
}

SimTank::SimTank(int id) :
  ID_(id)
{
  pmts_.reserve(300);
  pcls_.reserve(100);
}

