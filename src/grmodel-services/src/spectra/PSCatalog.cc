
#include <grmodel-services/spectra/PSCatalog.h>
#include <grmodel-services/spectra/GammaPointSource.h>
#include <grmodel-services/spectra/GenericSpectrum.h>
#include <grmodel-services/spectra/GenericBrokenSpectrum.h>
#include <grmodel-services/spectra/GenericCutoffSpectrum.h>
#include <grmodel-services/spectra/GenericParabolicSpectrum.h>
#include <grmodel-services/spectra/TabulatedLightCurve.h>
#include <grmodel-services/spectra/PeriodicLightCurve.h>

#include <data-structures/astronomy/EquPoint.h>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/HAWCNest.h>
#include <hawcnest/xml/XMLReader.h>
#include <hawcnest/xml/XMLErrorHandler.h>

#include <iomanip>
#include <map>

using namespace HAWCUnits;
using namespace std;

typedef std::map<std::string, std::string> IndexMap;

namespace PSCatalog {

SourceList
Build(const string& catalogFile, const HAWCNest& n, bool validateXML)
{
  SourceList sources;
  if (catalogFile.empty())
    log_fatal("No point source catalog specified.");

  // Iterate through the list of sources
  XMLReader r(catalogFile, validateXML ? XMLReader::SCHEMA : XMLReader::NONE);
  XMLBranch topB = r.GetTopBranch();

  string name;
  string assoc; 
  string spType;
  string spcName;
  string lcType;
  string lcName;
  string srcName;
  double ra;
  double dec;

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

  // Minimum/maximum range of energies in model or spectrum measurement
  vector<double> Erange;

  double z = 0; // Redshift

  // Data from light curve
  double mjd0;
  double period;
  vector<double> mjd;
  vector<double> flx;

  // Source extent data
  SourceExtent::Shape shape(SourceExtent::NONE);
  double length(0), width(0), angle(0);
  string extType;

  HAWCNest& nest = const_cast<HAWCNest&>(n);

  // Loop through sources
  for (XMLBranch srcB = topB.GetFirstChild(); srcB;
       srcB = srcB.GetNextSibling())
  {
    srcB.GetChild("name").GetData(name);
    srcB.GetChild("assoc").GetData(assoc);

    // Loop through source properties
    for (XMLBranch chB = srcB.GetFirstChild(); chB; chB = chB.GetNextSibling()) 
    {
      // Source spectrum
      if (chB.GetXMLBranchNameString() == "spectrum") {
        atts = chB.GetAttributes();
        spType = atts.find("type")->second;

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
        else if (spType == "BrokenPowerLaw") {
          chB.GetChild("index").GetData(a);
          chB.GetChild("index1").GetData(idx1);
          chB.GetChild("Ebreak").GetData(E1);
        }
        else if (spType == "LogParabola") {
          chB.GetChild("alpha").GetData(a);
          chB.GetChild("beta").GetData(b);
        }
        else if (spType == "PLExpCutoff" || spType == "PLExpCutoff_LAT") {
          chB.GetChild("index").GetData(a);
          chB.GetChild("cutoffE").GetData(Ec);
        }
      }

      // Other source properties
      else if (chB.GetXMLBranchNameString() == "name")
        chB.GetData(name);
      else if (chB.GetXMLBranchNameString() == "assoc")
        chB.GetData(assoc);
      else if (chB.GetXMLBranchNameString() == "raJ2000")
        chB.GetData(ra);
      else if (chB.GetXMLBranchNameString() == "decJ2000")
        chB.GetData(dec);
      else if (chB.GetXMLBranchNameString() == "redshift")
        chB.GetData(z);

      // Source extent (if any)
      else if (chB.GetXMLBranchNameString() == "extent") {
        atts = chB.GetAttributes();
        extType = atts.find("type")->second;

        if (extType == "box")
          shape = SourceExtent::BOX;
        else if (extType == "ellipse")
          shape = SourceExtent::ELLIPSE;
        else
          shape = SourceExtent::NONE;

        if (shape != SourceExtent::NONE) {
          chB.GetChild("raAxis").GetData(length);
          chB.GetChild("decAxis").GetData(width);
          chB.GetChild("angle").GetData(angle);
        }
      }

      // Light curve (optional)
      else if (chB.GetXMLBranchNameString() == "lightCurve") {
        mjd.clear();
        flx.clear();
        lcName = "";

        atts = chB.GetAttributes();
        lcType = atts.find("type")->second;

        if (lcType == "TabulatedLightCurve") {
          chB.GetChild("mjd").GetData(mjd);
          chB.GetChild("flux").GetData(flx);
          lcName = name + "_lc_" + lcType;

          nest.Service<TabulatedLightCurve>(lcName)
            ("mjd", mjd)
            ("flux", flx);
        }
        else if (lcType == "PeriodicLightCurve") {
          chB.GetChild("mjd0").GetData(mjd0);
          chB.GetChild("period").GetData(period);
          chB.GetChild("phase").GetData(mjd);
          chB.GetChild("value").GetData(flx);
          lcName = name + "_lc_" + lcType;

          nest.Service<PeriodicLightCurve>(lcName)
            ("mjd0", mjd0)
            ("period", period)
            ("phase", mjd)
            ("value", flx);
        }
      }
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
    else {
      z = 0.;

      continue;
    }

    // Push point sources into generator services
    srcName = assoc.empty() ? name : name + " : " + assoc;

    nest.Service<GammaPointSource>(srcName)
      ("sourceRA", ra)
      ("sourceDec", dec)
      ("redshift", z)
      ("sourceSpectrum", spcName)
      ("lightCurve", lcName)
      ("sourceShape", int(shape))
      ("sourceLength", length)
      ("sourceWidth", width)
      ("sourceAngle", angle);

    sources.push_back(srcName);

    mjd.clear();
    flx.clear();
    z = 0.;

    shape = SourceExtent::NONE;
    length = width = angle = 0.;
  }
  return sources;
}

} // namespace PSCatalog

