/*!
 * @file Parser.h
 * @author Segev BenZvi
 * @brief Parse expression of HAWCUnits.
 * @date 16 Feb 2012
 * @version $Id: Parser.h 13283 2012-11-17 02:46:12Z sybenzvi $
 */

#ifndef HAWCNEST_HAWCUNITS_PARSER_H_INCLUDED
#define HAWCNEST_HAWCUNITS_PARSER_H_INCLUDED

#include <string>

namespace HAWCUnits {

  /*!
   * @class Parser
   * @author Segev BenZvi
   * @date 16 Feb 2012
   * @ingroup hawcnest_api
   * @brief A parser of expressions using HAWC units
   *
   * In static configuration files it is often useful to specify inputs in
   * terms of basic HAWC units.  For example, in XML we might have
   *
   * @code
   *   <height unit="meter"> 4.0 </height>
   * @endcode
   *
   * This parser will convert such unit strings into double-precision values
   * once the unit string has been lexically analyzed.  The strings can include
   * simple functions of units and arithmetic combinations, such as
   *
   * @code
   *   <voltageNoiseDensity unit="nV / sqrt(Hz)"> 7 </voltageNoiseDensity>
   * @endcode
   *
   * The parser supports correct arithmetic operator precedence, so that
   * expressions like
   *
   * @code
   *   2 * Hz + 4 * kHz
   * @endcode
   *
   * will be evaluated correctly.
   *
   * This class is based on the parsing functions on p. 190-192 of "The C++
   * Programming Language" by B. Stroustrup (2003).
   */
  class Parser {

    public:

      Parser() { }

      /// Parse a numerical expression with functions, units, and operators
      double Evaluate(const std::string& expr) const;

    private:
    
      /// Highest-precedence operations: unit/number evaluation, parentheses
      double Atomic(bool get) const;

      /// Intermediate-precedence operations: powers/exponents
      double Oper(bool get) const;

      /// Low-precedence operations: multiplication and division
      double Term(bool get) const;

      /// Lowest-precedence operations: addition and subtraction
      double Expr(bool get) const;

  };

}

#endif // HAWCNEST_HAWCUNITS_PARSER_H_INCLUDED

