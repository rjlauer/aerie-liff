/*!
 * @file HAWCUnits.cc 
 * @brief Basic I/O and evaluation of unit constants.
 * @author Segev BenZvi 
 * @date 19 Jun 2011 
 * @version $Id: HAWCUnits.cc 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#include <hawcnest/HAWCUnits.h>
#include <iomanip>

using namespace std;

namespace HAWCUnits {

  Evaluator::Evaluator()
  {
    // Expand the X-macros in unit_defs.h to create a units dictionary
    #define X(name, value) unitsMap_[#name] = value;
    #include <hawcnest/impl/unit_defs.h>
    #undef X
  }

  ostream&
  operator<<(std::ostream& os, const Evaluator& e)
  {
    map<string, double>::iterator it;
    for (it = e.unitsMap_.begin(); it != e.unitsMap_.end(); ++it) {
      os << "  " << left << setw(16) << it->first << " " << it->second;
      if (distance(it, e.unitsMap_.end()) > 1)
        os << endl;
    }

    return os;
  }

}

