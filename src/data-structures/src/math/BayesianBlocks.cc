/*!
 * @file BayesianBlocks.cc
 * @brief Implementation of the Bayesian Blocks binning algorithm of J.D.
 *        Scargle et al., ApJ 764 (2013) 167.  Based on the python
 *        implementation written by Jake VanderPlas.
 * @author Segev BenZvi
 * @date 17 May 2014
 * @version $Id: BayesianBlocks.cc 19975 2014-05-18 03:43:57Z sybenzvi $
 */

#include <data-structures/math/BayesianBlocks.h>
#include <hawcnest/Logging.h>

using namespace std;

namespace BayesianBlocks {

  // Calculate binning for an unbinned data series t.
  Array
  calculateBins(const Array& t, const Prior& pr, const Fitness& fit)
  {
    // Find unique values of t
    vector<double> unq_t;

    // Generate an array of bin counts x
    Array x;
    vector<double>::const_iterator i;
    size_t j = 0;
    size_t k = 0;
    double bincount = 1;
    for (i = t.begin(), j = 0, k = 0; i != t.end(); ++i, ++j, ++k) {
      if (unq_t.empty()) {
        unq_t.push_back(*i);
      }
      else {
        if (unq_t.back() == *i) {
          ++bincount;
          continue;
        }
        unq_t.push_back(*i);
        x.push_back(bincount);
        bincount = 1;
      }
    }
    x.push_back(bincount);

    // Create a fake error array and calculate the optimal binning
    Array sigma(x.size(), 1.);
    return calculateBins(t, x, sigma, pr, fit);
  }

  // Calculate binning for a data series x vs. t.
  Array
  calculateBins(const Array& t, const Array& x,
                 const Prior& pr, const Fitness& fit)
  {
    // Create a fake error array and calculate the optimal binning
    Array sigma(t.size(), 1.);
    return calculateBins(t, x, sigma, pr, fit);
  }

  // Calculate binning for a data series x vs. t, with uncertainties sigma on
  // the values x.
  Array
  calculateBins(const Array& t, const Array& x, const Array& sigma,
                 const Prior& pr, const Fitness& fit)
  {
    // Find unique values of t
    Array unq_t;
    std::unique_copy(t.begin(), t.end(), std::back_inserter(unq_t));
    if (t.size() != unq_t.size())
      log_fatal("t array has duplicate values.");

    // Create quantities ak, bk needed by fitness function
    const size_t N = unq_t.size();

    Array ak_raw(N, 1);
    Array bk_raw(x);
    double norm;
    size_t j;
    for (j = 0; j < ak_raw.size(); ++j) {
      norm = 1. / (sigma[j]*sigma[j]);
      ak_raw[j] *= norm;
      bk_raw[j] *= norm;
    }

    // Create an array of N+1 bin edges
    Array edges(N+1);
    edges[0] = unq_t[0];
    for (j = 1; j < N; ++j)
      edges[j] = 0.5*(unq_t[j] + unq_t[j-1]);
    edges[N] = unq_t[N-1];

    Array blockLength(N+1);
    for (j = 0; j <= N; ++j)
      blockLength[j] = unq_t[N-1] - edges[j];

    // Create arrays to store the best configuration
    Array best(N, 0);
    vector<int> last(N, 0);

    Array Tk(N);        // width/duration of each block
    Array Nk(N);        // number of elements in each block
    Array ak(N);        // ak of eq. 31
    Array bk(N);        // bk of eq. 31
    Array A_R(N, 0);    // intermediate block variable
    Array fitVec(N, 0); // maximum likelihood block fitness

    // Start with the first data cell; add one cell at each iteration
    for (size_t R = 0; R < N; ++R) {
      // width/duration of each block
      for (j = 0; j < R+1; ++j)
        Tk[j] = blockLength[j] - blockLength[R+1];

      // number of elements in each block
      size_t iN = R + 1;
      double cumSum = 0.;
      double cumSumAk = 0.;
      double cumSumBk = 0.;
      for (int l = R; l >= 0; --l) {
        cumSum += x[l];
        cumSumAk += ak_raw[l];
        cumSumBk += bk_raw[l];
        --iN;
        Nk[iN] = cumSum;
        ak[iN] = 0.5*cumSumAk;
        bk[iN] = -cumSumBk;
      }

      // Evaluate the fitness function
      if (fit.GetName() == "events")
        fit(R+1, &Nk[0], &Tk[0], &fitVec[0]);
      else if (fit.GetName() == "measures")
        fit(R+1, &ak[0], &bk[0], &fitVec[0]);
      else
        log_fatal(fit.GetName() << " has not been implemented.");

      const double prior = pr(R+1, N);

      double aMax = 0.;
      int iMax = 0;
      for (j = 0; j < R+1; ++j) {
        A_R[j] = fitVec[j] - prior;
        if (j > 0)
          A_R[j] += best[j-1];
        if (A_R[j] > aMax) {
          aMax = A_R[j];
          iMax = j;
        }
      }

      last[R] = iMax;
      best[R] = A_R[iMax];
    }

    // Find changepoints by iteratively peeling off the last block
    vector<int> changePoints(N, 0);
    int iCP = N;
    int idx = N;
    while (true) {
      --iCP;
      changePoints[iCP] = idx;
      if (idx == 0)
        break;
      idx = last[idx - 1];
    }

    Array nedges(changePoints.size() - iCP);
    for (j = iCP; j < changePoints.size(); ++j)
      nedges[j-iCP] = edges[changePoints[j]];

    return nedges;
  }

}

