/*!
 * @file LDModels.cc
 * @author Hugo Ayala
 * @date 4 Jan 2015
 * @version $Id: LDModels.cc 34039 2016-08-26 15:57:37Z johnm $
 * @brief Definition of various LDF models used in core fitting
 */

#include <data-structures/reconstruction/core-fitter/LDModels.h>
#include <data-structures/physics/PhysicsConstants.h>
#include <data-structures/math/SpecialFunctions.h>
#include <hawcnest/HAWCUnits.h>

//#include <iostream>
#include <cmath>

using namespace std;
using namespace SpecialFunctions::Gamma;

/***NKG Functions***/
const double rmol = 124.21; //rmol = chi_o*E_s/(E_c*rho) [m]

// two factors in the evaluation of the NKG function. They are pulled out and pre-calculated
// to make their evaluation much faster. Like just under a factor of 2 faster. 
double nkg_factor1(double r, double s, bool onlyNKG){
  static double rmol2 = rmol * rmol;
  if(onlyNKG){
    return pow(r/rmol,s-2.)*pow(1+r/rmol,s-4.5)/(2*HAWCUnits::pi*rmol2);
  }else{
    return pow(r/rmol,s-3.)*pow(1+r/rmol,s-4.5)/(2*HAWCUnits::pi*rmol2);
  }
}

double nkg_factor1_precalc(double r, double s,bool onlyNKG){
  static double precalc[40000][300];
  static double cached = false;

  if(!cached){
    for(int ri = 0 ; ri < 40000 ; ri++){
      for(int si = 0 ; si < 300 ; si++){
        double reval = ((double)ri)/100.;
        double seval = ((double)si)/100.;
        precalc[ri][si] = nkg_factor1(reval,seval,onlyNKG);
      }
    }
    cached = true;
  }

  if(r>1 and r < 399 and s > 0.1 and s < 2.9){
    int rbinlow = (int)(r * 100.);
    int rbinhigh = rbinlow + 1;
    double rfrac = r * 100. - ((int)(r * 100.));


    int sbinlow = (int)(s * 100.);
    int sbinhigh = sbinlow + 1;
    double sfrac = s * 100. - ((int)(s * 100.));


    double rlowslow = precalc[rbinlow][sbinlow];
    double rlowshigh = precalc[rbinlow][sbinhigh];

    double rhighslow = precalc[rbinhigh][sbinlow];
    double rhighshigh = precalc[rbinhigh][sbinhigh];

    return
      rlowslow * (1. - rfrac) * (1-sfrac) +
      rlowshigh * (1. - rfrac) * (sfrac) +
      rhighshigh * (rfrac) * (sfrac) +
      rhighslow * (rfrac) * (1-sfrac);
  }
  else{
    return nkg_factor1(r,s,onlyNKG);
  }
}

// second factor pulled out and cached for the sake of speed
double nkg_factor2(double r){
  return log(r/rmol)+log(1+r/rmol);
}

double nkg_factor2_precalc(double r){
  static double precalc[40000];
  static double cached = false;
  if(r <= 1. or r >399.){
    return nkg_factor2(r);
  }

  if(!cached){
    for(unsigned i = 0 ; i < 40000 ; i++){
      double reval = ((double)i)/100. ;
      precalc[i] = nkg_factor2(reval);
    }
    cached = true;
  }

  int binlow = (int)(r * 100.);
  int binhigh = binlow + 1;
  double frac = r * 100. - ((int)(r * 100.));


  double vallow = precalc[binlow];
  double valhigh = precalc[binhigh];

  return (vallow * (1. - frac)) + (valhigh * frac);

}
//NKG function for variable age parameter
void nkgfunc(double x, double y, double a[4], double *zmod, double dzda[4], double C, double digamma[3],bool onlyNKG)
{
  double argx, argy, argx2, argy2, rmol2, r;
 // a[0]=x0, a[1]=y0, a[2]=age, a[3]=ampl
  argx = (x-a[0]);
  argx2 = argx*argx;
  argy = (y-a[1]);
  argy2 = argy*argy;
  rmol2 = rmol*rmol;
  r=sqrt(argx2+argy2);

  // C is the value of the inverse of the Beta function
  *zmod = a[3]*C*nkg_factor1_precalc(r,a[2],onlyNKG);
  //mimic LMFit wit gaussian function.
  //Results are not very different with and without next ifs
  //if (fabs(argx/(4*a[2])) > 5.0) *zmod = 0;
  //if (fabs(argy/(4*a[2])) > 5.0) *zmod = 0;
  if (a[2]<0 || a[2]>2.25) *zmod = 0; // range of age parameter

  if(onlyNKG){
    dzda[0] = - *zmod *((a[2]-4.5)*pow(1+r/rmol,-1)+(a[2]-2)*pow(r/rmol,-1))*argx/(r*rmol);
    dzda[1] = - *zmod *((a[2]-4.5)*pow(1+r/rmol,-1)+(a[2]-2)*pow(r/rmol,-1))*argy/(r*rmol);
  } else {
    dzda[0] = - *zmod *((a[2]-4.5)*pow(1+r/rmol,-1)+(a[2]-3)*pow(r/rmol,-1))*argx/(r*rmol);
    dzda[1] = - *zmod *((a[2]-4.5)*pow(1+r/rmol,-1)+(a[2]-3)*pow(r/rmol,-1))*argy/(r*rmol);
  }
  dzda[2] = *zmod *(nkg_factor2_precalc(r)+2*digamma[0]-digamma[1]-digamma[2]);
  dzda[3] = *zmod/a[3];
}

void nkgfunc(double x, double y, double a[4], double *zmod, double C, bool onlyNKG)
{
  double argx, argy, argx2, argy2, rmol2, r;
 // a[0]=x0, a[1]=y0, a[2]=age, a[3]=ampl
  argx = (x-a[0]);
  argx2 = argx*argx;
  argy = (y-a[1]);
  argy2 = argy*argy;
  rmol2 = rmol*rmol;
  r=sqrt(argx2+argy2);

  // C is the value of the inverse of the Beta function
  *zmod = a[3]*C*nkg_factor1_precalc(r,a[2],onlyNKG);
}

//NKG function with rmol variable and fixed age
void nkgfunc(double x, double y, double a[4], double dzda[4], double *zmod, bool onlyNKG){
 double argx, argy, argx2, argy2, rmol2, r;
 // a[0]=x0, a[1]=y0, a[2]=rmol, a[3]=ampl
  // Setting age value. Using values that Dirk found from his analysis for now.
  double s=0.9;
  if(onlyNKG) s=0.4;
  argx = (x-a[0]);
  argx2 = argx*argx;
  argy = (y-a[1]);
  argy2 = argy*argy;
  rmol2 = a[2]*a[2];
  r=sqrt(argx2+argy2);

  if(onlyNKG){
    *zmod=a[3]*pow(r/a[2],s-2.)*pow(1+r/a[2],s-4.5)/(2*HAWCUnits::pi*rmol2);
  }else{
    *zmod=a[3]*pow(r/a[2],s-3.)*pow(1+r/a[2],s-4.5)/(2*HAWCUnits::pi*rmol2);
  }

  if(onlyNKG){
    dzda[0] = - *zmod *((s-4.5)*pow(1+r/a[2],-1)+(s-2)*pow(r/a[2],-1))*argx/(r*a[2]);
    dzda[1] = - *zmod *((s-4.5)*pow(1+r/a[2],-1)+(s-2)*pow(r/a[2],-1))*argy/(r*a[2]);
  } else {
    dzda[0] = - *zmod *((s-4.5)*pow(1+r/a[2],-1)+(s-3)*pow(r/a[2],-1))*argx/(r*a[2]);
    dzda[1] = - *zmod *((s-4.5)*pow(1+r/a[2],-1)+(s-3)*pow(r/a[2],-1))*argy/(r*a[2]);
  }
  dzda[2] = *zmod *(2+(s-3)+(s-4.5)*r/(a[2]+r))/a[2];
  dzda[3] = *zmod/a[3];
}

void nkgfunc(double x, double y, double a[4], double *zmod, bool onlyNKG){
  double s=0.4;
  double argx, argy, argx2, argy2, rmol2, r;
  if(onlyNKG) s=0.9;
  argx = (x-a[0]);
  argx2 = argx*argx;
  argy = (y-a[1]);
  argy2 = argy*argy;
  rmol2 = a[2]*a[2];
  r=sqrt(argx2+argy2);

  if(onlyNKG){
    *zmod=a[3]*pow(r/a[2],s-2.)*pow(1+r/a[2],s-4.5)/(2*HAWCUnits::pi*rmol2);
  }else{
    *zmod=a[3]*pow(r/a[2],s-3.)*pow(1+r/a[2],s-4.5)/(2*HAWCUnits::pi*rmol2);
  }
}

//Evaluate the logarithm of NKG for log-charge based calculations
void
log_nkgfunc(double x, double y, double a[4], double *zmod,double C, bool onlyNKG)
{
 // double s=0.4;
  //double argx, argy, argx2, argy2, rmol2, r;
 // if(onlyNKG) s=0.9;
 // a[0]=x0, a[1]=y0, a[2]=age, a[3]=ampl
 // argx = (x-a[0]);
 // argx2 = argx*argx;
 // argy = (y-a[1]);
 // argy2 = argy*argy;
 // rmol2 = a[2]*a[2];
 // r=sqrt(argx2+argy2);

  double argx, argy, argx2, argy2, rmol2, r,s;
 // a[0]=x0, a[1]=y0, a[2]=age, a[3]=ampl
  argx = (x-a[0]);
  argx2 = argx*argx;
  argy = (y-a[1]);
  argy2 = argy*argy;
  rmol2 = rmol*rmol;
  r=sqrt(argx2+argy2);
  s = a[2];

  // C is the value of the inverse of the Beta function
  //*zmod = a[3]*C*nkg_factor1_precalc(r,a[2],onlyNKG);



  if(onlyNKG){
   *zmod = C+log10(a[3]) + (s-2)*log10(r/rmol) + (s-4.5)*log10(1+r/rmol) - log10(2*HAWCUnits::pi*rmol2);
  }else{
    *zmod = C + log10(a[3]) + (s-3)*log10(r/rmol) + (s-4.5)*log10(1+r/rmol) - log10(2*HAWCUnits::pi*rmol2);
  }
}

// Evaluates the NKG function, standalone (slower) version
double nkg(double r, double amplitude, double s, bool onlyNKG) {
  double x = r;
  double y = 0.;
  double a[4];
  a[0] = 0.;
  a[1] = 0.;
  a[2] = s;
  a[3] = amplitude;
  double C=G(4.5-s)/(G(s)*G(4.5-2*s));
  double psi[3];
  psi[0] = DG(4.5-2*s);
  psi[1] = DG(s);
  psi[2] = DG(4.5-s);
  double zmod,dzda[4];
  nkgfunc(x,y,a,&zmod,dzda,C,psi,onlyNKG);
  return zmod;
}

// Combination of a Gaussian and a r^-3 tail
double
sfcf(double r, double A, double sigma, double B, double rmol)
{
  double g = exp(-0.5*(r*r/(sigma*sigma))) / (2*HAWCUnits::pi * sigma*sigma);
  double t = B * pow(0.5 + r/rmol, -3.);
  return A * (g + t);
}

//NKG function in log space  (Kelly Malone version)
double
lat(double r, double A, double B, double C)
{
  double x = A + B*(log10(r/124.21)+log10(1+r/124.21))-3*log10(r/124.21)-4.5*log10(1+r/124.21);
  // return x;
  return pow( 10.,x );  // to compare with charge (and NOT log10(charge)) 
}

//NKG function in log space  (J. Matthews version)
double
latGamCore(double r, double A, double B, double C)
{
  double x = A + B*(log10(r/36.9231)+log10(1+r/130.))-3*log10(r/36.9231)-4.5*log10(1+r/130.);
  return pow( 10.,x );  // to compare with charge (and NOT log10(charge)) 
}

// Root-friendly C function wrapper
// par: x0, y0, amplitude, age, onlyNKG
double nkg2d(double *x, double *par) {
  double xx = x[0] - par[0];
  double yy = x[1] - par[1];
  double r=sqrt(xx*xx+yy*yy);
  double amplitude = par[2];
  double age = par[3];
  bool onlyNKG = (par[4]!=0);
  return nkg(r, amplitude, age, onlyNKG);
}

/***Gauss Function***/
void funcs(double x, double y, double a[4], double *zmod, double dzda[4])
{
  double argx, argy, expx, expy, argx2, argy2;

  argx = (x-a[0])/a[2];
  argx2 = argx*argx;
  argy = (y-a[1])/a[2];
  argy2 = argy*argy;
  if (fabs(argx) > 5.0)
    expx = 0.0;
  else
    expx = exp(-0.5*argx2);
  if (fabs(argy) > 5.0)
    expy = 0.0;
  else
    expy = exp(-0.5*argy2);
  *zmod = a[3]/(2.*HAWCUnits::pi*a[2]*a[2])*expx*expy;
  dzda[0] = argx/a[2] * *zmod;
  dzda[1] = argy/a[2] * *zmod;
  dzda[2] = *zmod/a[2]*(argx2 + argy2 - 2.0);
  dzda[3] = *zmod/a[3];
}
void funcs(double x, double y, double a[4], double *zmod)
{
  double argx, argy, expx, expy, argx2, argy2;

  argx = (x-a[0])/a[2];
  argx2 = argx*argx;
  argy = (y-a[1])/a[2];
  argy2 = argy*argy;
  if (fabs(argx) > 5.0)
    expx = 0.0;
  else
    expx = exp(-0.5*argx2);
  if (fabs(argy) > 5.0)
    expy = 0.0;
  else
    expy = exp(-0.5*argy2);
  *zmod = a[3]/(2.*HAWCUnits::pi*a[2]*a[2])*expx*expy;
}
