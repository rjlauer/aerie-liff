/*!
 * @file ExtendedSourceDetectorResponse.cc
 * @author Robert Lauer
 * @date 4 February 2015
 * @brief Implementation of DetectorResponse access for an extended source
 * @version $Id: ExtendedSourceDetectorResponse.cc 36120 2016-12-02 10:39:12Z samm $
 */
 
#include <utility>

#include <liff/BinList.h>
#include <liff/ExtendedSourceDetectorResponse.h>

#include <hawcnest/HAWCUnits.h>

using namespace std;
using namespace threeML;
using namespace HAWCUnits;

void ExtendedSourceDetectorResponse::SetModel
    (ModelInterface &mi, bool reconvolute) {

  mi_ = mi;
  decBinId_.clear();
  decLowerEdge_.clear();
  decUpperEdge_.clear();
  log_debug("Getting Boundaries for Extended sources " << sourceId_);
  mi_.getExtendedSourceBoundaries(sourceId_, &minra_, &maxra_, &mindec_, &maxdec_);
  log_debug("minra: " << minra_ << " maxra: " << maxra_ << " mindec: " << mindec_ << " maxdec: " << maxdec_);

  log_debug("Getting NHit bin maps");
  AnalysisBinMap nhbmap = dr_.GetAnalysisBinMap();
  log_debug("Source dec size: " << mindec_ << " " << maxdec_)

  for (int i = dr_.GetDecBinIndex(mindec_);
       i <= dr_.GetDecBinIndex(maxdec_); i++) {
    decBinId_.push_back(i);
    DecBin db = dr_.GetDecBinMap()[i];
    decLowerEdge_.push_back(db.lowerEdge_);
    decUpperEdge_.push_back(db.upperEdge_);
    log_debug("Dec Bounds: " << db.lowerEdge_ << " " << db.upperEdge_);
    log_debug("Dec Bounds: " << i << " " << decLowerEdge_[i] << " " << decUpperEdge_[i]);
  }
  numRegions_ = (int) decBinId_.size();
  log_debug("Number of dec bands: " << numRegions_);

  // re-convolute extended source with PSF, otherwise only re-scale the flux
  if (reconvolute) {
    convolutedExpectedSignalMap_.clear();
    prevCount_.clear();
  } else if (convolutedExpectedSignalMap_.size() > 0) {
    RescaleCounts();
  }
}

void ExtendedSourceDetectorResponse::ConvolutePSF(const BinName& nhbin,
                                                  rangeset<int> &roiPix) {

  if (prevCount_.find(nhbin) != prevCount_.end()) prevCount_.erase(nhbin);

  arr<double> tempArr(nside_ * nside_ * 12);
  tempArr.fill(0.);
  Healpix_Map<double> tempMap(tempArr, RING);
  //healpix pixel size in RA on the equator
  //nside_ only gets changed after ResetSources, so it is safe to assume it is a constant.
  double dGrid = 90. / nside_;
  if (nside_>1000) dGrid = 90. / 512;

  if (positions_.empty()) GetPositions(nside_);

  if (!healpixIds_.size()) {
    for (int idDec = 0; idDec < gridDec_; ++idDec) {
      double itDec = positions_[idDec * gridRA_].second;
      for (int idRA = 0; idRA < gridRA_; ++idRA) {
        double itRA = (positions_[idDec * gridRA_ + idRA]).first;
        //no need to get expected signal outside the extended source
        if (itDec >= mindec_ && itDec <= maxdec_ && ((minra_ <= maxra_ && itRA >= minra_ && itRA <= maxra_) ||
            (minra_ > maxra_ && (itRA >= minra_ || itRA <= maxra_))) &&
            mi_.isInsideAnyExtendedSource(itRA, itDec)) {
          double tempCount = GetExpectedSignal(nhbin, itRA, itDec);
          fftwIn_[idRA * gridDec_ + idDec] = tempCount;

          bool pixelFound = false;
          if (!pixelFound && tempCount > 1e-30) { //to prevent fluctuation around 0 due to double precision
            prevCount_[nhbin] = make_pair(SkyPos(itRA, itDec), tempCount);
            pixelFound = true;
          }
        } else {
          fftwIn_[idRA * gridDec_ + idDec] = 0.;
        }
      }
    }

    fftw_execute(fftwFP_);

    int nGrid = gridRA_ * gridDec_;
    double centerDec = (positions_[0].second+positions_[gridDec_*(gridRA_-1)].second)/2.;
    double centerRA  = (positions_[gridRA_/2-1].first+positions_[gridRA_/2].first)/2.;
    if (fabs(positions_[gridRA_/2-1].first-positions_[gridRA_/2].first)>180.) {
      if (centerRA>180.) {
        centerRA -= 180.;
      } else {
        centerRA += 180.;
      }
    }
    int centerDecBinIndex = dr_.GetDecBinIndex(centerDec);

    const BinPair bin(nhbin, centerDecBinIndex);
    if (pixelatedFTPsf_.find(bin) == pixelatedFTPsf_.end()) {
      log_debug("Calculating pixelatedFTPsf for NH bin " << nhbin << " Dec bin " << centerDecBinIndex);
      pixelatedFTPsf_[bin] = CalculatePixelatedFTPsf(nhbin, centerRA, centerDec, dGrid, nGrid);
    }

    //Multiply by the Fourier transformation of PSF
    for (int idDec = 0; idDec < gridDec_ / 2 + 1; ++idDec) {
      for (int idRA = 0; idRA < gridRA_; ++idRA) {
        double psfFT = (pixelatedFTPsf_[bin]).first.GetBinContent(
            (pixelatedFTPsf_[bin]).first.FindBin(
                double(idDec) / gridDec_)) *
            ((pixelatedFTPsf_[bin]).first.GetBinContent(
                (pixelatedFTPsf_[bin]).first.FindBin(
                    double(idRA) / gridRA_)) +
                (pixelatedFTPsf_[bin]).first.GetBinContent(
                    (pixelatedFTPsf_[bin]).first.FindBin(
                        double(gridRA_ - idRA) / gridRA_)));

        psfFT += (pixelatedFTPsf_[bin]).second.GetBinContent(
            (pixelatedFTPsf_[bin]).second.FindBin(
                double(idDec) / gridDec_)) *
            ((pixelatedFTPsf_[bin]).second.GetBinContent(
                (pixelatedFTPsf_[bin]).second.FindBin(
                    double(idRA) / gridRA_)) +
                (pixelatedFTPsf_[bin]).second.GetBinContent(
                    (pixelatedFTPsf_[bin]).second.FindBin(
                        double(gridRA_ - idRA) / gridRA_)));

        fftwOut_[idRA * (gridDec_ / 2 + 1) + idDec][0] *= psfFT;
        fftwOut_[idRA * (gridDec_ / 2 + 1) + idDec][1] *= psfFT;
      }
    }

    fftw_execute(fftwBP_);

    //Interpolate the result in Healpix pixels
    for (unsigned k = 0; k < roiPix.size(); ++k) {
      for (int j = roiPix.ivbegin(k); j < roiPix.ivend(k); ++j) {

        SkyPos point(tempMap.pix2ang(j));
        double tempRA = point.RA();
        double tempDec = point.Dec();
        double dRA = dGrid / cos(tempDec * HAWCUnits::pi / 180.);
        double hWidthRA = dRA * (gridRA_ / 2 - 0.5);
        int indexRA1 = -1;
        double interpRA1;

        if (fabs(tempRA - centerRA) <= hWidthRA) {
          indexRA1 = (int) ((tempRA - centerRA + hWidthRA) / dRA);
          interpRA1 = (tempRA - centerRA + hWidthRA) / dRA - indexRA1;
        } else if (fabs(tempRA + 360. - centerRA) <= hWidthRA) {
          indexRA1 = (int) ((tempRA + 360. - centerRA + hWidthRA) / dRA);
          interpRA1 = (tempRA + 360. - centerRA + hWidthRA) / dRA - indexRA1;
        } else if (fabs(tempRA - 360. - centerRA) <= hWidthRA) {
          indexRA1 = (int) ((tempRA - 360. - centerRA + hWidthRA) / dRA);
          interpRA1 = (tempRA - 360. - centerRA + hWidthRA) / dRA - indexRA1;
        }

        int indexDec1 = int((tempDec - centerDec) / dGrid + gridDec_ / 2 - 0.5);
        double interpDec1 = (tempDec - centerDec) / dGrid + gridDec_ / 2 - 0.5 - indexDec1;

        if (indexRA1 >= 0 && indexRA1 < gridRA_ - 1 && indexDec1 >= 0 && indexDec1 < gridDec_ - 1) {
          tempMap[j] = fftwIn_[indexRA1 * gridDec_ + indexDec1] * (1. - interpRA1) * (1. - interpDec1) +
              fftwIn_[indexRA1 * gridDec_ + indexDec1 + 1] * (1. - interpRA1) * interpDec1 +
              fftwIn_[(indexRA1 + 1) * gridDec_ + indexDec1] * interpRA1 * (1. - interpDec1) +
              fftwIn_[(indexRA1 + 1) * gridDec_ + indexDec1 + 1] * interpRA1 * interpDec1;
        }
      }
    }
  } else {
    double minDec = 90.;
    double maxDec = -90.;

    for (unsigned k = 0; k < healpixIds_.size(); ++k) {
      for (int j = healpixIds_.ivbegin(k); j < healpixIds_.ivend(k); ++j) {
        SkyPos point(tempMap.pix2ang(j));
        minDec = min(minDec, point.Dec());
        maxDec = max(maxDec, point.Dec());
        if (((minra_ <= maxra_ && point.RA() >= minra_ && point.RA() <= maxra_) ||
            (minra_ > maxra_ && (point.RA() >= minra_ || point.RA() <= maxra_)))) {
          double tempCount = GetExpectedSignal(nhbin, point.RA(), point.Dec());
          tempMap[j] = tempCount;
          log_debug("ExpSig: " << tempMap[j]);

          bool pixelFound = false;
          if (!pixelFound && tempCount > 1e-30) { //to prevent fluctuation around 0 due to double precision
            prevCount_[nhbin] = make_pair(SkyPos(point.RA(), point.Dec()), tempCount);
            pixelFound = true;
          }
        }
      }
    }

    double centerDec = (minDec + maxDec) / 2.;

    Alm<xcomplex<double> > alm(nside_ * 2, nside_ * 2);
    map2alm_iter(tempMap, alm, 3);

    //Only use one PSF for the whole region
    TF1Ptr ExtPSF = GetPsfFunction(nhbin, minra_, centerDec);
    string expFormulaROOT5 = "[0]*(x*(([1]*exp(-(x*((x/2)/[2]))))+((1-[1])*exp(-(x*((x/2)/[3]))))))";
    string expFormulaROOT6 = "[p0]*(x*(([p1]*exp(-(x*((x/2)/[p2]))))+((1-[p1])*exp(-(x*((x/2)/[p3]))))))";
    if ((ExtPSF->GetExpFormula() != expFormulaROOT5) && (ExtPSF->GetExpFormula() != expFormulaROOT6)) {
      log_warn("The PSF function in detector response file: ");
      log_warn(ExtPSF->GetExpFormula());
      log_warn("is different from expected: ");
      log_warn("[0]*(x*(([1]*exp(-(x*((x/2)/[2]))))+((1-[1])*exp(-(x*((x/2)/[3]))))))");
      log_warn("Check the PSF expression");
    }

    if (ExtPSF->GetNpar() != 4) {
      log_fatal("Is the PSF in detector response file a double Gaussian?");
    }

    double sA = sqrt(ExtPSF->GetParameter(2));
    double sB = sqrt(ExtPSF->GetParameter(3));
    double A = ExtPSF->GetParameter(1) * sA * sA /
        (ExtPSF->GetParameter(1) * sA * sA + (1 - ExtPSF->GetParameter(1)) * sB * sB);
    log_debug("sA: " << sA << " sB: " << sB << " A: " << A);

    Alm<xcomplex<double> > alm2(nside_ * 2, nside_ * 2);
    alm2 = alm;

    smoothWithGauss(alm, sA * degree * 2.3548);
    smoothWithGauss(alm2, sB * degree * 2.3548);

    alm.Scale(A);
    alm2.Scale(1. - A);
    alm.Add(alm2);
    alm2map(alm, tempMap);
  }

  convolutedExpectedSignalMap_.insert(
    pair<BinName, SkyMap<double> >(nhbin, SkyMap<double>(tempMap, roiPix))
  );
}

void ExtendedSourceDetectorResponse::RescaleCounts() {

  for (MapMap::iterator it = convolutedExpectedSignalMap_.begin();
       it != convolutedExpectedSignalMap_.end(); ++it) {
    if (prevCount_.find(it->first) == prevCount_.end()) {
      log_warn("Previous count for nhbin " << it->first << " not found");
      log_warn("Will reconvolute the model map.");
      convolutedExpectedSignalMap_.erase(it);
    } else {
      const double tempCount = GetExpectedSignal(
        it->first,
        prevCount_[it->first].first.RA(),
        prevCount_[it->first].first.Dec()
      );
      if (tempCount != prevCount_[it->first].second) {
        if (tempCount <= 1e-30) {
          log_warn("New count went from positive to negative (or zero) in bin "
                   << it->first);
          log_warn("Will reconvolute the model map.");
          convolutedExpectedSignalMap_.erase(it);
        } else {
          it->second.Scale(tempCount / prevCount_[it->first].second);
        }
        prevCount_[it->first].second = tempCount;
      }
    }
  }
}


double ExtendedSourceDetectorResponse::GetExtendedSourceConvolutedSignal(
  const BinName& nhbin, const int nside, rangeset<int> &roiPix,
  const int healpixId) {

  nside_ = nside;

  if (convolutedExpectedSignalMap_.find(nhbin) == convolutedExpectedSignalMap_.end()) {
    ConvolutePSF(nhbin, roiPix);
  }

  if (convolutedExpectedSignalMap_[nhbin].Nside() == nside) {
    return convolutedExpectedSignalMap_[nhbin][healpixId];
  } else {
    log_warn("We should not get here, but if happening,");
    log_warn("it means the nside used in PSF convolution ("<<nside_<<")");
    log_warn("is different from the one in data ("<<nside<<").");
    log_warn("The code will be very slow.");
    Healpix_Map<double> tempMap(nside, RING, SET_NSIDE);
    int tempPixel = (convolutedExpectedSignalMap_[nhbin]).ang2pix(tempMap.pix2ang(healpixId));
    return convolutedExpectedSignalMap_[nhbin][tempPixel];
  }
}


double ExtendedSourceDetectorResponse::GetDecRegionLowerEdge(int regionId)
const {
  checkRegionId(regionId);
  return decLowerEdge_[regionId];
}


double ExtendedSourceDetectorResponse::GetDecRegionUpperEdge(int regionId)
const {
  checkRegionId(regionId);
  return decUpperEdge_[regionId];
}


TF1Ptr ExtendedSourceDetectorResponse::GetPsfFunction(
    const BinName& nhbin, const double ra, const double dec) {
  if ((dec < -90) || (dec > 90)) {
    log_fatal("Invalid dec [degrees] coordinate provided: " << dec);
  }
  for (int i = decBinId_[0]; i <= decBinId_[numRegions_ - 1]; ++i) {
    if ((dec < decUpperEdge_[i - decBinId_[0]]) && (dec >= decLowerEdge_[i - decBinId_[0]])) {
      return dr_.GetBin(i, nhbin)->GetPsfFunction();
    }
  }
  log_info("Query for PSF outside of boundaries of extended source " << sourceId_);
  return dr_.GetBin(dr_.GetDecBinIndex(dec), nhbin)->GetPsfFunction();
}

//Next function
double ExtendedSourceDetectorResponse::GetExpectedSignal(
    const BinName& nhbin, const double ra, const double dec) {
  int decb1 = 0;
  int decb2 = 0;
  double dec1 = 0.0; //for declination centers
  double dec2 = 0.0;

  ResponseBinPtr rb1;
  ResponseBinPtr rb2;
  log_trace("Pos: " << ra << "," << dec);
  for (int i = decBinId_[0]; i <= decBinId_[numRegions_ - 1]; ++i) {
    if ((dec < decUpperEdge_[i - decBinId_[0]]) && (dec >= decLowerEdge_[i - decBinId_[0]])) {
      rb1 = dr_.GetBin(i, nhbin);
      rb2 = rb1;
      decb1 = i;
      decb2 = i;
      dec1 = dr_.GetDecBinMap()[i].simDec_;
      if (dec < dec1 && decb1 > 0) {
        decb2 = decb1 - 1;
        rb2 = dr_.GetBin(decb2, nhbin);
      }
      if (dec > dec1 && decb1 < int(dr_.GetDecBinMap().size()) - 1) {
        decb2 = decb1 + 1;
        rb2 = dr_.GetBin(decb2, nhbin);
      }
      dec2 = dr_.GetDecBinMap()[decb2].simDec_;
      break;
    }
  }
  double w1, w2; // weights for the interpolation
  if (dec1 == dec2) {
    w1 = 1.;
    w2 = 0.;
  } else {
    w1 = (dec - dec2) / (dec1 - dec2);
    w2 = (dec - dec1) / (dec2 - dec1);
  }

  if (!rb1) {
    log_trace("Query for signal expectation at declination outside of "
                  << " boundaries of extended source " << sourceId_);
    return 0.;
  }
  //always reweight for each coordinate if it is an extended source
  //Dec 1
  vector<double> energies = rb1->GetLogEnBins();
  for (vector<double>::iterator i = energies.begin();
       i != energies.end(); ++i) {
    *i = pow(10., *i + 6.); //Changing from TeV to MeV
  }

  double pixelArea = HAWCUnits::pi / (3 * nside_ * nside_);
  vector<double> fluxes =
      mi_.getExtendedSourceFluxes(sourceId_, ra, dec, energies); // In (MeV s cm2 sr)^-1
  transform(fluxes.begin(), fluxes.end(), fluxes.begin(),
            bind1st(multiplies<double>(), pixelArea));// In (MeV s cm2)^-1
  //convert to TeV^-1 cm^-1 s^-1
  for (vector<double>::iterator i = fluxes.begin();
       i != fluxes.end(); ++i) {
    (*i) *= 1e6;
  }
  rb1->ReweightEnergies(fluxes);
  rb2->ReweightEnergies(fluxes);
  //log_debug("Weights: "<<w1<<" "<<w2);
  return w1 * rb1->GetExpectedSignal() + w2 * rb2->GetExpectedSignal();
}


void ExtendedSourceDetectorResponse::checkRegionId(int regionId) const {
  if ((regionId < 0) || (regionId >= numRegions_)) {
    log_fatal("Source region ID " << regionId << " not defined!");
  }
}

pair<TH1D, TH1D> ExtendedSourceDetectorResponse::CalculatePixelatedFTPsf(
    const BinName& nhbin, const double ra, const double dec, const double dGrid,
    const int nGrid) {

  const TF1Ptr ExtPSF = GetPsfFunction(nhbin, ra, dec);

  static const string expFormulaROOT5 =
    "[0]*(x*(([1]*exp(-(x*((x/2)/[2]))))+((1-[1])*exp(-(x*((x/2)/[3]))))))";
  static const string expFormulaROOT6 =
    "[p0]*(x*(([p1]*exp(-(x*((x/2)/[p2]))))+((1-[p1])*exp(-(x*((x/2)/[p3]))))))"
    ;

  const string formula = ExtPSF->GetExpFormula().Data();
  if (formula != expFormulaROOT5 && formula != expFormulaROOT6) {
    log_warn("The PSF function in detector response file: ");
    log_warn(ExtPSF->GetExpFormula());
    log_warn("is different from expected: ");
    log_warn("[0]*(x*(([1]*exp(-(x*((x/2)/[2]))))+((1-[1])*exp(-(x*((x/2)/[3]))))))");
    log_warn("Check the PSF expression");
  }

  if (ExtPSF->GetNpar() != 4) {
    log_fatal("Is the PSF in detector response file a double Gaussian?");
  }

  double sA = sqrt(ExtPSF->GetParameter(2));
  double sB = sqrt(ExtPSF->GetParameter(3));
  double A = ExtPSF->GetParameter(1) * sA * sA /
      (ExtPSF->GetParameter(1) * sA * sA + (1 - ExtPSF->GetParameter(1)) * sB * sB);
  log_debug("sA: " << sA << " sB: " << sB << " A: " << A);

  double freqStart = 0.;
  double freqStop = 1.;

  log_debug("Creating pixelated-FT-PSF for bin " << nhbin << " with "
                << nGrid << " frequency bins.");

  const string suffix = Form("_esource%d_bin%s", sourceId_, nhbin.c_str());

  string name = "pixFTPSFA" + suffix;
  TH1D pixPSFA(name.c_str(), name.c_str(), nGrid, freqStart - (freqStop - freqStart) / nGrid / 2.,
               freqStop - (freqStop - freqStart) / nGrid / 2.);

  name = "pixFTPSFB" + suffix;
  TH1D pixPSFB(name.c_str(), name.c_str(), nGrid, freqStart - (freqStop - freqStart) / nGrid / 2.,
               freqStop - (freqStop - freqStart) / nGrid / 2.);

  for (int k = 1; k <= nGrid; ++k) {
    double center = pixPSFA.GetBinCenter(k);
    pixPSFA.SetBinContent(k, sqrt(A / nGrid) *
        exp(-center * center * HAWCUnits::pi * HAWCUnits::pi * 2. * sA * sA / dGrid / dGrid));
    pixPSFB.SetBinContent(k, sqrt((1. - A) / nGrid) *
        exp(-center * center * HAWCUnits::pi * HAWCUnits::pi * 2. * sB * sB / dGrid / dGrid));
  }
  pixPSFA.SetBinContent(0, 1); //underflow
  pixPSFA.SetBinContent(nGrid + 1, 0); //overflow
  pixPSFB.SetBinContent(0, 1); //underflow
  pixPSFB.SetBinContent(nGrid + 1, 0); //overflow

  return make_pair(pixPSFA, pixPSFB);
}

vector<pair<double, double> > ExtendedSourceDetectorResponse::GetPositions(int nside, bool reset) {

  if (reset || nside_ != nside) positions_.clear();
  nside_ = nside;
  if (!positions_.empty()) return positions_;

  healpixIds_.clear();
  log_debug(minra_<<" "<<maxra_<<" "<<mindec_<<" "<<maxdec_);

  double maxaDec = min(fabs(mindec_), fabs(maxdec_));
  double sizeDec = maxdec_ - mindec_;

  double sizeRA = maxra_ - minra_;
  if (maxra_ < minra_) sizeRA += 360.;

  double centerRA = (maxra_ + minra_) / 2.;
  if (maxra_ < minra_) centerRA += 180.;
  double centerDec = (maxdec_ + mindec_) / 2.;

  if (maxaDec < 60. && sizeDec < 60. && sizeRA <
      60.) { //use fft only for small and low declination extended sources, otherwise use sperical harmonics (need to verify the threshold)
    double dGrid = 90. / nside_; //RA size of a healpix pixel on equator
    log_debug("dGrid: " << dGrid);
    if (nside_>1000) dGrid = 90. / 512;
    double minaDec = min(fabs(mindec_), fabs(maxdec_));

    //reserve 2 degree on each side for PSF
    int tempGridRA = (int) ceil((sizeRA * cos(minaDec * HAWCUnits::pi / 180.) + 4.) / dGrid / 2.) * 2;
    int tempGridDec = (int) ceil((sizeDec + 4.) / dGrid / 2.) * 2;
    if (tempGridRA > gridRA_ || tempGridDec > gridDec_) {
      gridRA_ = tempGridRA;
      gridDec_ = tempGridDec;
      log_debug("gridRA: " << gridRA_);
      log_debug("gridDec: " << gridDec_);

      pixelatedFTPsf_.clear();

      if (fftwIn_) {
        delete[] fftwIn_;
      }
      if (fftwOut_) {
        delete[] fftwOut_;
      }
      if (fftwFP_) {
        fftw_destroy_plan(fftwFP_);
      }
      if (fftwBP_) {
        fftw_destroy_plan(fftwBP_);
      }

      fftwIn_ = new double[gridRA_ * gridDec_];
      fftwOut_ = new fftw_complex[gridRA_ * (gridDec_ / 2 + 1)];
      fftwFP_ = fftw_plan_dft_r2c_2d(gridRA_, gridDec_, fftwIn_, fftwOut_, FFTW_MEASURE);
      fftwBP_ = fftw_plan_dft_c2r_2d(gridRA_, gridDec_, fftwOut_, fftwIn_, FFTW_MEASURE);
    }

    for (int idDec = 0; idDec < gridDec_; idDec++) {
      double itDec = centerDec + dGrid * (idDec - (gridDec_ / 2 - 0.5));
      double dRA = dGrid / cos(itDec * HAWCUnits::pi / 180.);

      for (int idRA = 0; idRA < gridRA_; idRA++) {
        double itRA = centerRA + dRA * (idRA - (gridRA_ / 2 - 0.5));
        if (itRA < 0.) {
          itRA += 360.;
        } else if (itRA >= 360.) {
          itRA -= 360.;
        }

        positions_.push_back(make_pair(itRA, itDec));
      }
    }
  } else { //spherical harmonics smoothing

    Healpix_Map<double> tempMap(nside_, RING, SET_NSIDE);

    vector<pointing> polygon;
    polygon.push_back(SkyPos(minra_, mindec_).GetPointing());
    if (mindec_ > 0.)
      polygon.push_back(
          SkyPos(centerRA, atan(cos(sizeRA / 2. * degree) * tan(mindec_ * degree)) / degree).GetPointing());
    polygon.push_back(SkyPos(maxra_, mindec_).GetPointing());
    polygon.push_back(SkyPos(maxra_, maxdec_).GetPointing());
    if (maxdec_ < 0.)
      polygon.push_back(
          SkyPos(centerRA, atan(cos(sizeRA / 2. * degree) * tan(maxdec_ * degree)) / degree).GetPointing());
    polygon.push_back(SkyPos(minra_, maxdec_).GetPointing());

    rangeset<int> pixset;
    tempMap.query_polygon(polygon, pixset);
    healpixIds_ = pixset;

    for (unsigned k = 0; k < pixset.size(); ++k) {
      for (int j = pixset.ivbegin(k); j < pixset.ivend(k); ++j) {
        SkyPos point(tempMap.pix2ang(j));
        positions_.push_back(make_pair(point.RA(),point.Dec()));
      }
    }
  }

  return positions_;
}
