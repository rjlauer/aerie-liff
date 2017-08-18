/*!
 * @file HealpixSigFluxMap.cc
 * @author Robert Lauer, Colas Riviere
 * @date 06 Fev 2016
 * @brief Make a significance map in HEALPix and output result to a FITS file.
 * @brief MultiSource version, can fit index, can include extra sources.
 * @version "$Id: HealpixSigFluxMap.cc 40038 2017-08-01 20:47:02Z criviere $"
 */

#include <liff/BinList.h>
#include <liff/SkyMapCollection.h>
#include <liff/Util.h>
#include <liff/LikeHAWC.h>
#include <liff/MultiSource.h>

#include <data-structures/geometry/S2Point.h>

#include <hawcnest/CommandLineConfigurator.h>
#include <hawcnest/HAWCUnits.h>
#include <data-structures/astronomy/EquPoint.h>
#include <astro-service/StdAstroService.h>

#include <healpix_map_fitsio.h>

#include <iostream>

#include <boost/tokenizer.hpp>

using namespace std;
using namespace threeML;
using namespace HAWCUnits;

const double HEALPIX_UNSEEN = -1.6375e30;


int GetNearbySources(MultiSource &outsources,
                     const MultiSource &insources,
                     double sourceROIRadius);


int main(int argc, char **argv) {
  CommandLineConfigurator cl;
  cl.AddOption<string>("maptype,a", "Unset",
                       "Map type: if 'InnerGalaxy' or 'a-map'/'b-map' or 'allsky', overwrite position options. You can also input a fits file with a mask map (first column), and it will compute every pixels corresponding to a non-zero pixel in the mask");
  cl.AddOption<double>("RA,r", 83.63, "Right Ascension in degree");
  cl.AddOption<double>("Dec,d", 22.01, "Declination in degree");
  cl.AddOption<double>("mjd", "Rotate input coordinates from J2000 to this MJD");
  cl.AddOption<double>("extendedRadius", -1.,
                       "Source radius (disk). Negative = Point source.");
  cl.AddOption<double>("edgeRA,g", 2., "Length of side in RA of map area to scan");
  cl.AddOption<double>("edgeDec,l", 2., "Length of side in Dec of map area to scan");
  cl.AddOption<int>("nside,p", 512, "nside of Healpix map");
  cl.AddOption<int>("parts", 1, "divide map into x parts, by pixel number");
  cl.AddOption<int>("select", 1, "select which part, between 1 and --parts is calculated");
  cl.AddOption<string>("mapfile,m", "", "Map Tree file name");
  cl.AddOption<double>("ntransits,n", -1, "Number of transits (optional, leave at -1 to load duration from maptree)");
  cl.AddOption<string>("detfile,e", "", "Detect response file name");
  string defaultSpectrum = "SimplePowerLaw,3.5e-11,2.63";
  double defaultPivot = 1.0;
  cl.AddOption<double>("pivot", defaultPivot, "Pivot energy [TeV]");
  cl.AddOption<string>("spectrum,s",
                       defaultSpectrum,
                       "Source spectral type and input spectrum - norm index [cutoff], e.g. 'SimplePowerLaw,3.5e-11,2.63' or 'CutOffPowerLaw,3.76e-11,2.39,14.3'");
  AddBinOptions(cl);
  cl.AddOption<string>("tophat",
                       "",
                       "Top-hat instead of per-pixel fit, input comma-separated list of radius [deg.] values of bin apertures");
  cl.AddFlag("backgroundNormFit,b", "Fit background norm");
  cl.AddFlag("verbose", "Set to show logging for each pixel, i.e. source position.");
  cl.AddOption<string>("output,o", "", "Output fits file name");
  cl.AddFlag("multi",
             "Use MultiSource class instead of TF1PointSource");
  cl.AddFlag("indexfree", "Set index free in the fit.");
  cl.AddOption<string>("model", "", "Source model file, added to background "
      "(requires --multi). See example in "
      "liff/examples/sources.dat. "
      "Warning: Untill a better solution is implemented, the pivot in the file "
      "should be the same as provided by --pivot.");
  cl.AddOption<double>("padding", 3., "Padding");
  cl.AddFlag("skipknown",
             "Skip computation of stuff far from modeled sources.");

  if (!cl.ParseCommandLine(argc, argv))
    return 1;

  bool useMPS = cl.HasFlag("multi");
  bool useExtended = (cl.GetArgument<double>("extendedRadius") > 0);
  double extendedRadius = 1.;
  if (useExtended) {
    extendedRadius = cl.GetArgument<double>("extendedRadius");
  }

  //Read command line input
  string mapFileName = cl.GetArgument<string>("mapfile");
  if (mapFileName.empty()) {
    log_fatal("Please provide a map file");
  }

  string detectorResponseFileName = cl.GetArgument<string>("detfile");
  if (detectorResponseFileName.empty()) {
    log_fatal("Please provide a detector response file");
  }
  
  const BinListConstPtr binList = ParseBinOptions(cl, mapFileName);

  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep(",");
  string apinput = cl.GetArgument<string>("tophat");

  bool doTopHat = false;
  string tophat = "";
  vector<double> apertures;
  double maxSourceRadius = 0;
  const unsigned nBins = binList->GetNBins();
  if (!apinput.empty()) {
    tokenizer aTokens(apinput, sep);
    for (tokenizer::iterator tok_iter = aTokens.begin();
         tok_iter != aTokens.end(); ++tok_iter) {
      apertures.push_back(atof(tok_iter->c_str()));
      if (atof(tok_iter->c_str()) > maxSourceRadius) {
        maxSourceRadius = atof(tok_iter->c_str());
      }
    }
    if (apertures.size() != nBins) {
      log_fatal("Number of tophat radius values (" << apertures.size()
                    << ") does not match number of analysis bins ("
                    << nBins << ").");
    }
    doTopHat = true;
    tophat = "TopHat_";
    log_info("Using fast top-hat apertures with bin-radii: " << apinput);
  }
  if (cl.GetArgument<double>("extendedRadius") > maxSourceRadius) {
    maxSourceRadius = cl.GetArgument<double>("extendedRadius");
  }

  //****Define the pixels we want to look at
  double padding = cl.GetArgument<double>("padding");
  const double roiRadius = padding + maxSourceRadius;
  string mapType = cl.GetArgument<string>("maptype");
  double sourceRA = cl.GetArgument<double>("RA");
  double sourceDec = cl.GetArgument<double>("Dec");
  if(cl.HasFlag("mjd")){

    double mjdInput = cl.GetArgument<double>("mjd");
    
    StdAstroService astroService;
    Configuration conf = astroService.DefaultConfiguration();
    astroService.Initialize(conf);
    
    EquPoint sourcePos(sourceRA*HAWCUnits::degree,sourceDec*HAWCUnits::degree);
    ModifiedJulianDate mjd(mjdInput*HAWCUnits::day);
    astroService.PrecessFromJ2000ToEpoch(mjd, sourcePos);

    sourceRA = sourcePos.GetRA()/HAWCUnits::degree;
    sourceDec = sourcePos.GetDec()/HAWCUnits::degree;

    log_info("Input coordinates were rotated from J2000 to MJD " << mjdInput << ":" << sourceRA << "," << sourceDec);

  }
  double edgeRA = cl.GetArgument<double>("edgeRA");
  double edgeDec = cl.GetArgument<double>("edgeDec");

  int nside = cl.GetArgument<int>("nside");
  bool notValidNside = true;
  int powers2 = 2;
  while (powers2 <= nside) {
    if (powers2 == nside)
      notValidNside = false;
    powers2 *= 2;
  }

  if (notValidNside) log_fatal("Invalid nside value");
  Healpix_Ordering_Scheme scheme = string2HealpixScheme("RING");
  Healpix_Map<double> hMap(nside, scheme, SET_NSIDE);
  rangeset<int> pixset;

  //Define Sky Range for Tests
  bool usingMask = false;
  int ncharType = mapType.size();
  if( (ncharType > 4 && mapType.substr(ncharType-4,4) == "fits") ||
      (ncharType > 7 && mapType.substr(ncharType-7,7) == "fits.gz")){

    usingMask = true;
    
  }
  
  if(usingMask){
    pixset.clear();
    Healpix_Map<double> mask = hMap;
    Healpix_Map<double> maskImport;
    read_Healpix_map_from_fits(mapType,maskImport);
    mask.Import(maskImport); //Adjust NSIDE and SCHEME is neccesary

    for(int i = 0; i<mask.Npix(); i++){
      if(mask[i]!=0){
        pixset.add(i);
      }
    }
  }else if (mapType == "allsky") {
    hMap.query_strip(20 * degree, 120 * degree, false, pixset); //i.e. dec -30 to +70
  }
  else {
    vector<pointing> polygon;
    if (mapType == "InnerGalaxy") {
      polygon.push_back(SkyPos(270, -15).GetPointing());
      polygon.push_back(SkyPos(270, -25).GetPointing());
      polygon.push_back(SkyPos(276, -25).GetPointing());
      polygon.push_back(SkyPos(294, 11).GetPointing());
      polygon.push_back(SkyPos(294, 21).GetPointing());
      polygon.push_back(SkyPos(288, 21).GetPointing());
    }
    else if (mapType == "a-map") {
      polygon.push_back(SkyPos(50, -30.).GetPointing());
      polygon.push_back(SkyPos(310, -30).GetPointing());
      polygon.push_back(SkyPos(310, 70).GetPointing());
      polygon.push_back(SkyPos(50, 70).GetPointing());
    }
    else if (mapType == "b-map") {
      polygon.push_back(SkyPos(230, -30.).GetPointing());
      polygon.push_back(SkyPos(130, -30).GetPointing());
      polygon.push_back(SkyPos(130, 70).GetPointing());
      polygon.push_back(SkyPos(230, 70).GetPointing());
    }
    else {
      polygon.push_back(SkyPos(sourceRA - edgeRA / 2., sourceDec - edgeDec / 2.).GetPointing());
      polygon.push_back(SkyPos(sourceRA - edgeRA / 2., sourceDec + edgeDec / 2.).GetPointing());
      polygon.push_back(SkyPos(sourceRA + edgeRA / 2., sourceDec + edgeDec / 2.).GetPointing());
      polygon.push_back(SkyPos(sourceRA + edgeRA / 2., sourceDec - edgeDec / 2.).GetPointing());
    }
    hMap.query_polygon(polygon, pixset);
    // for a/b-map, selection has to be inverted
    if ((mapType == "a-map") || (mapType == "b-map")) {
      rangeset<int> bigstrip;
      hMap.query_strip(30 * degree, 110 * degree, false, bigstrip); //theta from 0 to 180
#if HEALPIX_VERSION < 330
      bigstrip.subtract(pixset);
#else
      bigstrip = bigstrip.op_andnot(pixset);
#endif
      pixset = bigstrip;
    }
  }

  // all defined pixels:
  vector<int> pixels;
  pixset.toVector(pixels);

  int nParts = cl.GetArgument<int>("parts");
  int iPart = cl.GetArgument<int>("select");
  if ((iPart < 1) || (iPart > nParts)) {
    log_fatal("Value for --select has to be between 1 and --parts included, "
                  "here: " << iPart << "/" << nParts);
  }
  if (nParts > int(pixels.size())) {
    log_fatal("More pixel intervals (" << nParts << ") "
        "than pixels (" << pixels.size() << ")");
  }
  int pixStart = ((iPart - 1) * pixels.size()) / nParts;
  int pixStop = (iPart * pixels.size()) / nParts;
  log_info("pixStart " << pixStart << " pixStop " << pixStop);


  //****Setup Data
  SkyMapCollection data;
  double radiusToLoadInMemory = sqrt(edgeRA * edgeRA + edgeDec * edgeDec) * 0.6 + padding + maxSourceRadius;
  log_debug("edgeRA " << edgeRA << " edgeDec " << edgeDec << " padding " << padding << " maxSourceRadius "
                << maxSourceRadius << " => radiusToLoadInMemory " << radiusToLoadInMemory);

  if (mapType == "InnerGalaxy") {
    sourceRA = 282.;
    sourceDec = -3.;
    radiusToLoadInMemory = 50.;
    //Set source position
    SkyPos sourcePosition(sourceRA, sourceDec);
    data.SetDisc(sourcePosition, radiusToLoadInMemory * degree);
  }
  else if (mapType == "a-map") {
    sourceRA = 180.;
    sourceDec = 20.;
    radiusToLoadInMemory = 360.;
    //Set source position
    SkyPos sourcePosition(sourceRA, sourceDec);
    data.SetDisc(sourcePosition, radiusToLoadInMemory * degree);
  }
  else if (mapType == "b-map") {
    sourceRA = 0.;
    sourceDec = 20.;
    radiusToLoadInMemory = 360.;
    //Set source position
    SkyPos sourcePosition(sourceRA, sourceDec);
    data.SetDisc(sourcePosition, radiusToLoadInMemory * degree);
  }
  else if (mapType == "allsky" || usingMask) {
    pointing pointStart = hMap.pix2ang(pixels[pixStart]);
    double decStart = 90 - (pointStart.theta) / degree;
    pointing pointEnd = hMap.pix2ang(pixels[pixStop - 1]);
    double decEnd = 90 - (pointEnd.theta) / degree;
    log_info("Declinations of points to be tested: " << decStart << " to " << decEnd);
    double minDec = min(decStart, decEnd) - radiusToLoadInMemory;
    double maxDec = max(decStart, decEnd) + radiusToLoadInMemory;
    //Set source position
    log_info("Loading data map for dec range " << minDec << " to " << maxDec);
    data.SetDecBand(minDec * degree, maxDec * degree);
    //Give some dummy values to prevent crash with ROI initialization.
    sourceRA = pointStart.phi / degree;
    sourceDec = decStart;
  } else {
    //Set source position
    SkyPos sourcePosition(sourceRA, sourceDec);
    log_info("Loading data map around RA " << sourceRA << " Dec " << sourceDec <<
        " with a radiusToLoadInMemory of " << radiusToLoadInMemory);
    data.SetDisc(sourcePosition, radiusToLoadInMemory * degree);
  }

  data.LoadMaps(mapFileName, *binList);

  double nTransits = cl.GetArgument<double>("ntransits");
  if (nTransits >= 0) {
    data.SetTransits(nTransits);
    log_debug("Transit signal fraction set to " << nTransits);
  }
  else {
    nTransits = data.GetTransits();
  }

  //****Setup Physics Models
  string spectrum = cl.GetArgument<string>("spectrum");
  double pivot = cl.GetArgument<double>("pivot"); //TeV

  Func1Ptr sourceSpectrum = MakeSpectrum("sourceSpectrum", spectrum,
                                         numeric_limits<double>::quiet_NaN(),
                                         numeric_limits<double>::quiet_NaN(),
                                         pivot);
  double startNorm = sourceSpectrum->GetParameter(0);
  double startIndex = sourceSpectrum->GetParameter(1);

  //***** Set various source models, even though only one will ultimately be
  //      used: Point Source, Multiple Point Sources, Extended Source

  // Point source model
  TF1PointSource pointSource("TestSource",
                             sourceRA,
                             sourceDec,
                             sourceSpectrum);
  log_info("TF1PointSource option");
  pointSource.describe();

  // Or the same, but with multiple version
  MultiSource multiSource = MultiSource();
  Func1Ptr spectr;
  if (!useExtended) {
    multiSource.addPointSource(TF1PointSource("TestSource",
                                              sourceRA,
                                              sourceDec,
                                              sourceSpectrum));
    spectr = multiSource.getPointSourceSpectrum(0);
  } else {
    multiSource.addExtendedSource(TF1ExtendedSource("TestSource",
                                                    sourceRA,
                                                    sourceDec,
                                                    sourceSpectrum,
                                                    extendedRadius));
    spectr = multiSource.getExtendedSourceSpectrum(0);
  }

  if (cl.GetArgument<string>("model") != "") {
    int ns = AddSourcesFromFile(multiSource, cl.GetArgument<string>("model"), pivot);
    log_info("Added " << ns << " fixed sources from file "
                 << cl.GetArgument<string>("model"));
  }
  log_info("MultiSource option");
  multiSource.describe();

  // Radius to select at what max distance will be the model sources to load.
  const double sourceROIRadius = roiRadius + 1;
  // A copy of the multiPSource containing only nearby sources, to avoid
  // waisting resources computing on sources which are too far anyway
  MultiSource usedMultiSource = MultiSource();
  int nSources = GetNearbySources(usedMultiSource,
                                  multiSource,
                                  sourceROIRadius);
  log_info("Use " << nSources << " fixed sources at first iteration");
  usedMultiSource.describe();


  TF1ExtendedSource
      extendedSource = TF1ExtendedSource("TestSource", sourceRA, sourceDec, sourceSpectrum, extendedRadius);
  extendedSource.describe();


  //Likelihood:
  if (!useMPS and !useExtended) {
    log_info("Using TF1PointSource version");
    gLikeHAWC = new LikeHAWC(&data, detectorResponseFileName, pointSource, sourceRA, sourceDec, roiRadius,
                             true, *binList);
    //turn CommonNorm fit on/off:
    gLikeHAWC->ClearFreeParameterList();
    if (cl.HasFlag("indexfree")) {
      gLikeHAWC->AddFreeParameter(sourceSpectrum, 1); //The index will be free
      log_info(" - Index is free.");
    }
    gLikeHAWC->SetCommonNormFree(true);
  } else if (useMPS) {
    log_info("Using MultiSource version");
    gLikeHAWC = new LikeHAWC(&data, detectorResponseFileName, usedMultiSource, sourceRA, sourceDec,
                             roiRadius, true, *binList);
    //gLikeHAWC->ResetSources(usedMultiPointSource);
    //turn CommonNorm fit on/off:
    gLikeHAWC->ClearFreeParameterList();
    gLikeHAWC->SetCommonNormFree(false);
    log_info("Source 0:");
    gLikeHAWC->AddFreeParameter(spectr, 0); //The norm will be free
    log_info(" - Normalization is free.");
    if (cl.HasFlag("indexfree")) {
      gLikeHAWC->AddFreeParameter(spectr, 1); //The index will be free
      log_info(" - Index is free.");
    }
  } else if (useExtended) {
    log_info("Using TF1ExtendedSource version");
    gLikeHAWC = new LikeHAWC(&data, detectorResponseFileName, extendedSource,
                             sourceRA, sourceDec, roiRadius, true,
                             *binList);
    log_info("LikeHAWC is set.");
    gLikeHAWC->SetROI(gLikeHAWC->MatchROI(padding));
    log_info("ROI is set.");
    gLikeHAWC->ClearFreeParameterList();
    gLikeHAWC->SetCommonNormFree(true);
    if (cl.HasFlag("indexfree")) {
      gLikeHAWC->AddFreeParameter(sourceSpectrum, 1); //The index will be free
      log_info(" - Index is free.");
    }
  } else {
    log_fatal("Logic error, I should never get here. Fix me.")
  }



  //Here we create a reference to CommonNorm in theLikeHAWC, so that it always
  //reflects the current value used in the likelihood and that changes to
  //CommonNorm are propagated to the likelihood:
  double &refCommonNorm = gLikeHAWC->CommonNorm();
  double &refCommonNormError = gLikeHAWC->CommonNormError();

  //turn BackgroundNorm fit on/off:
  if (cl.HasFlag("backgroundNormFit")) {
    gLikeHAWC->SetBackgroundNormFree(true);
    cout << "Fitting background norm in all bins." << endl;
  }

  //verbosity:
  bool verbose = cl.HasFlag("verbose");

  //****Loop over Positions around the Source

  string pixparts = "";
  if (nParts > 1) {
    pixparts = Form("_%04dof%04d", iPart, nParts);
    log_info("Calculating significances for " << pixStop - pixStart << " pixels in part "
                 << iPart << " of " << nParts);
  }
  else {
    log_info("Calculating significances for " << pixStop - pixStart << " pixels.");
  }

  int perc = 100;
  double percentpix = 1. * (pixStop - pixStart) / perc;
  //use 10% instead of 1% divisions if too few pixels:
  if (percentpix < 10) {
    perc = 10;
    percentpix = 1. * (pixStop - pixStart) / perc;
  }

  SkyMap<double> sigMap(hMap, pixset);
  sigMap.SetInsideValue(0);
  SkyMap<double> fluxMap(hMap, pixset);
  fluxMap.SetInsideValue(0);
  SkyMap<double> fluxErrMap(hMap, pixset);
  fluxErrMap.SetInsideValue(0);
  SkyMap<double> indexMap(hMap, pixset);
  indexMap.SetInsideValue(0);
  SkyMap<double> indexErrMap(hMap, pixset);
  indexErrMap.SetInsideValue(0);

  double maxSigma = -10.;
  SkyPos maxPos;

  int nperc = 1;
  for (int p = pixStart; p < pixStop; p++) {
    if ((!verbose) && ((p - pixStart) / percentpix > nperc)) {
      cout << nperc * 100. / perc << "% of pixels done." << endl;
      nperc++;
    }
    int pix = pixels[p];

    //Set New Position
    SkyPos position = SkyPos(sigMap.pix2ang(pix));
    if (verbose) {
      cout << "p " << p << " pix " << pix << " (pixel "<<p-pixStart<<" of "<<pixStop-pixStart<<")"<< endl;
      cout << "coordinates: RA " << position.RA() << " , Dec " << position.Dec() << endl;
    }

    //change test source position:
    if (!useMPS && !useExtended) {
      gLikeHAWC->GetPointSourceDetectorResponse(0)->SetSkyPos(position);
      gLikeHAWC->SetROI(gLikeHAWC->MatchROI(max(roiRadius, ceil(maxSourceRadius))));
    } else if (useMPS) {
      switch(multiSource.getTotalSourceType(0)) {
        case MultiSource::POINT: {
          multiSource.setPointSourcePosition(0, position.RA(), position.Dec());
          break;
        }
        case MultiSource::EXTENDED: {
          multiSource.setExtendedSourcePosition(0, position.RA(), position.Dec());
          break;
        }
        default: {
          log_fatal("Unknown source type: "<<multiSource.getTotalSourceType(0));
        }
      }
      int nFixedSources = GetNearbySources(usedMultiSource,
                                           multiSource,
                                           sourceROIRadius);
      if (cl.HasFlag("skipknown") && nFixedSources < 1) {
        sigMap.SetPixel(pix, HEALPIX_UNSEEN);
        fluxMap.SetPixel(pix, HEALPIX_UNSEEN);
        fluxErrMap.SetPixel(pix, HEALPIX_UNSEEN);
        indexMap.SetPixel(pix, HEALPIX_UNSEEN);
        indexErrMap.SetPixel(pix, HEALPIX_UNSEEN);
        continue;
      }
      log_info("Use " << nFixedSources << " fixed sources");
      usedMultiSource.describe();

      switch(multiSource.getTotalSourceType(0)) {
        case MultiSource::POINT: {
          gLikeHAWC->UpdateSources();
          break;
        }
        case MultiSource::EXTENDED: {
          gLikeHAWC->ResetSources(extendedSource, padding);
          break;
        }
        default: {
          log_fatal("Unknown source type: "<<multiSource.getTotalSourceType(0));
        }
      }
      log_info("Setting fixed ROI with RA " << position.RA()
                   << " Dec " << position.Dec() << " r " << roiRadius);
      gLikeHAWC->SetROI(position.RA(), position.Dec(), roiRadius, true);
    } else if (useExtended) { // Extended, but no multi sources!

      extendedSource.setSourcePosition(position.RA(), position.Dec());

      //extendedSource = TF1ExtendedSourcePtr(
      //new TF1ExtendedSource("TestSource", position.RA(), position.Dec(), sourceSpectrum, extendedRadius));

      ////gLikeHAWC->UpdateSources(); //// Colas, 2016-06-16: This used to work, but not anymore for an unknown reason. So calling the slow alternative...
      gLikeHAWC->ResetSources(extendedSource, padding);

      log_info("Setting fixed ROI with RA " << position.RA()
                   << " Dec " << position.Dec() << " r " << roiRadius);
      gLikeHAWC->SetROI(position.RA(), position.Dec(), roiRadius, true);
      //gLikeHAWC->SetupCalcBins(analysisBinStart, analysisBinStop);
    } else {
      log_fatal("Logic error, I should never get here. Fix me.")
    }

    //always reset CommonNorm !
    double sigma = 0;
    if (!useMPS) {

      if (false) { // Skip that for testing
        //use TopHat faster functions if --tophat bin radii are given:

        //Find Initial Guesses (using a gaussian approx.)
        if (doTopHat) {
          gLikeHAWC->EstimateTopHatNormAndSigma(refCommonNorm, sigma, position, apertures);
        }
        else {
          gLikeHAWC->EstimateNormAndSigma(refCommonNorm, sigma);
        }
        //minimizer tends to be unstable for negative initial common norm
        if (refCommonNorm < 0) refCommonNorm = 0;

        if (verbose) {
          cout << "  Gausian Approximation: Common Norm: " << refCommonNorm
              << "  Sigma: " << sigma << endl;
        }
        if (isnan(refCommonNorm)) {
          sigMap.SetPixel(pix, -5.);
          fluxMap.SetPixel(pix, -5.);
          fluxErrMap.SetPixel(pix, -5.);
          cout << endl << "  CommonNorm from Gaussian Weights is nan, skipping pixel!" << endl;
          refCommonNorm = 1;
          refCommonNormError = 1;
          continue;
        }
      } else {
        refCommonNorm = 1;
        refCommonNormError = 1;
      }
    } else {
      // Reset to initial value
      spectr->SetParameter(0, startNorm);
      spectr->SetParameter(1, startIndex);
      refCommonNorm = 1;
      refCommonNormError = 1;
    }


    double testStatistics = 0;
    if (doTopHat) {
      testStatistics = gLikeHAWC->CalcTopHatTestStatistic(position, apertures);
    }
    else {
      // Regular test statistics
      testStatistics = gLikeHAWC->CalcTestStatistic();
      if (useMPS && cl.GetArgument<string>("model") != "") {
        // I want the fixed sources in the null hypothesis, don't optimize
        // The optimization already happened in the call of
        // gLikeHAWC->CalcTestStatistic() above
        double llhSignalHypothesis = gLikeHAWC->CalcLogLikelihoodUpdateSources(false);
        double savedAmplitude = spectr->GetParameter(0);
        spectr->SetParameter(0, 0.); // Turn off the test source
        double llhNullHypothesis = gLikeHAWC->CalcLogLikelihoodUpdateSources(false);
        testStatistics = 2 * (llhSignalHypothesis - llhNullHypothesis);
        log_debug(testStatistics << " " << llhSignalHypothesis << " " << llhNullHypothesis);
        // Put back the correct amplitude
        spectr->SetParameter(0, savedAmplitude);
      }
    }

    if (testStatistics < 0) {
      cout << "  TS=" << testStatistics << " is negative, which shouldn't happen for nested "
          << "models." << endl;
      cout << "  Assuming it is just a rounding difference between Model and "
          << "BG LL maximization, we set TS=0." << endl;
      testStatistics = 0;
      refCommonNorm = 0;
      refCommonNormError = 1;
    }
    sigma = sqrt(testStatistics);

    if (isnan(refCommonNorm)) {
      cout << endl << "  CommonNorm from LL is nan!" << endl << endl;
      refCommonNorm = 1.;
      refCommonNormError = 1;
    }
    else if (((!useMPS) && (refCommonNorm < 0)) ||
        ((useMPS) && (spectr->GetParameter(0) < 0))) {
      sigma = -sqrt(testStatistics); // get negative sigmas...
    }

    if (verbose) {
      cout << "  LL maximization      : Common Norm: " << refCommonNorm
          << "  Sigma: " << sigma << endl;
    }

    //Fill Map
    sigMap.SetPixel(pix, sigma);
    if (!useMPS) {
      fluxMap.SetPixel(pix, refCommonNorm * startNorm);
      fluxErrMap.SetPixel(pix, refCommonNormError * startNorm);
      indexMap.SetPixel(pix, 1);
      indexErrMap.SetPixel(pix, 1);
    } else {
      fluxMap.SetPixel(pix, spectr->GetParameter(0));
      fluxErrMap.SetPixel(pix, spectr->GetParameterError(0));
      indexMap.SetPixel(pix, -spectr->GetParameter(1));
      indexErrMap.SetPixel(pix, spectr->GetParameterError(1));
    }

    if (sigma > maxSigma) {
      maxSigma = sigma;
      maxPos = position;
    }

  }

  cout << endl;
  cout << "Maximum significance: " << maxSigma << " at (" << maxPos.RA() << "," << maxPos.Dec() << ")" << endl;

  // Do we have to have all N maps opened at once?
  sigMap.SetOutsideValue(-5.);
  sigMap.FillHealpixMap(hMap);

  Healpix_Map<double> hFMap(nside, scheme, SET_NSIDE);
  fluxMap.SetOutsideValue(-5.);
  fluxMap.FillHealpixMap(hFMap);
  Healpix_Map<double> hFEMap(nside, scheme, SET_NSIDE);
  fluxErrMap.SetOutsideValue(-5.);
  fluxErrMap.FillHealpixMap(hFEMap);

  Healpix_Map<double> hIMap(nside, scheme, SET_NSIDE);
  indexMap.SetOutsideValue(-5.);
  indexMap.FillHealpixMap(hIMap);
  Healpix_Map<double> hIEMap(nside, scheme, SET_NSIDE);
  indexErrMap.SetOutsideValue(-5.);
  indexErrMap.FillHealpixMap(hIEMap);


  string fitsFile = cl.GetArgument<string>("output");

  if (fitsFile.empty())
    fitsFile = Form("sigMap_%s%s_%.0ftransits_NSide%d%s.fits", tophat.c_str(), mapType.c_str(), nTransits, nside,
                    pixparts.c_str());

  fitshandle out;
  arr<string> colname(5);
  colname[0] = "significance";
  colname[1] = "flux";
  colname[2] = "flux error";
  colname[3] = "index";
  colname[4] = "index error";
  out.create(fitsFile);
  prepare_Healpix_fitsmap(out, hMap, PLANCK_FLOAT64, colname);
  out.write_column(1, hMap.Map());
  out.write_column(2, hFMap.Map());
  out.write_column(3, hFEMap.Map());
  out.write_column(4, hIMap.Map());
  out.write_column(5, hIEMap.Map());

  //Fill Header
  out.goto_hdu(1);
  
  double nEvents;
  double startMJD;
  double stopMJD;
  double duration;
  double totDur;
  double maxDur;
  double minDur;
  string maptype;
  string epoch;

  data.GetBinInfo((*binList)[0],
                  startMJD,
                  stopMJD,
                  nEvents,
                  totDur,
                  duration,
                  maptype,
                  maxDur,
                  minDur,
                  epoch);
  
  for (unsigned i = 0; i < nBins; ++i) {

    double nEventsAux;
    double startMJDAux;
    double stopMJDAux;
    double durationAux;
    double totDurAux;
    double maxDurAux;
    double minDurAux;
    string maptypeAux;
    string epochAux;  
    
    data.GetBinInfo((*binList)[i],
               startMJDAux,
               stopMJDAux,
               nEventsAux,
               totDurAux,
               durationAux,
               maptypeAux,
               maxDurAux,
               minDurAux,
               epochAux);

    if(startMJD != startMJDAux)
      startMJD = -1;

    if(stopMJD != stopMJDAux)
      stopMJD = -1;

    if(nEvents != nEventsAux)
      nEvents = -1;

    if(totDur != totDurAux)
      totDur = -1;

    if(duration != durationAux)
      duration = -1;

    if(maptype != maptypeAux)
      maptype = "unknown";

    if(maxDur != maxDurAux)
      maxDur = -1;

    if(minDur != minDurAux)
      minDur = -1;

    if(epoch != epoch)
      epoch = "unknown";

  }
  
  
  out.set_key("NEVENTS" , nEvents    , "Number of events in map");
  out.set_key("STARTMJD", startMJD   , "MJD of first event");
  out.set_key("STOPMJD" , stopMJD    , "MJD of last event used");
  out.set_key("DURATION", duration   , "Avg integration time [hours]");
  out.set_key("TOTDUR"  , totDur     , "Total integration time [hours]");
  out.set_key("MAXDUR"  , maxDur     , "Max integration time [hours]");
  out.set_key("MINDUR"  , minDur     , "Min integration time [hours]");
  out.set_key("MAPTYPE" , maptype    , "e.g. Sky, Moon, Sun, Antisidereal");
  out.set_key("EPOCH"   , epoch      , "e.g. J2000, current, J2016, B1950, etc.");
  
  out.close();
  log_info("Created new fits output file: " << fitsFile);
  return 0;
}


int
GetNearbySources(MultiSource &outsources,
                 const MultiSource &insources,
                 double sourceROIRadius) {
  outsources.clear();
  // Source 0 is the test source, to be added no matter what.
  // Can be point or extended.
  log_debug("Number of point, extended, and total sources: "
            <<insources.getNumberOfPointSources()<<" "
            <<insources.getNumberOfExtendedSources()<<" "
            <<insources.getTotalNumberOfSources());
  outsources.addSourceFrom(insources, 0);
  
  // Getting the position of the test source, to add only the ones nearby.
  double refRA, refDec;
  MultiSource::sourcetype refsource = insources.getTotalSourceType(0);
  switch (refsource) {
    case MultiSource::POINT:
      log_info("Getting test point source");
      insources.getPointSourcePosition(0, &refRA, &refDec);
      break;
    case MultiSource::EXTENDED:
      log_info("Getting test extended source");
      insources.getExtendedSourcePosition(0, &refRA, &refDec);
      break;
    default:
      log_fatal("Unknown source type: "<<refsource);
      return -1;
  }
  S2Point refPoint((90 - refDec) * HAWCUnits::degree,
                   refRA * HAWCUnits::degree);
  
  // Looping over all the sources and adding the ones nearby.
  int counter = 0;
  for (int i = 1; i < insources.getTotalNumberOfSources(); i++) {
    double iRA, iDec;
    double iRadius = 0.;
    string iName;
    // Get the source position, radius and name.
    MultiSource::sourcetype refsource = insources.getTotalSourceType(i);
    switch (refsource) {
      case MultiSource::POINT: {
        log_info("Adding fixed point source");
        int iPSIndex = insources.getPointSourceIndexFromTotalIndex(i);
        insources.getPointSourcePosition(iPSIndex, &iRA, &iDec);
        iName = insources.getPointSourceName(iPSIndex);
        break;
      }
      case MultiSource::EXTENDED: {
        log_info("Adding fixed extended source");
        int iExtIndex = insources.getExtendedSourceIndexFromTotalIndex(i);
        insources.getExtendedSourcePosition(iExtIndex, &iRA, &iDec);
        iRadius = insources.getExtendedSourceRadius(iExtIndex);
        iName = insources.getExtendedSourceName(iExtIndex);
        break;
      }
      default:
        log_fatal("Unknown source type: "<<refsource);
        return -1;
    }
    S2Point iPoint((90. - iDec) * HAWCUnits::degree, iRA * HAWCUnits::degree);
    // Compute the distance to the reference source.
    double iDist = refPoint.Angle(iPoint) / HAWCUnits::degree;
    log_debug("Distance to fixed source " << i << ": " << iDist
                  << " deg (" << iName << ")");
    if (iDist < sourceROIRadius+iRadius) {
      log_info(" => source added");
      outsources.addSourceFrom(insources, i);
      ++counter;
    } else {
      log_debug(" => source discarded");
    }
  }
  return counter;
}
