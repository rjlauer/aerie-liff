/*!
 * @file CRCatalog.cc
 * @brief Implementation of the spectrum list builder.
 * @author Segev BenZvi
 * @date 31 Dec 2012
 * @version $Id: CRCatalog.cc 33320 2016-07-18 18:20:48Z zhampel $
 */

#include <grmodel-services/spectra/CRCatalog.h>
#include <grmodel-services/spectra/GenericSpectrum.h>
#include <grmodel-services/spectra/GenericCutoffSpectrum.h>
#include <grmodel-services/spectra/GenericParabolicSpectrum.h>
#include <grmodel-services/spectra/GenericBrokenSpectrum.h>
#include <grmodel-services/spectra/GenericDoubleBrokenSpectrum.h>
#include <grmodel-services/spectra/IsotropicCosmicRaySource.h>
#include <grmodel-services/spectra/TabulatedSpectrum.h>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/HAWCNest.h>
#include <hawcnest/xml/XMLReader.h>
#include <hawcnest/xml/XMLErrorHandler.h>

#include <iomanip>
#include <map>

using namespace HAWCUnits;
using namespace std;

typedef std::map<std::string, std::string> IndexMap;

namespace CRCatalog {

SourceMap
Build(const string& catalogFile, const HAWCNest& n, bool validateXML)
{
  SourceMap sources;
  if (catalogFile.empty())
    log_fatal("No cosmic ray spectrum catalog specified.");

  // Iterate through the list of sources
  XMLReader r(catalogFile, validateXML ? XMLReader::SCHEMA : XMLReader::NONE);
  XMLBranch topB = r.GetTopBranch();

  string name;
  string tabFileName;
  string spType;
  string spcName;
  string pclType;

  XMLBranch spcB;
  IndexMap atts;

  double Emin = 100*GeV;  // Minimum energy
  double Emax = 100*TeV;  // Maximum energy
  double A;               // Flux normalization
  double E0;              // Flux normalization energy
  double a;               // Spectral index intercept
  double b;               // Spectral index slope (for LogParabolic spectra)
  double Ec;              // Cutoff energy (for exponential cutoff spectra)
  double E1;              // Break energy (for broken power laws)
  double idx1;            // Index after break energy (for broken power laws)
  double E2;              // Second break energy (for double broken power laws)
  double idx2;            // Index after second break (for double broken PL)

  // Minimum/maximum range of energies in model or spectrum measurement
  vector<double> Erange;

  HAWCNest& nest = const_cast<HAWCNest&>(n);

  // Loop through sources
  for (XMLBranch srcB = topB.GetFirstChild(); srcB;
       srcB = srcB.GetNextSibling())
  {
    srcB.GetChild("name").GetData(name);

    // Loop through source properties
    for (XMLBranch chB = srcB.GetFirstChild(); chB; chB = chB.GetNextSibling()) 
    {
      if (chB.GetXMLBranchNameString() == "spectrum") {
        atts = chB.GetAttributes();
        spType = atts.find("type")->second;

        if (spType == "TabulatedSpectrum")
        {
          chB.GetChild("tabFile").GetData(tabFileName);
        }
        else {
          chB.GetChild("energyRange").GetData(Erange);
          if (Erange.size() != 2)
            log_fatal("energyRange requires a minimum and maximum.");
          Emin = Erange[0];
          Emax = Erange[1];
          Erange.clear();

          chB.GetChild("normF").GetData(A);
          chB.GetChild("normE").GetData(E0);

          if (spType == "PowerLaw")
            chB.GetChild("index").GetData(a);
          else if (spType == "LogParabola") {
            chB.GetChild("alpha").GetData(a);
            chB.GetChild("beta").GetData(b);
          }
          else if (spType == "PLExpCutoff" || spType == "PLExpCutoff_LAT") {
            chB.GetChild("index").GetData(a);
            chB.GetChild("cutoffE").GetData(Ec);
          }
          else if (spType == "BrokenPowerLaw") {
            chB.GetChild("index").GetData(a);
            chB.GetChild("index1").GetData(idx1);
            chB.GetChild("Ebreak").GetData(E1);
          }
          else if (spType == "DoubleBrokenPowerLaw") {
            chB.GetChild("index").GetData(a);
            chB.GetChild("index1").GetData(idx1);
            chB.GetChild("Ebreak1").GetData(E1);
            chB.GetChild("index2").GetData(idx2);
            chB.GetChild("Ebreak2").GetData(E2);
          }
        }
      }
      else if (chB.GetXMLBranchNameString() == "name")
        chB.GetData(name);
      else if (chB.GetXMLBranchNameString() == "particleType")
        chB.GetData(pclType);
    }

    spcName = name + "_spectrum_" + spType;

    // Construct power law sources as PointSource services
    if (spType == "PowerLaw") {
      nest.Service<GenericSpectrum>(spcName)
        ("fluxNorm", A)
        ("energyNorm", E0)
        ("spIndex", a)
        ("energyMin", Emin)
        ("energyMax", Emax);
    }
    // Construct exponentially cutoff sources
    else if (spType == "PLExpCutoff" || spType == "PLExpCutoff_LAT") {
      // Note: Fermi-LAT uses an unusual convention for the normalization
      if (spType == "PLExpCutoff_LAT")
        A *= exp(E0/Ec);

      nest.Service<GenericCutoffSpectrum>(spcName)
        ("fluxNorm", A)
        ("energyNorm", E0)
        ("spIndex", a)
        ("energyCutoff", Ec)
        ("energyMin", Emin)
        ("energyMax", Emax);
    }
    // Log parabola PointSource services
    else if (spType == "LogParabola") {
      nest.Service<GenericParabolicSpectrum>(spcName)
        ("fluxNorm", A)
        ("energyNorm", E0)
        ("indexA", a)
        ("indexB", b)
        ("energyMin", Emin)
        ("energyMax", Emax);
    }
    // Broken power law
    else if (spType == "BrokenPowerLaw") {
      nest.Service<GenericBrokenSpectrum>(spcName)
        ("fluxNorm",A)
        ("energyNorm",E0)
        ("spIndex1",a)
        ("energyBreak",E1)
        ("spIndex2",idx1)
        ("energyMin",Emin)
        ("energyMax",Emax);
    }
    else if (spType == "DoubleBrokenPowerLaw") {
      nest.Service<GenericDoubleBrokenSpectrum>(spcName)
        ("fluxNorm",A)
        ("energyNorm",E0)
        ("spIndex1",a)
        ("energyBreak1",E1)
        ("spIndex2",idx1)
        ("energyBreak2",E2)
        ("spIndex3",idx2)
        ("energyMin",Emin)
        ("energyMax",Emax);
    }
    else if (spType == "TabulatedSpectrum") {
      nest.Service<TabulatedSpectrum>(spcName)
        ("infilename",tabFileName);
    }
    else {
      continue;
    }

    // Push point sources into generator services
    nest.Service<IsotropicCosmicRaySource>(name)
      ("sourceSpectrum", spcName)
      ("particleType", pclType);

    // Store map of particle type 
    sources.insert(make_pair(Particle::GetParticleType(pclType), name));
  }

  return sources;
}

} // namespace CRCatalog
