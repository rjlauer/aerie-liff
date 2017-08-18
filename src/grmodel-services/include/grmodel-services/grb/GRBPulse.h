/*!
 * @file GRBPulse.h
 * @author Segev BenZvi
 * @date 14 Jul 2011
 * @brief GRB pulse parameterization based on a double-exponential flare
 *        with a band-function spectrum.
 * @version $Id: GRBPulse.h 18002 2013-11-25 17:47:47Z sybenzvi $
 */

#ifndef GRMODEL_SERVICES_GRB_GRBPULSE_H_INCLUDED
#define GRMODEL_SERVICES_GRB_GRBPULSE_H_INCLUDED

class ModifiedJulianDate;

/*!
 * @class GRBPulse
 * @author Nicola Omodei <nicola.omodei@pi.infn.it>
 * @author Segev BenZvi (adapted for AERIE)
 * @date 14 Jul 2011
 * @brief GRB phenomenological pulse description
 *
 * This class has been adapted from the GRBobsPulse class in the Fermi
 * ScienceTools GRBobs package.  Pulses can be used to construct GRB light
 * curves.
 *
 * <h3>Contents</h3>
 * <ul>
 *   <li><a href="#energySpectrum">Energy Spectrum</a></li>
 *   <li><a href="#pulseShape">Pulse Shape</a></li>
 * </ul>
 *
 * <a name="energySpectrum">
 * <h4>Energy Spectrum</h4>
 * The energy spectrum is parameterized as two power laws using the function of
 * <a href="http://adsabs.harvard.edu/abs/1993ApJ...413..281B">Band et al.,
 * ApJ <strong>413</strong> (1993) 281-292</a>:
 * \f[
 *   f(E) = A
 *   \begin{cases}
 *     \left(\frac{E}{\text{100 keV}}\right)^\alpha\ e^{-E/E_0}, &
 *     E \leq (\alpha-\beta)\cdot E_0, \\
 *     \left(\frac{(\alpha-\beta)\cdot E_0}{\text{100 keV}}\right)^{\alpha-\beta}
 *     \left(\frac{E}{\text{100 keV}}\right)^\beta\ e^{\alpha-\beta}, &
 *     E > (\alpha-\beta)\cdot E_0.
 *   \end{cases}
 * \f]
 * The location of the "bend" in the power law is called the peak energy
 * \f$E_\text{peak}\f$ because on an \f$E^2\times\text{flux}\f$ plot the
 * spectrum tends to peak at this location.  The peak energy is given by
 * \f[
 *   E_\text{peak} = (2+\alpha-W_E)E_0,
 * \f]
 * where \f$W_E\f$ is a constant used to control the energy-dependence of the
 * width of the pulse.
 *
 * <a name="pulseShape">
 * <h4>Pulse Shape</h4>
 * The shape of the pulse is given by a 5-parameter fit specified in 
 * <a href="http://adsabs.harvard.edu/abs/1996ApJ...459..393N">Norris et
 * al., ApJ <strong>459</strong> (1996) 393-412</a>:
 * \f[
 *   I(t) = A
 *   \begin{cases}
 *     \exp{\left(-\frac{|t-t_\text{peak}|}{\sigma_r}\right)^\nu}, &
 *     t \leq t_\text{peak} \\
 *     \exp{\left(-\frac{|t-t_\text{peak}|}{\sigma_d}\right)^\nu}, &
 *     t > t_\text{peak}
 *   \end{cases}
 * \f]
 * Notes:
 * <ol>
 *   <li>\f$\sigma_r\f$ and \f$\sigma_d\f$ are the rise and decay times.</li>
 *   <li>\f$\nu\f$ is "peakedness," a measure of pulse sharpness.  Larger
         \f$\nu\f$ gives a flatter pulse.</li>
 * </ol>
 *
 * At low energy, pulses are wider and have a later and slower rise.  This is
 * parameterized in the Fermi software as a FWHM energy relation:
 * \f[
 *   \begin{aligned}
 *     \sigma_r(E) &= \sigma_r\left(\frac{E}{\text{100 keV}}\right)^{-W_E}, \\
 *     \sigma_d(E) &= \sigma_d\left(\frac{E}{\text{100 keV}}\right)^{-W_E}.
 *   \end{aligned}
 * \f]
 * Peaks at different energies are also shifted with respect to 100 keV, the
 * energy used in the Norris parameterization:
 * \f[
 *   t_\text{p}(\text{100 keV}) - t_\text{p}(E) = \Delta t\cdot\sigma_r
 *   \left(1-\left(\frac{E}{\text{100 keV}}\right)^{-W_E}\right)
 *   (\ln{100})^{1/\nu}.
 * \f]
 * Notes:
 * <ol>
 *   <li> In the Fermi software, \f$\Delta t=0.5\f$.</li>
 *   <li> In the Fermi software, \f$W_E=0.33\f$ or \f$0\f$.</li>
 * </ol>
 */
class GRBPulse {

  public:

    GRBPulse();

    double GetIntensity(const double E, const ModifiedJulianDate& mjd) const;

  private:

    double tPeak_;    ///< Time of pulse maximum
    double tStart_;   ///< Start time of pulse
    double tEnd_;     ///< End time of pulse
    double sRise_;    ///< Exponential rise time of pulse
    double sDecay_;   ///< Exponential decay time of pulse
    double I_;        ///< Pulse intensity
    double nu_;       ///< Peakedness parameter (>1 = "flat top")

    double Epeak_;    ///< Peak energy
    double alpha_;    ///< Low-energy spectral index
    double beta_;     ///< High-energy spectral index
    double Ec_;       ///< Critical energy for spectral index transition

};

#endif // GRMODEL_SERVICES_GRB_GRBPULSE_H_INCLUDED

