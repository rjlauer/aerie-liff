/*!
 * @file RNGService.h
 * @brief Abstract random number generator interface.
 * @author Segev BenZvi
 * @date 22 Sep 2010
 * @version $Id: RNGService.h 17728 2013-10-26 17:38:45Z sybenzvi $
 */

#ifndef RNGSERVICE_RNGSERVICE_H_INCLUDED
#define RNGSERVICE_RNGSERVICE_H_INCLUDED

/*!
 * @class RNGService
 * @author Segev BenZvi
 * @date 22 Sep 2010
 * @ingroup rndm_gen
 * @brief Abstract interface for services which generate random numbers from a
 *        few commonly used distributions.
 */
class RNGService {

  public:

    virtual ~RNGService() { }

    /*!
     * @brief Draw an integer from a Poisson distribution with mean @a mu.
     * 
     * Return an integer \f$k\f$ from the Poisson distribution, defined by
     * \f[
     *   p(k)=\frac{\lambda^k e^{-\lambda}}{k!}.
     * \f]
     */
    virtual int Poisson(const double mu=1) const = 0;

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
    virtual int Binomial(const int n=10, const double p=0.5) const = 0;

    /*!
     * @brief Draw a value from a Gaussian of mean @a mu and width @a sigma
     *
     * Generate a random number \f$x\f$ from a symmetric Gaussian distribution
     * of mean \f$\mu\f$ and width \f$\sigma\f$.  The probability density is
     * \f[
     *   p(x)=\frac{1}{\sqrt{2\pi}\sigma}e^{-\frac{(x-\mu)^2}{2\sigma^2}}.
     * \f]
     */
    virtual double Gaussian(const double mu=0, const double sigma=1) const = 0;

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
    virtual double LogNormal(const double mu=0, const double sigma=1) const = 0;

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
    virtual double Rician(const double nu=0, const double sigma=1) const = 0;

    /*!
     * @brief Generate a random uniform number between the values @a a and @a b.
     */
    virtual double Uniform(const double a=0, const double b=1) const = 0;

    /*!
     * @brief Generate an exponential number with rate parameter @a lambda.
     *
     * Generate a random number \f$x\f$ from an exponential distribution
     * \f[
     *   p(x)=\lambda e^{-\lambda x}
     * \f]
     * where \f$\lambda\f$ is the rate parameter (or inverse scale parameter).
     */
    virtual double Exponential(const double lambda=1) const = 0;

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
    virtual double PowerLaw(const double n,
                            const double a, const double b) const = 0;

    /*!
     * @brief Draw from a power law with index @a n and exponential cutoff with
     *        rate parameter @a lambda on the interval [@a a, @a b].
     *
     * Generate a random number \f$x\f$ from a power law distribution
     * \f[
     *   p(x)\propto x^n e^{-\lambda x}.
     * \f]
     */
    virtual double CutoffPowerLaw(const double n, const double lambda,
                                  const double a, const double b) const = 0;

};

#endif // RNGSERVICE_RNGSERVICE_H_INCLUDED

