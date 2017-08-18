/*!
 * @file Func1.h
 * @author Patrick Younk
 * @date 25 Jul 2014
 * @brief Wrap the ROOT TF1 class.
 * @version $Id: Func1.h 34170 2016-08-31 17:29:43Z imc $
 */

#ifndef FUNC_1_H_INCLUDED
#define FUNC_1_H_INCLUDED

#include <TF1.h>
#include <TROOT.h>

#include <sstream>

#include <hawcnest/PointerTypedefs.h>
#include <hawcnest/Logging.h>

/*!
 * @class Func1
 * @author Patrick Younk
 * @date 25 Jul 2014
 * @ingroup
 * @brief Wrapper for TF1
 */

class Func1: public TF1 {

 public:

  Func1() : TF1() { };

  //This constructor is primarily for the Python wrapper, so that Func1
  //can be created starting from an arbitrary ROOT.TF1 object, casted to void*
  //by using ROOT.AsCObject and the appropriate SWIG typemap
  Func1(const char *name, void *mfcn, Double_t xmin, Double_t xmax, Int_t npar) :
      TF1(name, (TF1 *) mfcn, xmin, xmax, npar) {
  }

  Func1(const char *name, const char *formula, double xmin = 0, double xmax = 1) :
      TF1(name, formula, xmin, xmax) {
  }

  int GetNParamters() const { return GetNpar(); };

  double GetParameter(int ipar) const { return TF1::GetParameter(ipar); };

  double GetParameterError(int ipar) const { return GetParError(ipar); };

  std::string GetParameterName(int ipar) const {
    std::string name = GetParName(ipar);
    return name;
  }

  void GetParameterBounds(int ipar, double &parmin, double &parmax) const {
    GetParLimits(ipar, parmin, parmax);
  }

  bool IsFree(int ipar) {

    double parmin;
    double parmax;

    GetParameterBounds(ipar, parmin, parmax);
    if (parmin == 0 && parmax == 0) return true; //Free and unbounded
    if (parmin == parmax) return false;            //Fixed
    else return true;                            //Free but bounded
  }

  void SetParameterName(int ipar, const std::string &name) {
    SetParName(ipar, name.c_str());
  }

  void SetParameter(const std::string &name, double parvalue) {
    TF1::SetParameter(name.c_str(), parvalue);
  }

  // Use various versions of TF1::SetParameter. Signatures include:
  //   1) TF1::SetParameter(const char* pname, double pval)
  //   2) TF1::SetParameter(int ipar, double pval)
  using TF1::SetParameter;

  void SetParameterError(int ipar, double err) {
    SetParError(ipar, err);
  }

  void SetParameterBounds(int ipar, double parmin, double parmax) {
    SetParLimits(ipar, parmin, parmax);
  }

  void PrintOut() {
    std::stringstream ss;
    ss << "Function: " << GetTitle() << ":" << std::endl;
    for (int i = 0; i < GetNParamters(); ++i) {
      ss << "  (Par " << i << ") " << GetParameterName(i) << ": "
         << GetParameter(i) << " +/- " << GetParameterError(i);
      if(i<GetNParamters()-1)
        ss << std::endl;
    }
    log_info(std::endl << ss.str());
  }
};
SHARED_POINTER_TYPEDEFS(Func1);

#endif
