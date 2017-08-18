/*!
 * @file Math.cc 
 * @brief Unit tests for the mathematical data structures.
 * @author Segev BenZvi 
 * @date 27 Jan 2012 
 * @ingroup unit_test
 * @version $Id: Math.cc 19975 2014-05-18 03:43:57Z sybenzvi $
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/output_test_stream.hpp>

#include <hawcnest/HAWCUnits.h>
#include <hawcnest/test/OutputConfig.h>

#include <data-structures/math/PowerLaw.h>
#include <data-structures/math/BrokenPowerLaw.h>
#include <data-structures/math/DoubleBrokenPowerLaw.h>
#include <data-structures/math/BayesianBlocks.h>
#include <data-structures/math/DigitalLogic.h>
#include <data-structures/math/Trace.h>
#include <data-structures/math/TabulatedFunction.h>
#include <data-structures/math/SpecialFunctions.h>

#include <cmath>

using namespace HAWCUnits;
using boost::test_tools::output_test_stream;
using namespace std;


BOOST_AUTO_TEST_SUITE(MathTest)

  //____________________________________________________________________________
  // Various power laws and reweighting calculations
  BOOST_AUTO_TEST_CASE(PowerLawTest)
  {
    const double x0  = 1.;
    const double x1  = HAWCUnits::infinity;//1e2;
    const double A   = 1.;
    const double xN  = 1.;
    const double i1  = -3.3;
    const double xb1 = 10.;
    const double i2  = -2.7;
    const double xb2 = 50.;
    const double i3  = -3.;

    // Check power law initializations
    PowerLaw p1(x0, x1, A, xN, i2);

    BOOST_CHECK_EQUAL(p1.GetMinX(), 1.);
    BOOST_CHECK_EQUAL(p1.GetMaxX(), HAWCUnits::infinity);
    BOOST_CHECK_EQUAL(p1.GetNormalization(), 1.);
    BOOST_CHECK_EQUAL(p1.GetSpectralIndex(), -2.7);

    PowerLaw p2(x0, x1, 10., xN, -2.);

    BOOST_CHECK_EQUAL(p2.GetMinX(), 1.);
    BOOST_CHECK_EQUAL(p2.GetMaxX(), HAWCUnits::infinity);
    BOOST_CHECK_EQUAL(p2.GetNormalization(), 10.);
    BOOST_CHECK_EQUAL(p2.GetSpectralIndex(), -2.);

    // Broken power law initializations
    BrokenPowerLaw p3(x0, x1, A, xN, i1, xb1, i2);

    BOOST_CHECK_EQUAL(p3.GetMinX(), x0);
    BOOST_CHECK_EQUAL(p3.GetMaxX(), HAWCUnits::infinity);
    BOOST_CHECK_EQUAL(p3.GetNormalization(), A);
    BOOST_CHECK_EQUAL(p3.GetSpectralIndex(5.), i1);
    BOOST_CHECK_EQUAL(p3.GetSpectralIndex(50.), i2);

    // Double broken power law initializations
    DoubleBrokenPowerLaw p4(x0, x1, A, xN, i1, xb1, i2, xb2, i3);

    BOOST_CHECK_EQUAL(p4.GetMinX(), x0);
    BOOST_CHECK_EQUAL(p4.GetMaxX(), HAWCUnits::infinity);
    BOOST_CHECK_EQUAL(p4.GetNormalization(), A);
    BOOST_CHECK_EQUAL(p4.GetSpectralIndex(5.), i1);
    BOOST_CHECK_EQUAL(p4.GetSpectralIndex(20.), i2);
    BOOST_CHECK_EQUAL(p4.GetSpectralIndex(75.), i3);

    // Normalization weight
    BOOST_CHECK_CLOSE(p1.GetNormWeight(1.), -(i2 + 1.), 1e-10);
    BOOST_CHECK_CLOSE(p2.GetNormWeight(1.), 1.0, 1e-10);

    // Evaluation
    BOOST_CHECK_CLOSE(p1.Evaluate(10.), pow(10., i2), 1e-10);
    BOOST_CHECK_CLOSE(p2.Evaluate(10.), 10.*pow(10., -2.), 1e-10);

    BOOST_CHECK_CLOSE(p3.Evaluate(5.),  0.00493627090176, 1e-10);
    BOOST_CHECK_CLOSE(p3.Evaluate(50.), 6.4980191708489e-6, 1e-10);

    BOOST_CHECK_CLOSE(p4.Evaluate(5.),  0.00493627090176, 1e-10);
    BOOST_CHECK_CLOSE(p4.Evaluate(50.), 6.4980191708489e-6, 1e-10);
    BOOST_CHECK_CLOSE(p4.Evaluate(75.), 1.9253390135851e-6, 1e-10);

    // Integration tests
    BOOST_CHECK_CLOSE(
      p1.Integrate(10., 100.),
      A * (pow(100.,i2+1) - pow(10.,i2+1)) / ((i2+1)*pow(xN, i2+1)),
      1e-10);

    BOOST_CHECK_CLOSE(
      p1.Integrate(x0, x1),
      -A * pow(x0,i2+1) / ((i2+1)*pow(xN,i2)),
      1e-10);

    BOOST_CHECK_CLOSE(
      p3.Integrate(x0, xb1),
      A * (pow(xb1,i1+1) - pow(x0,i1+1)) / ((i1+1)*pow(xN, i1+1)),
      1e-10);

    // Reweighting
    //BOOST_CHECK_CLOSE(p1.Reweight(p2, 10.), -(i2+1)*pow(10., i2+2.), 1e-10);
  }

  //____________________________________________________________________________
  // Test of the Bayesian Blocks optimal binning algorithm
  BOOST_AUTO_TEST_CASE(BayesianBlocksTest)
  {
    double a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    double b[] = { 1, 3, 5, 100, 97, 8, 7, 6, 5 };

    BayesianBlocks::Array t(a, a+9);
    BayesianBlocks::Array x(b, b+9);

    BayesianBlocks::P0Prior pr(0.01);
    BayesianBlocks::EventFitness fit;

    vector<double> edges = BayesianBlocks::calculateBins(t, x, pr, fit);
    BOOST_CHECK_EQUAL(edges.size(), 4);

    double nedges[] = { 1, 3.5, 5.5, 9 };
    for (size_t i = 0; i < 4; ++i)
      BOOST_CHECK_EQUAL(nedges[i], edges[i]);
  }

  //____________________________________________________________________________
  // Test of digital logic pulses and their combinations
  BOOST_AUTO_TEST_CASE(DigitalLogicTest)
  {
    //                           _____
    // Set up a simple pulse: ___|   |___
    DigitalLogic::Pulse p1;
    p1.AddEdge(DigitalLogic::Edge(DigitalLogic::Edge::RISING, 10.));
    p1.AddEdge(DigitalLogic::Edge(DigitalLogic::Edge::FALLING, 30.));

    BOOST_CHECK_EQUAL(p1.GetNEdges(), 2u);

    // Check Edge access
    const DigitalLogic::Edge& e1 = p1.GetFirstEdge();
    const DigitalLogic::Edge& e2 = p1.GetLastEdge();

    BOOST_CHECK(e1.IsRising());
    BOOST_CHECK(!e2.IsRising());

    BOOST_CHECK_EQUAL(e1.GetTime(), 10.);
    BOOST_CHECK_EQUAL(e2.GetTime(), 30.);
    BOOST_CHECK(e1 < e2);

    //                                      _____
    // Set up a second delayed pulse:  _____|   |___
    DigitalLogic::Pulse p2;
    p2.AddEdge(DigitalLogic::Edge(DigitalLogic::Edge::RISING, 20.));
    p2.AddEdge(DigitalLogic::Edge(DigitalLogic::Edge::FALLING, 40.));

    BOOST_CHECK_EQUAL(p2.GetNEdges(), 2u);

    // Check logical AND
    DigitalLogic::Pulse p3 = DigitalLogic::AND(p1, p2);

    BOOST_CHECK_EQUAL(p3.GetNEdges(), 2u);

    BOOST_CHECK(p3.GetFirstEdge().IsRising());
    BOOST_CHECK(!p3.GetLastEdge().IsRising());

    BOOST_CHECK_EQUAL(p3.GetFirstEdge().GetTime(), 20.);
    BOOST_CHECK_EQUAL(p3.GetLastEdge().GetTime(), 30.);

    // Check logical OR
    p3 = DigitalLogic::OR(p1, p2);

    BOOST_CHECK_EQUAL(p3.GetNEdges(), 2u);

    BOOST_CHECK(p3.GetFirstEdge().IsRising());
    BOOST_CHECK(!p3.GetLastEdge().IsRising());

    BOOST_CHECK_EQUAL(p3.GetFirstEdge().GetTime(), 10.);
    BOOST_CHECK_EQUAL(p3.GetLastEdge().GetTime(), 40.);

    // Check DELAY
    p3 = DigitalLogic::DELAY(p3, 10.);

    BOOST_CHECK_EQUAL(p3.GetFirstEdge().GetTime(), 20.);
    BOOST_CHECK_EQUAL(p3.GetLastEdge().GetTime(), 50.);

    // Check logical NOT
    p3 = DigitalLogic::NOT(p3);

    BOOST_CHECK(!p3.GetFirstEdge().IsRising());
    BOOST_CHECK(p3.GetLastEdge().IsRising());
  }

  //____________________________________________________________________________
  // Test of signal traces
  BOOST_AUTO_TEST_CASE(SignalTrace)
  {
    Trace<double, 200> tr(0., 0.1);

    BOOST_CHECK_EQUAL(tr.GetSize(), 200u);
    BOOST_CHECK_EQUAL(tr.GetDt(), 0.1);
    BOOST_CHECK_EQUAL(tr.GetT0(), 0.);

    BOOST_CHECK_EQUAL(tr.GetDt(), 0.1);

    tr.SetDt(0.2);
    tr.SetT0(5.);
    BOOST_CHECK_EQUAL(tr.GetDt(), 0.2);
    BOOST_CHECK_EQUAL(tr.GetT0(), 5.);

    // Test arithmetic operations
    BOOST_CHECK_EQUAL(tr.Integral(), 0.);

    tr += 1.;
    BOOST_CHECK_EQUAL(tr.Integral(), tr.GetSize()*tr.GetDt());

    tr -= 0.5;
    BOOST_CHECK_EQUAL(tr.Integral(), 0.5*tr.GetSize()*tr.GetDt());

    tr *= 4.;
    BOOST_CHECK_EQUAL(tr.Integral(), 2.*tr.GetSize()*tr.GetDt());

    tr /= 2.;
    BOOST_CHECK_EQUAL(tr.Integral(), tr.GetSize()*tr.GetDt());

    // Test random access and min/max functions
    tr[20] = -1.;
    tr[40] = 10.;

    BOOST_CHECK_EQUAL(tr[20], -1.);
    BOOST_CHECK_EQUAL(tr[40], 10.);

    BOOST_CHECK_EQUAL(tr.Minimum(), -1.);
    BOOST_CHECK_EQUAL(tr.Maximum(), 10.);

    BOOST_CHECK_EQUAL(tr.MinimumBin(), 20u);
    BOOST_CHECK_EQUAL(tr.MaximumBin(), 40u);

    // Test iterator access
    Trace<double, 200>::iterator iT = tr.begin();

    BOOST_CHECK_EQUAL(iT->first, 0.);
    BOOST_CHECK_EQUAL(iT->second, 1.);

    ++iT; // pre-increment
    BOOST_CHECK_EQUAL(iT->first, tr.GetDt());
    BOOST_CHECK_EQUAL(iT->second, 1.);

    tr[2] = 2.;

    iT++; // post-increment
    BOOST_CHECK_EQUAL(iT->first, 2*tr.GetDt());
    BOOST_CHECK_EQUAL(iT->second, 2.);
  }

  //____________________________________________________________________________
  // Test of Tabulated function
  BOOST_AUTO_TEST_CASE(TabulatedFunc)
  {
    TabulatedFunction<double> f;

    BOOST_CHECK(f.IsEmpty());

    // Push back data out of order
    f.PushBack(0., 1.);
    f.PushBack(2., 3.);
    f.PushBack(1., 5.);
    f.PushBack(3., 4.);

    BOOST_CHECK(!f.IsEmpty());
    BOOST_CHECK_EQUAL(f.GetN(), 4u);

    // Sort by the x-data:
    f.Sort();

    // Check iterator access
    TabulatedFunction<double>::ConstIterator iT;
    {
      double x[4] = { 0., 1., 2., 3. };
      double y[4] = { 1., 5., 3., 4. };
      int i = 0;

      for (iT = f.Begin(); iT != f.End(); ++iT) {
        BOOST_CHECK_EQUAL(iT->GetX(), x[i]);
        BOOST_CHECK_EQUAL(iT->GetY(), y[i]);
        ++i;
      }
    }

    // Sort by the y-data:
    f.SortByOrdinate();

    // Check iterator access
    {
      double x[4] = { 0., 2., 3., 1. };
      double y[4] = { 1., 3., 4., 5.};
      int i = 0;

      for (iT = f.Begin(); iT != f.End(); ++iT) {
        BOOST_CHECK_EQUAL(iT->GetX(), x[i]);
        BOOST_CHECK_EQUAL(iT->GetY(), y[i]);
        ++i;
      }
    }

    // Check linear interpolation
    f.Sort();

    BOOST_CHECK_EQUAL(f.Evaluate(0.5), 3.);
    BOOST_CHECK_EQUAL(f.Evaluate(1.), 5.);
    BOOST_CHECK_EQUAL(f.Evaluate(1.5), 4.);
    BOOST_CHECK_EQUAL(f.Evaluate(2.), 3.);
    BOOST_CHECK_EQUAL(f.Evaluate(2.5), 3.5);
  }

  //____________________________________________________________________________
  // Various power laws and reweighting calculations
  BOOST_AUTO_TEST_CASE(SpecialFunctionTest)
  {
    using namespace SpecialFunctions;

    // Exponential integrals
    BOOST_CHECK_CLOSE(Expint::En(1, 1),   0.21938393439552051, 1e-5);
    BOOST_CHECK_CLOSE(Expint::En(3, 0.1), 0.41629145790827882, 1e-5);
    BOOST_CHECK_CLOSE(Expint::Ei(0.1),   -1.6228128139692766,  1e-5);
    BOOST_CHECK_CLOSE(Expint::Ei(10.),    2492.2289762418773,  1e-5);
    BOOST_CHECK_CLOSE(Expint::Ei(1e-2),  -4.0179294654266693,  1e-5);

    // Beta functions
    BOOST_CHECK_CLOSE(Beta::B(1.1, 2.1),  0.41072165602187843,  1e-5);

    // Spherical harmonics
    double ylm1[] = { 0.282095,
                    -0.0876607, -0.469675,  0.0372098,
                    -0.0211109,  0.188422,  0.558897, -0.0799804, -0.0203865 };
    int j = 0;
    double theta = 164*degree;
    double phi = 113*degree;
    for (int l = 0; l <= 2; ++l) {
      for (int m = -l; m <= l; ++m) {
        BOOST_CHECK_CLOSE(ylm1[j++], Legendre::ReY(l,m, theta,phi), 1e-3);
      }
    }

    double ylm2[] = { 0.282095,
                     -0.3454941, 2.99173e-17, -2.11547e-17, 
                      4.730495e-17, -4.730495e-17, -0.315392, -2.8964e-33, -0.386274 };
    j = 0;
    theta = 90*degree;
    phi = 90*degree;
    for (int l = 0; l <= 2; ++l) {
      for (int m = -l; m <= l; ++m) {
        BOOST_CHECK_CLOSE(ylm2[j++], Legendre::ReY(l,m, theta,phi), 1e-2);
      }
    }
  }

BOOST_AUTO_TEST_SUITE_END()

