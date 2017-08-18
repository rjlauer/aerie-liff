/*!
 * @file TestCRCatalog.cc
 * @brief Integration test of the CRCatalog XML catalog-based cosmic-ray flux.
 * @author Segev BenZvi
 * @date 17 Feb 2014
 * @ingroup integration_test
 * @version $Id: TestCRCatalog.cc 19533 2014-04-17 02:14:11Z sybenzvi $
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/output_test_stream.hpp>
#include <boost/format.hpp>

#include <hawcnest/HAWCNest.h>
#include <hawcnest/HAWCUnits.h>
#include <hawcnest/test/OutputConfig.h>

#include <data-structures/time/ModifiedJulianDate.h>

#include <grmodel-services/spectra/CRCatalog.h>
#include <grmodel-services/spectra/CosmicRaySource.h>

#include <cmath>

// Some stringification macros
#define STR(s) XSTR(s)
#define XSTR(s) #s

using boost::test_tools::output_test_stream;
using namespace HAWCUnits;
using namespace std;

BOOST_AUTO_TEST_SUITE(CRCatalogTest)

  // ___________________________________________________________________________
  // Check that configuration from a file works normally
  BOOST_AUTO_TEST_CASE(CatalogConfiguration)
  {
    string catalogFile = string(STR(TEST_DIR)) + "/CREAM2-spectrum.xml";

    HAWCNest nest;

    // Read in spectral catalog without validating it
    CRCatalog::Build(catalogFile, nest, false);

    BOOST_CHECK_NO_THROW(nest.Configure());
  }

  // ___________________________________________________________________________
  // Try to catch access to a nonexistent particle flux
  BOOST_AUTO_TEST_CASE(UnimplementedFlux)
  {
    string catalogFile = string(STR(TEST_DIR)) + "/CREAM2-spectrum.xml";

    HAWCNest nest;

    // Read in spectral catalog without validating it
    CRCatalog::SourceMap sources = CRCatalog::Build(catalogFile, nest, false);

    nest.Configure();

    // Titanium flux is not implemented
    BOOST_CHECK_EQUAL(sources.count(Ti48Nucleus), 0);
  }

  // ___________________________________________________________________________
  // Test spectrum access using the CREAM-2 model
  BOOST_AUTO_TEST_CASE(CREAM2Spectrum)
  {
    string catalogFile = string(STR(TEST_DIR)) + "/CREAM2-spectrum.xml";

    HAWCNest nest;

    // Read in spectral catalog without validating it
    CRCatalog::SourceMap srcs = CRCatalog::Build(catalogFile, nest, false);

    nest.Configure();

    ModifiedJulianDate m(55555*day);
    double E = 1*TeV;

    // Access proton flux
    {
      const CosmicRaySource& c = GetService<CosmicRaySource>(srcs[PPlus]);
      BOOST_CHECK_CLOSE(c.GetFlux(E, m), 0.0866947/(TeV*meter2*s*sr), 1e-2);
    }

    // Access helium flux
    {
      const CosmicRaySource& c = GetService<CosmicRaySource>(srcs[He4Nucleus]);
      BOOST_CHECK_CLOSE(c.GetFlux(E, m), 0.0667051/(TeV*meter2*s*sr), 1e-2);
    }

    // Access carbon flux
    {
      const CosmicRaySource& c = GetService<CosmicRaySource>(srcs[C12Nucleus]);
      BOOST_CHECK_CLOSE(c.GetFlux(E, m), 0.0102627/(TeV*meter2*s*sr), 1e-2);
    }

    // Access oxygen flux
    {
      const CosmicRaySource& c = GetService<CosmicRaySource>(srcs[O16Nucleus]);
      BOOST_CHECK_CLOSE(c.GetFlux(E, m), 0.0160791/(TeV*meter2*s*sr), 1e-2);
    }

    // Access neon flux
    {
      const CosmicRaySource& c = GetService<CosmicRaySource>(srcs[Ne20Nucleus]);
      BOOST_CHECK_CLOSE(c.GetFlux(E, m), 0.00365092/(TeV*meter2*s*sr), 1e-2);
    }

    // Access magnesium flux
    {
      const CosmicRaySource& c = GetService<CosmicRaySource>(srcs[Mg24Nucleus]);
      BOOST_CHECK_CLOSE(c.GetFlux(E, m), 0.00655997/(TeV*meter2*s*sr), 1e-2);
    }

    // Access silicon flux
    {
      const CosmicRaySource& c = GetService<CosmicRaySource>(srcs[Si28Nucleus]);
      BOOST_CHECK_CLOSE(c.GetFlux(E, m), 0.00752108/(TeV*meter2*s*sr), 1e-2);
    }

    // Access iron flux
    {
      const CosmicRaySource& c = GetService<CosmicRaySource>(srcs[Fe56Nucleus]);
      BOOST_CHECK_CLOSE(c.GetFlux(E, m), 0.0168687/(TeV*meter2*s*sr), 1e-2);
    }
  }

  // ___________________________________________________________________________
  // Test spectrum access using the ATIC-2 model
  BOOST_AUTO_TEST_CASE(ATIC2Spectrum)
  {
    string catalogFile = string(STR(TEST_DIR)) + "/ATIC2-spectrum.xml";

    HAWCNest nest;

    // Read in spectral catalog without validating it
    CRCatalog::SourceMap srcs = CRCatalog::Build(catalogFile, nest, false);

    nest.Configure();

    ModifiedJulianDate m(55555*day);
    double E = 1*TeV;

    // Access proton flux
    {
      const CosmicRaySource& c = GetService<CosmicRaySource>(srcs[PPlus]);
      BOOST_CHECK_CLOSE(c.GetFlux(E, m), 0.088641/(TeV*meter2*s*sr), 1e-2);
    }

    // Access helium flux
    {
      const CosmicRaySource& c = GetService<CosmicRaySource>(srcs[He4Nucleus]);
      BOOST_CHECK_CLOSE(c.GetFlux(E, m), 0.064480/(TeV*meter2*s*sr), 1e-2);
    }

    // Access carbon flux
    {
      const CosmicRaySource& c = GetService<CosmicRaySource>(srcs[C12Nucleus]);
      BOOST_CHECK_CLOSE(c.GetFlux(E, m), 0.0134896/(TeV*meter2*s*sr), 1e-2);
    }

    // Access oxygen flux
    {
      const CosmicRaySource& c = GetService<CosmicRaySource>(srcs[O16Nucleus]);
      BOOST_CHECK_CLOSE(c.GetFlux(E, m), 0.023604/(TeV*meter2*s*sr), 1e-2);
    }

    // Access neon flux
    {
      const CosmicRaySource& c = GetService<CosmicRaySource>(srcs[Ne20Nucleus]);
      BOOST_CHECK_CLOSE(c.GetFlux(E, m), 0.00532/(TeV*meter2*s*sr), 1e-2);
    }

    // Access magnesium flux
    {
      const CosmicRaySource& c = GetService<CosmicRaySource>(srcs[Mg24Nucleus]);
      BOOST_CHECK_CLOSE(c.GetFlux(E, m), 0.00915/(TeV*meter2*s*sr), 1e-2);
    }

    // Access silicon flux
    {
      const CosmicRaySource& c = GetService<CosmicRaySource>(srcs[Si28Nucleus]);
      BOOST_CHECK_CLOSE(c.GetFlux(E, m), 0.0100144/(TeV*meter2*s*sr), 1e-2);
    }

    // Access iron flux
    {
      const CosmicRaySource& c = GetService<CosmicRaySource>(srcs[Fe56Nucleus]);
      BOOST_CHECK_CLOSE(c.GetFlux(E, m), 0.05073/(TeV*meter2*s*sr), 1e-2);
    }
  }

  // ___________________________________________________________________________
  // Test spectrum access using the HESS fit to the e+e- spectrum
  BOOST_AUTO_TEST_CASE(HESSElectronSpectrum)
  {
    string catalogFile = string(STR(TEST_DIR)) + "/HESS-electron-spectrum.xml";

    HAWCNest nest;

    // Read in spectral catalog without validating it
    CRCatalog::SourceMap srcs = CRCatalog::Build(catalogFile, nest, false);

    nest.Configure();

    ModifiedJulianDate m(55555*day);
    double E = 1*TeV;

    const CosmicRaySource& c = GetService<CosmicRaySource>(srcs[EMinus]);
    BOOST_CHECK_EQUAL(c.GetMinEnergy(), 700*GeV);
    BOOST_CHECK_EQUAL(c.GetMaxEnergy(), 7*TeV);
    BOOST_CHECK_CLOSE(c.GetFlux(E, m), 1.17e-4/(TeV*meter2*s*sr), 1e-2);
  }

  // ___________________________________________________________________________
  // Test spectrum access using combined HESS, ATIC, etc. fit to e+e- spectrum
  BOOST_AUTO_TEST_CASE(CombinedElectronSpectrum)
  {
    string catalogFile = string(STR(TEST_DIR)) + "/Combined-electron-spectrum.xml";

    HAWCNest nest;

    // Read in spectral catalog without validating it
    CRCatalog::SourceMap srcs = CRCatalog::Build(catalogFile, nest, false);

    nest.Configure();

    ModifiedJulianDate m(55555*day);
    double E = 1*TeV;

    const CosmicRaySource& c = GetService<CosmicRaySource>(srcs[EMinus]);
    BOOST_CHECK_EQUAL(c.GetMinEnergy(), 10*GeV);
    BOOST_CHECK_EQUAL(c.GetMaxEnergy(), 7*TeV);
    BOOST_CHECK_CLOSE(c.GetFlux(E, m), 0.64599e-4/(TeV*meter2*s*sr), 1e-2);
  }

BOOST_AUTO_TEST_SUITE_END()
