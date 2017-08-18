#include <TMinuit.h>

#include <liff/LikeHAWC.h>

#include <liff/Minimize.h>

using namespace std;

LikeHAWC *gLikeHAWC;

/*****************************************************/

int InternalMinimize() {

  int nFree = 0;
  FreeParameterList freeParList;

  //free BG shape parameters

  CalcBinVector &calcBinVector = gLikeHAWC->GetCalcBins();

  for (unsigned k = 0; k < calcBinVector.size(); ++k) {

    InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
    FreeParameterList binfbgp = imb.GetFreeBackgroundParameterList();
    nFree += binfbgp.size();
    freeParList.insert(freeParList.end(), binfbgp.begin(), binfbgp.end());
  }
  if (nFree > 0) {
    log_debug("Fitting BGModel with " << nFree << " free parameters.");
  }

  //Other free parameters:
  FreeParameterList ofp =
      gLikeHAWC->GetInternalModel()->GetFreeParameterList();
  nFree += ofp.size();
  freeParList.insert(freeParList.end(), ofp.begin(), ofp.end());
  bool updateSources = false;
  if (ofp.size() > 0) {
    log_debug("Fitting " << ofp.size() << " additional free parameters"
                  << " in InternalModel.");
    log_debug("Resetting source lists in each Minimization step to capture"
                  << " potential changes in convolution with DetectorResponse.");
    updateSources = true;
  }

  //BGNorm fit
  vector<double> bn_values;
  vector<double> bn_errors;
  bool bnFit = gLikeHAWC->GetInternalModel()->IsBackgroundNormFree();
  if (bnFit) {
    for (unsigned k = 0; k < calcBinVector.size(); ++k) {
      InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();

      bn_values.push_back(imb.BackgroundNorm());
      bn_errors.push_back(imb.BackgroundNormError());
      nFree += 1;
    }
    log_debug("Fitting background normalization separately in all "
                  << nFree << " CalcBins.");
  }

  //Common Norm fit
  bool cnFit = gLikeHAWC->GetInternalModel()->IsCommonNormFree();
  if (cnFit) {
    nFree += 1;
    log_debug("Fitting CommonNorm.");
  }

  if (nFree == 0) {
    log_trace("No free parameters in InternalModel)");
    return 0;
  }

  log_debug("Minimizing with " << nFree << " free parameters...");

  //Setup Minimizer
  TMinuit *theMinuit = new TMinuit(nFree);

  // minuit verbosity, -1 = no printing, 0 = A little printing
  int Verbosity = gLikeHAWC->GetInternalModel()->GetInternalFitVerbosity();
  theMinuit->SetPrintLevel(Verbosity);
  if (updateSources) theMinuit->SetFCN(InternalFitFuncUpdateSources);
  else theMinuit->SetFCN(InternalFitFunc);

  double arglist[2];  // arguments to pass to the Minuit interpreter
  int flag = 0;

  arglist[0] = 0.5;   //Set for errors (1 for Chi2, 0.5 for LL)
  arglist[1] = 1;    //  unused
  theMinuit->mnexcm("SET ERR", arglist, 1, flag);

  if (flag) {
    log_warn("Minuit SET ERR failed, flag=" << flag);
    return -1;
  }
  //Setup Free Parameters
  //
  int np = 0;
  //free parameters
  for (unsigned n = 0; n < freeParList.size(); n++) {
    int ParId = freeParList[n].ParId;
    double ParValue = freeParList[n].FuncPointer->GetParameter(ParId);
    double ParError = freeParList[n].FuncPointer->GetParError(ParId);
    if (ParError == 0) ParError = 0.1 * ParValue;
    string ParName = freeParList[n].FuncPointer->GetParName(ParId);
    theMinuit->mnparm(np, ParName.c_str(), ParValue, ParError, 0, 0, flag);
    np++;
  }
  //BackgroundNorm...
  if (bnFit) {
    for (unsigned b = 0; b < bn_values.size(); b++) {
      double BNValue = bn_values[b];
      double BNError = bn_errors[b];
      if (BNError == 0) BNError = 0.1 * BNValue;
      string BNName = Form("BackgroundNorm_bin%d", b);
      theMinuit->mnparm(np, BNName.c_str(), BNValue, BNError, 0, 0, flag);
      np++;
    }
  }
  //and CommonNorm...
  if (cnFit) {
    double CNValue = gLikeHAWC->GetInternalModel()->CommonNorm();
    double CNError = gLikeHAWC->GetInternalModel()->CommonNormError();
    if (CNError == 0) CNError = CNValue;
    string CNName = "CommonNorm";
    theMinuit->mnparm(np, CNName.c_str(), CNValue, CNError, 0, 0, flag);
    np++;
  }

  //****Call Minimizer
  arglist[0] = 1000;   // maxcalls
  arglist[1] = 0.1;    // tolerance

  theMinuit->mnexcm("MINIMIZE", arglist, 2, flag);  //Call the Minimizer

  if (flag) {
    log_warn("FIT ERROR: From MIGRAD, flag=" << flag);
    return -1;
  }

  //Retrieve Parameters
  np = 0;
  for (unsigned n = 0; n < freeParList.size(); n++) {
    int ParId = freeParList[n].ParId;
    double ParValue;
    double ParError;
    theMinuit->GetParameter(np, ParValue, ParError);
    freeParList[n].FuncPointer->SetParameter(ParId, ParValue);
    freeParList[n].FuncPointer->SetParError(ParId, ParError);
    np++;
  }
  //and BackgroundNorm...
  if (bnFit) {
    for (unsigned k = 0; k < calcBinVector.size(); ++k) {
      InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
      double BNValue;
      double BNError;
      theMinuit->GetParameter(np, BNValue, BNError);
      imb.BackgroundNorm() = BNValue;
      imb.BackgroundNormError() = BNError;
      np++;
    }
  }
  //and CommonNorm...
  if (cnFit) {
    double CNValue;
    double CNError;
    theMinuit->GetParameter(np, CNValue, CNError);
    gLikeHAWC->GetInternalModel()->CommonNorm() = CNValue;
    gLikeHAWC->GetInternalModel()->CommonNormError() = CNError;
    np++;
  }

  delete theMinuit;

  return 0;
}

/*****************************************************/

void InternalFitFunc(int &npar, double *gin, double &LL,
                     double *par, int iflag) {

  //npar:  Number of parameters
  //gin:   First derivitives in an array (optional) - output
  //LL:    Calculated value of the function - output
  //par:   The current parameters in an array
  //iflag: Indicates what is to be calculated

  //We always calculate LL, and never calculate gin.
  //We do not use iflag or npar

  //Iterate through Model List First Time
  //Will Call all Active and Passive Models

  /*
  //check number of free parameters
  int nFree =  //here we would have to do exactly what is done in Minimize();
  if (nFree != npar) {
    log_fatal("Number of parameters different between gModel and Minimizer!");
  }
  */

  //Assign Parameters
  int n = 0;
  CalcBinVector &calcBinVector = gLikeHAWC->GetCalcBins();

  //BG fit: free parameters in each CalcBin:
  for (unsigned k = 0; k < calcBinVector.size(); k++) {
    InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
    FreeParameterList binfbgp = imb.GetFreeBackgroundParameterList();
    for (unsigned j = 0; j < binfbgp.size(); j++) {
      int ParId = binfbgp[j].ParId;
      binfbgp[j].FuncPointer->SetParameter(ParId, par[n]);
      n++;
    }
  }
  // There should be no "other free parameters",
  // otherwise use InternalFitFuncUpdateSources
  /*
  //Other free parameters:
  FreeParameterList ofp =
    gLikeHAWC->GetInternalModel()->GetFreeParameterList();
  for (unsigned j=0; j<ofp.size(); j++) {
    int ParId = ofp[j].ParId;
    ofp[j].FuncPointer->SetParameter(ParId, par[n]);
    n++;
  }
  */
  //Background Norm fit
  bool bnFit = gLikeHAWC->GetInternalModel()->IsBackgroundNormFree();
  if (bnFit) {
    for (unsigned k = 0; k < calcBinVector.size(); k++) {
      InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
      imb.BackgroundNorm() = par[n];
      n++;
    }
  }
  //Common Norm fit
  bool cnFit = gLikeHAWC->GetInternalModel()->IsCommonNormFree();
  if (cnFit) {
    gLikeHAWC->GetInternalModel()->CommonNorm() = par[n];
    //n++;
  }

  LL = -gLikeHAWC->CalcLogLikelihood(false);

}

/*****************************************************/

void InternalFitFuncUpdateSources(int &npar, double *gin, double &LL,
                                  double *par, int iflag) {

  //npar:  Number of parameters
  //gin:   First derivitives in an array (optional) - output
  //LL:    Calculated value of the function - output
  //par:   The current parameters in an array
  //iflag: Indicates what is to be calculated

  //We always calculate LL, and never calculate gin.
  //We do not use iflag or npar

  //Iterate through Model List First Time
  //Will Call all Active and Passive Models

  /*
  //check number of free parameters
  int nFree =  //here we would have to do exactly what is done in Minimize();
  if (nFree != npar) {
    log_fatal("Number of parameters different between gModel and Minimizer!");
  }
  */

  //Assign Parameters
  int n = 0;

  CalcBinVector &calcBinVector = gLikeHAWC->GetCalcBins();

  //BG fit: free parameters in each CalcBin:
  for (unsigned k = 0; k < calcBinVector.size(); k++) {
    InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
    FreeParameterList binfbgp = imb.GetFreeBackgroundParameterList();
    for (unsigned j = 0; j < binfbgp.size(); j++) {
      int ParId = binfbgp[j].ParId;
      binfbgp[j].FuncPointer->SetParameter(ParId, par[n]);
      n++;
    }
  }
  //Other free parameters:
  FreeParameterList ofp =
      gLikeHAWC->GetInternalModel()->GetFreeParameterList();
  for (unsigned j = 0; j < ofp.size(); j++) {
    int ParId = ofp[j].ParId;
    ofp[j].FuncPointer->SetParameter(ParId, par[n]);
    n++;
  }
  //Background Norm fit
  bool bnFit = gLikeHAWC->GetInternalModel()->IsBackgroundNormFree();
  if (bnFit) {
    for (unsigned k = 0; k < calcBinVector.size(); k++) {
      InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
      imb.BackgroundNorm() = par[n];
      n++;
    }
  }
  //Common Norm fit
  bool cnFit = gLikeHAWC->GetInternalModel()->IsCommonNormFree();
  if (cnFit) {
    gLikeHAWC->GetInternalModel()->CommonNorm() = par[n];
    n++;
  }

  LL = -gLikeHAWC->CalcLogLikelihoodUpdateSources(false);
}


/*****************************************************/

int InternalBGMinimize() {

  int nFree = 0;
  FreeParameterList freeParList;

  CalcBinVector &calcBinVector = gLikeHAWC->GetCalcBins();

  //BG fit
  //free parameters in each CalcBin:
  for (unsigned k = 0; k < calcBinVector.size(); ++k) {
    InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
    FreeParameterList binfbgp = imb.GetFreeBackgroundParameterList();
    nFree += binfbgp.size();
    freeParList.insert(freeParList.end(), binfbgp.begin(), binfbgp.end());
  }
  //BGNorm fit
  vector<double> bn_values;
  vector<double> bn_errors;
  bool bnFit = gLikeHAWC->GetInternalModel()->IsBackgroundNormFree();
  if (bnFit) {
    for (unsigned k = 0; k < calcBinVector.size(); ++k) {
      InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
      bn_values.push_back(imb.BackgroundNorm());
      bn_errors.push_back(imb.BackgroundNormError());
      nFree += 1;
    }
    log_debug("Fitting background normalization separately in all "
                  << nFree << " CalcBins.");
  }

  if (nFree == 0) {
    log_trace("No free parameters in BG-Model)");
    return 0;
  }

  log_debug("Fitting BGModel with " << nFree << " free parameters.");

  log_debug("Minimizing with " << nFree << " free parameters...");

  //Setup Minimizer
  TMinuit *theMinuit = new TMinuit(nFree);

  // minuit verbosity, -1 = no printing, 0 = A little printing
  int Verbosity = gLikeHAWC->GetInternalModel()->GetInternalFitVerbosity();
  theMinuit->SetPrintLevel(Verbosity);
  theMinuit->SetFCN(InternalBGFitFunc);

  double arglist[2];  // arguments to pass to the Minuit interpreter
  int flag = 0;

  arglist[0] = 0.5;   //Set for errors (1 for Chi2, 0.5 for LL)
  arglist[1] = 1;    //  unused
  theMinuit->mnexcm("SET ERR", arglist, 1, flag);

  if (flag) {
    log_warn("Minuit SET ERR failed, flag=" << flag);
    return -1;
  }


  //Setup Free Parameters
  int np = 0;
  for (unsigned n = 0; n < freeParList.size(); n++) {
    int ParId = freeParList[n].ParId;
    double ParValue = freeParList[n].FuncPointer->GetParameter(ParId);
    double ParError = freeParList[n].FuncPointer->GetParError(ParId);
    if (ParError == 0) ParError = 0.1 * ParValue;
    string ParName = freeParList[n].FuncPointer->GetParName(ParId);
    theMinuit->mnparm(np, ParName.c_str(), ParValue, ParError, 0, 0, flag);
    np++;
  }
  //BackgroundNorm...
  if (bnFit) {
    for (unsigned b = 0; b < bn_values.size(); b++) {
      double BNValue = bn_values[b];
      double BNError = bn_errors[b];
      if (BNError == 0) BNError = 0.1 * BNValue;
      string BNName = Form("BackgroundNorm_bin%d", b);
      theMinuit->mnparm(np, BNName.c_str(), BNValue, BNError, 0, 0, flag);
      np++;
    }
  }

  //****Call Minimizer
  arglist[0] = 1000;   // maxcalls
  arglist[1] = 0.1;    // tolerance

  theMinuit->mnexcm("MINIMIZE", arglist, 2, flag);  //Call the Minimizer

  if (flag) {
    log_warn("FIT ERROR: From MIGRAD, flag=" << flag);
    return -1;
  }

  //Retrieve Parameters
  np = 0;
  for (unsigned n = 0; n < freeParList.size(); n++) {
    int ParId = freeParList[n].ParId;
    double ParValue;
    double ParError;
    theMinuit->GetParameter(np, ParValue, ParError);
    freeParList[n].FuncPointer->SetParameter(ParId, ParValue);
    freeParList[n].FuncPointer->SetParError(ParId, ParError);
    np++;
  }
  //and BackgroundNorm...
  if (bnFit) {
    for (unsigned k = 0; k < calcBinVector.size(); ++k) {
      InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
      double BNValue;
      double BNError;
      theMinuit->GetParameter(np, BNValue, BNError);
      imb.BackgroundNorm() = BNValue;
      imb.BackgroundNormError() = BNError;
      np++;
    }
  }

  delete theMinuit;

  return 0;
}

/*****************************************************/

void InternalBGFitFunc(int &npar, double *gin, double &LL,
                       double *par, int iflag) {

  //npar:  Number of parameters
  //gin:   First derivitives in an array (optional) - output
  //LL:    Calculated value of the function - output
  //par:   The current parameters in an array
  //iflag: Indicates what is to be calculated

  //We always calculate LL, and never calculate gin.
  //We do not use iflag or npar

  //Iterate through Model List First Time
  //Will Call all Active and Passive Models

  /*
  //check number of free parameters
  int nFree =  //here we would have to do exactly what is doen in Minimize();
  if (nFree != npar) {
    log_fatal("Number of parameters different between gModel and Minimizer!");
  }
  */

  //Assign Parameters
  int n = 0;

  CalcBinVector &calcBinVector = gLikeHAWC->GetCalcBins();

  //BG fit: free parameters in each CalcBin:
  for (unsigned k = 0; k < calcBinVector.size(); k++) {
    InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
    FreeParameterList binfbgp = imb.GetFreeBackgroundParameterList();
    for (unsigned j = 0; j < binfbgp.size(); j++) {
      int ParId = binfbgp[j].ParId;
      binfbgp[j].FuncPointer->SetParameter(ParId, par[n]);
      n++;
    }
  }
  //Background Norm fit
  bool bnFit = gLikeHAWC->GetInternalModel()->IsBackgroundNormFree();
  if (bnFit) {
    for (unsigned k = 0; k < calcBinVector.size(); k++) {
      InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
      imb.BackgroundNorm() = par[n];
      n++;
    }
  }

  LL = -gLikeHAWC->CalcBackgroundLogLikelihood(false);
}


/*****************************************************/

int InternalTopHatMinimize() {

  int nFree = 0;
  FreeParameterList freeParList;

  CalcBinVector &calcBinVector = gLikeHAWC->GetCalcBins();

  //free BG shape parameters
  for (unsigned k = 0; k < calcBinVector.size(); ++k) {
    InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
    FreeParameterList binfbgp = imb.GetFreeBackgroundParameterList();
    nFree += binfbgp.size();
    freeParList.insert(freeParList.end(), binfbgp.begin(), binfbgp.end());
  }
  if (nFree > 0) {
    log_debug("Fitting BGModel with " << nFree << " free parameters.");
  }

  //Other free parameters:
  FreeParameterList ofp =
      gLikeHAWC->GetInternalModel()->GetFreeParameterList();
  nFree += ofp.size();
  freeParList.insert(freeParList.end(), ofp.begin(), ofp.end());
  bool updateSources = false;
  if (ofp.size() > 0) {
    log_debug("Fitting " << ofp.size() << " additional free parameters"
                  << " in InternalModel.");
    log_debug("Resetting source lists in each Minimization step to capture"
                  << " potential changes in convolution with DetectorResponse.");
    updateSources = true;
  }

  //BGNorm fit
  vector<double> bn_values;
  vector<double> bn_errors;
  bool bnFit = gLikeHAWC->GetInternalModel()->IsBackgroundNormFree();
  if (bnFit) {
    for (unsigned k = 0; k < calcBinVector.size(); ++k) {
      InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
      bn_values.push_back(imb.BackgroundNorm());
      bn_errors.push_back(imb.BackgroundNormError());
      nFree += 1;
    }
    log_debug("Fitting background normalization separately in all "
                  << nFree << " CalcBins.");
  }

  //Common Norm fit
  bool cnFit = gLikeHAWC->GetInternalModel()->IsCommonNormFree();
  if (cnFit) {
    nFree += 1;
    log_debug("Fitting CommonNorm.");
  }

  if (nFree == 0) {
    log_trace("No free parameters in InternalModel)");
    return 0;
  }

  log_debug("Minimizing with " << nFree << " free parameters...");

  //Setup Minimizer
  TMinuit *theMinuit = new TMinuit(nFree);

  // minuit verbosity, -1 = no printing, 0 = A little printing
  int Verbosity = gLikeHAWC->GetInternalModel()->GetInternalFitVerbosity();
  theMinuit->SetPrintLevel(Verbosity);
  if (updateSources) theMinuit->SetFCN(InternalTopHatFitFuncUpdateSources);
  else theMinuit->SetFCN(InternalTopHatFitFunc);

  double arglist[2];  // arguments to pass to the Minuit interpreter
  int flag = 0;

  arglist[0] = 0.5;   //Set for errors (1 for Chi2, 0.5 for LL)
  arglist[1] = 1;    //  unused
  theMinuit->mnexcm("SET ERR", arglist, 1, flag);

  if (flag) {
    log_warn("Minuit SET ERR failed, flag=" << flag);
    return -1;
  }
  //Setup Free Parameters
  //
  int np = 0;
  //free parameters
  for (unsigned n = 0; n < freeParList.size(); n++) {
    int ParId = freeParList[n].ParId;
    double ParValue = freeParList[n].FuncPointer->GetParameter(ParId);
    double ParError = freeParList[n].FuncPointer->GetParError(ParId);
    if (ParError == 0) ParError = 0.1 * ParValue;
    string ParName = freeParList[n].FuncPointer->GetParName(ParId);
    theMinuit->mnparm(np, ParName.c_str(), ParValue, ParError, 0, 0, flag);
    np++;
  }
  //BackgroundNorm...
  if (bnFit) {
    for (unsigned b = 0; b < bn_values.size(); b++) {
      double BNValue = bn_values[b];
      double BNError = bn_errors[b];
      if (BNError == 0) BNError = 0.1 * BNValue;
      string BNName = Form("BackgroundNorm_bin%d", b);
      theMinuit->mnparm(np, BNName.c_str(), BNValue, BNError, 0, 0, flag);
      np++;
    }
  }
  //and CommonNorm...
  if (cnFit) {
    double CNValue = gLikeHAWC->GetInternalModel()->CommonNorm();
    double CNError = gLikeHAWC->GetInternalModel()->CommonNormError();
    if (CNError == 0) CNError = CNValue;
    string CNName = "CommonNorm";
    theMinuit->mnparm(np, CNName.c_str(), CNValue, CNError, 0, 0, flag);
    np++;
  }

  //****Call Minimizer
  arglist[0] = 1000;   // maxcalls
  arglist[1] = 0.1;    // tolerance

  theMinuit->mnexcm("MINIMIZE", arglist, 2, flag);  //Call the Minimizer

  if (flag) {
    log_warn("FIT ERROR: From MIGRAD, flag=" << flag);
    return -1;
  }

  //Retrieve Parameters
  np = 0;
  for (unsigned n = 0; n < freeParList.size(); n++) {
    int ParId = freeParList[n].ParId;
    double ParValue;
    double ParError;
    theMinuit->GetParameter(np, ParValue, ParError);
    freeParList[n].FuncPointer->SetParameter(ParId, ParValue);
    freeParList[n].FuncPointer->SetParError(ParId, ParError);
    np++;
  }
  //and BackgroundNorm...
  if (bnFit) {
    for (unsigned k = 0; k < calcBinVector.size(); ++k) {
      InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
      double BNValue;
      double BNError;
      theMinuit->GetParameter(np, BNValue, BNError);
      imb.BackgroundNorm() = BNValue;
      imb.BackgroundNormError() = BNError;
      np++;
    }
  }
  //and CommonNorm...
  if (cnFit) {
    double CNValue;
    double CNError;
    theMinuit->GetParameter(np, CNValue, CNError);
    gLikeHAWC->GetInternalModel()->CommonNorm() = CNValue;
    gLikeHAWC->GetInternalModel()->CommonNormError() = CNError;
    np++;
  }

  delete theMinuit;

  return 0;
}

/*****************************************************/

void InternalTopHatFitFunc(int &npar, double *gin, double &LL,
                           double *par, int iflag) {

  //npar:  Number of parameters
  //gin:   First derivitives in an array (optional) - output
  //LL:    Calculated value of the function - output
  //par:   The current parameters in an array
  //iflag: Indicates what is to be calculated

  //We always calculate LL, and never calculate gin.
  //We do not use iflag or npar

  //Iterate through Model List First Time
  //Will Call all Active and Passive Models

  /*
  //check number of free parameters
  int nFree =  //here we would have to do exactly what is done in Minimize();
  if (nFree != npar) {
    log_fatal("Number of parameters different between gModel and Minimizer!");
  }
  */

  //Assign Parameters
  int n = 0;

  CalcBinVector &calcBinVector = gLikeHAWC->GetCalcBins();

  //BG fit: free parameters in each CalcBin:
  for (unsigned k = 0; k < calcBinVector.size(); k++) {
    InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
    FreeParameterList binfbgp = imb.GetFreeBackgroundParameterList();
    for (unsigned j = 0; j < binfbgp.size(); j++) {
      int ParId = binfbgp[j].ParId;
      binfbgp[j].FuncPointer->SetParameter(ParId, par[n]);
      n++;
    }
  }
  // There should be no "other free parameters",
  // otherwise use InternalFitFuncUpdateSources
  /*
  //Other free parameters:
  FreeParameterList ofp =
    gLikeHAWC->GetInternalModel()->GetFreeParameterList();
  for (unsigned j=0; j<ofp.size(); j++) {
    int ParId = ofp[j].ParId;
    ofp[j].FuncPointer->SetParameter(ParId, par[n]);
    n++;
  }
  */
  //Background Norm fit
  bool bnFit = gLikeHAWC->GetInternalModel()->IsBackgroundNormFree();
  if (bnFit) {
    for (unsigned k = 0; k < calcBinVector.size(); k++) {
      InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
      imb.BackgroundNorm() = par[n];
      n++;
    }
  }
  //Common Norm fit
  bool cnFit = gLikeHAWC->GetInternalModel()->IsCommonNormFree();
  if (cnFit) {
    gLikeHAWC->GetInternalModel()->CommonNorm() = par[n];
    n++;
  }

  LL = -gLikeHAWC->CalcTopHatLogLikelihood(gLikeHAWC->topHatCenter,
                                           gLikeHAWC->topHatRadius, false);
}

/*****************************************************/

void InternalTopHatFitFuncUpdateSources(int &npar, double *gin, double &LL,
                                        double *par, int iflag) {

  //npar:  Number of parameters
  //gin:   First derivitives in an array (optional) - output
  //LL:    Calculated value of the function - output
  //par:   The current parameters in an array
  //iflag: Indicates what is to be calculated

  //We always calculate LL, and never calculate gin.
  //We do not use iflag or npar

  //Iterate through Model List First Time
  //Will Call all Active and Passive Models

  /*
  //check number of free parameters
  int nFree =  //here we would have to do exactly what is done in Minimize();
  if (nFree != npar) {
    log_fatal("Number of parameters different between gModel and Minimizer!");
  }
  */

  //Assign Parameters
  int n = 0;

  CalcBinVector &calcBinVector = gLikeHAWC->GetCalcBins();

  //BG fit: free parameters in each CalcBin:
  for (unsigned k = 0; k < calcBinVector.size(); k++) {
    InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
    FreeParameterList binfbgp = imb.GetFreeBackgroundParameterList();
    for (unsigned j = 0; j < binfbgp.size(); j++) {
      int ParId = binfbgp[j].ParId;
      binfbgp[j].FuncPointer->SetParameter(ParId, par[n]);
      n++;
    }
  }
  //Other free parameters:
  FreeParameterList ofp =
      gLikeHAWC->GetInternalModel()->GetFreeParameterList();
  for (unsigned j = 0; j < ofp.size(); j++) {
    int ParId = ofp[j].ParId;
    ofp[j].FuncPointer->SetParameter(ParId, par[n]);
    n++;
  }
  //Background Norm fit
  bool bnFit = gLikeHAWC->GetInternalModel()->IsBackgroundNormFree();
  if (bnFit) {
    for (unsigned k = 0; k < calcBinVector.size(); k++) {
      InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
      imb.BackgroundNorm() = par[n];
      n++;
    }
  }
  //Common Norm fit
  bool cnFit = gLikeHAWC->GetInternalModel()->IsCommonNormFree();
  if (cnFit) {
    gLikeHAWC->GetInternalModel()->CommonNorm() = par[n];
    n++;
  }

  LL = -gLikeHAWC->CalcTopHatLogLikelihoodUpdateSources(
      gLikeHAWC->topHatCenter, gLikeHAWC->topHatRadius, false);
}


/*****************************************************/

int InternalTopHatBGMinimize() {

  int nFree = 0;
  FreeParameterList freeParList;

  CalcBinVector &calcBinVector = gLikeHAWC->GetCalcBins();

  //BG fit
  //free parameters in each CalcBin:
  for (unsigned k = 0; k < calcBinVector.size(); ++k) {
    InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
    FreeParameterList binfbgp = imb.GetFreeBackgroundParameterList();
    nFree += binfbgp.size();
    freeParList.insert(freeParList.end(), binfbgp.begin(), binfbgp.end());
  }
  //BGNorm fit
  vector<double> bn_values;
  vector<double> bn_errors;
  bool bnFit = gLikeHAWC->GetInternalModel()->IsBackgroundNormFree();
  if (bnFit) {
    for (unsigned k = 0; k < calcBinVector.size(); ++k) {
      InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
      bn_values.push_back(imb.BackgroundNorm());
      bn_errors.push_back(imb.BackgroundNormError());
      nFree += 1;
    }
    log_debug("Fitting background normalization separately in all "
                  << nFree << " CalcBins.");
  }

  if (nFree == 0) {
    log_trace("No free parameters in BG-Model)");
    return 0;
  }

  log_debug("Fitting BGModel with " << nFree << " free parameters.");

  log_debug("Minimizing with " << nFree << " free parameters...");

  //Setup Minimizer
  TMinuit *theMinuit = new TMinuit(nFree);

  // minuit verbosity, -1 = no printing, 0 = A little printing
  int Verbosity = gLikeHAWC->GetInternalModel()->GetInternalFitVerbosity();
  theMinuit->SetPrintLevel(Verbosity);
  theMinuit->SetFCN(InternalTopHatBGFitFunc);

  double arglist[2];  // arguments to pass to the Minuit interpreter
  int flag = 0;

  arglist[0] = 0.5;   //Set for errors (1 for Chi2, 0.5 for LL)
  arglist[1] = 1;    //  unused
  theMinuit->mnexcm("SET ERR", arglist, 1, flag);

  if (flag) {
    log_warn("Minuit SET ERR failed, flag=" << flag);
    return -1;
  }


  //Setup Free Parameters
  int np = 0;
  for (unsigned n = 0; n < freeParList.size(); n++) {
    int ParId = freeParList[n].ParId;
    double ParValue = freeParList[n].FuncPointer->GetParameter(ParId);
    double ParError = freeParList[n].FuncPointer->GetParError(ParId);
    if (ParError == 0) ParError = 0.1 * ParValue;
    string ParName = freeParList[n].FuncPointer->GetParName(ParId);
    theMinuit->mnparm(np, ParName.c_str(), ParValue, ParError, 0, 0, flag);
    np++;
  }
  //BackgroundNorm...
  if (bnFit) {
    for (unsigned b = 0; b < bn_values.size(); b++) {
      double BNValue = bn_values[b];
      double BNError = bn_errors[b];
      if (BNError == 0) BNError = 0.1 * BNValue;
      string BNName = Form("BackgroundNorm_bin%d", b);
      theMinuit->mnparm(np, BNName.c_str(), BNValue, BNError, 0, 0, flag);
      np++;
    }
  }

  //****Call Minimizer
  arglist[0] = 1000;   // maxcalls
  arglist[1] = 0.1;    // tolerance

  theMinuit->mnexcm("MINIMIZE", arglist, 2, flag);  //Call the Minimizer

  if (flag) {
    log_warn("FIT ERROR: From MIGRAD, flag=" << flag);
    return -1;
  }

  //Retrieve Parameters
  np = 0;
  for (unsigned n = 0; n < freeParList.size(); n++) {
    int ParId = freeParList[n].ParId;
    double ParValue;
    double ParError;
    theMinuit->GetParameter(np, ParValue, ParError);
    freeParList[n].FuncPointer->SetParameter(ParId, ParValue);
    freeParList[n].FuncPointer->SetParError(ParId, ParError);
    np++;
  }
  //and BackgroundNorm...
  if (bnFit) {
    for (unsigned k = 0; k < calcBinVector.size(); ++k) {
      InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
      double BNValue;
      double BNError;
      theMinuit->GetParameter(np, BNValue, BNError);
      imb.BackgroundNorm() = BNValue;
      imb.BackgroundNormError() = BNError;
      np++;
    }
  }

  delete theMinuit;

  return 0;
}

/*****************************************************/

void InternalTopHatBGFitFunc(int &npar, double *gin, double &LL,
                             double *par, int iflag) {

  //npar:  Number of parameters
  //gin:   First derivitives in an array (optional) - output
  //LL:    Calculated value of the function - output
  //par:   The current parameters in an array
  //iflag: Indicates what is to be calculated

  //We always calculate LL, and never calculate gin.
  //We do not use iflag or npar

  //Iterate through Model List First Time
  //Will Call all Active and Passive Models

  /*
  //check number of free parameters
  int nFree =  //here we would have to do exactly what is doen in Minimize();
  if (nFree != npar) {
    log_fatal("Number of parameters different between gModel and Minimizer!");
  }
  */

  //Assign Parameters
  int n = 0;

  CalcBinVector &calcBinVector = gLikeHAWC->GetCalcBins();

  //BG fit: free parameters in each CalcBin:
  for (unsigned k = 0; k < calcBinVector.size(); k++) {
    InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
    FreeParameterList binfbgp = imb.GetFreeBackgroundParameterList();
    for (unsigned j = 0; j < binfbgp.size(); j++) {
      int ParId = binfbgp[j].ParId;
      binfbgp[j].FuncPointer->SetParameter(ParId, par[n]);
      n++;
    }
  }
  //Background Norm fit
  bool bnFit = gLikeHAWC->GetInternalModel()->IsBackgroundNormFree();
  if (bnFit) {
    for (unsigned k = 0; k < calcBinVector.size(); k++) {
      InternalModelBin &imb = calcBinVector[k]->GetInternalModelBin();
      imb.BackgroundNorm() = par[n];
      n++;
    }
  }

  LL = -gLikeHAWC->CalcTopHatBackgroundLogLikelihood
      (gLikeHAWC->topHatCenter, gLikeHAWC->topHatRadius, false);
} 



