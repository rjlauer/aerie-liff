#include <liff/TF1ExtendedSource.h>
#include <liff/Util.h>

#include <data-structures/geometry/S2Point.h>
#include <hawcnest/HAWCUnits.h>

#include <healpix_map_fitsio.h>
#include <TGraph2D.h>

#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace HAWCUnits;

namespace threeML {

  TF1ExtendedSource::TF1ExtendedSource(string name,
                                       double ra,
                                       double dec,
                                       Func1Ptr function,
                                       double radius) :
    name_(name),
    ra_(ra),
    dec_(dec),
    radius_(radius),
    function_(function)
  {
    describe();
  }

  TF1ExtendedSource::TF1ExtendedSource(string name,
                                       double ra,
                                       double dec,
                                       string fluxfile,
                                       double radius) :
    name_(name),
    ra_(ra),
    dec_(dec),
    radius_(radius),
    fluxfile_(fluxfile)
  {
    vector <vector<double> > read_spectrum = GetSpectrum(fluxfile_);
    energylist_   = read_spectrum.at(0);
    fluxlist_     = read_spectrum.at(1);
  }
  
  TF1ExtendedSource::TF1ExtendedSource(string name,
                                       double ra,
                                       double dec,
                                       string spectrumfile,
                                       string scalingfile,
                                       double radius) :
    name_(name),
    ra_(ra),
    dec_(dec),
    radius_(radius),
    spectrumfile_(spectrumfile),
    scalingfile_(scalingfile)
  {
    vector <vector<double> > read_spectrum = GetSpectrum(spectrumfile_);
    energylist_   = read_spectrum.at(0);
    spectrumlist_ = read_spectrum.at(1);
    scalingfunc_  = GetScaling(scalingfile_);
    log_debug("Testing TGraph2d interpolation: Scaling Factor at (" << ra << "," << dec << ") "<<  scalingfunc_->Interpolate(ra, dec));
  }

  TF1ExtendedSource::TF1ExtendedSource(std::string name,
                                       std::string region,
                                       Func1Ptr function) :
    name_(name),
    function_(function),
    region_(region)
  {
    regionmap_ = getExtendedSourceRegion(region_);
  }



  void
  TF1ExtendedSource::describe() const {
    if (! fluxfile_.empty()) {
      cout << "TF1 Extended Source:" << endl;
      cout << "  Name:             " << name_ << endl;
      cout << "  R.A. (J2000):     " << ra_ << " deg" << endl;
      cout << "  Dec. (J2000):     " << dec_ << " deg" << endl;
      cout << "  Flux File:        " << fluxfile_ << endl;
    }
    else if (! spectrumfile_.empty()) {
      cout << "TF1 Extended Source:" << endl;
      cout << "  Name:             " << name_ << endl;
      cout << "  R.A. (J2000):     " << ra_ << " deg" << endl;
      cout << "  Dec. (J2000):     " << dec_ << " deg" << endl;
      cout << "  Spectrum File:    " << spectrumfile_ << endl;
      cout << "  Scaling File:     " << scalingfile_ << endl;
    }
    else if (! region_.empty()) {
      cout << "TF1 Extended Source:" << endl;
      cout << "  Name:             " << name_ << endl;
      cout << "  Template region:  " << region_ << endl;
      function_->PrintOut();
    }
    else {
      if (!function_) {
        log_fatal("No function defined!");
      }
      cout << "TF1 Extended Source:" << endl;
      cout << "  Name:          " << name_ << endl;
      cout << "  R.A. (J2000):  " << ra_ << " deg" << endl;
      cout << "  Dec. (J2000):  " << dec_ << " deg" << endl;
      cout << "  Radius:        " << radius_ << " deg" << endl;
      function_->PrintOut();
    }
  }


  void
  TF1ExtendedSource::getExtendedSourceBoundaries(int srcid,
                                                 double *j2000_ra_min,
                                                 double *j2000_ra_max,
                                                 double *j2000_dec_min,
                                                 double *j2000_dec_max) const {
    if (!region_.empty()) {
      (*j2000_ra_min) = 360.;
      (*j2000_ra_max) = 0.;
      (*j2000_dec_min) = 90.;
      (*j2000_dec_max) = -90.;
      const int npix = regionmap_.Npix();
      for (int i=0; i<npix;i++) {
        if (regionmap_[i] < 1.) continue;
        else {
          SkyPos a(regionmap_.pix2ang(i));
          if (a.RA() < *j2000_ra_min) *j2000_ra_min = a.RA();
          if (a.RA() > *j2000_ra_max) *j2000_ra_max = a.RA();
          if (a.Dec() > *j2000_dec_max) *j2000_dec_max = a.Dec();
          if (a.Dec() < *j2000_dec_min) *j2000_dec_min = a.Dec();
        }
      }

    } else {
      (*j2000_ra_min) = ra_ - radius_ / cos(dec_ * degree);
      while ((*j2000_ra_min) > 360) (*j2000_ra_min) -= 360;
      while ((*j2000_ra_min) < 0) (*j2000_ra_min) += 360;
      (*j2000_ra_max) = ra_ + radius_ / cos(dec_ * degree);
      while ((*j2000_ra_max) > 360) (*j2000_ra_max) -= 360;
      while ((*j2000_ra_max) < 0) (*j2000_ra_max) += 360;
      (*j2000_dec_min) = dec_ - radius_;
      if ((*j2000_dec_min) < -90) (*j2000_dec_min) = -90;
      (*j2000_dec_max) = dec_ + radius_;
      if ((*j2000_dec_max) > 90) (*j2000_dec_max) = 90;
    }
    log_debug("Ext. source boundaries: ");
    log_debug("RA: " << *j2000_ra_min << " to " << *j2000_ra_max);
    log_debug("Dec: " << *j2000_dec_min << " to " << *j2000_dec_max);
  }

  vector<double>
  TF1ExtendedSource::getExtendedSourceFluxes(int srcid,
                                             double j2000_ra,
                                             double j2000_dec,
                                             vector<double> energies) const {

    vector<double> fluxes(energies.size(),0.0);
    unsigned int esize = energies.size();
    unsigned int fluxsize = fluxlist_.size();
    unsigned int specsize = spectrumlist_.size();
    if (isInsideAnyExtendedSource(j2000_ra,j2000_dec)) {
      if( fluxsize != 0 ) {                     // user input the file that has the flux, no scaling is needed
        fluxes = CalculateFluxes(energies, energylist_, fluxlist_);
      }
      else if( specsize != 0 ) {                // user input the files that has the spectrum and scaling
        fluxes = CalculateFluxes(energies, energylist_, spectrumlist_);
        double scalingfactor = scalingfunc_->Interpolate(j2000_ra, j2000_dec);
        for(unsigned int i=0; i<esize; ++i) {
          fluxes[i] = fluxes[i] * scalingfactor;
        }
      }
      else {                                    // spectrum from function
        for (unsigned int i=0; i<esize; i++) {
          fluxes[i] = 1.e-6*(function_->Eval(energies[i]*1.e-6));
        }
      }
      log_trace("fluxes");
      log_trace(fluxes[0]);
      return fluxes;
    } else {
      return fluxes;
    }
  }

  vector<double>
  TF1ExtendedSource::getPointSourceFluxes(int srcid,
                                          vector<double> energies) const {
    log_warn("You're requesting the point source flux of an extended source!");
    vector<double> fluxes(energies.size(), 0.0);
    return fluxes;
  }

  bool
  TF1ExtendedSource::isInsideAnyExtendedSource(double j2000_ra, double j2000_dec) const {
    if(!region_.empty()) {
      pointing loc = SkyPos(j2000_ra, j2000_dec).GetPointing();
      int pix = regionmap_.ang2pix(loc);
      return (regionmap_[pix] >= 1.);
    } else {
      // Using a cirular region
      S2Point axisA((90 - j2000_dec) * degree, j2000_ra * degree);
      S2Point axisB((90 - dec_) * degree, ra_ * degree);
      double dist = (axisA.Angle(axisB)) / degree;
      return (dist <= radius_);
    }  
  }

  Healpix_Map<double>
  TF1ExtendedSource::getExtendedSourceRegion(string region) const {
    Healpix_Map<double> regionmap;
    read_Healpix_map_from_fits(region,regionmap,1);
    return regionmap;
  }

}
