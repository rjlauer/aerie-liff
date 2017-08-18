/*!
 * @file StdRNGService.cc
 * @brief Random number generator implementation.
 * @author Segev BenZvi
 * @date 22 Sep 2010
 * @version $Id: StdRNGService.cc 17727 2013-10-26 16:31:52Z sybenzvi $
 */

#include <rng-service/StdRNGService.h>

#include <hawcnest/HAWCUnits.h>

#include <boost/nondet_random.hpp>

#include <cmath>
#include <ctime>
#include <unistd.h>
#include <hawcnest/RegisterService.h>

using namespace std;
using namespace HAWCUnits;

REGISTER_SERVICE(StdRNGService);

namespace {

  template<typename T>
  int sgn(const T& val) {
    return (T(0) < val) - (val < T(0));
  }

}

StdRNGService::StdRNGService() :
  rngEngine_(),
  uniformRNG_(rngEngine_, UDist()),
  normalRNG_(rngEngine_, NDist()),
  expRNG_(rngEngine_, EDist())
{
}

Configuration
StdRNGService::DefaultConfiguration()
{
  Configuration config;
  config.Parameter<int>("seed", -1);
  return config;
}

void
StdRNGService::Initialize(const Configuration& config)
{
  int seed;
  config.GetParameter("seed", seed);

  // Negative seed: draw random value from /dev/urandom and seed with that
  if (seed < 0)
    rngEngine_.seed(boost::random::random_device()());
  // Zero seed: set seed using system clock, plus process ID
  else if (seed == 0)
    rngEngine_.seed(static_cast<unsigned int>(time(0) + getpid()));
  // Else use a fixed seed specified by the user
  else
    rngEngine_.seed(static_cast<unsigned int>(seed));
}

double
StdRNGService::Gaussian(const double mu, const double sigma)
  const
{
  return mu + sigma * normalRNG_();
}

double
StdRNGService::LogNormal(const double mu, const double sigma)
  const
{
  return exp(mu + sigma*normalRNG_());
}

int
StdRNGService::Poisson(const double mu)
  const
{
  // Implement PTRD algorithm of W. Hörmann, Ins. Math. Econ. 12:39, 1993
  // (transformed rejection method to generate Poisson random variables).
  if (mu < 0.)
    return 0;
  
  // If the mean is below 10, use a simple CDF inversion algorithm to get the
  // return value
  if (mu < 10.) {
    double emu = exp(-mu);
    int x = 0;
    double u = Uniform();
    while (u > emu) {
      u -= emu;
      ++x;
      emu *= mu/x;
    }
    return x;
  }

  // Table of ln(k!)
  static const double logkfac[10] = {
    0., 0.,
    0.69314718055994529, 1.7917594692280550, 3.1780538303479458,
    4.7874917427820458, 6.5792512120101012, 8.5251613610654147,
    10.604602902745251, 12.801827480081469
  };

  // Step 0: setup
  const double smu = sqrt(mu);
  const double b = 0.921 + 2.53*smu;
  const double a = -0.059 + 0.02483*b;
  const double inv_alpha = 1.1239 + 1.1328/(b - 3.4);
  const double vr = 0.9277 - 3.6224/(b - 2.);
  const double logSqrt2pi = log(sqrt(2*pi));

  double U;
  double V;
  double us;
  int k;

  while (true) {
    // Step 1: generate uniform RNs
    V = Uniform();
    if (V < 0.86*vr) {
      U = V/vr - 0.43;
      return int(floor((2*a/(0.5 - abs(U)) + b)*U + mu + 0.445));
    }

    // Step 2: generate more uniform RNs
    if (V >= vr)
      U = Uniform(-0.5, 0.5);
    else {
      U = V/vr - 0.93;
      U = sgn(U)*0.5 - U;
      V = Uniform(0, vr);
    }

    // Step 3.0: go to step 1
    us = 0.5 - abs(U);
    if (us < 0.013 && V > us)
      continue;

    // Step 3.1: calculate return value for large k
    k = floor((2*a/us + b)*U + mu + 0.445);
    V *= inv_alpha/(a/(us*us) + b);
    if (k >= 10. && log(V*smu) <= (k+0.5)*log(mu/k) - mu - logSqrt2pi + k -
                                  (1./12. - (1./360. - 1/(1260.*k*k))/(k*k))/k)
    {
      return int(k);
    }

    // Step 3.2: calculate return value for small k
    if (k >= 0. && k <= 9.) {
      if (log(V) <= k*log(mu) - mu - logkfac[int(k)])
        return int(k);
    }
  }
}

int
StdRNGService::BinomialInversion(const int n, const double p)
  const
{
  const double q = 1. - p;
  const double s = p / q;
  const double a = (n+1) * s;
  double r = pow(q, n);
  double u = Uniform();
  int k = 0.;
  while (u > r) {
    u = u - r;
    ++k;
    r *= (a/k) - s;
  }
  return k;
}

int
StdRNGService::StirlingCorrection(int k)
  const
{
  static const double fc[10] = {
    0.08106146679532726, 0.04134069595540929, 0.02767792568499834,
    0.02079067210376509, 0.01664469118982119, 0.01387612882307075,
    0.01189670994589177, 0.01041126526197209, 0.009255462182712733,
    0.008330563433362871
  };

  if (k < 10)
    return fc[k];
  else {
    const double ikp1 = 1./(k + 1);
    return (1./12 - (1./360 - 1./1260*(ikp1*ikp1))*(ikp1*ikp1))*ikp1;
  }
}

int
StdRNGService::BinomialRejection(const int n, const double pr)
  const
{
  // Binomial transformation rejection algorithm used in boost::random, from
  // W. Hörmann, J. Stat. Comp. Sim. 46:101, 1993
  // The algorithm is valid for np>=10 and p<0.5.

  // Step 0: prepare constants
  const double p = (0.5 < pr) ? (1. - pr) : pr;
  const int m = int((n+1)*p);
  const double r = p/(1.-p);
  const double nr = (n+1)*r;
  const double npq = n*p*(1.-p);
  const double snpq = sqrt(npq);
  const double b = 1.15 + 2.53*snpq;
  const double a = -0.0873 + 0.0248*b + 0.01*p;
  const double c = n*p + 0.5;
  const double alpha = (2.83 + 5.1/b) * snpq;
  const double vr = 0.92 - 4.2/b;
  const double urvr = 0.86 * vr;

  double U;
  double V;

  while (true) {
    // Step 1: generate a uniform random number v
    V = Uniform();
    if (V <= urvr) {
      U = V/vr - 0.43;
      return int(floor((2*a/(0.5-abs(U)) + b)*U + c));
    }

    // Step 2: if V > vr, generate a uniform number in (-0.5,0.5)
    if (V >= vr)
      U = Uniform(-0.5, 0.5);
    else {
      U = V/vr - 0.93;
      U = sgn(U)*0.5 - U;
      V = Uniform(0., vr);
    }

    // Step 3: generate k
    double us = 0.5 - abs(U);
    int k = int(floor((2*a/us + b)*U + c));
    if (k < 0 || k > n)
      continue;

    V *= alpha/(a/(us*us) + b);
    int km = abs(k - m);

    // Step 3.1: recursive evaluation of Stirling correction factors
    if (km <= 15) {
      double f = 1.;
      if (m < k) {
        int i = m;
        do {
          ++i;
          f *= nr/i - r;
        }
        while (i != k);
      }
      else if (m > k) {
        int i = k;
        do {
          ++i;
          V *= nr/i - r;
        }
        while (i != m);
      }

      if (V <= f)
        return k;
      else
        continue;
    }
    // Step 3.2: squeeze-acceptance or rejection
    else {
      V = log(V);
      double rho = (km/npq) * (((km/3. + 0.625)*km + 1./6)/npq + 0.5);
      double t = -km*km/(2*npq);

      if (V < t-rho)
        return k;
      if (V > t + rho)
        continue;

      // Step 3.3: final setup
      int nm = n - m + 1;
      double h = (m + 0.5)*log((m + 1)/(r*nm)) + StirlingCorrection(m)
                                               + StirlingCorrection(n - m);

      // Step 3.4: final acceptance-rejection test
      int nk = n - k + 1;
      if (V <= h + (n + 1)*log(double(nm)/nk)
                 + (k + 0.5)*log(nk*r/(k + 1))
                 - StirlingCorrection(k)
                 - StirlingCorrection(n - k))
        return k;
      else
        continue;
    }
  }
}

int
StdRNGService::Binomial(const int n, const double p)
  const
{
  // Use inversion algorithm for binomail mean < 10
  if (n*p < 10.) {
    if (p > 0.5)
      return n - BinomialInversion(n, 1.-p);
    else
      return BinomialInversion(n, p);
  }
  // Use rejection algorithm if mean >= 10 and p <= 0.5.  Algorithm from
  // W. Hörmann, J. Stat. Comp. Sim. 46:101, 1993, used in boost::random
  else {
    if (p > 0.5)
      return n - BinomialRejection(n, p);
    else
      return BinomialRejection(n, p);
  }
}

double
StdRNGService::Rician(const double nu, const double sigma)
  const
{
  double theta = Uniform(0., 360*degree);
  double x = Gaussian(nu*cos(theta), sigma);
  double y = Gaussian(nu*sin(theta), sigma);
  return sqrt(x*x + y*y);
}

double
StdRNGService::Uniform(const double a, const double b)
  const
{
  return a + (b-a)*uniformRNG_();
}

double
StdRNGService::Exponential(const double lambda)
  const
{
  return expRNG_() / lambda;
}

double
StdRNGService::PowerLaw(const double n, const double a, const double b)
  const
{
  const double u = Uniform();

  if (n == -1.) {
    return pow(a, 1.-u) * pow(b, u);
  }
  else {
    const double a_np1 = pow(a, n+1.);
    const double b_np1 = pow(b, n+1.);

    return pow((b_np1 - a_np1)*u + a_np1, 1./(n+1.));
  }
}

double
StdRNGService::CutoffPowerLaw(const double n, const double lambda,
                              const double a, const double b)
  const
{
  double x;
  do {
    x = this->PowerLaw(n, a, b);
  }
  while (uniformRNG_() > exp(-lambda*x));

  return x;
}

