#include <liff/TF1PointSource.h>
#include <math.h>
#include <iostream>
#include <fstream>

#include <liff/DetectorResponse.h>
#include <hawcnest/HAWCUnits.h>
#include <grmodel-services/ebl/Dominguez11EBLModel.h>
#include <grmodel-services/ebl/EBLAbsorptionService.h>
#include <grmodel-services/ebl/Franceschini08EBLModel.h>
#include <grmodel-services/ebl/Gilmore09EBLModel.h>
#include <grmodel-services/ebl/Gilmore09SplineEBLModel.h>
#include <grmodel-services/ebl/Gilmore12FiducialEBLModel.h>
#include <grmodel-services/ebl/Gilmore12FixedEBLModel.h>


using namespace std;

namespace threeML {

  TF1PointSource::TF1PointSource(string name,
                                 double ra,
                                 double dec,
                                 Func1Ptr function) :
    name_(name),
    ra_(ra),
    dec_(dec),
    function_(function),
    z_(0) 
  {
    describe();
  }
  
  TF1PointSource::TF1PointSource(string name,
                                 double ra,
                                 double dec,
                                 string fluxfile) :
    name_(name),
    ra_(ra),
    dec_(dec),
    z_(0),
    fluxfile_(fluxfile)
  {
    std::ifstream fileforspectra(fluxfile_.c_str());
    double energyin,fluxin;
    while(fileforspectra>>energyin>>fluxin) {
      energylist_.push_back(energyin*1e6);  // convert energies from TeV to MeV
      fluxlist_.push_back(fluxin*1.e-6);    // convert energies from 1/TeV/s/cm^2 to 1/MeV/s/cm^2
    }
    describe();
  }
  
  TF1PointSource::TF1PointSource(string name,
                                 double ra,
                                 double dec,
                                 double photonIndex,
                                 double normalization,
                                 double pivotEnergy,
                                 double cutoff) {
    // Preparing spectrum
    normalization *= 1e6;
    cutoff *= 1e-6;
    pivotEnergy *= 1e-6;
    // Preparing spectrum
    Func1Ptr sourceSpectrum = Func1Ptr(new Func1("sourceSpectrum", "[0]*pow(x/[3],-[1])*exp(-x/[2])", 0., 1e10));
    sourceSpectrum->SetParameterName(0, "Norm");
    sourceSpectrum->SetParameter(0, normalization);
    sourceSpectrum->SetParameterError(0, normalization / 2.);
    double normMin = 0;
    double normMax = 1e-5;
    sourceSpectrum->SetParameterBounds(0, normMin, normMax);
    sourceSpectrum->SetParameterName(1, "Index");
    sourceSpectrum->SetParameter(1, -photonIndex);
    sourceSpectrum->SetParameterError(1, 0.1);
    sourceSpectrum->SetParameterBounds(1, 1., 10.);
    sourceSpectrum->SetParameterName(2, "CutOff");
    sourceSpectrum->SetParameter(2, cutoff);
    sourceSpectrum->SetParameterError(2, cutoff / 2.);
    sourceSpectrum->SetParameterBounds(2, 0., 1.e10);
    sourceSpectrum->SetParameterName(3, "Pivot");
    sourceSpectrum->SetParameter(3, pivotEnergy);
    sourceSpectrum->SetParameterError(3, 0);
    sourceSpectrum->SetParameterBounds(3, pivotEnergy, pivotEnergy);

    // Initializing instance
    name_ = name;
    ra_ = ra;
    dec_ = dec;
    function_ = sourceSpectrum;
    z_ = 0;
  }
 

  void 
  TF1PointSource::SetEBLAbsorption(double z, std::string model, std::string drfile) {
    model_ = model;
    if (model_=="Gilmore12FiducialEBLModel") {
        eblAbsorption_ = new Gilmore12FiducialEBLModel();
    } else if (model_=="Dominguez11EBLModel") {
        eblAbsorption_ = new Dominguez11EBLModel();
    } else if (model_=="Franceschini08EBLModel") {
        eblAbsorption_ = new Franceschini08EBLModel();
    } else if (model_=="Gilmore09EBLModel") {
        eblAbsorption_ = new Gilmore09EBLModel();
    } else if (model_=="Gilmore09SplineEBLModel") {
        eblAbsorption_ = new Gilmore09SplineEBLModel();
    } else if (model_=="Gilmore12FixedEBLModel") {
        eblAbsorption_ = new Gilmore12FixedEBLModel();
    } else {
        log_fatal("EBL Model '"<<model_<<"' not implemented in grmodel-services.");
    }
    attenuationHash_.clear();
    z_ = z;
    log_info("EBL absorption for redshift z="<<z_<<" according to "<<model_);
    if (!drfile.empty()){
      //get list of energy bins in DR file and fill attenuation
      DetectorResponse dr = DetectorResponse(drfile);
      std::vector<double> energies = dr.GetLogEnBins(); 
      log_info("Precomputing attenuation for z="<<z_<<" for "
                <<energies.size()<<" DR-histogram energy bins.");
      for (vector<double>::iterator i = energies.begin();
             i != energies.end(); ++i) {
        *i = pow(10., *i + 6.); //in MeV
        attenuationHash_[*i] =  eblAbsorption_->GetAttenuation((*i)*HAWCUnits::MeV, z_);
        log_trace("Energy "<<(*i)*1.e-6<<" TeV: " << attenuationHash_[*i]);
      }
    }
  }


  void
  TF1PointSource::describe() const {
    if (!function_ ^ fluxfile_.empty()) {
      if (function_) {
        log_info(endl
                 <<"TF1 Point Source: " << endl
                 << "  Name:          " << name_ << endl
                 << "  R.A. (J2000):  " << ra_ << " deg" << endl
                 << "  Dec. (J2000):  " << dec_ << " deg");
        function_->PrintOut();
      }
      else {
        log_info("TF1 Point Source: " << endl
                 << "  Name:          " << name_ << endl
                 << "  R.A. (J2000):  " << ra_ << " deg" << endl
                 << "  Dec. (J2000):  " << dec_ << " deg" << endl
                 << "  Flux file:     " << fluxfile_);
      }
      if (z_>0) {
        log_info("EBL absorption for redshift z="<<z_<<" according to "<<model_);
      }
    }
    else {
      log_fatal("Neither function nor fluxfile defined!");
    }
  }
  
  vector<double>
  TF1PointSource::getPointSourceFluxes(int srcid, vector<double> energies) const {
    vector<double> fluxes(energies.size(), 0.0);
    unsigned int n = energies.size();
    
    for (unsigned int i = 0; i < n; ++i) {
      if ( !fluxfile_.empty() ) {
        unsigned int esize = energylist_.size();
        unsigned int j;
        for (i = 0; i < n; ++i) {
          if (energies[i] < energylist_[0]) {
            //energy is below those specified in the file
            fluxes[i] = 0.;
          }
          else if (energies[i] > energylist_[esize - 1]) {
            //energy is above those specified in the file
            fluxes[i] = 0.;
          }
          else {
            for (j = 1; j < esize; ++j) {
              if (energies[i] >= energylist_[j - 1] && energies[i] <= energylist_[j]) {
                //interpolate logarithmically
                fluxes[i] = exp(log(fluxlist_[j - 1]) + (log(fluxlist_[j]) - log(fluxlist_[j - 1]))
                                / (log(energylist_[j]) - log(energylist_[j - 1]))
                                * (log(energies[i]) - log(energylist_[j - 1])));
                break;
              }
            }
          }
        }
      }
      else {
        if (!function_) {
          log_fatal("No function defined!");
        }
        // The TF1 is assumed to be based on TeV, but ModelInterface input/output
        // is based on MeV
        fluxes[i] = 1.e-6 * (function_->Eval(energies[i] * 1.e-6));
      }
      //EBL attenuation
      if (z_>0) {
        double atten = 1.;
        try {
          atten = attenuationHash_.at(energies[i]);
        } 
        catch (...) {
          atten = eblAbsorption_->GetAttenuation(energies[i]*HAWCUnits::MeV, z_);
        }
        fluxes[i] *= atten;
      }
    }
    return fluxes;
  }
  
  void
  TF1PointSource::getPointSourcePosition(int srcid,
                                         double *j2000_ra,
                                         double *j2000_dec) const {
    (*j2000_ra) = ra_;
    (*j2000_dec) = dec_;
  }

  void
  TF1PointSource::setPointSourcePosition(int srcid,
                                         double j2000_ra,
                                         double j2000_dec) {
    ra_ = j2000_ra;
    dec_ = j2000_dec;
  }

  vector<double>
  TF1PointSource::getExtendedSourceFluxes(int srcid,
                                          double j2000_ra, double j2000_dec,
                                          vector<double> energies) const {
    vector<double> fluxes(energies.size(), 0.0);
    return fluxes;
  }
}
