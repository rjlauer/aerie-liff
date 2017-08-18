/*!
 * @file LDModels.h
 * @author Hugo Ayala
 * @date 4 Jan 2015
 * @version $Id: LDModels.h 34039 2016-08-26 15:57:37Z johnm $
 * @brief Definition of different Lateral Distribution models for core fits
 */

#ifndef LD_MODELS_H_INCLUDED
#define LD_MODELS_H_INCLUDED

//When defining a function use always 

/***NKG Functions***/
// NKG Function
//Functions to cache the NKG function and make the fit go faster
double ngk_factor1(double r, double s, bool onlyNKG);
double nkg_factor1_precalc(double r, double s,bool onlyNKG);
double nkg_factor2(double r);
double nkg_factor2_precalc(double r);

// Function with variable age and fixed rmol
void nkgfunc(double x, double y, double a[4], double *zmod, double dzda[4],double C, double digamma[3], bool onlyNKG);
void nkgfunc(double x, double y, double a[4], double *zmod, double C, bool onlyNKG);
// Function with variable rmol and fixed age
void nkgfunc(double x, double y, double a[4], double dzda[4], double *zmod, bool onlyNKG);
void nkgfunc(double x, double y, double a[4], double *zmod, bool onlyNKG);

// Evaluates the NKG function, standalone (slower) version
double nkg(double r, double amplitude, double s, bool onlyNKG);

// NKG function in log space
double lat(double r, double amplitude, double age, double chisquare);
// jajm note: version used to plot using GamCoreFitResult.cc 
double latGamCore(double r, double amplitude, double age, double chisquare);

// Evaluate the logarithm of NKG for log-charge based calculations
//double log_ngk_factor1(double r, double s, bool onlyNKG);
//double log_nkg_factor1_precalc(double r, double s,bool onlyNKG);
void log_nkgfunc(double x, double y, double a[4], double *zmod,double C, bool onlyNKG);

// Evaluate the super-fast core fit, a patch of a Gaussian with a r^-3 tail
double sfcf(double r, double A, double sigma, double B, double rmol);

// Root-friendly C function wrapper
// par: x0, y0, amplitude, age, onlyNKG
double nkg2d(double *x, double *par);

/***Gauss Function***/
void funcs(double x, double y, double a[4], double *zmod, double dzda[4]);
void funcs(double x, double y, double a[4], double *zmod);

#endif // LD_MODELS_H_INCLUDED
