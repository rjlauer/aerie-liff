/*!
 * @file LogLogSpectrum.h
 * @author Giacomo Vianello
 * @date 26 Sep 2014
 * @brief Function for fitting the base-10 log of a differential flux.
 * @version $Id: LogLogSpectrum.h 32112 2016-05-16 21:53:25Z criviere $
 */

#ifndef LOG_LOG_SPECTRUM_H_INCLUDED
#define LOG_LOG_SPECTRUM_H_INCLUDED

#include <liff/Func1.h>

class LogLogSpectrum: public Func1 {

  public:

    /// Constructor with simple power law default
    LogLogSpectrum
        (const char *name = "a", const char *formula = "log10([0]) - [1] * x",
         double xmin = -3., double xmax = 6.)
        : Func1(name, formula, xmin, xmax) {

      SimplePowerLaw(3.5e-11, 2.63);
    }

    //This constructor is primarily for the Python wrapper, so that LogLogSpectrum
    //can be created starting from an arbitrary ROOT.TF1 object, casted to void*
    //by using ROOT.AsCObject and the appropriate SWIG typemap
    //Remember that *fcn must return the log10 of the differential flux
    LogLogSpectrum(const char *name, void *mfcn, Double_t xmin, Double_t xmax, Int_t npar) :
        Func1(name, (TF1 *) mfcn, xmin, xmax, npar) { };

    /// Sets differential flux norm at 1 TeV
    void SetNorm(double norm) {
      SetParameter(0, norm);
    }

    /// Returns differential flux norm at 1 TeV
    double GetNorm() const {
      return GetParameter(0);
    }

    /// Index (should be positive)
    void SetIndex(double val) {
      SetParameter(1, val);
    }
    double GetIndex() const {
      return GetParameter(1);
    }

    /// Cutoff in TeV
    void SetCutoff(double val) {
      SetParameter(2, val);
    }
    double GetCutoff() const {
      return GetParameter(2);
    }

    /// Defines a simple power law
    void SimplePowerLaw(double norm, double si) {
      //*this = LogLogSpectrum(GetName(),"log10([0]) - [1] * x");
      SetTitle("SimplePowerLaw");
      SetNorm(norm);
      SetParameter(1, si);
      SetParameterName(0, "DiffNorm1TeV");
      SetParameterName(1, "SpectralIndex");
    }

    /// Defines a cut-off power law
    void CutOffPowerLaw(double norm, double si, double cutoff) {
      *this = LogLogSpectrum(GetName(),
                             "log10([0]) - [1] * x - log10(exp(1.))*pow(10.,x-log10([2]))");
      SetTitle("CutOffPowerLaw");
      SetNorm(norm);
      SetParameter(1, si);
      SetParameterName(1, "SpectralIndex");
      SetParameter(2, cutoff);
      SetParameterName(2, "CutOffTeV");
    }

};
SHARED_POINTER_TYPEDEFS(LogLogSpectrum);

#endif
