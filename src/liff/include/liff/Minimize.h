/*!
 * @file Minimize.h
 * @author Patrick Younk, Robert Lauer
 * @date 25 Jul 2014
 * @brief Internal Likelihood minimization functions, called via MINUIT.
 * @version $Id: Minimize.h 31179 2016-04-09 20:26:44Z sybenzvi $
 */

#ifndef MINIMIZE_H
#define MINIMIZE_H

/*****************************************************/

/// Minimize InternalModel
int InternalMinimize();

/*****************************************************/

/// Internal Model fit function
void InternalFitFunc(int &npar, double *gin, double &LL,
                     double *par, int iflag);

/*****************************************************/

/// Internal Model fit function, reset sources in each iteration
void InternalFitFuncUpdateSources(int &npar, double *gin, double &LL,
                                  double *par, int iflag);

/*****************************************************/

/// Minimize -LL for internal BG-Model (inside InternallModel) only
int InternalBGMinimize();

/*****************************************************/

/// Background-only internal fit function
void InternalBGFitFunc(int &npar, double *gin, double &LL,
                       double *par, int iflag);

/*****************************************************/

/// Minimize InternalModel for top hat 
int InternalTopHatMinimize();

/*****************************************************/

/// Internal Model fit function for top hat
void InternalTopHatFitFunc(int &npar, double *gin, double &LL,
                           double *par, int iflag);

/*****************************************************/
/// Internal Model top hat fit function, update sources in each iteration
void InternalTopHatFitFuncUpdateSources(int &npar, double *gin, double &LL,
                                        double *par, int iflag);

/*****************************************************/

/// Minimize Background-only for top hat 
int InternalTopHatBGMinimize();

/*****************************************************/
/// Background-only  fit function for top hat
void InternalTopHatBGFitFunc(int &npar, double *gin, double &LL,
                             double *par, int iflag);


#endif
