/*!
 * @file sf.cc 
 * @brief Demonstrate the use of special mathematical functions.
 * @author Segev BenZvi 
 * @date 19 Apr 2012 
 * @version $Id: sf.cc 22107 2014-10-05 14:56:06Z sybenzvi $
 */

#include <data-structures/math/SpecialFunctions.h>

#include <iostream>

using namespace SpecialFunctions;
using namespace std;

int main()
{
  double a = 4.;
  double x = 1.;

  cout << "Gamma functions:" << endl;
  cout << "G(" << a << ") = " << Gamma::G(a) << endl;
  cout << "g(" << a << "," << x << ") = " << Gamma::g(a, x) << endl;
  cout << "P(" << a << "," << x << ") = " << Gamma::P(a, x) << endl;
  cout << "G(" << a << "," << x << ") = " << Gamma::G(a, x) << endl;
  cout << "Q(" << a << "," << x << ") = " << Gamma::Q(a, x) << endl;

  a = -4.;
  cout << "G(" << a << "," << x << ") = " << Gamma::G(a, x) << endl;

  a = 4.;
  double b = 2.7;

  cout << "\nBeta functions: " << endl;
  cout << "B(" << a << "," << b << ") = " << Beta::B(a, b) << endl;
  cout << "ln B(" << a << "," << b << ") = " << Beta::lnB(a, b) << endl;
  cout << "I(" << a << "," << b << "," << x << ") = " << Beta::I(a,b,x) << endl;

  a = -4.1;
  b = -b;
  cout << "B(" << a << "," << b << ") = " << Beta::B(a, b) << endl;
  cout << "ln B(" << a << "," << b << ") = " << Beta::lnB(a, b) << endl;


  cout << "\nExponential integrals: " << endl;
  cout << "Ei(" << x << ") = " << Expint::Ei(x) << endl;
  for (int n = 1; n <= 5; ++n) {
    cout << "E" << n << "(" << x << ") = " << Expint::En(n, x) << endl;
  }

  return 0;
}

