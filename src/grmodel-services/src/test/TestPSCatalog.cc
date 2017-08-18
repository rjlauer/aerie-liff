/*!
 * @file TestPSCatalog.cc
 * @brief Integration test of the PSCatalog catalog-based gamma-ray point and
 *        extended source flux.
 * @author Segev BenZvi
 * @date 18 Feb 2014
 * @ingroup integration_test
 * @version $Id: TestPSCatalog.cc 18862 2014-02-18 23:38:45Z sybenzvi $
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

#include <grmodel-services/spectra/PSCatalog.h>
#include <grmodel-services/spectra/GammaPointSource.h>

#include <cmath>

// Some stringification macros
#define STR(s) XSTR(s)
#define XSTR(s) #s

using boost::test_tools::output_test_stream;
using namespace HAWCUnits;
using namespace std;

BOOST_AUTO_TEST_SUITE(PSCatalogTest)

  // ___________________________________________________________________________
  // Check that configuration from a file works normally
  BOOST_AUTO_TEST_CASE(GeV_1FHLSourceCatalog)
  {
    string catalogFile = string(STR(TEST_DIR)) + "/1FHL-PS-catalog.xml";

    HAWCNest nest;

    // Build a list of point sources without validating the XML file
    PSCatalog::SourceList sources;
    sources = PSCatalog::Build(catalogFile, nest, false);

    BOOST_CHECK_NO_THROW(nest.Configure());

    BOOST_CHECK_EQUAL(sources.size(), 214);
  }

  // ___________________________________________________________________________
  // Check that configuration from a file works normally
  BOOST_AUTO_TEST_CASE(GeV_2FHLSourceCatalog)
  {
    string catalogFile = string(STR(TEST_DIR)) + "/2FGL-PS-catalog.xml";

    HAWCNest nest;

    // Build a list of point sources without validating the XML file
    PSCatalog::SourceList sources;
    sources = PSCatalog::Build(catalogFile, nest, false);

    BOOST_CHECK_NO_THROW(nest.Configure());

    BOOST_CHECK_EQUAL(sources.size(), 210);
  }

  // ___________________________________________________________________________
  // Check that configuration from a file works normally
  BOOST_AUTO_TEST_CASE(TeVSourceCatalog)
  {
    string catalogFile = string(STR(TEST_DIR)) + "/TeV-src-catalog.xml";

    HAWCNest nest;

    // Build a list of point sources without validating the XML file
    PSCatalog::SourceList sources;
    sources = PSCatalog::Build(catalogFile, nest, false);

    // Configuration works
    BOOST_CHECK_NO_THROW(nest.Configure());

    // Check number of sources
    BOOST_CHECK_EQUAL(sources.size(), 87);

    // Check sources in the source list
    BOOST_CHECK(find(sources.begin(), sources.end(), "TeV J1256-057 : 3C279")
                != sources.end());
    BOOST_CHECK(find(sources.begin(), sources.end(), "TeV J0534+220 : Crab")
                != sources.end());

    // Extract some sources and check their properties
    ModifiedJulianDate mjd(55555*day);

    {
      const PointSource& ps = GetService<PointSource>("TeV J0534+220 : Crab");
      BOOST_CHECK_CLOSE(ps.GetFlux(1*TeV, mjd), 3.506e-11/(cm2*s*TeV), 1e-2);
      BOOST_CHECK_CLOSE(ps.GetFlux(10*TeV, mjd), 7.61177e-14/(cm2*s*TeV), 1e-2);
    }

    {
      const PointSource& ps = GetService<PointSource>("TeV J1256-057 : 3C279");
      BOOST_CHECK_CLOSE(ps.GetFlux(200*GeV, mjd), 5.2e-10/(cm2*s*TeV), 1e-2);
      BOOST_CHECK_CLOSE(ps.GetFlux(1*TeV, mjd), 6.97e-13/(cm2*s*TeV), 1e-2);
    }
  }

BOOST_AUTO_TEST_SUITE_END()
