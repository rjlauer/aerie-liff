/*!
 * @file TabulatedSpectrum.cc
 * @brief Implementation of flux table.
 * @author Brian Baughman
 * @date 6 Dec 2012
 * @version $Id: TabulatedSpectrum.cc 19164 2014-03-25 14:20:59Z bbaugh $
 */

#include <grmodel-services/spectra/TabulatedSpectrum.h>

#include <rng-service/RNGService.h>

#include <data-structures/math/PowerLaw.h>
#include <data-structures/time/ModifiedJulianDate.h>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/Parser.h>
#include <hawcnest/RegisterService.h>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;
using namespace boost;
using namespace HAWCUnits;

REGISTER_SERVICE(TabulatedSpectrum);

Configuration
TabulatedSpectrum::DefaultConfiguration()
{
  Configuration config;
  config.Parameter<string>("infilename");
  config.Parameter<string>("comment", "#");
  config.Parameter<string>("delimiter", " ");
  config.Parameter<int>("nIntegrationBins", 10000);
  return config;
}

void
TabulatedSpectrum::Initialize(const Configuration& config)
{
  string infileName, comment, delimiter;
  config.GetParameter("infilename", infileName);
  config.GetParameter("comment", comment);
  config.GetParameter("delimiter", delimiter);
  config.GetParameter("nIntegrationBins", nIntegrationBins_);

  ifstream input(infileName.c_str());
  string line;

  const string floatPattern("[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?");
  const regex fG("^" + floatPattern + "\\s*[" + delimiter + "]+\\s*" + floatPattern);

  const string labelPattern("\\w+\\s+\\[.*\\]");
  const regex hG("^" + labelPattern + "\\s*[" + delimiter + "]+\\s*" + labelPattern);

  vector<string> fields;
  vector<double> units;

  double logE;
  double logF;

  while (input.good()) {
    // Get line, remove whitespace
    getline(input, line);
    trim(line);

    // Skip comments
    if (line.compare(0, comment.size(), comment) == 0)
      continue;

    // Skip blank lines
    if (line.compare("") == 0)
      continue;

    // Header line: extract column units
    if (regex_match(line, hG)) {
      GetUnits(line, units);
      continue;
    }

    // Split data fields and multiply by units
    if (regex_match(line, fG)) {
      fields.clear();
      split(fields, line, is_any_of(delimiter + " "), token_compress_on);
      if (units.size() < 2)
      log_fatal("Tabulated Spectrum File does not contain units!");
      try {
        logE = log10(lexical_cast<double>(fields.at(0)) * units[0]);
        logF = log10(lexical_cast<double>(fields.at(1)) * units[1]);
        logFvslogE_.PushBack(logE, logF);
      }
      catch (const bad_lexical_cast& err) {
        log_fatal(err.what() << ". Check DELIMITER parameter");
      }
    }
  }

  if (logFvslogE_.IsEmpty())
    log_fatal("Flux table was not loaded.  Check DELIMITER parameter.");

  // Sort table in energy
  logFvslogE_.Sort();
}

double
TabulatedSpectrum::GetFlux(const double E, const ModifiedJulianDate& mjd,
                           const ParticleType& type)
  const
{
  if (E < GetMinEnergy(type) || E > GetMaxEnergy(type))
    return 0.;

  const double logE = log10(E);
  const double logF = logFvslogE_.Evaluate(logE);
  return pow(10., logF);
}

double
TabulatedSpectrum::GetFluxWeight(const double E,
                                 const ModifiedJulianDate& mjd,
                                 const PowerLaw& p,
                                 const ParticleType& type)
  const
{
  return GetFlux(E, mjd, type) / p.Evaluate(E) 
    * p.Integrate(p.GetMinX(), p.GetMaxX());
}

double
TabulatedSpectrum::GetMinEnergy(const ParticleType& type)
  const
{
  return pow(10., logFvslogE_.Front().GetX());
}

double
TabulatedSpectrum::GetMaxEnergy(const ParticleType& type)
  const
{
  return pow(10., logFvslogE_.Back().GetX());
}

double
TabulatedSpectrum::GetRandomEnergy(const RNGService& rng,
                                   const double E0, const double E1,
                                   const ParticleType& type)
  const
{
  // Start from a power law of index -1 and then sample from that
  double E;
  static const ModifiedJulianDate mjd(55555*day);
  static const double F0 = GetFlux(E0,mjd,type);
  static const PowerLaw pl(E0, E1, F0, E0, -1.);

  while (true) {
    E = rng.PowerLaw(-1., E0, E1);
    if (rng.Uniform() <= GetFlux(E,mjd,type)/pl.Evaluate(E))
      break;
  }

  return E;
}

double
TabulatedSpectrum::Integrate(const double E0, const double E1,
                             const ModifiedJulianDate& mjd,
                             const ParticleType& type)
  const
{
  // Just perform a simple Riemann sum of the spectrum
  double Elo, Ehi, Ecenter, dE, dFdE, deltaF, iF = 0.;
  double logEmin = log10(E0);
  double logEmax = log10(E1);
  double logdE = (logEmax - logEmin) / nIntegrationBins_;

  for (int i = 0; i < nIntegrationBins_; ++i) {
    Elo = pow(10., logEmin + i*logdE);
    Ehi = pow(10., logEmin + (i+1)*logdE);
    Ecenter = 0.5*(Elo + Ehi);
    dE = Ehi - Elo;
    dFdE = GetFlux(Ecenter, mjd, type);
    deltaF = dFdE * dE;
    iF += deltaF;
  }

  return iF;
}

double
TabulatedSpectrum::GetProbToKeep(const double E, const PowerLaw& pl,
                                 const ModifiedJulianDate& mjd,
                                 const ParticleType& type)
  const
{
  log_fatal("Function not implemented.");
  return 0.;
}

void
TabulatedSpectrum::GetUnits(std::string& line, vector<double>& units)
{
  size_t i, j;
  HAWCUnits::Parser p;

  while (line.size()) {
    i = line.find_first_of("[") + 1;
    j = line.find_first_of("]") - i;
    units.push_back(p.Evaluate(line.substr(i, j)));
    std::cout << line.substr(i, j) << std::endl;
    line = line.substr(i+j+1);
    
  }
}

