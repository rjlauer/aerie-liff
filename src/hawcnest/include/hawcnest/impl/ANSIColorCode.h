/*!
 * @file ANSIColorCode.h 
 * @brief Definition of color codes for terminal printouts.
 * @author Javier Gonzalez
 * @author Segev BenZvi 
 * @date 30 Jan 2012 
 * @version $Id: ANSIColorCode.h 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#ifndef ANSICOLORCODE_H_INCLUDED
#define ANSICOLORCODE_H_INCLUDED

#include <iosfwd>
#include <string>

/*!
 * @class ANSIColorCode
 * @author Javier Gonzalez
 * @author Segev BenZvi
 * @ingroup hawcnest_impl
 * @date 30 Jan 2012
 * @brief This class encapsulates ANSI terminal color codes.  Streaming is
 *        overloaded so that the codes will only print to stdout and stderr.
 */
class ANSIColorCode {

  public:

    ANSIColorCode(const std::string& c) : code_(c) { }
  
  private:

    std::string code_;

  friend std::ostream& operator<<(std::ostream& os, const ANSIColorCode& ac);

};

#endif // ANSICOLORCODE_H_INCLUDED
