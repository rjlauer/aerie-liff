/*!
 * @file Lexer.cc 
 * @brief Lexer definition for handling functions of units.
 * @author Segev BenZvi 
 * @date 16 Feb 2012 
 * @version $Id: Lexer.cc 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#include <hawcnest/Lexer.h>
#include <hawcnest/Logging.h>

#include <cmath>

using namespace HAWCUnits;
using namespace std;

Lexer::Lexer() :
  currentToken_(END),
  stringValue_(""),
  numberValue_(0),
  fPtr_(NULL)
{
  fMap_["abs"] = std::abs;
  fMap_["sqrt"] = std::sqrt;
  fMap_["sin"] = std::sin;
  fMap_["cos"] = std::cos;
  fMap_["tan"] = std::tan;
  fMap_["asin"] = std::asin;
  fMap_["acos"] = std::acos;
  fMap_["atan"] = std::atan;
  fMap_["sinh"] = std::sinh;
  fMap_["cosh"] = std::cosh;
  fMap_["tanh"] = std::tanh;
  fMap_["exp"] = std::exp;
  fMap_["log"] = std::log;
  fMap_["log10"] = std::log10;
}

void
Lexer::SetInput(const std::string& str)
{
  input_.clear();
  input_.str("");
  input_ << str;
  currentToken_ = START;
}

Lexer::TokenValue
Lexer::GetToken()
{
  // Skip whitespace
  char ch;
  do {
    if (!input_.get(ch))
      return currentToken_ = END;
  } while (isspace(ch));

  TokenValue tv;

  switch (ch) {
    case 0:
      tv = END;
      break;
    case '*':
    case '/':
    case '+':
    case '-':
    case '^':
    case '(':
    case ')':
      tv = currentToken_ = TokenValue(ch);
      break;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': case '.':
      input_.putback(ch);
      input_ >> numberValue_;
      tv = currentToken_ = NUMBER;
      break;
    default:
      if (isalpha(ch)) {
        stringValue_ = ch;
        while (input_.get(ch) && isalnum(ch))
          stringValue_ += ch;
        input_.putback(ch);
        FunctionMap::iterator iF;
        if ((iF = fMap_.find(stringValue_)) != fMap_.end()) {
          fPtr_ = iF->second;
          tv = currentToken_ = FUNCTION;
          break;
        }
        tv = currentToken_ = NAME;
        break;
      }
      tv = END;
      log_fatal("Bad token.");
  }

  return tv;
}

