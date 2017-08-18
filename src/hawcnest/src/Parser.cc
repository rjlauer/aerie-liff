/*!
 * @file Parser.cc 
 * @brief Parser class for evaluating functions of units and constants.
 * @author Segev BenZvi 
 * @date 16 Feb 2012 
 * @version $Id: Parser.cc 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#include <hawcnest/Lexer.h>
#include <hawcnest/Parser.h>
#include <hawcnest/Logging.h>

#include <hawcnest/HAWCUnits.h>

#include <cmath>

using namespace HAWCUnits;
using namespace std;

double
Parser::Atomic(bool get)
  const
{
  Lexer& lexer = Lexer::GetInstance();

  if (get)
    lexer.GetToken();

  switch (lexer.GetTokenType()) {

    // Floating point number
    case Lexer::NUMBER:
      lexer.GetToken();
      return lexer.GetNumber();

    // Variable
    case Lexer::NAME:
    {
      lexer.GetToken();
      const double& u = Evaluator::GetInstance().GetUnit(lexer.GetString());
      if (u == 0)
        log_fatal("Unrecognized name '" << lexer.GetString() << "'");
      return u;
    }

    // Function
    case Lexer::FUNCTION:
      return (*lexer.GetFunction())(Atomic(true));

    // Evaluate unary minus
    case Lexer::MINUS:
      return -Atomic(true);

    // Left parenthesis
    case Lexer::LP:
    {
      double e = Expr(true);
      if (lexer.GetTokenType() != Lexer::RP)
        log_fatal("Expected ')'");
      lexer.GetToken();
      return e;
    }

    // End of the expression
    case Lexer::END:
      return 1;

    default:
      log_fatal_nothrow("Atomic type expected");
      throw runtime_error("Atomic type expected");
  }
}

double
Parser::Oper(bool get)
  const
{
  Lexer& lexer = Lexer::GetInstance();
  double left = Atomic(get);
  for (;;) {
    switch (lexer.GetTokenType()) {

      case Lexer::POW:
        left = std::pow(left, Atomic(get));
        break;

      default:
        return left;
    }
  }
}

double
Parser::Term(bool get)
  const
{
  Lexer& lexer = Lexer::GetInstance();
  double left = Oper(get);
  for (;;) {
    switch (lexer.GetTokenType()) {

      case Lexer::MUL:
        left *= Oper(get);
        break;

      case Lexer::DIV:
        if (double d = Oper(true)) {
          left /= d;
          break;
        }
        log_fatal("Attempted division by zero.");

      default:
        return left;
    }
  }
}

double
Parser::Expr(bool get)
  const
{
  Lexer& lexer = Lexer::GetInstance();
  double left = Term(get);
  for (;;) {
    switch (lexer.GetTokenType()) {

      case Lexer::PLUS:
        left += Term(true);
        break;

      case Lexer::MINUS:
        left -= Term(true);
        break;

      default:
        return left;
    }
  }
}

double
Parser::Evaluate(const std::string& expr)
  const
{
  Lexer& lexer = Lexer::GetInstance();
  lexer.SetInput(expr);
  double ret = Expr(true);
  if (lexer.GetTokenType() != Lexer::END) {
    log_fatal("Expression \"" << expr << "\" does not evaluate");
  }
  return ret;
}

