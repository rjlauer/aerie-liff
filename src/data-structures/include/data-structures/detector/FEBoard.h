/*!
 * @file FEBoard.h
 * @brief Electronic model of the Milagro front-end board.
 * @author Segev BenZvi
 * @version $Id: FEBoard.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

#ifndef DATACLASSES_DETECTOR_FEBOARD_H_INCLUDED
#define DATACLASSES_DETECTOR_FEBOARD_H_INCLUDED

namespace det {

  /*!
   * @class ABChannel
   * @author Segev BenZvi
   * @date 10 Feb 2012
   * @brief Functional description of a channel on the Milagro analog board
   *
   * Logically the analog channels in the Milagro front-end boards comprise
   * three parts:
   * <ol>
   *   <li>An amplifier that shapes and amplifies the signal.</li>
   *   <li>An RC integrator.</li>
   *   <li>A voltage discriminator.</li>
   * </ol>
   *
   * The input to a channel is a voltage pulse from the PMT, and the output is
   * an ECL logic pulse from the discriminator.  The channel output is fed into
   * a digital board that implements coincidence logic.
   */
  class ABChannel {

    public:

      ABChannel();

      ABChannel(const double rInt, const double cInt,
                const double gm, const double vThr);

      /// Retrieve the transconductance of the operational amplifier
      double GetGm() const { return gm_; }
      void SetGm(const double gm) { gm_ = gm; }

      /// Retrieve the value of the integrating resistor
      double GetRint() const { return rInt_; }
      void SetRint(const double ri) { rInt_ = ri; }

      /// Retrieve the value of the integrating capacitor
      double GetCint() const { return cInt_; }
      void SetCint(const double ci) { cInt_ = ci; }

      /// Retrieve the voltage threshold of the channel discriminator
      double GetThreshold() const { return vThr_; }
      void SetThreshold(const double vt) { vThr_ = vt; }
    
    private:

      double rInt_;       ///< Resistance in integrator circuit
      double cInt_;       ///< Capacitance in integrator circuit
      double gm_;         ///< Channel op-amp transconductance
      double vThr_;       ///< Discriminator voltage threshold

    friend class AnalogBoard;

  };

  /*!
   * @class AnalogCard
   * @author Segev BenZvi
   * @date 10 Feb 2012
   * @brief Representation of the analog section of a Milagro front-end board
   *
   * The analog boards in Milagro split the PMT input into a low- and
   * high-gain channel for improved dynamic range.  The channels have
   * unique amplification and RC time constants, and the signals are separately
   * discriminated using preset voltage thresholds.  At the end of the board two
   * ECL logic pulses are passed to a digital card.
   */
  class AnalogCard {

    public:

      AnalogCard() { }

      AnalogCard(const ABChannel& loCh, const ABChannel& hiCh) :
        loCh_(loCh),
        hiCh_(hiCh)
      {
      }

      /// Access the low-gain channel
      const ABChannel& GetLowThreshChannel() const { return loCh_; }

      /// Access the high-gain channel
      const ABChannel& GetHighThreshChannel() const { return hiCh_; }

    private:

      ABChannel loCh_;    ///< Low-threshold analog channel
      ABChannel hiCh_;    ///< High-threshold analog channel

  };

  /*!
   * @class FEBoard
   * @author Ian Wisher
   * @author Segev BenZvi 
   * @date 23 Sep 2010
   * @brief FEB electronics: a blocking capacitor and terminating resistor on
   *        input, followed by analog and digital cards
   *
   * This represents the Milagro front-end boards, which comprise an analog
   * card and a digital card.  On the each channel of the analog card the
   * signals are inverted, amplified, shaped, and discriminated.  The output
   * from each channel is an ECL pulse from the discriminator.  There are
   * actually two channels per signal (low- and high-gain).
   *
   * The low- and high-gain ECL pulses are sent to the digital cards, where the
   * signals are split, delayed, and recombined to build the usual two-edge
   * pulses (for small events passing only the low threshold) and four-edge
   * events (for large pulses passing both thresholds).
   */
  class FEBoard {
    
    public:

      FEBoard();

      /// Initialize given DC blocking capacitor, terminating resistor,
      /// op-amp transconductance, integrator resistance and capacitance,
      /// and voltage noise RMS

      FEBoard(const double cb, const double rt, const double vRMS,
              const AnalogCard& ac);

      virtual ~FEBoard() { }

      /// Retrieve the value of the DC blocking capacitor at the board input
      double GetCblock() const { return cBlock_; }
      void SetCblock(const double cl) { cBlock_ = cl; }

      /// Retrieve the value of the terminating resistor at the board input
      double GetRterm() const { return rTerm_; }
      void SetRterm(const double rt) { rTerm_ = rt; }

       /// Retrieve the FEB noise introduced at the input
       double GetVoltageNoiseDensity() const { return vNoiseD_; }
       void SetVoltageNoiseDensity(const double rms) { vNoiseD_ = rms; }

       /// Retrieve the analog card properties of the board
       const AnalogCard& GetAnalogCard() const { return ac_; }
       void SetAnalogCard(const AnalogCard& ac) { ac_ = ac; }
 
    protected:

      double cBlock_;     ///< Blocking capacitance at input
      double rTerm_;      ///< Terminating resistance at input
      double vNoiseD_;    ///< Voltage noise density of the FEB

      AnalogCard ac_;     ///< The analog board

  };

}

#endif // DATACLASSES_DETECTOR_FEBOARD_H_INCLUDED

