/*!
 * @file PointerTypedefs.h
 * @author Troy Straszheim
 * @brief Shared pointer typedefs, copied from IceTray.
 * @date 11 Mar 2010 
 * @version $Id: PointerTypedefs.h 17766 2013-11-01 04:23:51Z sybenzvi $
 *  
 * Copyright (C) 2007
 * Troy D. Straszheim  <troy@icecube.umd.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 * 
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef HAWCNEST_SHARED_POINTER_TYPEDEFS_H_INCLUDED
#define HAWCNEST_SHARED_POINTER_TYPEDEFS_H_INCLUDED

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

/*!
 * @def SHARED_POINTER_TYPEDEFS
 * @brief macro to define shared pointer typedefs
 * @ingroup hawcnest_api
 */
#define SHARED_POINTER_TYPEDEFS(C) \
  typedef boost::shared_ptr<C> C##Ptr; \
  typedef boost::shared_ptr<const C> C##ConstPtr;

#endif // HAWCNEST_SHARED_POINTER_TYPEDEFS_H_INCLUDED

