/*!
 * @file SimParticle.h
 * @brief Ground-level particle data.
 * @author Segev BenZvi
 * @ingroup event_data
 * @version $Id: SimParticle.h 18879 2014-02-19 21:41:29Z sybenzvi $
 */

#ifndef DATACLASSES_EVENT_SIMPARTICLE_H_INCLUDED
#define DATACLASSES_EVENT_SIMPARTICLE_H_INCLUDED

#include <data-structures/physics/Particle.h>

/*!
 * @class SimParticle
 * @author Segev BenZvi
 * @ingroup event_data
 * @brief Energy and geometry properties of simulated particles that reached
 *        ground level and potentially triggered a PMT
 */
class SimParticle : public Baggable {

  public:

    ParticleType particleId_;
    float energy_;
    float theta_;
    float phi_;
    float x_;
    float y_;

};

SHARED_POINTER_TYPEDEFS(SimParticle);

#endif // DATACLASSES_EVENT_SIMPARTICLE_H_INCLUDED

