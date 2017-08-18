/*!
 * @file HAWCUnits.h 
 * @brief Definition of base units for online and offline calculations.
 * @author Segev BenZvi 
 * @date 22 Jul 2009 
 * @version $Id: HAWCUnits.h 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#ifndef HAWCNEST_HAWCUNITS_H_INCLUDED
#define HAWCNEST_HAWCUNITS_H_INCLUDED

#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <hawcnest/Logging.h>

namespace HAWCUnits {

  // Expand the X-macro definitions in unit_defs.h to provide a list of
  // constants of form "const double name = value;"
  #define X(name, value) static const double name = value;
  #include <hawcnest/impl/unit_defs.h>
  #undef X
  /*!
   * @class Evaluator
   * @author Segev BenZvi
   * @date 19 Jun 2011
   * @brief Provide a dictionary of unit name strings that can be converted to
   *        double-precision unit values
   */
  class Evaluator {

    public:

      static Evaluator& GetInstance() {
        static Evaluator eval_;
        return eval_;
      }

      const double& GetUnit(const std::string& name) const {
        mapIter_ = unitsMap_.find(name);
        if ( mapIter_ == unitsMap_.end() )
          log_fatal("Unit " << name << " not defined!");
        return mapIter_->second;
      };
    
      bool IsDefined(const std::string& name) const {
        mapIter_ = unitsMap_.find(name);
        if ( mapIter_ == unitsMap_.end() )
          return false;
        return true;
      };


    private:

      mutable std::map<std::string, double> unitsMap_;
      mutable std::map<std::string, double>::const_iterator mapIter_;
      Evaluator();
      Evaluator(const Evaluator&);
      Evaluator& operator=(const Evaluator&);

    friend std::ostream& operator<<(std::ostream& os, const Evaluator& e);

  };
}

#endif // HAWCNEST_HAWCUNITS_H_INCLUDED

