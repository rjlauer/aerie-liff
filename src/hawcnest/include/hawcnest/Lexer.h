/*!
 * @file Lexer.h
 * @brief A lexer for analyzing expressions using HAWCUnits
 * @author Segev BenZvi
 * @date 16 Feb 2012
 * @version $Id: Lexer.h 13283 2012-11-17 02:46:12Z sybenzvi $
 */

#ifndef HAWCNEST_HAWCUNITS_LEXER_H_INCLUDED
#define HAWCNEST_HAWCUNITS_LEXER_H_INCLUDED

#include <map>
#include <sstream>
#include <string>

namespace HAWCUnits {

  /*!
   * @class Lexer
   * @author Segev BenZvi
   * @date 16 Feb 2012
   * @ingroup hawcnest_api
   * @brief A lexer which can be used to analyze expressions using HAWC units
   *
   * In static configuration files it is often useful to specify inputs in
   * terms of basic HAWC units.  For example, in XML we might have
   *
   * @code
   *   <height unit="meter"> 4.0 </height>
   * @endcode
   *
   * This lexer will convert such unit strings into a series of tokens for
   * parsing.  The strings can include simple functions of units and arithmetic
   * combinations, such as
   *
   * @code
   *   <voltageNoiseDensity unit="nV / sqrt(Hz)"> 7 </voltageNoiseDensity>
   * @endcode
   *
   * This class is based on the lexing functions on p. 190-192 of "The C++
   * Programming Language" by B. Stroustrup (2003).
   */
  class Lexer {

    public:

      enum TokenValue {
        NAME,
        NUMBER,
        FUNCTION,
        START,
        END,
        PLUS='+', MINUS='-', MUL='*', DIV='/', POW='^', LP='(', RP=')',
      };

      typedef double (*FunctionPointer)(double);
      typedef std::map<std::string, FunctionPointer> FunctionMap;

      static Lexer& GetInstance() {
        static Lexer lexer_;
        return lexer_;
      }

      void SetInput(const std::string& str);

      TokenValue GetToken();
      const TokenValue& GetTokenType() const { return currentToken_; }
      const std::string& GetString() const { return stringValue_; }
      double GetNumber() const { return numberValue_; }
      FunctionPointer GetFunction() const { return fPtr_; }

    private:

      std::stringstream input_;

      TokenValue currentToken_;
      std::string stringValue_;
      double numberValue_;

      FunctionPointer fPtr_;
      FunctionMap fMap_;

      // Private constructors and copy constructors prevent copying of the lexer
      Lexer();
      Lexer(const Lexer&);
      Lexer& operator=(const Lexer&);

  };

}

#endif // HAWCNEST_HAWCUNITS_LEXER_H_INCLUDED

