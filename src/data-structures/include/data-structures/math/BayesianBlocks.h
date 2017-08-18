/*!
 * @file BayesianBlocks.h
 * @brief Implementation of the Bayesian Blocks binning algorithm of J.D.
 *        Scargle et al., ApJ 764 (2013) 167.  Based on the python
 *        implementation written by Jake VanderPlas.
 * @author Segev BenZvi
 * @date 17 May 2014
 * @version $Id: BayesianBlocks.h 19975 2014-05-18 03:43:57Z sybenzvi $
 */

#ifndef DATACLASSES_MATH_BAYESIAN_BLOCKS_H_INCLUDED
#define DATACLASSES_MATH_BAYESIAN_BLOCKS_H_INCLUDED

#include <cmath>
#include <string>
#include <vector>

namespace BayesianBlocks {

  typedef std::vector<double> Array;

  /*!
   * @brief Interface for ncp_prior on number of blocks in a data set.
   */
  class Prior {
    public:
      virtual double operator()(const double N, const double Ntot) const = 0;
  };

  /*!
   * @brief Geometric prior on the number of blocks in the data, parameterized
   *        by gamma.  Eq. 3 in J.D. Scargle et al., ApJ 764 (2013) 167.
   */
  class GammaPrior : public Prior {

    public:

      GammaPrior(const double gamma=0.05) : gamma_(gamma) { }

      double operator()(const double N, const double Ntot) const {
        return std::log(1.-gamma_) - std::log(1.-std::pow(gamma_, Ntot+1))
                               + N * std::log(gamma_);
      }

    private:

      double gamma_;

  };

  /*!
   * @brief Simulation-tuned prior parameterized by false-positive rate p0,
   *        from eq. 21 of J.D. Scargle et al., ApJ 764 (2013) 167.
   */
  class P0Prior : public Prior {

    public:

      P0Prior(const double p0=0.05) : p0_(p0) { }

      double operator()(const double N, const double Ntot) const
      { return 4. - std::log(73.53 * p0_ * std::pow(N, -0.478)); }

    private:

      double p0_;

  };

  /*!
   * @brief Prior on number of blocks in point measurements (y vs. x), from end
   *        of Section 3.3 of J.D. Scargle et al., ApJ 764 (2013) 167.
   */
  class PointPrior : public Prior {
    public:
      double operator()(const double N, const double Ntot) const
      { return 1.32 + 0.577*std::log10(N); }
  };

  /*!
   * @brief Interface for block fitness functions.
   */
  class Fitness {
    public:
      Fitness(const std::string& name) : name_(name) { }

      const std::string& GetName() const { return name_; }

      virtual Array operator()(const Array&, const Array&)
                              const = 0;
      virtual Array operator()(const size_t n, const double*, const double*)
                              const = 0;
      virtual void operator()(const size_t n, const double*, const double*,
                              double*) const = 0;
    protected:
      std::string name_;
  };

  /*!
   * @brief Maximum likelihood of binned or unbinned event data, from eq. 19 of
   *        J.D. Scargle et al., ApJ 764 (2013) 167.
   */
  class EventFitness : public Fitness {
    public:
      EventFitness() : Fitness("events") { }

      Array operator()(const Array& N, const Array& T) const {
        return this->operator()(N.size(), &N[0], &T[0]);
      }

      Array operator()(const size_t n, const double* N, const double* T) const {
        Array result(n, 0);
        this->operator()(n, N, T, &result[0]);
        return result;
      }

      void operator()(const size_t n, const double* N, const double* T,
                      double* result)
        const
      {
        for (size_t k = 0; k < n; ++k)
          result[k] = N[k] * (std::log(N[k]) - std::log(T[k]));
      }
  };

  /*!
   * @brief Maximum block likelihood of point measurements, from eq. 41 of
   *        J.D. Scargle et al., ApJ 764 (2013) 167.
   */
  class PointFitness : public Fitness {
    public:
      PointFitness() : Fitness("measures") { }

      Array operator()(const Array& a, const Array& b) const {
        return this->operator()(a.size(), &a[0], &b[0]);
      }

      Array operator()(const size_t n, const double* a, const double* b) const {
        Array result(n, 0);
        this->operator()(n, a, b, &result[0]);
        return result;
      }

      void operator()(const size_t n, const double* a, const double* b,
                      double* result)
        const
      {
        for (size_t k = 0; k < n; ++k)
          result[k] = (b[k] * b[k]) / (4 * a[k]);
      }
  };

  using namespace std;

  /*!
   * @brief Calculate binning for an unbinned data series t.
   */
  Array
  calculateBins(const Array& t,
                const Prior& pr, const Fitness& fit);

  /*!
   * @brief Calculate binning for a data series x vs. t.
   */
  Array
  calculateBins(const Array& t, const Array& x,
                const Prior& pr, const Fitness& fit);

  /*!
   * @brief Calculate binning for a data series x vs. t, with uncertainties
   *        sigma on the values x.
   */
  Array
  calculateBins(const Array& t, const Array& x, const Array& sigma,
                const Prior& pr, const Fitness& fit);

}

#endif // DATACLASSES_MATH_BAYESIAN_BLOCKS_H_INCLUDED

