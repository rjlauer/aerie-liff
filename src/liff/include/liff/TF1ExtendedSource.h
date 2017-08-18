/*!
 * @file TF1ExtendedSource.h
 * @author Colas Riviere
 * @date 30 Nov 2015
 * @brief TF1ExtendedSource for extended source with arbitrary TF1 spectrum
 */

#ifndef TF1_EXTENDED_SOURCE_H_INCLUDED
#define TF1_EXTENDED_SOURCE_H_INCLUDED

#include <string>
#include <vector>
#include <liff/ModelInterface.h>
#include <liff/Func1.h>

#include <TGraph2D.h>

#include <hawcnest/Logging.h>

#include <healpix_map.h>

namespace threeML {

  class TF1ExtendedSource: public ModelInterface {
    public:

      //The Func1 (a TF1 wrapper) object needs to take energy in [TeV] as input
      // and return differential flux in [TeV^-1 cm^-2 s^-1]
      TF1ExtendedSource(std::string name,
                        double ra,
                        double dec,
                        Func1Ptr function,
                        double radius); // in deg
      TF1ExtendedSource(std::string name,
                        double ra,
                        double dec,
                        std::string fluxfile,
                        double radius); // 5deg
      TF1ExtendedSource(std::string name,
                        double ra,
                        double dec,
                        std::string spectrumfile,
                        std::string scalingfile,
                        double radius); // 5deg

      ///////////
      //Arbitrary region of the sky. Map should have 1s and 0s
      TF1ExtendedSource(std::string name,
                        std::string region,
                        Func1Ptr function); // Function to use an arbitrary region. Return Flux in [TeV^-1 cm^-2 s^-1 sr^-1]

      //The use of "const" at the end of a method declaration means
      //that the method will not change anything in the class
      //(not even private members)

      void describe() const;

      void setSourcePosition(double j2000_ra, double j2000_dec) {
        ra_ = j2000_ra;
        dec_ = j2000_dec;
      }

      //Point source interface

      int getNumberOfPointSources() const { return 0; };

      void getPointSourcePosition(int srcid, double *j2000_ra, double *j2000_dec) const {
        log_fatal("Don't do that, not implemented!");
      }

      //Fluxes are differential fluxes in MeV^-1 cm^-1 s^-1
      //What's the meaning of this?
      std::vector<double> getPointSourceFluxes(int srcid, std::vector<double> energies) const;

      std::string getPointSourceName(int srcid) const {
        log_fatal("Don't do that, not implemented!");
        return "Nothing"; // To silence compiler warning
      }

      //Extended source interface

      int getNumberOfExtendedSources() const { return 1; };

      //Fluxes are differential fluxes in MeV^-1 cm^-1 s^-1 sr^-1
      //For now there is only power-law or power-law with cutoff spectrum
      std::vector<double> getExtendedSourceFluxes(int srcid, double j2000_ra,
                                                  double j2000_dec, std::vector<double> energies) const;

      std::string getExtendedSourceName(int srcid) const { return name_; };

      bool isInsideAnyExtendedSource(double j2000_ra, double j2000_dec) const;

      void getExtendedSourceBoundaries(int srcid, double *j2000_ra_min,
                                       double *j2000_ra_max,
                                       double *j2000_dec_min,
                                       double *j2000_dec_max) const;

      Healpix_Map<double> getExtendedSourceRegion(std::string region)
          const; //Get a file with the pixel numbers for the extended source region

      Func1Ptr getFunction() const { return function_; };

      double getRadius() const {
        if(region_!="")
          log_fatal("\"region_\" is set, so \"radius_\" should not be used.");
        return radius_;
      };

      void getExtendedSourcePosition(int srcid, double *j2000_ra, double *j2000_dec) const {
        if(region_!="")
          log_fatal("\"region_\" is set, so (RA,Dec) should not be used.");
        (*j2000_ra) = ra_;
        (*j2000_dec) = dec_;
      };

    private:

      std::string name_; // Source name, useless per se, just for printing on
                         // the screen
      
      // The next 3 members (radius_, ra_, dec_) are only used if region_ is
      // not set, then the source is a uniform disk defined by these.
      double ra_;
      double dec_;
      double radius_;
      
      Func1Ptr function_; // Energy spectrum function, only used if ??? is not
                          // defined
      std::string fluxfile_; // ???
      std::string spectrumfile_; // ???
      std::string scalingfile_; // ??? Only used by Tolga?
      std::vector<double> energylist_; // Vector of energies for which the flux
                                       // is provided when using ???, useless
                                       // when using function_
      std::vector<double> spectrumlist_; // ???
      std::vector<double> fluxlist_; // ???
      TGraph2D* scalingfunc_; // ??? Only used by Tolga?
  
      std::string region_; // ???
      Healpix_Map<double> regionmap_; // ???

  };
  SHARED_POINTER_TYPEDEFS(TF1ExtendedSource);
}

#endif // TF1_EXTENDED_SOURCE_H_INCLUDED
