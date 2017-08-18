/*!
 * @file is_shared_ptr.h 
 * @brief Implementation of shared pointer boolean functors.
 * @author Troy Straszheim
 * @author John Pretz 
 * @date 9 Sep 2008 
 * @version $Id: is_shared_ptr.h 14881 2013-04-28 14:02:42Z sybenzvi $
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef ICETRAY_IS_SHARED_PTR_H_INCLUDED
#define ICETRAY_IS_SHARED_PTR_H_INCLUDED

#include <boost/shared_ptr.hpp>

/**
   traits classes that identify if a template parameter is a shared_ptr or not
   @param T
*/
template <typename T>
struct is_shared_ptr 
{
  const static bool value = false;
};

template <typename T>
struct is_shared_ptr<boost::shared_ptr<T> >
{
  const static bool value = true;
};

template <typename T>
struct is_shared_ptr<boost::shared_ptr<const T> >
{
  const static bool value = true;
};

template <typename T>
struct is_shared_ptr<const boost::shared_ptr<T> >
{
  const static bool value = true;
};

template <typename T>
struct is_shared_ptr<const boost::shared_ptr<const T> >
{
  const static bool value = true;
};

#endif

