/*!
 * @file Util.cc
 * @author Patrick Younk
 * @date 25 Jul 2014
 * @brief Utility classes for a likelihood fitter.
 * @version $Id: Util.cc 40024 2017-07-31 17:37:49Z hao $
 */

#include <liff/Util.h>

#include <hawcnest/Logging.h>
#include <hawcnest/HAWCUnits.h>

#include <healpix_map_fitsio.h>

#include <fstream>

using namespace std;
using namespace HAWCUnits;
using namespace threeML;


///Construct from RA and Dec in degrees
SkyPos::SkyPos(double RA, double Dec, bool Galactic) {

  double Phi = RA * degree;
  double Theta = (90. - Dec) * degree;

  pos_.SetX(1.);
  pos_.SetMag(1.);
  pos_.SetPhi(Phi);
  pos_.SetTheta(Theta);
}

///Construct from a pointing object
SkyPos::SkyPos(pointing x) {

  double Phi = x.phi;
  double Theta = x.theta;

  pos_.SetX(1.);
  pos_.SetMag(1.);
  pos_.SetPhi(Phi);
  pos_.SetTheta(Theta);
}

/// Returns RA in degrees
double SkyPos::RA() const {

  return (pos_.Phi() > 0.) ? pos_.Phi() / degree : 360. + pos_.Phi() / degree;
}

/// Returns Dec in degrees
double SkyPos::Dec() const {

  return 90. - pos_.Theta() / degree;

}

/// Returns a pointing object
pointing SkyPos::GetPointing() const {

  return pointing(pos_.Theta(), pos_.Phi());

}

/// Distance Between two Points, in degrees
double SkyPos::Angle(SkyPos x) const {

  return pos_.Angle(x.GetVector()) / degree;
}

void SkyPos::Dump() const {
  log_debug("RA " << RA() << " deg, Dec " << Dec() << " deg.");
  return;
}


//Need the methods to return galactic coordiinates....



double
StringToDouble(const string &s)
{
  istringstream i(s);
  double x;
  if (!(i >> x))
    log_fatal("Could not convert string \"" << s << "\" to double");
  return x;
}

// Makes a Func1Ptr for a cutoff power law using 4 doubles as input
Func1Ptr
MakeSpectrum(string name,
             double norm, double index, double pivotE, double cutoffE,
             double cutEMin, double cutEMax)
{
  string formula = "[0]*pow(x/[3],-[1])*exp(-x/[2])";
  if (!isnan(cutEMin)) {
    formula += Form("*(x>%.3e)",cutEMin);
  }
  if (!isnan(cutEMax)) {
    formula += Form("*(x<%.3e)",cutEMax);
  }
  Func1Ptr spectrum = boost::make_shared<Func1>((name+"_spectrum").c_str(),
                               formula.c_str(),
                               0.,
                               1e10);
  spectrum->SetParameterName(0, "Norm");
  spectrum->SetParameter(0, norm);
  spectrum->SetParameterError(0, norm / 2.);
  double normMin = 0;
  double normMax = 1e-5;
  spectrum->SetParameterBounds(0, normMin, normMax);
  spectrum->SetParameterName(1, "Index");
  spectrum->SetParameter(1, index);
  spectrum->SetParameterError(1, 0.1);
  spectrum->SetParameterBounds(1, 1., 10.);
  spectrum->SetParameterName(2, "CutOff");
  spectrum->SetParameter(2, cutoffE);
  spectrum->SetParameterError(2, cutoffE / 2.);
  spectrum->SetParameterBounds(2, 0., 1.e10);
  spectrum->SetParameterName(3, "Pivot");
  spectrum->SetParameter(3, pivotE);
  spectrum->SetParameterError(3, 0);
  spectrum->SetParameterBounds(3, pivotE, pivotE);

  return spectrum;
}

// Makes a Func1Ptr for a cutoff power law using a string like
// 'CutOffPowerLaw,2.62e-11,2.29,42.7' as input
Func1Ptr
MakeSpectrum(string name, string spectrumString, double cutEMin, double cutEMax, double pivot)
{
  stringstream sSpectrum(spectrumString);
  string function, sAmplitude, sIndex, sCutoff;
  double amplitude, index, cutoff;
  if (!getline(sSpectrum, function, ',')) {
    log_fatal("Could not parse spectrum string  \"" << spectrumString << "\"");
  }
  if (function != "SimplePowerLaw" && function != "CutOffPowerLaw" && function != "CutoffPowerLaw" ) {
    log_fatal("Unknown function: \"" << function <<
              "\", I only know SimplePowerLaw or CutOffPowerLaw so far.");
  }
  if (!getline(sSpectrum, sAmplitude, ',')) {
    log_fatal("Could not parse spectrum string  \"" << spectrumString << "\"");
  }
  amplitude = StringToDouble(sAmplitude);
  if (!getline(sSpectrum, sIndex, ',')) {
    log_fatal("Could not parse spectrum string  \"" << spectrumString << "\"");
  }
  index = StringToDouble(sIndex);
  if (function == "CutOffPowerLaw" || function == "CutoffPowerLaw") {
    if (!getline(sSpectrum, sCutoff, ',')) {
      log_fatal("Could not parse spectrum string  \"" << spectrumString << "\"");
    }
    cutoff = StringToDouble(sCutoff);
  }
  else {
    cutoff = 1e6;
  }

  log_debug(" - Spectrum: " << function << " " << amplitude << " "
            << index << " " << cutoff << " " << pivot);

  return MakeSpectrum(name, amplitude, index, pivot, cutoff, cutEMin, cutEMax);
}


int
AddSourcesFromFile(MultiSource &sources, string fileName, double pivot)
{
  ifstream file(fileName.c_str());
  if (!file.is_open()) {
    log_fatal("Failed to read source file " << fileName);
  }

  int nAdded = 0;
  string line;

  while (getline(file, line)) {
    if (line.size() == 0)
      continue;
    if (line[0] == '#')
      continue;
    stringstream linestream(line);
    string name, typePosition, spectrum;

    // Split into main feilds
    if (!(linestream >> name >> typePosition >> spectrum)) {
      log_fatal("Could not parse line \"" << line << "\"");
    }

    // Parse source position/shape
    stringstream sShapePosition(typePosition);
    string shape, sRA, sDec, sRadius;
    double ra, dec, radius = 0.;
    if (!getline(sShapePosition, shape, ',')) {
      log_fatal("Could not parse line \"" << line << "\"");
    }
    if (shape != "Point" && shape != "Disk") {
      log_fatal("Unknown shource shape: \"" << shape <<
                "\", I only know Point or Disk so far.");
    }
    if (!getline(sShapePosition, sRA, ',')) {
      log_fatal("Could not parse line \"" << line << "\"");
    }
    ra = StringToDouble(sRA);
    if (!getline(sShapePosition, sDec, ',')) {
      log_fatal("Could not parse line \"" << line << "\"");
    }
    dec = StringToDouble(sDec);
    if (shape == "Disk") {
      if (!getline(sShapePosition, sRadius, ',')) {
        log_fatal("Could not parse line \"" << line << "\"");
      }
      radius = StringToDouble(sRadius);
    }

    log_debug("Read line \"" << line << "\"");
    log_debug(" - ShapePosition: " << shape << " " << ra << " " << dec << " "
              << radius);

    // Parse source spectrum
    Func1Ptr spectrumFunc = MakeSpectrum(
                              name,
                              spectrum,
                              std::numeric_limits<double>::quiet_NaN(),
                              std::numeric_limits<double>::quiet_NaN(),
                              pivot
                            );

    // Now, add the sources
    if (shape == "Disk") {
      log_info("Adding extended source " << name);
      sources.addExtendedSource(
        TF1ExtendedSource(name, ra, dec, spectrumFunc, radius)
        );
    }
    else if (shape == "Point") {
      log_info("Adding point source " << name);
      sources.addPointSource(TF1PointSource(name, ra, dec,spectrumFunc));
    }
    else {
      log_fatal("Wait... what? How did I even make it here? "
                "I checked that already!");
    }
    ++nAdded;
  }

  file.close();
  return nAdded;
}

vector<vector<double> > GetSpectrum(string filename) {
  double energyin, spectrumin;
  vector<double> energy, spectrum;
  if(filename.empty() ){
    log_fatal("Spectrum file is not provided");
  }
  ifstream fileforspectrum(filename.c_str());
  while(fileforspectrum>>energyin>>spectrumin) {
    energy.push_back(energyin*1e6);
    spectrum.push_back(spectrumin*1e-6);
  }
  vector<vector<double> > r;
  r.push_back(energy);
  r.push_back(spectrum);
  return r;
}

TGraph2D * GetScaling(string filename) {
  if(filename.empty()) {
    log_fatal("Scaling file is not provided");
  }
  return new TGraph2D(filename.c_str());
}

vector<double> CalculateFluxes(vector<double> energies, vector<double> energylist, vector<double> fluxlist) {
  vector<double> fluxes(energies.size(),0.0);
  unsigned int esize = energies.size();
  unsigned int fluxsize = fluxlist.size();
  unsigned int i,j;
  for(i=0; i<esize; ++i) {
    if(energies[i]<energylist[0])
      fluxes[i]=0.;                       // energy is below those specified in the file
    else if(energies[i]>energylist[fluxsize-1])
      fluxes[i]=0.;                       // energy is above those specified in the file
    else {
      for(j=1; j<fluxsize; ++j) {
        if( (energies[i] >= energylist[j-1])  && (energies[i] <= energylist[j]) ) {
          //interpolate logarithmically
          fluxes[i] = exp(log(fluxlist[j-1])+(log(fluxlist[j])-log(fluxlist[j-1]))
                          / (log(energylist[j])-log(energylist[j-1]))
                          * (log(energies[i])-log(energylist[j-1])));
          break;
        }
      }
    }
  }
  return fluxes;
}

vector<int> maskPixels(string mapPath, float threshold, bool greater, int nSide){
  if (greater) {
    log_info("Masking pixels in which "<<mapPath<< " is equal/greater than "<<threshold)
  } else {
    log_info("Masking pixels in which "<<mapPath<< " is less than "<<threshold)
  }

  Healpix_Map<double> tMap;
  read_Healpix_map_from_fits(mapPath, tMap);
  if (tMap.Nside() != nSide)
    log_info("adjusting nside");

  Healpix_Map<double> tMap2;
  tMap2.SetNside(nSide, RING);
  tMap2.Import(tMap);

  vector<int> maskedPixels;

  for (int i=0; i < tMap2.Npix(); ++i){
    if (greater) {
      if (tMap2[i] >= threshold) {
        maskedPixels.push_back(i);
      }
    } else {
      if (tMap2[i] < threshold) {
        maskedPixels.push_back(i);
      }
    }
  }

  return maskedPixels;
}

