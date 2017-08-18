/*!
 * @file TestSpectrumFitPointSource.cc
 * @brief Unit test for fitting point source spectra with LiFF
 * @author Segev BenZvi
 * @date 12 May 2016
 * @ingroup integration_test
 * @version $Id: TestSpectrumFitPointSource.cc 36120 2016-12-02 10:39:12Z samm $
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/output_test_stream.hpp>
#include <boost/format.hpp>

#include <hawcnest/HAWCNest.h>
#include <hawcnest/HAWCUnits.h>
#include <hawcnest/test/OutputConfig.h>

#include <liff/BinList.h>
#include <liff/SkyMapCollection.h>
#include <liff/Util.h>
#include <liff/LikeHAWC.h>
#include <liff/TF1PointSource.h>

// Some stringification macros
#define STR(s) XSTR(s)
#define XSTR(s) #s

using boost::test_tools::output_test_stream;
using namespace HAWCUnits;
using namespace threeML;
using namespace std;

BOOST_AUTO_TEST_SUITE(SpectrumFitPointSource)

  // Global logger settings. Will affect *all* unit tests in LiFF; does not
  // need to be copied to other unit test files.
  BOOST_GLOBAL_FIXTURE(OutputConfig);

  BOOST_AUTO_TEST_CASE(FitNormPS)
  {
    const char* HAWC_SRC = getenv("HAWC_SRC");
    BOOST_CHECK(HAWC_SRC != NULL);

    string LIKE_CONF = string(HAWC_SRC) + string("/liff/config/");
    string mapfile = LIKE_CONF +
      "SubSkyMaps/Crab/maptree_20150519_v4_100days_CrabDisc5deg.root";
    string detfile = LIKE_CONF +
      "DetResponse/DetRes_aerie-svn-25030_HAWC250_20150519_v4_SensiPSF.root";

    // Source position
    double ra = 83.63;
    double dec = 22.01;
    SkyPos SourcePosition(ra, dec);
    double radius = 5.;

    // Spectrum parameters: simple power law
    //SimplePowerLaw,3.5e-11,2.63,1,0;
    double norm = 3.5e-11;
    double index = 2.63;
    double cutoff = 1e10;
    bool normFree = true;
    bool indexFree = false;
    //bool cutoffFree = false;

    // Transits and analysis bins
    //double ntransits = -1;
    const BinList binList(0, 9);

    // Load data
    SkyMapCollection Data;
    Data.SetDisc(SourcePosition, radius * degree);
    Data.LoadMaps(mapfile, binList);

    // Define source model
    double pivot = 1.;
    Func1Ptr spectrum = boost::make_shared<Func1>("spectrum",
      "[0]*pow(x/[3],-[1])*exp(-x/[2])", 0., 1e10);

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
    spectrum->SetParameter(2, cutoff);
    spectrum->SetParameterError(2, cutoff / 2.);
    spectrum->SetParameterBounds(2, 0., 1.e10);
    spectrum->SetParameterName(3, "Pivot");
    spectrum->SetParameter(3, pivot);
    spectrum->SetParameterError(3, 0);
    spectrum->SetParameterBounds(3, pivot, pivot);

    TF1PointSource PS("Source", ra, dec, spectrum);

    // Define the likelihood object
    LikeHAWC theLikeHAWC(&Data, detfile, PS, binList);

    if (normFree)
      theLikeHAWC.AddFreeParameter(spectrum, 0); //The norm will be free
    if (indexFree)
      theLikeHAWC.AddFreeParameter(spectrum, 1); //The slope will be free

    // Use theLikeHAWC to calculate results

    // Gaussian weights calculation as first guess for CommonNorm:
    double Sigma = 0.;
    // Create a reference to CommonNorm in theLikeHAWC, so that it always
    // reflects the current value used in the likelihood and that changes to
    // CommonNorm are propagated to the likelihood:
    double CommonNorm = 1.;

    theLikeHAWC.EstimateNormAndSigma(CommonNorm, Sigma);
    cout << "Analytical estimate of CommonNorm = " << CommonNorm <<
        " (Sigma: " << Sigma << ")" << endl;

    // use as first guess:
    if (!isnan(CommonNorm)) {
      double newNorm = CommonNorm * (spectrum->GetParameter(0));
      if (newNorm < normMin)
        newNorm = normMin; //stay within bounds
      if (newNorm > normMax)
        newNorm = normMax; //stay within bounds
      spectrum->SetParameter(0, newNorm);
    }

    // And now, the actual likelihood calculation
    double TS = theLikeHAWC.CalcTestStatistic();
    double sigma = sqrt(TS);

    // Check the TS and significance
    BOOST_CHECK_CLOSE(TS, 672.263, 1e-3);
    BOOST_CHECK_CLOSE(sigma, 25.928, 1e-3);

    // Check the output of the likelihood fit
    BOOST_CHECK_CLOSE(spectrum->GetParameter(0), 3.67455e-11, 1e-3);
  }

BOOST_AUTO_TEST_SUITE_END()

