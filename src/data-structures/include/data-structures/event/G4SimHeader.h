/*!
 * @file G4SimHeader.h
 * @brief Simulation header for CORSIKA + g4sim events.
 * @author Segev BenZvi
 * @ingroup event_data
 * @version $Id: G4SimHeader.h 23099 2014-12-02 19:58:41Z bbaugh $
 */

#ifndef DATACLASSES_EVENT_G4SIMHEADER_H_INCLUDED
#define DATACLASSES_EVENT_G4SIMHEADER_H_INCLUDED

#include <hawcnest/processing/Bag.h>
#include <hawcnest/SoftwareVersion.h>

#include <data-structures/physics/Particle.h>

#include <iostream>

/*!
 * @class G4SimHeader
 * @author Segev BenZvi
 * @date 7 Apr 2010
 * @ingroup event_data
 * @brief Metadata for a CORSIKA+GEANT4 simulation
 *
 */
class G4SimHeader : public Baggable {

  public:

    G4SimHeader();

    /// Simulation energy spectrum
    class Energy {
      public:
        Energy() : spectralIndex_(0), min_(0), max_(0) { }
        double spectralIndex_;
        double min_;
        double max_;
    };
    Energy energy_;

    /// Simulation zenith angle range
    class ZenithAngle {
      public:
        ZenithAngle() : min_(0), max_(0) { }
        double min_;
        double max_;
    };
    ZenithAngle zenithAngle_;

    /// Simulation core position limits
    class CorePosition {
      public:
        CorePosition() : xMax_(0), yMax_(0) { }
        double xMax_;
        double yMax_;
    };
    CorePosition corePos_;

    /// Corsika low- and high-energy hadronic interaction models
    class GeneratorFlags {
      public:
        GeneratorFlags() : GHEISHA_(0), VENUS_(0) { }
        int GHEISHA_;
        int VENUS_;
    };
    GeneratorFlags genFlags_;

    /// Local geomagnetic field settings
    class MagneticField {
      public:
        MagneticField() : x_(0), z_(0) { }
        double x_;
        double z_;
    };
    MagneticField B_;
  
  
    /// Run number
    int runNumber_;
  
    /// Area where particles are generated in hawcsim
    double throwArea_;
    
    /// Number of primary particles generated
    double nEvents_;
    
    /// Version of GEANT4 used in the simulation production
    SoftwareVersion geantVersion_;

    /// Version of g4sim/hawcsim used in the simulation production
    SoftwareVersion g4simVersion_;

    /// HAWCSim Detector type flag
    int detectorType_;

    /// Primary particle type
    ParticleType primPartID_;
 
    /// Dynamic core flag: 0=uniform in x,y, 1=radial, 2=flat on annulus R0,R1
    int dynamicCore_;

    /// Base name of the survey file used for the detector geometry
    std::string tankSurveyFile_;

    /// Name of the current input file being used in the processing stream
    std::string currentFile_;

  friend std::ostream& operator<<(std::ostream& os, const G4SimHeader& ge);

};

SHARED_POINTER_TYPEDEFS(G4SimHeader);

#endif // DATACLASSES_EVENT_G4SIMHEADER_H_INCLUDED

