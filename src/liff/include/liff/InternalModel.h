#ifndef INTERNAL_MODEL_H
#define INTERNAL_MODEL_H

#include <hawcnest/PointerTypedefs.h>

#include <TF2.h>

#include <liff/Util.h>

SHARED_POINTER_TYPEDEFS(TF1);
SHARED_POINTER_TYPEDEFS(TF2);

/*!
 * @class InternalModel
 * @author Robert Lauer
 * @date 9 July 2015
 * @ingroup 
 * @brief BGModel, CommonNorm, and DetectorResponse parameters for Inner Fit.
 */

/*  The Internal Model class holds the Common-Norm parameter, a pointer to the
 *  BG model (TF2) and a free parameter list (a list of parameters that are
 *  free in the inner fit). The free parameter list can include any TF1 or TF2
 *  parameters that are treated as internal to the HAWC plug-in, e.g. the PSF 
 *  parameters.
 *  The 2 dimensions of the BG-model TF2 are RA and Dec, measured
 *  in degrees.
 *  If the BG model has free parameters, then those will be fit in each
 *  CalcBin individually!
 *  
 *  The new source model extends the external ModelInterface in this way:
 *
 *    SourceModel = 
 *      CommonNorm(InternalModel)*[DetRes(InternalModel)x(ModelInterface)]
 *
 *  And the total expected counts are given by:
 *    
 *    TotalModel = SourceModel(InternalModel,ModelInterface)
 *                 + BGModel(InternalModel)
 *
 *
 */
/*****************************************************/

///Structure to hold info on a parameter to be optimized with a minimizer.
struct FreeParameter {
  TF1Ptr FuncPointer;
  int ParId;
};

///List of free parameters to be optimized by a minimizer.
typedef std::vector<FreeParameter> FreeParameterList;

class InternalModel {

  public:

    /// DetectorResponse fixed, flat BGModel with norm free
    InternalModel(bool CNfit = false, bool BGfit = false);

    /// DetectorResponse fixed, BGModel explicitly provided
    InternalModel(TF2Ptr BGModel, std::vector<int> FreeBGParIDs,
                  bool CNfit = false, bool BGfit = false);

    ///Reference toCommonNorm, multiplied to the flux of all sources
    double &CommonNorm() { return commonNorm_; };

    ///Reference to CommonNormError
    double &CommonNormError() { return commonNormError_; };

    ///Returns if CommonNorm is free in internal likelihood  fit
    bool IsCommonNormFree() const { return isCommonNormFree_; };

    ///Switch CommonNorm internal likelihood fit on/off
    void SetCommonNormFree(bool cnf = true) { isCommonNormFree_ = cnf; };

    ////Returns if BackgroundNorm is free in internal likelihood  fit
    bool IsBackgroundNormFree() const { return isBackgroundNormFree_; };

    ///Switch BackgroundNorm internal likelihood fit (in each bin) on/off
    void SetBackgroundNormFree(bool bnf = true) { isBackgroundNormFree_ = bnf; };

    ///Uses BGMap from data in each bin directly, no BGModel
    void UseBackgroundFromData();

    ///Sets the BGModel via a TF2, only given par.IDs free after initial fit
    void SetBackgroundModel(TF2Ptr BGModel, std::vector<int> FreeBGParIDs);

    ///Returns the BGModel TF2 pointer
    TF2Ptr GetBackgroundModel() const { return bgModel_; };

    ///Returns the BGModel free parameter ID list
    std::vector<int> GetFreeBackgroundParameterIDList() const { return freeBGParIDs_; };

    ///Returns free parameter list
    const FreeParameterList &GetFreeParameterList() { return freeParList_; };

    ///Clears free parameter list
    void ClearFreeParameterList() { freeParList_.clear(); };

    ///Adds new free parameter, indicate if DetectorResponse changes
    void AddFreeParameter(const char *func, int ParId, bool detResFree = false);

    ///Adds new free parameter, indicate if DetectorResponse changes
    void AddFreeParameter(TF1Ptr FuncPointer, int ParId, bool detResFree = false);

    ///Returns true if one of the free parameters change a DetectorResponse
    bool IsDetectorResponseFree() { return detResFree_; };

    ///Sets MINUIT verbosity: -1 = no printing, 0 = a little, 1 = full
    void SetInternalFitVerbosity(int v = -1) { verbosity_ = v; };

    ///Returns MINUIT verbosity levl
    int GetInternalFitVerbosity() const { return verbosity_; };

  private:

    double commonNorm_;
    double commonNormError_;
    bool isCommonNormFree_;
    bool isBackgroundNormFree_;

    ///The BGModel prototype that gets fit individually in each CalcBin
    TF2Ptr bgModel_;

    ///Lists of par-IDs that are free in the BGModel-likelihood-minimization
    std::vector<int> freeBGParIDs_;

    ///List of parameters that are free in the internal fit, other than BG fit
    FreeParameterList freeParList_;

    bool detResFree_;

    int verbosity_;

};

SHARED_POINTER_TYPEDEFS(InternalModel);

#endif
