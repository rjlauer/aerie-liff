/*!
 * @file LoadProject.h
 * @brief Dynamic library loading functions taken from IceTray v3.
 * @author Troy Straszheim
 * @version $Id: LoadProject.h 14881 2013-04-28 14:02:42Z sybenzvi $
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

#ifndef HAWCNEST_PYTHON_LOADPROJECT_H_INCLUDED
#define HAWCNEST_PYTHON_LOADPROJECT_H_INCLUDED

#include <string>

/// "proj" here is "hawcnest" or "data-structures", not "libhawcnest.so" 
/// or "libdata-structures" 
int load_project(const std::string& proj, bool doVerbose);

#endif // HAWCNEST_PYTHON_LOADPROJECT_H_INCLUDED

