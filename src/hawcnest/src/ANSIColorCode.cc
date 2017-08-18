/*!
 * @file ANSIColorCode.cc 
 * @brief Implementation of color coding for terminal printouts.
 * @author Javier Gonzalez
 * @author Segev BenZvi 
 * @date 30 Jan 2012 
 * @version $Id: ANSIColorCode.cc 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#include <hawcnest/impl/ANSIColorCode.h>

#include <cstdio>
#include <iostream>
#include <unistd.h>

using namespace std;

ostream&
operator<<(std::ostream& os, const ANSIColorCode& acc)
{
  // Print ANSI color codes if output is a terminal.
  // Suppress the codes when stdout or stderr are redirected.
  if ((os.rdbuf() == cout.rdbuf() && isatty(fileno(stdout))) ||
      (os.rdbuf() == cerr.rdbuf() && isatty(fileno(stderr))))
  {
    os << acc.code_;
  }
  return os;
}

