/*!
 * @file PointSourceDetectorResponse.cc
 * @author Robert Lauer
 * @date 4 February 2015
 * @brief Implementation of DetectorResponse access for a point source
 * @version $Id: PointSourceDetectorResponse.cc 40101 2017-08-09 03:01:16Z imc $
 */

#include <liff/BinList.h>
#include <liff/PointSourceDetectorResponse.h>

#include <hawcnest/HAWCUnits.h>

#define PSF_LIM 10.0 //Upper limit of the PSF (or DSF) in degrees ("infinity")

using namespace std;
using namespace threeML;
using namespace HAWCUnits;

int PointSourceDetectorResponse::SetModel
    (ModelInterface &mi, bool detResFree) {

  mi_ = mi;
  double ra, dec;
  mi_.getPointSourcePosition(sourceId_, &ra, &dec);

  int moved = 0;
  if ((fabs(ra - ra_) > 1e-6) || (fabs(dec - dec_) > 1e-6)) {
    moved = 1;
    skypos_ = SkyPos(ra, dec);
    log_debug("Setting position");
    SetSkyPos(skypos_);
  }
  //unlike before, now always ReweightEnergies, in case spectrum changed
  log_debug("Call ReweightEnergies");
  ReweightEnergies();

  // reset PSF cache if DetectorResponse has free parameters
  if (detResFree) {
    log_debug("Clearing pixelated-PSF cache due to free DetRes parameters.");
    pixelatedPsf_.clear();
    deltaFunctionPSF_.clear();
  }

  return moved;
}

void PointSourceDetectorResponse::SetSkyPos(SkyPos pos) {
  ra_ = pos.RA();
  dec_ = pos.Dec();
  log_debug("Changing position of source ID " << sourceId_
                << " to RA=" << ra_ << " , Dec=" << dec_);
  int newdbid1 = dr_.GetDecBinIndex(dec_);
  double simdec1 = dr_.GetDecBinMap()[newdbid1].simDec_;
  int newdbid2 = newdbid1;
  if (dec_ < simdec1 && newdbid1 > 0) {
    newdbid2 = newdbid1 - 1;
  }
  else if (dec_ > simdec1 && newdbid1 < ((int) dr_.GetDecBinMap().size() - 1)) {
    newdbid2 = newdbid1 + 1;
  }
  double simdec2 = dr_.GetDecBinMap()[newdbid2].simDec_;
  if ((newdbid1 != decBinId1_) || (newdbid2 != decBinId2_)) {
    //clear PSF cache and stuff only when main dec band changes
    if (newdbid1 != decBinId1_) {
      DecBin db = dr_.GetDecBinMap()[decBinId1_];
      decLowerEdge_ = db.lowerEdge_;
      decUpperEdge_ = db.upperEdge_;
      //log_debug("Clearing pixelated-PSF cache due to new dec-bin.");
      //pixelatedPsf_.clear();
      deltaFunctionPSF_.clear();
    }
    if ((newdbid1 == decBinId2_) && (newdbid2 == decBinId1_)) {
      //don't reweight energies if dec-bands are just flipped
      decBinId1_ = newdbid1;
      decBinId2_ = newdbid2;
    }
    else {
      //Reweight function includes re-interpolation of dec-bands
      decBinId1_ = newdbid1;
      decBinId2_ = newdbid2;
      log_debug("Call ReweightEnergies");
      ReweightEnergies();
    }
  }
  
  //but you have to re-interpolate:
  if (decBinId1_ == decBinId2_) {
    w1_ = 1.;
    w2_ = 0.;
  } else {
    w1_ = (dec_ - simdec2) / (simdec1 - simdec2);
    w2_ = (dec_ - simdec1) / (simdec2 - simdec1);
  }
  log_trace("Dec bin id: " << decBinId1_);
}

void PointSourceDetectorResponse::ReweightEnergies() {
  //reweight energies for all nHit bins if point source
  log_debug("Reweighting energies for source ID " << sourceId_);
  AnalysisBinMap nhbmap = dr_.GetAnalysisBinMap();
  AnalysisBinMap::iterator nh = nhbmap.begin();
  //get log-energy histogram binning from first analysis bin and use
  //it to query the MI for fuxes
  ResponseBinPtr rb = dr_.GetBin(decBinId1_, nh->first);
  vector<double> energies = rb->GetLogEnBins();
  //convert to MeV
  for (vector<double>::iterator i = energies.begin();
       i != energies.end(); ++i) {
    *i = pow(10., *i + 6.);
    log_trace("energies [MeV]: " << *i)
  }
  vector<double> fluxes = mi_.getPointSourceFluxes(sourceId_, energies);
  //convert to TeV^-1 cm^-1 s^-1
  for (vector<double>::iterator i = fluxes.begin();
       i != fluxes.end(); i++) {
    (*i) *= 1e6;
    log_trace("fluxes [TeV^-1 cm^-2 s^-1]: " << *i)
  }
  for (nh = nhbmap.begin(); nh != nhbmap.end(); ++nh) {
    log_trace("Source ID " << sourceId_ << " reweighting ResponseBin(d="
                  << decBinId1_ << ",n=" << nh->first << ")");
    rb = dr_.GetBin(decBinId1_, nh->first);
    log_trace("Call ReweightEnergies");
    rb->ReweightEnergies(fluxes);
    log_trace("Source ID " << sourceId_ << " reweighting ResponseBin(d="
                  << decBinId2_ << ",n=" << nh->first << ")");
    rb = dr_.GetBin(decBinId2_, nh->first);
    log_trace("Call ReweightEnergies");
    rb->ReweightEnergies(fluxes);
  }
}

TF1Ptr PointSourceDetectorResponse::GetPsfFunction(const BinName& nhbin) {
  return dr_.GetBin(decBinId1_, nhbin)->GetPsfFunction();
}

double PointSourceDetectorResponse::GetExpectedSignal(const BinName& nhbin) {
  //energy spectra in all bins already reweighted
  ResponseBinPtr rb1 = dr_.GetBin(decBinId1_, nhbin);
  ResponseBinPtr rb2 = dr_.GetBin(decBinId2_, nhbin);
  return w1_ * rb1->GetExpectedSignal() + w2_ * rb2->GetExpectedSignal();
}

double PointSourceDetectorResponse::GetSmearedSignal(
    const double distance, const double pixelArea, const BinName& nhbin) {
  const BinPair bin(nhbin, decBinId1_);
  if (pixelatedPsf_.find(bin) == pixelatedPsf_.end()) {
    pixelatedPsf_[bin] = CalculatePixelatedPsf(pixelArea, nhbin);
  }
  if (distance > PSF_LIM) {

    return 0.;

  }
  double relativeDensity = (pixelatedPsf_[bin]).GetBinContent(
      (pixelatedPsf_[bin]).FindBin(distance));
  return relativeDensity * GetExpectedSignal(nhbin);

}

TH1D PointSourceDetectorResponse::CalculatePixelatedPsf(
    const double pixelArea, const BinName& nhbin) {

  TF1Ptr DiffPSF = GetPsfFunction(nhbin);
  //find max radius where PSF goes to zero
  double radiusLimit = DiffPSF->GetX(0., 1e-8, PSF_LIM);
  log_debug("Radius Limit: " << radiusLimit);
  if (fabs(radiusLimit - 1e-8) < 1e-5) {
    //check that it hasn't just found the min values,
    //where the phase space factor makes the PSF zero
    radiusLimit = PSF_LIM;
  }
  double TotalCounts = DiffPSF->Integral(0., radiusLimit);

  double start = 0.;
  double stop = PSF_LIM;
  //create 5 times as many interpolation points as pixel-distance
  int nBins = (int) ceil(5. * stop / sqrt(pixelArea));

  if (radiusLimit <= sqrt(pixelArea)) {
    log_debug("PSF extent smaller than pixel size, no PSF convolution.");
    nBins = (int) ceil(2. * stop / sqrt(pixelArea));
    deltaFunctionPSF_[nhbin] = true;
  }
  else {
    deltaFunctionPSF_[nhbin] = false;
  }

  const string name = Form(
    "pixPSf_source%d_bin%s_dec%d", sourceId_, nhbin.c_str(), decBinId1_
  );
  log_debug("Creating pixelated-PSF for nh bin " << nhbin << " dec bin " << decBinId1_ << " with "
                << nBins << " radial bins.");

  TH1D pixPSF(name.c_str(), name.c_str(), nBins, start, stop);

  for (int k = 1; k <= nBins; ++k) {
    double center = pixPSF.GetBinCenter(k);
    double min = center - sqrt(pixelArea) / 2.;
    double max = center + sqrt(pixelArea) / 2.;
    if (min < 0) min = 0;
    if (max > radiusLimit) max = radiusLimit;
    if (min >= radiusLimit) {
      pixPSF.SetBinContent(k, 0.);
    }
    else {
      double RingCounts = DiffPSF->Integral(min, max);
      log_trace("  Integral of PSF for pixel at radius " << center
                    << ": " << RingCounts);
      double RingArea = pi * (max * max - min * min);
      double RelativeDensity = RingCounts * pixelArea / RingArea / TotalCounts;
      if (RelativeDensity > 1) RelativeDensity = 1;
      log_trace("  Relative density in pixel at radius " << center
                    << ": " << RelativeDensity);
      pixPSF.SetBinContent(k, RelativeDensity);
    }
  }
  return pixPSF;
}

bool PointSourceDetectorResponse::IsPSFDeltaFunction(
    const double pixelArea, const BinName& nhbin) {
  const BinPair bin(nhbin, decBinId1_);
  if (pixelatedPsf_.find(bin) == pixelatedPsf_.end()) {
    pixelatedPsf_[bin] = CalculatePixelatedPsf(pixelArea, nhbin);
  }
  return deltaFunctionPSF_[nhbin];
}

double PointSourceDetectorResponse::ComputeDSF(double *radius, double *parameters){

  static TF1 thetaIntegral("thetaIntegral",
                           "exp(-([1]^2 + x^2)/2/[0]) * TMath::BesselI0([1]*x/[0]) * x"
                           ,0,PSF_LIM);
  static const int npoints = 100;
  static bool firstCall = true;
  static double pointsX[npoints];
  static double pointsW[npoints];
  if(firstCall){
    thetaIntegral.CalcGaussLegendreSamplingPoints(npoints, pointsX, pointsW, 1e-10);
  }
  firstCall = false;
  
  double &r = radius[0]; //radius to evaluate
  double &amplitude = parameters[0];  
  double &frac1 = parameters[1];  
  double &var1 = parameters[2];  
  double &var2 = parameters[3];  
  double &R = parameters[4]; //radius of the disc  

  double integral1 = 0;
  double integral2 = 0;
  
  if(frac1==0){
    
    integral1 = 0;
    
  }else if(r*r/var1 > 100){//Use asymtotic behaviour

    integral1 = var1 * TMath::Erfc( (r-R) / sqrt(2*var1) ) / 2;

  }else{//There's no analytic solution in general, so integrate by gaussian quadrature

    thetaIntegral.SetParameter(0, var1);
    thetaIntegral.SetParameter(1, r);
    
    integral1 = thetaIntegral.IntegralFast(npoints, pointsX, pointsW, 0, R);
    
  }

  if(frac1==1){

    integral2 = 0;
    
  }else if(r*r/var2 > 100){//Use asymtotic behaviour

    integral2 = var2 * TMath::Erfc( (r-R) / sqrt(2*var2) ) / 2;

  }else{//There's no analytic solution in general, so integrate by gaussian quadrature

    thetaIntegral.SetParameter(0, var2);
    thetaIntegral.SetParameter(1, r);

    integral2 = thetaIntegral.IntegralFast(npoints, pointsX, pointsW, 0, R);
    
  }

  //The integration fails for extreme cases, but those are precisely the ones we can use the asymtotic bahaviour asymtotic for. The condition r/sigma > 10 catches most of them anyways 
  if(!isfinite(integral1))
    integral1 = var1 * TMath::Erfc( (r-R) / sqrt(2*var1) ) / 2;
  if(!isfinite(integral2))
    integral2 = var1 * TMath::Erfc( (r-R) / sqrt(2*var2) ) / 2;
  
  return amplitude * ( frac1 * integral1 + (1-frac1) * integral2 ) * r;
  
}

bool PointSourceDetectorResponse::IsPSFDoubleGaussian(const BinName& nhbin){

  const TF1Ptr PSF = dr_.GetBin(decBinId1_, nhbin)->GetPsfFunction();

  static const string expFormulaROOT5 =
    "[0]*(x*(([1]*exp(-(x*((x/2)/[2]))))+((1-[1])*exp(-(x*((x/2)/[3]))))))";
  static const string expFormulaROOT6 =
    "[p0]*(x*(([p1]*exp(-(x*((x/2)/[p2]))))+((1-[p1])*exp(-(x*((x/2)/[p3]))))))"
    ;

  const string formula = PSF->GetExpFormula().Data();
  return formula == expFormulaROOT5 || formula == expFormulaROOT6;

}

TF1Ptr PointSourceDetectorResponse::GetDsfFunction(const double discR,
                                                   const BinName& nhbin){

  const TF1Ptr PSF = dr_.GetBin(decBinId1_, nhbin)->GetPsfFunction();

  if (!IsPSFDoubleGaussian(nhbin)){
    log_fatal("It seems PSF is not a double gaussian. This currently only works for double gaussians. Fix me!");
  }
  
  const TF1Ptr DSF = TF1Ptr(
    new TF1(
      "dsf", this, &PointSourceDetectorResponse::ComputeDSF, 0, PSF_LIM, 5
    )
  );
  for (unsigned i = 0; i < 4; ++i)
    DSF->SetParameter(i, PSF->GetParameter(i));
  DSF->SetParameter(4, discR);

  return DSF;
  
}

TH1D PointSourceDetectorResponse::CalculatePixelatedDsf(
  const double discR, const double pixelArea, const BinName& nhbin) {

  const TF1Ptr DSF = GetDsfFunction(discR, nhbin);

  const double TotalCounts = DSF->Integral(0, PSF_LIM);
  
  const double pixelSize = sqrt(pixelArea);
  
  const int nBins = ceil(5. * PSF_LIM / pixelSize);

  const string name = Form(
    "pixDSf_source%d_bin%s_dec%d", sourceId_, nhbin.c_str(), decBinId1_
  );
  
  TH1D pixDSF(name.c_str(), name.c_str(), nBins, 0, PSF_LIM);

  for (int k = 1; k <= nBins; ++k) {
    const double centerR = pixDSF.GetBinCenter(k);
    const double minR = max(0., centerR - pixelSize / 2.);
    const double maxR = min(PSF_LIM, centerR + pixelSize / 2.);

    const double RingCounts = DSF->Integral(minR, maxR);

    const double RingArea = pi * (maxR * maxR - minR * minR);

    const double RelativeDensity =
      RingCounts * pixelArea / RingArea / TotalCounts;

    pixDSF.SetBinContent(k, RelativeDensity);
    
  }
  
  return pixDSF;
  
}
