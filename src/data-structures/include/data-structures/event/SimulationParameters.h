/*!
 * @file SimulationParameters.h
 * @brief Parameters used in simulation and reconstruction.
 * @author Brian Baughman
 * @ingroup event_data
 * @version $Id: SimulationParameters.h 18879 2014-02-19 21:41:29Z sybenzvi $
 */

#ifndef DATACLASSES_EVENT_SIMULATIONPARAMETERS_H_INCLUDED_H_INCLUDED
#define DATACLASSES_EVENT_SIMULATIONPARAMETERS_H_INCLUDED_H_INCLUDED

#include <hawcnest/processing/Bag.h>

#include <iostream>

/*!
 * @class SimulationParameters
 * @author Brian Baughman
 * @ingroup event_data
 * @brief Simulation parameters used to generate events
 */

class SimulationParameters : public Baggable {

  public:

    SimulationParameters():
      jitter_(0),
      tnoise_(0),
      qerr_(0),
      minHits_(0),
      prescale_(0),
      noise_(0)
    {};
    double jitter_; // PMT jitter in ns
    double tnoise_; // Timing noise on each hit in ns.
    double qerr_;   // Fractional charge error
    int minHits_;   // Minimum number of hits require to reconstruct event.
    int prescale_;  // Prescale factor of cut events.
    double noise_;  // Noise rate in Hz

    double GetJitter(void) const {return jitter_;}
    void SetJitter(double jitter)  {jitter_ = jitter;}

    double GetTNoise(void) const {return tnoise_;}
    void SetTNoise(double tnoise)  {tnoise_ = tnoise;}

    double GetQErr(void) const {return qerr_;}
    void SetQErr(double qerr)  {qerr_ = qerr;}

    int GetMinHits(void) const {return minHits_;}
    void SetMinHits(int minHits)  {minHits_ = minHits;}

    int GetPrescale(void) const {return prescale_;}
    void SetPrescale(int prescale)  {prescale_ = prescale;}

    double GetNoise(void) const {return noise_;}
    void SetNoise(double noise)  {noise_ = noise;}


};

SHARED_POINTER_TYPEDEFS(SimulationParameters);

#endif // DATACLASSES_EVENT_SIMULATIONPARAMETERS_H_INCLUDED_H_INCLUDED


