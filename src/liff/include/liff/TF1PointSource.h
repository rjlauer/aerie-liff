/*!
 * @file TF1PointSource.h
 * @author Robert Lauer
 * @date 16 Jul 2015
 * @brief TF1PointSource for point source with arbitrary TF1 spectrum
 */

#ifndef TF1_POINT_SOURCE_H_INCLUDED
#define TF1_POINT_SOURCE_H_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <liff/ModelInterface.h>
#include <liff/Func1.h>

#include <grmodel-services/ebl/EBLAbsorptionService.h>

#include <hawcnest/Logging.h>

namespace threeML {

  class TF1PointSource: public ModelInterface {
    public:

      //The Func1 (a TF1 wrapper) object needs to take enrgy in [TeV] as input
      // and return differential flux in [TeV^-1 cm^-2 s^-1]
      TF1PointSource(std::string, double ra, double dec, Func1Ptr function);

      //The Func1 (a TF1 wrapper) object needs to take enrgy in [TeV] as input
      // and return differential flux in [TeV^-1 cm^-2 s^-1]
      TF1PointSource(std::string, double ra, double dec, std::string);

      
      //Constructor for easier transition from obsolete FixedPointSource
      TF1PointSource(std::string = "Crab",
                     double ra = 83.63,
                     double dec = 22.01,
                     double photonIndex = -2.63,
                     double normalization = 3.45e-17, //ph. MeV^-1 cm^-2 s^-1
                     double pivotEnergy = 1e6,
                     double cutoff = 1e20);

      //apply EBL attenuation to spectrum
      void SetEBLAbsorption(double z, std::string model="Gilmore12FiducialEBLModel",
                            std::string drfile="");

      //The use of "const" at the end of a method declaration means
      //that the method will not change anything in the class
      //(not even private members)

      void describe() const;

      //Point source interface

      int getNumberOfPointSources() const { return 1; }

      void getPointSourcePosition(int srcid, double *j2000_ra, double *j2000_dec) const;

      void setPointSourcePosition(int srcid, double j2000_ra, double j2000_dec);

      //Fluxes are differential fluxes in MeV^-1 cm^-1 s^-1
      std::vector<double> getPointSourceFluxes(int srcid, std::vector<double> energies) const;

      std::string getPointSourceName(int srcid) const { return name_; }

      //Extended source interface

      int getNumberOfExtendedSources() const { return 0; }

      std::vector<double> getExtendedSourceFluxes(int srcid, double j2000_ra,
                                                  double j2000_dec, std::vector<double> energies) const;

      std::string getExtendedSourceName(int srcid) const {
        log_fatal("Don't do that, not implemented!");
        return "Nothing"; // To silence compiler warning
      }

      bool isInsideAnyExtendedSource(double j2000_ra, double j2000_dec) const {
        return false;
      }

      void getExtendedSourceBoundaries(int srcid, double *j2000_ra_min,
                                       double *j2000_ra_max,
                                       double *j2000_dec_min,
                                       double *j2000_dec_max) const { }

      Func1Ptr getFunction() const { return function_; };

    private:

      std::string name_;
      double ra_;
      double dec_;
      Func1Ptr function_;
      double z_;
      std::string model_;
      EBLAbsorptionService* eblAbsorption_;
      std::map<double, double> attenuationHash_;

      std::string fluxfile_;
      std::vector<double> energylist_;
      std::vector<double> fluxlist_;

  };
  SHARED_POINTER_TYPEDEFS(TF1PointSource);

}

#endif // TF1_POINT_SOURCE_H_INCLUDED
