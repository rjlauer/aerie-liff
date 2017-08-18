/*!
 * @file SimEventHeader.h
 * @brief Simulated event primary particle and air shower data.
 * @author Segev BenZvi
 * @ingroup event_data
 * @version $Id: SimEventHeader.h 26722 2015-08-19 18:00:45Z sybenzvi $
 */

#ifndef DATACLASSES_EVENT_SIMEVENTHEADER_H_INCLUDED_H_INCLUDED
#define DATACLASSES_EVENT_SIMEVENTHEADER_H_INCLUDED_H_INCLUDED

#include <data-structures/physics/Particle.h>

#include <hawcnest/processing/Bag.h>

#include <iostream>

/*!
 * @class SimEventHeader
 * @author Segev BenZvi
 * @ingroup event_data
 * @brief Primary shower data for a simulated event
 */
class SimEventHeader : public Baggable {

  public:

    SimEventHeader();

    ParticleType particleId_;     ///< Particle ID
    double energy_;               ///< Primary energy
    double theta_;                ///< Shower zenith angle
    double phi_;                  ///< Shower azimuth angle
    double xcore_;                ///< Core X in CORSIKA coordinates
    double ycore_;                ///< Core Y in CORSIKA coordinates
    double xcoreDet_;             ///< Core X in detector coordinates
    double ycoreDet_;             ///< Core Y in detector coordinates
    double xrange_;               ///< X range; depends on dynamic core option
    double yrange_;               ///< Y range; depends on dynamic core option
    double nmax_;                 ///< Gaisser-Hillas profile normalization
    double xmax_;                 ///< Gaisser-Hillas maximum slant depth
    double x0_;                   ///< Gaisser-Hillas "first interaction" depth
    double a_;                    ///< GH "interaction length" constant term
    double b_;                    ///< GH "interaction length" X coefficient
    double c_;                    ///< GH "interaction length" X^2 coefficient
    double age_;                  ///< Shower age s(X) = 3/(1 + 2*Xmax/X)
    double chi2ndf_;              ///< GH longitudinal profile figure of merit
    double height_;
    int nGround_;                 ///< Number of particles at ground level
    int nPond_;                   ///< Particles on Milagro "pond" (obselete)
    double tOffset_;
    double firstIntZ_;
    int nUsed_;
    double rWgt_;                 ///< Radial throw weight
    double wgtSim_;               ///< Combined energy+angular throw weights
    int evtNum_;
    int inEvtNum_;

    class ShowerComponent {
      public:
        ShowerComponent() : nPcls_(0), energy_(0) { }
        int nPcls_;
        double energy_;
    };

    ShowerComponent emShower_;
    ShowerComponent muonShower_;
    ShowerComponent hadronShower_;

    int nPMTs_;
    int nPcls_;

  friend std::ostream& operator<<(std::ostream& os, const SimEventHeader& sh);

};

SHARED_POINTER_TYPEDEFS(SimEventHeader);

#endif // DATACLASSES_EVENT_SIMEVENTHEADER_H_INCLUDED_H_INCLUDED

