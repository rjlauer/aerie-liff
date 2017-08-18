#include <liff/InternalModel.h>

#include <hawcnest/Logging.h>

#include <TDirectory.h>
#include <TROOT.h>

using namespace std;

/*****************************************************/

InternalModel::InternalModel(bool CNfit, bool BGfit)
    : commonNorm_(1),
      commonNormError_(1),
      isCommonNormFree_(CNfit),
      isBackgroundNormFree_(BGfit),
      detResFree_(false),
      verbosity_(-1) { }

/*****************************************************/

InternalModel::InternalModel(
    TF2Ptr BGModel, vector<int> FreeBGParIDs,
    bool CNfit, bool BGfit)
    : commonNorm_(1),
      commonNormError_(1),
      isCommonNormFree_(CNfit),
      isBackgroundNormFree_(BGfit),
      verbosity_(-1) {

  SetBackgroundModel(BGModel, FreeBGParIDs);

}

/*****************************************************/

void InternalModel::SetBackgroundModel(
    TF2Ptr BGModel, vector<int> FreeBGParIDs) {
  freeParList_.clear();
  bgModel_ = BGModel;
  for (vector<int>::iterator p = FreeBGParIDs.begin();
       p != FreeBGParIDs.end(); p++) {
    freeBGParIDs_.push_back(*p);
  }
}

/*****************************************************/
void InternalModel::UseBackgroundFromData() {
  bgModel_.reset();
  freeBGParIDs_.clear();
}

/*****************************************************/

void  InternalModel::AddFreeParameter
    (const char *funcName, int ParId, bool detResFree) {
  FreeParameter FP;
  TF1Ptr FuncPointer = TF1Ptr((TF1 *) gROOT->FindObject(funcName));
  if (FuncPointer) {
    AddFreeParameter(FuncPointer, ParId, detResFree);
  }
  else {
    log_error(funcName << " not found.");
  }
}

/*****************************************************/

void  InternalModel::AddFreeParameter
    (TF1Ptr FuncPointer, int ParId, bool detResFree) {
  FreeParameter FP;
  FP.FuncPointer = FuncPointer;
  FP.ParId = ParId;
  freeParList_.push_back(FP);
  detResFree_ = detResFree_ || detResFree;
}
