/*!
 * @file Distributions.cc
 * @brief Test the output of RNG distributions.
 * @author Segev BenZvi
 * @date 27 Jan 2014
 * @ingroup unit_test
 * @version $Id: Distributions.cc 19007 2014-03-08 17:10:39Z sybenzvi $
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <hawcnest/HAWCNest.h>
#include <hawcnest/HAWCUnits.h>
#include <hawcnest/test/OutputConfig.h>

#include <rng-service/StdRNGService.h>

using namespace std;

BOOST_AUTO_TEST_SUITE(RNGDistributions)

  // Global logger settings.
  // Note: this will affect all test suites in the astro-convert project
  BOOST_GLOBAL_FIXTURE(OutputConfig);

  //____________________________________________________________________________
  // Check a few moments of the Gaussian distribution
  BOOST_AUTO_TEST_CASE(Gaussian)
  {
    HAWCNest nest;
    nest.Service("StdRNGService", "rng")
      ("seed", 12345);
    nest.Configure();

    const RNGService& rng = GetService<RNGService>("rng");

    int n = 0;
    double mean = 0;
    double M2 = 0;
    double u = 0;
    double du = 0;

    // Check a normal distribution
    for (int i = 0; i < 100000; ++i) {
      u = rng.Gaussian();
      n = i + 1;
      du = u - mean;
      mean += du/n;
      M2 += du*(u - mean);
    }
    BOOST_CHECK_SMALL(mean, 1e-2);
    BOOST_CHECK_SMALL(sqrt(M2/(n-1)) - 1., 1e-2);

    n = 0;
    mean = 0;
    M2 = 0;
    u = 0;
    du = 0;

    // Check a gaussian of mean 11.7 and sigma 2.5
    for (int i = 0; i < 100000; ++i) {
      u = rng.Gaussian(11.7, 2.5);
      n = i + 1;
      du = u - mean;
      mean += du/n;
      M2 += du*(u - mean);
    }
    BOOST_CHECK_SMALL(mean - 11.7, 1e-2);
    BOOST_CHECK_SMALL(sqrt(M2/(n-1)) - 2.5, 1e-2);
  }

  //____________________________________________________________________________
  // Check a few moments of the Poisson distribution
  BOOST_AUTO_TEST_CASE(Poisson)
  {
    HAWCNest nest;
    nest.Service("StdRNGService", "rng")
      ("seed", 12345);
    nest.Configure();

    const RNGService& rng = GetService<RNGService>("rng");

    int n = 0;
    double mean = 0;
    double M2 = 0;
    double u = 0;
    double du = 0;

    // Check a Poisson distribution of mean 5.75
    for (int i = 0; i < 100000; ++i) {
      u = rng.Poisson(5.75);
      n = i + 1;
      du = u - mean;
      mean += du/n;
      M2 += du*(u - mean);
    }
    BOOST_CHECK_SMALL(mean - 5.75, 1e-2);
    BOOST_CHECK_SMALL(M2/(n-1) - 5.75, 1e-2);
  }

  //____________________________________________________________________________
  // Check a few moments of the Binomial distribution
  BOOST_AUTO_TEST_CASE(Binomial)
  {
    HAWCNest nest;
    nest.Service("StdRNGService", "rng")
      ("seed", 12345);
    nest.Configure();

    const RNGService& rng = GetService<RNGService>("rng");

    int n = 0;
    double mean = 0;
    double M2 = 0;
    double u = 0;
    double du = 0;

    // Check a Binomial distribution with m=10 and p=0.3 (mean=3)
    const int m = 10;
    const double p = 0.3;
    for (int i = 0; i < 100000; ++i) {
      u = rng.Binomial(m, p);
      n = i + 1;
      du = u - mean;
      mean += du/n;
      M2 += du*(u - mean);
    }
    BOOST_CHECK_SMALL(mean - m*p, 1e-2);
    BOOST_CHECK_SMALL(M2/(n-1) - m*p*(1.-p), 1e-2);
  }

  //____________________________________________________________________________
  // Check a few moments of the Exponential distribution
  BOOST_AUTO_TEST_CASE(Exponential)
  {
    HAWCNest nest;
    nest.Service("StdRNGService", "rng")
      ("seed", 12345);
    nest.Configure();

    const RNGService& rng = GetService<RNGService>("rng");

    int n = 0;
    double mean = 0;
    double M2 = 0;
    double u = 0;
    double du = 0;

    // Check an Exponential distribution with lambda=2
    const double lambda = 2.;
    for (int i = 0; i < 100000; ++i) {
      u = rng.Exponential(lambda);
      n = i + 1;
      du = u - mean;
      mean += du/n;
      M2 += du*(u - mean);
    }
    BOOST_CHECK_SMALL(mean - 1./lambda, 1e-2);
    BOOST_CHECK_SMALL(M2/(n-1) - 1./(lambda*lambda), 1e-2);
  }

  //____________________________________________________________________________
  // Check a few moments of the Uniform distribution
  BOOST_AUTO_TEST_CASE(Uniform)
  {
    HAWCNest nest;
    nest.Service("StdRNGService", "rng")
      ("seed", 12345);
    nest.Configure();

    const RNGService& rng = GetService<RNGService>("rng");

    int n = 0;
    double mean = 0;
    double M2 = 0;
    double u = 0;
    double du = 0;

    // Check a Uniform distribution on (0,1)
    for (int i = 0; i < 100000; ++i) {
      u = rng.Uniform();
      n = i + 1;
      du = u - mean;
      mean += du/n;
      M2 += du*(u - mean);
    }
    BOOST_CHECK_SMALL(mean - 0.5, 1e-2);
    BOOST_CHECK_SMALL(M2/(n-1) - 1./12, 1e-2);

    n = 0;
    mean = 0;
    M2 = 0;
    u = 0;
    du = 0;

    // Check a Uniform distribution on (a,b)
    const double a = 2.;
    const double b = 11.;
    for (int i = 0; i < 1000000; ++i) {
      u = rng.Uniform(a, b);
      n = i + 1;
      du = u - mean;
      mean += du/n;
      M2 += du*(u - mean);
    }
    BOOST_CHECK_SMALL(mean - 0.5*(a+b), 1e-2);
    BOOST_CHECK_SMALL(M2/(n-1) - ((b-a)*(b-a))/12, 1e-2);
  }

BOOST_AUTO_TEST_SUITE_END()

