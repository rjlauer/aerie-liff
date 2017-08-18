/*!
 * @file StdRNGService.h
 * @brief Random number generator implementation.
 * @author Segev BenZvi
 * @date 22 Sep 2010
 * @version $Id: StdRNGService.h 17973 2013-11-21 16:45:27Z sybenzvi $
 */

#ifndef RNGSERVICE_STDRNGSERVICE_H_INCLUDED
#define RNGSERVICE_STDRNGSERVICE_H_INCLUDED

#include <rng-service/RNGService.h>

#include <hawcnest/Service.h>

#include <boost/random.hpp>

/*!
 * @class StdRNGService
 * @author Segev BenZvi
 * @date 22 Sep 2010
 * @ingroup rndm_gen
 * @brief Implements a default service for sampling random numbers from several
 *        commonly used distributions.
 *
 * This service provides random number generation based on the @c boost::random
 * implementation of the Mersenne Twister (MT) pseudorandom number generator.
 * The Mersenne Twister has substantially better numerical properties than
 * standard library generators such as @c rand() and @c rand48() at a relatively
 * small cost of efficiency.  The details of @c boost::random are completely
 * hidden by the StdRNGService interface, so user code will not be coupled to
 * boost.
 *
 * Note that the service is probably not thread-safe, in the sense that
 * multiple copies of the RNG engine may be created in different threads.
 * If these copies have the same seed, then the different threads will generate
 * identical sequences of numbers.  Therefore, use with caution in a
 * multithreaded environment.  (As of October 2013 AERIE does not support
 * multithreading.)
 *
 * Also note that users are expected to run the RNG in an intelligent way when
 * submitting many jobs in parallel.  Each job should get a unique random seed,
 * or else they will all generate the same random number sequence.  By default
 * the RNG engine is seeded with a pseudo-random number from @c /dev/random, 
 * which should in principle ensure that parallel jobs are not seeded
 * identically.
 */
class StdRNGService : public RNGService {

  public:

    typedef RNGService Interface;

    StdRNGService();

    Configuration DefaultConfiguration();

    void Initialize(const Configuration& config);

    void Finish() { }

    /*!
     * @brief Draw an integer from a Poisson distribution with mean @a mu.
     * 
     * Return an integer \f$k\f$ from the Poisson distribution, defined by
     * \f[
     *   p(k)=\frac{\lambda^k e^{-\lambda}}{k!}.
     * \f]
     */
    int Poisson(const double mu=1) const;

    /*!
     * @brief Draw an integer from a Binomial distribution with parameters
     *        @a n, @a p.
     *
     * Return the number of successes \f$k\f$ drawn from \f$n\f$ trials with
     * the probability of success \f$p\f$ in a given trial.  The probability
     * is given by the binomial distribution
     * \f[
     *   p(k)=p^k(1-p)^{n-k}.
     * \f]
     */
    int Binomial(const int n=10, const double p=0.5) const;

    /*!
     * @brief Draw a value from a Gaussian of mean @a mu and width @a sigma
     *
     * Generate a random number \f$x\f$ from a symmetric Gaussian distribution
     * of mean \f$\mu\f$ and width \f$\sigma\f$.  The probability density is
     * \f[
     *   p(x)=\frac{1}{\sqrt{2\pi}\sigma}e^{-\frac{(x-\mu)^2}{2\sigma^2}}.
     * \f]
     */
    double Gaussian(const double mu=0, const double sigma=1) const;

    /*!
     * @brief Draw from a log-normal distribution of mean @a mu and width
     *        @a sigma.
     *
     * Generate a random number \f$x\f$ from a log-normal distribution
     * of mean \f$\mu\f$ and width \f$\sigma\f$.  The probability density is
     * \f[
     *   p(x)=\frac{1}{\sqrt{2\pi}x\sigma}e^{-\frac{(\ln{x}-\mu)^2}{2\sigma^2}}.
     * \f]
     */
    double LogNormal(const double mu=0, const double sigma=1) const;

    /*!
     * @brief Draw a number from a Rice distribution of mean @a nu and width
     *        @a sigma.
     *
     * Generate a random number \f$x\f$ from a Rice distribution
     * of mean \f$\nu\f$ and width \f$\sigma\f$.  The probability density is
     * \f[
     *   p(x)=\frac{x}{\sigma^2}e^{-\frac{x^2+\nu^2}{2\sigma^2}}
              I_0\left(\frac{x\nu}{\sigma^2}\right)
     * \f]
     * where \f$I_0(z)\f$ is the zero-order Bessel function of the first kind.
     */
    double Rician(const double nu=0, const double sigma=1) const;

    /*!
     * @brief Generate a random uniform number between the values @a a and @a b.
     */
    double Uniform(const double a=0, const double b=1) const;

    /*!
     * @brief Generate an exponential number with rate parameter @a lambda.
     *
     * Generate a random number \f$x\f$ from an exponential distribution
     * \f[
     *   p(x)=\lambda e^{-\lambda x}
     * \f]
     * where \f$\lambda\f$ is the rate parameter (or inverse scale parameter).
     */
    double Exponential(const double lambda=1) const;

    /*!
     * @brief Draw from a power law with index @a n on the interval
     *        [@a a, @a b].
     *
     * Generate a random number \f$x\f$ from a power law distribution
     * \f[
     *   p(x)\propto x^n
     * \f]
     * with spectral index \f$n\f$.
     */
    double PowerLaw(const double n,
                    const double a, const double b) const;

    /*!
     * @brief Draw from a power law with index @a n and exponential cutoff with
     *        rate parameter @a lambda on the interval [@a a, @a b].
     *
     * Generate a random number \f$x\f$ from a power law distribution
     * \f[
     *   p(x)\propto x^n e^{-\lambda x}.
     * \f]
     */
    double CutoffPowerLaw(const double n, const double lambda,
                          const double a, const double b) const;

  private:

    typedef boost::mt19937 RNGEngine;

    typedef boost::uniform_real<> UDist;
    typedef boost::variate_generator<RNGEngine&, UDist> UGenerator;

    typedef boost::normal_distribution<> NDist;
    typedef boost::variate_generator<RNGEngine&, NDist> NGenerator;

    typedef boost::exponential_distribution<> EDist;
    typedef boost::variate_generator<RNGEngine&, EDist> EGenerator;

    mutable RNGEngine rngEngine_;         ///< Random engine (Mersenne Twister)
    mutable UGenerator uniformRNG_;       ///< Uniform number generator
    mutable NGenerator normalRNG_;        ///< Normal number generator
    mutable EGenerator expRNG_;           ///< Exponential number generator

    /// Binomial variate generator using an inversion algorithm
    int BinomialInversion(const int n, const double p) const;

    /// Binomial variate generator using a transformation/rejection algorithm
    int BinomialRejection(const int n, const double p) const;

    /// Corrections to Stirling's approximation for log(k!)
    int StirlingCorrection(int k) const;

};


#endif // RNGSERVICE_STDRNGSERVICE_H_INCLUDED

