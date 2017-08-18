/*!
 * @file BayesianBuffer.cc
 * @brief Implementation of Bayesian Block algorithm with buffering
 * @author Tom Weisgarber
 * @date 28 April 2014
 * @version $Id: BayesianBuffer.cc 38436 2017-03-29 20:07:37Z tweisgarber $
 */

#include <cmath>
#include <hawcnest/Logging.h>
#include <hawcnest/HAWCUnits.h>

#include <data-structures/math/BayesianBuffer.h>
#include <data-structures/math/SpecialFunctions.h>

#include <TMath.h>
#include <TF1.h>
#include <iomanip>
#include <fstream>

using namespace std;

namespace BayesianBuffer {

  // solve the cdf for a chi^2 with k degrees of freedom
  // (variate should be a random variable between 0 and 1)
  double solve_chi2(double k,double variate) {
    if(variate <= 0.0) return 0.0;
    if(variate >= 1.0) return HAWCUnits::infinity;
    double a = 0.5*k;
    double start = 0.0;
    double end = 2.0;
    while(SpecialFunctions::Gamma::P(a,0.5*end) < variate) {
      start = end;
      end = 2.0*end;
    }
    double mid = 0.5*(start+end);
    double pstart = SpecialFunctions::Gamma::P(a,0.5*start);
    double pmid = SpecialFunctions::Gamma::P(a,0.5*mid);
    double pend = SpecialFunctions::Gamma::P(a,0.5*end);
    double old_diff = end-start;
    double delta = 1.0;
    while(delta > 0.0) {
      if(pmid > variate) {
        end = mid;
        pend = pmid;
      } else if(pmid < variate) {
        start = mid;
        pstart = pmid;
      } else {
        return mid;
      }
      mid = 0.5*(start+end);
      pmid = SpecialFunctions::Gamma::P(a,0.5*mid);
      delta = old_diff-(end-start);
      old_diff = end-start;
    }
    return mid;
  }

  // convert a value drawn from a chi^2 with k degrees of freedom to an equivalent value drawn from l degrees of freedom
  double convert_chi2_value(double value,double k,double l) {
    if(value <= 0.0) return value;
    if(k == l) return value;
    double a = 0.5*k;
    double b = 0.5*l;
    double pval = SpecialFunctions::Gamma::P(a,0.5*value);
    if(pval > 1.0) pval = 1.0;
//std::cout << "Target p value: " << pval << std::endl;
    double start = 0.0;
    double end = value;
    if(l > k) {
      start = value;
      end = 2.0*value;
      while(SpecialFunctions::Gamma::P(b,0.5*end) < pval) {
        start = end;
        end *= 2.0;
      }
    }
//std::cout << "Starting range: " << start << " to " << end << std::endl;
    double mid = 0.5*(start+end);
    double pstart = SpecialFunctions::Gamma::P(b,0.5*start);
    double pmid = SpecialFunctions::Gamma::P(b,0.5*mid);
    double pend = SpecialFunctions::Gamma::P(b,0.5*end);
    double old_diff = end-start;
    double delta = 1.0;
//    bool fail = (start < 1.0e-10 ? (end-start) > 1.0e-12 : 
//    while(end-start > std::max(1.0e-12 : 1.0e-12*start)) {
//    while(end-start > 1.0e-12) {
//    while(end-start > 0.0) {
    while(delta > 0.0) {
      if(pmid > pval) {
        end = mid;
        pend = pmid;
      } else if(pmid < pval) {
        start = mid;
        pstart = pmid;
      } else {
        return mid;
      }
      mid = 0.5*(start+end);
      pmid = SpecialFunctions::Gamma::P(b,0.5*mid);
      delta = old_diff-(end-start);
      old_diff = end-start;
//std::cout << "Current range: " << start << " to " << end << " with values " << pstart << " to " << pend << std::endl;
//std::cout << " -> difference: " << (end-start) << std::endl;
    }
    return mid;
  }

  // get the Li-Ma contribution to the statistic
  double get_li_ma_statistic_contribution(double non,double noff,double alpha) {
    if(alpha <= 0.0 || non < 0.0 || noff < 0.0) return 0.0;
    if(non == 0.0) return 2.0*noff*log(1.0+alpha);
    double alpha_inv = 1.0/alpha;
    if(noff == 0.0) return 2.0*non*log(1.0+alpha_inv);
    double r = non/noff;
    double x = r-alpha;
    if(x < 1.0e-7 && x > -1.0e-7) {
      return noff*x*x/(alpha*(1.0+alpha))*(1.0-(1.0+2.0*alpha)*x/(3.0*alpha*(1.0+alpha)));
    }
    return 2.0*noff*(r*log((1.0+alpha_inv)/(1.0+1.0/r))+log((1.0+alpha)/(1.0+r)));
  }

  // get the Li-Ma significance
  double get_sigma(double Non,double Noff,double alpha) {
    return (Non > alpha*Noff ? 1.0 : -1.0)*sqrt(get_li_ma_statistic_contribution(Non,Noff,alpha));
  }

  // convert a TS value and number of observations into a significance
  //
  // if TS value is too large (depending on bin count) the calculation fails because Gamma::Q is very small
  // and then ErfInverse makes it large again, and we lose precision. I combat this by using an asymptotic
  // form derived by looking at the ratio of the two expressions -TRW 8 May 2015
  //
  void interpret_ts_value(double ts,double n,double weighted,double& prob,double& sigma_like,bool check_limit) {
    double garg = 0.5*n;
    static const double a0 = 37.6833333361522449;
    static const double a1 = 4.69166666663475240;
    static const double a2 = -0.147727272762341461;
    static const double b0 = 69.8272727262656190;
    static const double b1 = 1.64700384124886945;
    static const double ln2 = log(2.0);
    static const double lnpi = log(HAWCUnits::pi);
    double limit = (n < 10.0 ? a0+a1*n+a2*n*n : b0+b1*n);
    prob = SpecialFunctions::Gamma::Q(garg,0.5*ts);
    if(ts > limit && check_limit) {
      double F = (n-1.0)*ln2+ts-(n-2.0)*log(ts)+2.0*SpecialFunctions::Gamma::lnG(garg)-lnpi-log(1.0+(n-2.0)/(ts*ts));
      sigma_like = sqrt(F-log(F));
    } else {
      sigma_like = sqrt(2.0)*TMath::ErfInverse(SpecialFunctions::Gamma::P(garg,0.5*ts));
    }
    if(weighted < 0.0) sigma_like = -sigma_like;
  }

  // get the probability, Li-Ma test statistic, and Li-Ma significance for multiple bins
  void get_significance(std::vector<std::pair<double,double> >& obs,double alpha,double& prob,double& ts,double& sigma_like,bool check_limit) {
    ts = 0.0;
    double weighted = 0.0;
    for(size_t i=0; i<obs.size(); ++i) {
      double ts_term = get_li_ma_statistic_contribution(obs[i].first,obs[i].second,alpha);
      ts += ts_term;
      if(obs[i].first > obs[i].second*alpha) {
        weighted += ts_term;
      } else {
        weighted -= ts_term;
      }
    }
    double n = (double)obs.size();
    interpret_ts_value(ts,n,weighted,prob,sigma_like,check_limit);
  }

  // get the probability, Li-Ma test statistic, and Li-Ma significance for a single bin
  void get_significance(std::pair<double,double> obs,double alpha,double& prob,double& ts,double& sigma_like,bool check_limit) {
    std::vector<std::pair<double,double> > vec;
    vec.push_back(obs);
    get_significance(vec,alpha,prob,ts,sigma_like,check_limit);
  }

  // same as before but for multiple alphas
  void get_significance(std::vector<std::pair<double,double> >& obs,std::vector<double>& alpha,double& prob,double& ts,double& sigma_like,bool check_limit) {
    ts = 0.0;
    double weighted = 0.0;
    for(size_t i=0; i<obs.size(); ++i) {
      double ts_term = get_li_ma_statistic_contribution(obs[i].first,obs[i].second,alpha[i]);
      ts += ts_term;
      if(obs[i].first > obs[i].second*alpha[i]) {
        weighted += ts_term;
      } else {
        weighted -= ts_term;
      }
    }
    double n = (double)obs.size();
    interpret_ts_value(ts,n,weighted,prob,sigma_like,check_limit);
  }

  // DataPoint constructors
  DataPoint::DataPoint(double Non_in, double Noff_in, double alpha_in,
                       double mjd_in, double average_mjd_in,double zenith_in)
  {
    N_.push_back(std::make_pair(Non_in,Noff_in));
    alpha_.push_back(alpha_in);
    mjd_ = mjd_in;
    average_mjd_ = average_mjd_in;
    zenith_ = zenith_in;
    Compute();
  }

  DataPoint::DataPoint(std::vector<std::pair<double,double> >& N_in, std::vector<double>& alphas, double mjd_in, double average_mjd_in,double zenith_in) {
    N_.resize(N_in.size(),std::make_pair(0.0,0.0));
    for(size_t i=0; i<N_in.size(); ++i) {
      N_[i].first = N_in[i].first;
      N_[i].second = N_in[i].second;
    }
    alpha_.resize(alphas.size());
    for(size_t i=0; i<alphas.size(); ++i) alpha_[i] = alphas[i];
    mjd_ = mjd_in;
    average_mjd_ = average_mjd_in;
    zenith_ = zenith_in;
    Compute();
  }

  // compute derived quantities
  void
  DataPoint::Compute()
  {
    alphaInverse_.resize(alpha_.size(),1.0);
    alphaLog_.resize(alpha_.size(),0.0);
    for(size_t i=0; i<alpha_.size(); ++i) {
      alphaInverse_[i] = 1.0/alpha_[i];
      alphaLog_[i] = log(alpha_[i]);
    }
    size_t totsize = GetBins();
    if(factorialTerm_.size() < totsize) factorialTerm_.resize(totsize);
    if(logTerm_.size() < totsize) logTerm_.resize(totsize);
    if(subLogTerm_.size() < totsize) subLogTerm_.resize(totsize);
    zero_.resize(totsize,true);
    double on_sum = 0.0;
    double off_sum = 0.0;
    for(size_t i=0; i<totsize; ++i) {
      on_sum += N_[i].first;
      off_sum += N_[i].second;
      zero_[i] = ((N_[i].first == 0.0) && (N_[i].second == 0.0));
      factorialTerm_[i] = SpecialFunctions::Gamma::lnG(N_[i].first+1.0) +
                          SpecialFunctions::Gamma::lnG(N_[i].second+1.0);
      logTerm_[i] = (N_[i].first + N_[i].second == 0.0 ? 0.0 : (N_[i].first + N_[i].second) * log(N_[i].first + N_[i].second));
      subLogTerm_[i] = (N_[i].first == 0.0 ? 0.0 : N_[i].first * log(N_[i].first)) + (N_[i].second == 0.0 ? 0.0 : N_[i].second * log(N_[i].second));
    }
    get_significance(N_,alpha_,lima_prob_,lima_ts_,lima_sig_);
    if(on_sum == 0.0 || off_sum == 0.0) {
      excess_error_ = 0.0;
    } else {
      // do we even need excess error?
      /*double err2 = alphaInverse_*alphaInverse_/(off_sum*off_sum)*on_sum;
      err2 += on_sum*on_sum*alphaInverse_*alphaInverse_/(off_sum*off_sum*off_sum);
      excess_error_ = sqrt(err2);*/
      excess_error_ = 0.0;
    }
  }

  // OptimizeInfo class
  OptimizeInfo::OptimizeInfo() {
    fitnessDistance_ = 0.0;
    fitnesses_.clear();
  }

//  void OptimizeInfo::AddChangePoint(unsigned int buffer_cp,unsigned int cp,double sig1,double bkg1,double sig2,double bkg2,unsigned int bin,unsigned int idx) {
  void OptimizeInfo::AddChangePoint(unsigned int buffer_cp,unsigned int cp,std::vector<std::vector<double> >& data,unsigned int idx) {
    if(idx >= change_.size()) {
      change_.resize(idx+1);
      bufferChange_.resize(idx+1);
      beforeSignal_.resize(idx+1);
      beforeBackground_.resize(idx+1);
      beforeAverageAlpha_.resize(idx+1);
      afterSignal_.resize(idx+1);
      afterBackground_.resize(idx+1);
      afterAverageAlpha_.resize(idx+1);
    }
    change_[idx].push_back(cp);
    bufferChange_[idx].push_back(buffer_cp);
    beforeSignal_[idx].push_back(std::vector<double>());
    beforeBackground_[idx].push_back(std::vector<double>());
    beforeAverageAlpha_[idx].push_back(std::vector<double>());
    afterSignal_[idx].push_back(std::vector<double>());
    afterBackground_[idx].push_back(std::vector<double>());
    afterAverageAlpha_[idx].push_back(std::vector<double>());
    size_t pt = beforeSignal_[idx].size()-1;
    for(size_t bin=0; bin<data.size(); ++bin) {
      if(data[bin].size() < 4) {
        static int ctr = 0;
        if(ctr < 1000) {
          log_warn("Not enough data!");
          ctr++;
          if(ctr == 1000) log_warn("Further insufficient data warning suppressed!");
        }
      } else {
        beforeSignal_[idx][pt].push_back(data[bin][0]);
        beforeBackground_[idx][pt].push_back(data[bin][1]);
        beforeAverageAlpha_[idx][pt].push_back(data[bin][4]);
        afterSignal_[idx][pt].push_back(data[bin][2]);
        afterBackground_[idx][pt].push_back(data[bin][3]);
        afterAverageAlpha_[idx][pt].push_back(data[bin][4]);
      }
    }
    /*if(bin >= change_[idx].size()) {
      change_[idx].resize(bin+1);
      bufferChange_[idx].resize(bin+1);
      beforeSignal_[idx].resize(bin+1);
      beforeBackground_[idx].resize(bin+1);
      afterSignal_[idx].resize(bin+1);
      afterBackground_[idx].resize(bin+1);
    }
    change_[idx][bin].push_back(cp);
    bufferChange_[idx][bin].push_back(buffer_cp);
    beforeSignal_[idx][bin].push_back(sig1);
    beforeBackground_[idx][bin].push_back(bkg1);
    afterSignal_[idx][bin].push_back(sig2);
    afterBackground_[idx][bin].push_back(bkg2);*/
    /*if(idx == 0) {
      change_.push_back(cp);
      bufferChange_.push_back(buffer_cp);
      beforeSignal_.push_back(sig1);
      beforeBackground_.push_back(bkg1);
      afterSignal_.push_back(sig2);
      afterBackground_.push_back(bkg2);
    } else {
      if((idx-1) >= lchange_.size()) {
        lchange_.resize(idx);
        lbufferChange_.resize(idx);
        lbeforeSignal_.resize(idx);
        lbeforeBackground_.resize(idx);
        lafterSignal_.resize(idx);
        lafterBackground_.resize(idx);
      }
      lchange_[idx-1].push_back(cp);
      lbufferChange_[idx-1].push_back(buffer_cp);
      lbeforeSignal_[idx-1].push_back(sig1);
      lbeforeBackground_[idx-1].push_back(bkg1);
      lafterSignal_[idx-1].push_back(sig2);
      lafterBackground_[idx-1].push_back(bkg2);
    }*/
    fitnesses_.clear();
  }


  // BayesianBuffer constructor with prior gamma factor and buffer size
  BayesianBuffer::BayesianBuffer(double gamma_in,unsigned int bufferSize_in,unsigned int rebinning_in,bool ignore_zero_in,double mjd_jump_in,std::vector<double>* gammas,double correction,bool quiet)
  {
    ignoreZero_ = ignore_zero_in;
    priors_.clear();
    if(gamma_in > 1.0)
      gamma_in = 1.0;
    if(rebinning_in == 0)
      rebinning_in = 1;
    // if gammas vector is specfied, run in priorless mode
    if(gammas) {
      if(gammas->size() == 1) {
        gamma_ = gammas->at(0);
        priors_.push_back(log(gammas->at(0)));
        if(!quiet) log_info("Added prior " << priors_[0]);
        prior_ = log(gammas->at(0));
        multipriors_ = false;
      } else {
        for(unsigned int i=0; i<gammas->size(); ++i) {
          priors_.push_back(log(gammas->at(i)));
          if(!quiet) log_info("Added prior " << priors_[i]);
        }
        gamma_ = 1.0;
        prior_ = 0.0;
        multipriors_ = true;
      }
    } else {
      gamma_ = gamma_in;
      priors_.push_back(log(gamma_in));
      if(!quiet) log_info("Added prior " << priors_[0]);
      prior_ = log(gamma_in);
      multipriors_ = false;
    }
    bufferSize_ = bufferSize_in;
    shifts_ = 0;
    rebinning_ = rebinning_in;
    binctr_ = 0;
    newBlockComplete_ = false;
    mjdJumpLimit_ = mjd_jump_in;
    keyword_ = "";
    cache_.push_back(std::make_pair(0.0,0.0));
    cacheAlpha_.push_back(0.0);
    ResetCache();
    //priorCorrection_ = NULL;
    priorCorrectionValue_.resize(bufferSize_,0.0);
    computeBufferFitnesses_ = false;
    basePriorCorrection_ = correction;
    bufferedMode_ = true;
  }

  // clear all data
  void BayesianBuffer::ClearData() {
    shifts_ = 0;
    binctr_ = 0;
    newBlockComplete_ = false;
    ResetCache();
    for(size_t i=0; i<points_.size(); ++i) {
      if(points_[i]) delete points_[i];
    }
    points_.clear();
    blockFitness_.clear();
    if(!IsBuffered()) bufferSize_ = 0;
  }

  // BayesianBuffer destructor
  BayesianBuffer::~BayesianBuffer()
  {
    for(unsigned int i = 0; i < points_.size(); ++i) {
      if(points_[i])
        delete points_[i];
    }
    //if(priorCorrection_) delete priorCorrection_;
  }

/*  // set the prior correction string
  void BayesianBuffer::SetPriorCorrection(std::string str) {
    if(priorCorrection_) delete priorCorrection_;
    priorCorrection_ = new TF1("",str.c_str(),0.0,(double)GetBufferSize());
    if(str.find("[0]") != std::string::npos) priorCorrection_->SetParameter(0,(double)((GetBufferSize()-1)/2));
    priorCorrectionValue_.resize(bufferSize_,0.0);
    for(size_t i=0; i<priorCorrectionValue_.size(); ++i) {
      priorCorrectionValue_[i] = EvalPriorCorrection(i);
    }
  }

  // evaluate the prior correction
  double BayesianBuffer::EvalPriorCorrection(size_t bin) const {
    if(bin >= GetBufferSize()) return 0.0;
    if(!priorCorrection_) return 0.0;
    return priorCorrection_->Eval((double)(bin));
  }*/

  // read the prior correction from a file
  void BayesianBuffer::ReadPriorCorrection(std::string filename) {
    log_debug("Reading prior corrections from file " << filename);
    ifstream file;
    file.open(filename.c_str());
    size_t file_bin = 0;
    double file_value = 0.0;
    while(file.good()) {
      file >> file_bin >> file_value;
      if(!file.good()) continue;
      if(file_bin < priorCorrectionValue_.size()) {
        log_debug("Setting prior correction for bin " << file_bin << " to " << file_value);
        priorCorrectionValue_[file_bin] = file_value;
      } else {
        log_warn("Found bad bin in file " << filename << ": " << file_bin << " when buffer size is " << priorCorrectionValue_.size());
      }
    }
  }

  // read the weights from a file and rescale them
  void BayesianBuffer::ReadWeightsLLH(std::string filename) {
    log_debug("Reading LLH weights from file " << filename);
    ifstream file;
    file.open(filename.c_str());
    size_t file_bin = 0;
    double file_value = 0.0;
    while(file.good()) {
      file >> file_bin >> file_value;
      if(!file.good()) continue;
      if(!(file_bin < weightLLH_.size())) weightLLH_.resize(file_bin+1,-1.0);
      log_info("Found weight " << file_value << " for analysis bin " << file_bin);
      if(file_value < 0.0) log_warn("Treating negative weight as positive!");
      weightLLH_[file_bin] = (file_value < 0.0 ? -file_value : file_value);
    }
    double sumw2 = 0.0;
    for(size_t i=0; i<weightLLH_.size(); i++) sumw2 += (weightLLH_[i] > 0.0 ? weightLLH_[i]*weightLLH_[i] : 0.0);
    for(size_t i=0; i<weightLLH_.size(); i++) weightLLH_[i] = weightLLH_[i]/sqrt(sumw2);
    // set any unset weights to 1
    for(size_t i=0; i<weightLLH_.size(); i++) {
      if(weightLLH_[i] < 0.0) {
        log_warn("Setting weight for analysis bin " << i << " to 1");
        weightLLH_[i] = 1.0;
      }
    }
    for(size_t i=0; i<weightLLH_.size(); i++) {
      log_debug("Weight for analysis bin " << i << " is " << weightLLH_[i]);
    }
  }

  // get the prior for the given prior index
  double BayesianBuffer::GetPrior(unsigned int pidx) const {
    if(!multipriors_) return GetPrior();
    if(pidx >= priors_.size()) return 0.0;
    return priors_[pidx];
    //return priors_[pidx]+basePriorCorrection_;
  }

  // get the Non of the given point in the buffer
  double BayesianBuffer::GetPointNon(unsigned int idx,size_t bin) const {
    if(idx >= points_.size()) return 0.0;
    return points_[idx]->GetNon(bin);
  }

  // get the Noff of the given point in the buffer
  double BayesianBuffer::GetPointNoff(unsigned int idx,size_t bin) const {
    if(idx >= points_.size()) return 0.0;
    return points_[idx]->GetNoff(bin);
  }

  // get the alpha of the given point in the buffer
  double BayesianBuffer::GetPointAlpha(unsigned int idx,size_t bin) const {
    if(idx >= points_.size()) return 0.0;
    return points_[idx]->GetAlpha(bin);
  }

  // get the MJD of the given point in the buffer
  double BayesianBuffer::GetPointMJD(unsigned int idx) const {
    if(idx >= points_.size()) return 0.0;
    return points_[idx]->GetMJD();
  }

  // get the average MJD of combined points in the buffer
  double BayesianBuffer::GetPointAverageMJD(unsigned int idx) const {
    if(idx >= points_.size()) return 0.0;
    return points_[idx]->GetAverageMJD();
  }

  // get the zenith of the point
  double BayesianBuffer::GetPointZenith(unsigned int idx) const {
    if(idx >= points_.size()) return 0.0;
    return points_[idx]->GetZenith();
  }

  // get the Li-Ma significance of the given point in the buffer
  double BayesianBuffer::GetPointLiMaSig(unsigned int idx) const {
    if(idx >= points_.size()) return 0.0;
    return points_[idx]->GetLiMaSig();
  }

  // get the index from the very start of observations
  unsigned int BayesianBuffer::GetPointIndex(unsigned int idx) const {
    return idx+shifts_;
  }

  // reset the data cache
  void BayesianBuffer::ResetCache() {
    for(size_t i=0; i<cache_.size(); ++i) {
      cache_[i].first = 0.0;
      cache_[i].second = 0.0;
    }
    for(size_t i=0; i<cacheAlpha_.size(); ++i) {
      cacheAlpha_[i] = 0.0;
    }
    cacheMJD_ = -1.0;
    cacheAverageMJD_ = 0.0;
    cacheAverageZenith_ = 0.0;
    cacheCount_ = 0;
    cacheNon_ = 0.0;
    cacheNoff_ = 0.0;
  }

  // compute the fitness of the block in the buffer including idx1 and up to
  // but not including idx2
  double
  BayesianBuffer::Fitness(unsigned int idx1,unsigned int idx2)
    const
  {
    size_t maxbin = 0;
    for(unsigned int i = idx1; i < idx2; ++i) {
      size_t testbins = points_[i]->GetBins();
      if(testbins > maxbin) maxbin = testbins;
    }
    double H = 0.0;
    for(size_t bin = 0; bin < maxbin; ++bin) {
      double N = 0.0;
      double Noff = 0.0;
      double Non = 0.0;
      double inv_alpha = 0.0;
      for (unsigned int i = idx1; i < idx2; ++i) {
        if (!points_[i]->IsZero(bin)) {
          Non += points_[i]->GetNon(bin);
          Noff += points_[i]->GetNoff(bin);
          inv_alpha += points_[i]->GetAlphaInverse(bin);
          N += 1.0;
        }
      }
      if(N != 0.0) {
        double t1 = (Non == 0.0 ? 0.0 : log(Non/N));
        double t2 = (Noff/inv_alpha);
        double t3 = log(t2);
        for (unsigned int i = idx1; i < idx2; ++i) {
          if (!points_[i]->IsZero(bin)) {
            H -= points_[i]->GetNon(bin)*t1;
            H += points_[i]->GetNoff(bin)*(points_[i]->GetLogAlpha(bin)-t3);
            H += points_[i]->GetAlphaInverse(bin)*t2;
            H += points_[i]->GetFactorialTerm(bin);
          }
        }
      }
      H += Non;
    }
    return -H;
  }

  // get the total signal (Non) over some range
  double BayesianBuffer::GetSignalSum(unsigned int idx1,unsigned int idx2) const {
    if(idx2 > points_.size()) idx2 = points_.size();
    if(idx1 == idx2) log_warn("Warning: GetSignalSum(" << idx1 << "," << idx2 << ") returns 0");
    if(idx1 >= points_.size() || idx2 <= idx1) return 0.0;
    double non = 0.0;
    for(unsigned int i = idx1; i < idx2; ++i) {
      for(size_t bin = 0; bin < points_[i]->GetBins(); ++bin) {
        non += GetPointNon(i,bin);
      }
    }
    return non;
  }

  // get the total signal (Non) over some range for a single bin
  double BayesianBuffer::GetSignalSum(unsigned int idx1,unsigned int idx2,unsigned int bin) const {
    if(idx2 > points_.size()) idx2 = points_.size();
    if(idx1 == idx2) log_warn("Warning: GetSignalSum(" << idx1 << "," << idx2 << "," << bin << ") returns 0");
    if(idx1 >= points_.size() || idx2 <= idx1) return 0.0;
    double non = 0.0;
    for(unsigned int i = idx1; i < idx2; ++i) {
      non += GetPointNon(i,bin);
    }
    return non;
  }

  // get the total background (alpha*Noff) over some range
  double BayesianBuffer::GetBackgroundSum(unsigned int idx1,unsigned int idx2) const {
    if(idx2 > points_.size()) idx2 = points_.size();
    if(idx1 == idx2) log_warn("Warning: GetBackgroundSum(" << idx1 << "," << idx2 << ") returns 0");
    if(idx1 >= points_.size() || idx2 <= idx1) return 0.0;
    double noff = 0.0;
    for(unsigned int i = idx1; i < idx2; ++i) {
      for(size_t bin = 0; bin < points_[i]->GetBins(); ++bin) {
        noff += GetPointNoff(i,bin)*GetPointAlpha(i,bin);
      }
    }
    return noff;
  }

  // get the total background (alpha*Noff) over some range for a single bin
  double BayesianBuffer::GetBackgroundSum(unsigned int idx1,unsigned int idx2,unsigned int bin) const {
    if(idx2 > points_.size()) idx2 = points_.size();
    if(idx1 == idx2) log_warn("Warning: GetBackgroundSum(" << idx1 << "," << idx2 << "," << bin << ") returns 0");
    if(idx1 >= points_.size() || idx2 <= idx1) return 0.0;
    double noff = 0.0;
    for(unsigned int i = idx1; i < idx2; ++i) {
      noff += GetPointNoff(i,bin)*GetPointAlpha(i,bin);
    }
    return noff;
  }

  // get the average alpha over some range (probably not a meaningful quantity!)
  double BayesianBuffer::GetAverageAlpha(unsigned int idx1,unsigned int idx2) const {
    if(idx2 > points_.size()) idx2 = points_.size();
    if(idx1 == idx2) log_warn("Warning: GetAverageAlpha(" << idx1 << "," << idx2 << ") returns 0");
    if(idx1 >= points_.size() || idx2 <= idx1) return 0.0;
    double alpha = 0.0;
    for(unsigned int i = idx1; i < idx2; ++i) {
      double factor = (points_[i]->GetBins() > 0 ? 1.0/((double)points_[i]->GetBins()) : 0.0);
      for(size_t bin = 0; bin < points_[i]->GetBins(); ++bin) {
        alpha += GetPointAlpha(i,bin)*factor;
      }
    }
    return (idx2 > idx1 ? alpha/((double)(idx2-idx1)) : 0.0);
  }

  // get the average alpha over some range for a single bin
  double BayesianBuffer::GetAverageAlpha(unsigned int idx1,unsigned int idx2,unsigned int bin) const {
    if(idx2 > points_.size()) idx2 = points_.size();
    if(idx1 == idx2) log_warn("Warning: GetAverageAlpha(" << idx1 << "," << idx2 << "," << bin << ") returns 0");
    if(idx1 >= points_.size() || idx2 <= idx1) return 0.0;
    double alpha = 0.0;
    for(unsigned int i = idx1; i < idx2; ++i) {
      alpha += GetPointAlpha(i,bin);
    }
    return (idx2 > idx1 ? alpha/((double)(idx2-idx1)) : 0.0);
  }

  // private call to Fitness used for making sure we only call Fitness() when
  // necessary and otherwise use the stored fitnesses in blockFitness_
  double
  BayesianBuffer::GetFitness(unsigned int idx1, unsigned int idx2)
    const
  {
    return Fitness(idx1,idx2)+GetPrior();
  }

  // get the stored value of the fitness from the blocks
  double
  BayesianBuffer::GetBlockFitness(unsigned int idx1,unsigned int idx2) const {
    if(!UseBlockFitness()) return GetFitness(idx1,idx2);
    if(idx1 >= blockFitness_.size()) {
      log_warn("Warning: GetBlockFitness(" << idx1 << "," << idx2 << ") first index (max " << blockFitness_.size()-1 << ") out of range...returning 0");
      return 0.0;
    }
    if(idx2 >= blockFitness_[idx1].size()+idx1+1) {
      log_warn("Warning: GetBlockFitness(" << idx1 << "," << idx2 << ") second index (max " << blockFitness_[idx1].size()+idx1 << ") out of range...returning 0");
      return 0.0;
    }
    return blockFitness_[idx1][idx2-idx1-1];
  }

  // add a DataPoint to the buffer
  void
  BayesianBuffer::AddDataPoint(double Non_in, double Noff_in, double alpha_in,
                           double mjd_in,double zenith)
  {
    std::vector<std::pair<double,double> > N_in;
    N_in.push_back(std::make_pair(Non_in,Noff_in));
    std::vector<double> alphas;
    alphas.push_back(alpha_in);
    AddDataPoint(N_in,alphas,mjd_in,zenith);
  }

  // add a DataPoint to the buffer
  void
  BayesianBuffer::AddDataPoint(std::vector<std::pair<double,double> >& N_in, std::vector<double>& alphas,
                           double mjd_in,double zenith)
  {
/*std::cout << "Entering AddDataPoint!" << std::endl;
std::cout << "  Number of observations: " << N_in.size() << "   number of alphas: " << alphas.size() << std::endl;
std::cout << "  MJD: " << std::setprecision(12) << mjd_in << std::endl;
for(size_t i=0; i<N_in.size(); i++) {
std::cout << "  point " << i << ": (" << N_in[i].first << "," << N_in[i].second << ";" << alphas[i] << ")" << std::endl;
}
std::cout << "Proceeding in AddDataPoint!" << std::endl;*/
    AddToCache(N_in,alphas,mjd_in,zenith);
    if(binctr_ >= rebinning_) {
      // only add the data cache if it's not zero; otherwise, discard it
      if(cacheNon_+cacheNoff_ > 0.0 || !ignoreZero_) {
        //points_.push_back(new DataPoint(cacheNon_, cacheNoff_, cacheAlpha_, cacheMJD_));
        points_.push_back(new DataPoint(cache_, cacheAlpha_, cacheMJD_, (cacheCount_ > 0 ? cacheAverageMJD_/((double)cacheCount_) : cacheMJD_), (cacheCount_ > 0 ? cacheAverageZenith_/((double)cacheCount_) : 0.0)));
        if(!IsBuffered()) bufferSize_ = points_.size();
        if(UseBlockFitness()) blockFitness_.push_back(std::deque<double>());
        if(points_.size() > bufferSize_) {
          if(points_[0]) delete points_[0];
          points_.pop_front();
          if(UseBlockFitness()) blockFitness_.pop_front();
          shifts_++;
        }
        if(IsSingleSearch()) {
        } else {
          if(UseBlockFitness()) {
            for (unsigned int i = 0; i < points_.size(); ++i) blockFitness_[i].push_back(GetFitness(i,points_.size()));
          }
        }
        newBlockComplete_ = true;
      }
      binctr_ = 0;
      ResetCache();
    }
  }

  // add to the data cache
  void BayesianBuffer::AddToCache(double Non,double Noff,double alpha,double mjd,double zenith) {
    std::vector<std::pair<double,double> > N;
    N.push_back(std::make_pair(Non,Noff));
    std::vector<double> alphas;
    alphas.push_back(alpha);
    AddToCache(N,alphas,mjd,zenith);
  }

  // add to the data cache
  void BayesianBuffer::AddToCache(std::vector<std::pair<double,double> >& N,std::vector<double>& alphas,double mjd,double zenith) {
    if(N.size() > cache_.size()) cache_.resize(N.size(),std::make_pair(0.0,0.0));
    for(size_t i=0; i<N.size(); ++i) {
      cache_[i].first = cache_[i].first+N[i].first;
      cache_[i].second = cache_[i].second+N[i].second;
      cacheNon_ += N[i].first;
      cacheNoff_ += N[i].second;
    }
    cacheAlpha_.resize(alphas.size());
    for(size_t i=0; i<alphas.size(); ++i) cacheAlpha_[i] = alphas[i];
    if(cacheMJD_ == -1.0)
      cacheMJD_ = mjd;
    cacheAverageMJD_ += mjd;
    cacheAverageZenith_ += zenith;
    cacheCount_++;
    binctr_++;
  }

  // determine the optimum segmentation of DataPoints in the buffer and return
  // the change points (does not return change points at the edge of the buffer)
  OptimizeInfo
  BayesianBuffer::OptimizeByInfo(std::vector<std::vector<unsigned int> >& changes)
  {
    OptimizeInfo info;
    newBlockComplete_ = false;
    std::vector<std::vector<unsigned int> > new_changes;
    changes.clear();
    changes.resize(priors_.size());
    std::vector<double>* fitnesses = NULL;
    double correction = 0.0;
    std::vector<double>* bufferFitnesses = NULL;
    if(IsTriggeredSearch()) {
      if(multipriors_) {
        new_changes.resize(priors_.size());
        OptimizeTriggered(new_changes);
      } else {
        new_changes.resize(1);
        OptimizeTriggered(new_changes);
      }
    } else if(IsSingleSearch()) {
      fitnesses = new std::vector<double>();
      if(computeBufferFitnesses_) bufferFitnesses = new std::vector<double>();
      if(multipriors_) {
        new_changes.resize(priors_.size());
        OptimizeSingle(new_changes,fitnesses,bufferFitnesses,&correction);
      } else {
        new_changes.resize(1);
        OptimizeSingle(new_changes,fitnesses,bufferFitnesses,&correction);
      }
    } else if(multipriors_) {
      std::vector<std::vector<double> > bests;
      std::vector<std::vector<unsigned int> > lasts;
      bests.resize(priors_.size());
      lasts.resize(priors_.size());
      for(unsigned int R = 1; R <= points_.size(); ++R) {
        for(unsigned int i=0; i<priors_.size(); i++) {
          unsigned int lastChangePoint = (R > 1 ? lasts[i][R-2] : 0);
          unsigned int rMin = (R - lastChangePoint > bufferSize_ ? R - bufferSize_ : 0);
          double maxA = (UseBlockFitness() ? blockFitness_[lastChangePoint][R-lastChangePoint-1] : GetFitness(lastChangePoint,R-lastChangePoint-1))+priors_[i];
          unsigned int rOpt = lastChangePoint;
          for(unsigned int r = rMin; r < R; ++r) {
            double A = (UseBlockFitness() ? blockFitness_[r][R-r-1] : GetFitness(r,R-r-1)) + priors_[i] + (r == 0 ? 0.0 : bests[i][r-1]);
            if(A > maxA || r == 0) {
              maxA = A;
              rOpt = r;
            }
          }
          bests[i].push_back(maxA);
          lasts[i].push_back(rOpt);
        }
      }
      unsigned int idx = points_.size();
      new_changes.resize(priors_.size());
      for(unsigned int i=0; i<priors_.size(); i++) {
        new_changes[i].push_back(idx);
        while(new_changes[i][new_changes[i].size() - 1] != 0)
          new_changes[i].push_back(lasts[i][new_changes[i][new_changes[i].size() - 1] - 1]);
        new_changes[i].pop_back();
        std::sort(new_changes[i].begin(),new_changes[i].end());
        if(new_changes[i].size() > 0)
          new_changes[i].pop_back();
      }
    } else {
      new_changes.resize(1);
      std::vector<double> best;
      std::vector<unsigned int> last;
      for (unsigned int R = 1; R <= points_.size(); ++R) {
        unsigned int lastChangePoint = (R > 1 ? last[R-2] : 0);
        unsigned int rMin = (R-lastChangePoint > bufferSize_ ? R-bufferSize_ : 0);
        double maxA = (UseBlockFitness() ? blockFitness_[lastChangePoint][R - lastChangePoint -1] : GetFitness(lastChangePoint,R-lastChangePoint-1));
        unsigned int rOpt = lastChangePoint;
        for (unsigned int r = rMin; r < R; ++r) {
          double A = (UseBlockFitness() ? blockFitness_[r][R - r - 1] : GetFitness(r,R-r-1)) + (r == 0 ? 0.0 : best[r - 1]);
          if (A > maxA || r == 0) {
            maxA = A;
            rOpt = r;
          }
        }
        best.push_back(maxA);
        last.push_back(rOpt);
      }
      unsigned int idx = points_.size();
      new_changes[0].push_back(idx);
      while(new_changes[0][new_changes[0].size() - 1] != 0)
        new_changes[0].push_back(last[new_changes[0][new_changes[0].size() - 1] - 1]);
      new_changes[0].pop_back();
      std::sort(new_changes[0].begin(),new_changes[0].end());
      if(new_changes[0].size() > 0)
        new_changes[0].pop_back();
    }
    // trim off change points that don't meet the MJD sanity check
    for(unsigned int i=0; i<new_changes.size(); i++) {
      for(unsigned int j=0; j<new_changes[i].size(); j++) {
        double mjd_diff = GetPointMJD(new_changes[i][j])-GetPointMJD(new_changes[i][j]-1);
        if(mjdJumpLimit_ > 0.0) {
          if(mjd_diff < mjdJumpLimit_ && mjd_diff > 0.0) changes[i].push_back(new_changes[i][j]);
        } else {
          changes[i].push_back(new_changes[i][j]);
        }
      }
    }
    // make the change points in terms of absolute positions instead of buffer positions
    unsigned int maxbin = 0;
    for(unsigned int i = 0; i < points_.size(); ++i) {
      unsigned int testbins = points_[i]->GetBins();
      if(testbins > maxbin) maxbin = testbins;
    }
    info.Reserve(priors_.size());
    for(unsigned int i=0; i<priors_.size(); i++) {
      for(unsigned int j = 0; j < changes[i].size(); ++j) {
        std::vector<std::vector<double> > data;
        data.resize(maxbin);
        for(unsigned int bin=0; bin<data.size(); ++bin) {
          data[bin].resize(6);
          if(j == 0) {
            data[bin][0] = GetSignalSum(0,changes[i][j],bin);
            data[bin][1] = GetBackgroundSum(0,changes[i][j],bin);
            data[bin][2] = GetSignalSum(changes[i][j],(changes[i].size() == 1 ? points_.size() : changes[i][j+1]),bin);
            data[bin][3] = GetBackgroundSum(changes[i][j],(changes[i].size() == 1 ? points_.size() : changes[i][j+1]),bin);
            data[bin][4] = GetAverageAlpha(0,changes[i][j],bin);
            data[bin][5] = GetAverageAlpha(changes[i][j],(changes[i].size() == 1 ? points_.size() : changes[i][j+1]),bin);
          } else if(j == changes[i].size()-1) {
            data[bin][0] = GetSignalSum(changes[i][j-1],changes[i][j],bin);
            data[bin][1] = GetBackgroundSum(changes[i][j-1],changes[i][j],bin);
            data[bin][2] = GetSignalSum(changes[i][j],points_.size(),bin);
            data[bin][3] = GetBackgroundSum(changes[i][j],points_.size(),bin);
            data[bin][4] = GetAverageAlpha(changes[i][j-1],changes[i][j],bin);
            data[bin][5] = GetAverageAlpha(changes[i][j],points_.size(),bin);
          } else {
            data[bin][0] = GetSignalSum(changes[i][j-1],changes[i][j],bin);
            data[bin][1] = GetBackgroundSum(changes[i][j-1],changes[i][j],bin);
            data[bin][2] = GetSignalSum(changes[i][j],changes[i][j+1],bin);
            data[bin][3] = GetBackgroundSum(changes[i][j],changes[i][j+1],bin);
            data[bin][4] = GetAverageAlpha(changes[i][j-1],changes[i][j],bin);
            data[bin][5] = GetAverageAlpha(changes[i][j],changes[i][j+1],bin);
          }
        }
        info.AddChangePoint(changes[i][j],changes[i][j]+shifts_,data,i);
//          info.AddChangePoint(changes[i][j],changes[i][j]+shifts_,sig_before,bkg_before,sig_after,bkg_after,bin,i);
      }
    }
    // return the difference between the fitnesses
    //return best[best.size()-1]-blockFitness_[0][points_.size()-1];
    //if(changes.size() == 0) return 0.0;
    double fitbase = Fitness(0,points_.size());
    double fitbest = fitbase;
    if(changes[0].size() > 0) {
      fitbest = Fitness(0,changes[0][0]);
      fitbest += Fitness(changes[0][changes[0].size()-1],points_.size());
      for(unsigned int i=1; i<changes[0].size(); ++i) {
        fitbest += Fitness(changes[0][i-1],changes[0][i]);
      }
    }
    for(unsigned int i=0; i<changes.size(); i++) {
      for(unsigned int j = 0; j < changes[i].size(); ++j) changes[i][j] = changes[i][j] + shifts_;
    }
    info.fitnessDistance_ = fitbest-fitbase;
    // add detailed fitness information for single change points
    if(fitnesses) {
      info.AddSingleFitnesses(fitnesses,bufferFitnesses,correction);
      if(bufferFitnesses) delete bufferFitnesses;
      delete fitnesses;
    }
    return info;
  }

  double BayesianBuffer::Optimize(std::vector<std::vector<unsigned int> >& changes) {
    OptimizeInfo info = OptimizeByInfo(changes);
    return info.fitnessDistance_;
  }

  // call Optimize() and then fill an OptimizeInfo class
  /*OptimizeInfo BayesianBuffer::OptimizeByInfo(std::vector<unsigned int>& changes) {
    double diff = Optimize(changes);
    OptimizeInfo info;
    info.fitnessDistance_ = diff;
    return info;
  }*/

  // get the fitness difference between the 1-block partition and the best
  // 2-block partition
  double BayesianBuffer::GetFitnessDiff0to1(bool fixed) const {
    double fitbase = Fitness(0,points_.size());
    double fitbest = fitbase;
    unsigned int max = (fixed ? 2 : points_.size());
    for(unsigned int i=1; i<max; ++i) {
      double candidate = Fitness(0,i)+Fitness(i,points_.size());
      if(candidate > fitbest) fitbest = candidate;
    }
    return fitbest-fitbase;
  }

  // return the MJDs of the change points instead of the change points themselves
  double BayesianBuffer::OptimizeMJD(std::vector<std::vector<double> >& mjds) {
    mjds.clear();
    std::vector<std::vector<unsigned int> > changes;
    double result = Optimize(changes);
    mjds.resize(changes.size());
    for(unsigned int i = 0; i < changes.size(); ++i) {
      for(unsigned int j=0; j < changes[i].size(); ++j) {
        mjds[i].push_back(points_[(changes[i][j] - shifts_)]->GetMJD());
      }
    }
    return result;
  }



  // ExcessRatioBB class
  ExcessRatioBB::ExcessRatioBB(double gamma_in, unsigned int bufferSize_in, unsigned int rebinning_in, std::vector<double>& alpha_in,bool ignore_zero_in,double mjd_limit_in,std::vector<double>* gammas,double correction,bool quiet) : BayesianBuffer(gamma_in, bufferSize_in, rebinning_in,ignore_zero_in,mjd_limit_in,gammas,correction,quiet) {
    alpha_.resize(alpha_in.size());
    for(size_t i=0; i<alpha_in.size(); ++i) alpha_[i] = alpha_in[i];
  }

  ExcessRatioBB::~ExcessRatioBB() {
  }

  double ExcessRatioBB::Fitness(unsigned int idx1, unsigned int idx2) const {
    size_t maxbin = 0;
    for(unsigned int i = idx1; i < idx2; ++i) {
      size_t testbins = points_[i]->GetBins();
      if(testbins > maxbin) maxbin = testbins;
    }
    double H = 0.0;
    for(size_t bin = 0; bin < maxbin; ++bin) {
      double N = 0.0;
      double Noff = 0.0;
      double Non = 0.0;
      for(unsigned int i = idx1; i < idx2; ++i) {
        if(!points_[i]->IsZero(bin)) {
          Non += points_[i]->GetNon(bin);
          Noff += points_[i]->GetNoff(bin);
          N += 1.0;
        }
      }
      if(N > 0.0 && Noff > 0.0) {
        double F = Non / (alpha_[bin] * Noff) - 1.0;
        double t1 = 1.0/(1.0 + alpha_[bin] + alpha_[bin] * F);
        for(unsigned int i = idx1; i < idx2; ++i) {
          if(!points_[i]->IsZero(bin)) {
            double sum = points_[i]->GetNon(bin) + points_[i]->GetNoff(bin);
            H -= sum * log(sum * t1);
            H += points_[i]->GetFactorialTerm(bin);
          }
        }
        H += Non + Noff;
        H -= log(alpha_[bin] + alpha_[bin] * F) * Non;
      }
    }
//    return -H + GetPrior();
    return -H;
  }



  // RatioBB class
  RatioBB::RatioBB(double gamma_in,unsigned int bufferSize_in,unsigned int rebinning_in,std::vector<double>& alpha_in,bool ignore_zero_in,double mjd_limit_in,std::vector<double>* gammas,double correction,bool quiet) : BayesianBuffer(gamma_in,bufferSize_in,rebinning_in,ignore_zero_in,mjd_limit_in,gammas,correction,quiet) {
    alpha_.resize(alpha_in.size());
    for(size_t i=0; i<alpha_in.size(); ++i) alpha_[i] = alpha_in[i];
  }

  RatioBB::~RatioBB() {
  }

  double RatioBB::Fitness(unsigned int idx1,unsigned int idx2) const {
    size_t maxbin = 0;
    for(unsigned int i = idx1; i < idx2; ++i) {
      size_t testbins = points_[i]->GetBins();
      if(testbins > maxbin) maxbin = testbins;
    }
    double sum = 0.0;
    for(size_t bin = 0; bin < maxbin; ++bin) {
      double N = 0.0;
      double M = 0.0;
      for(unsigned int i = idx1; i < idx2; ++i) {
        if(!points_[i]->IsZero(bin)) {
          N += points_[i]->GetNon(bin);
          M += points_[i]->GetNoff(bin);
          if(GetKeyword() == "offset") {
            sum += points_[i]->GetSubLogTerm(bin)-points_[i]->GetLogTerm(bin);
          } else {
            sum += points_[i]->GetLogTerm(bin)-points_[i]->GetFactorialTerm(bin);
          }
        }
      }
      if(GetKeyword() == "offset") {
        if(N > 0.0) sum += N * (1.0 + log(1.0 + M / N) );
        if(M > 0.0) sum += M * (1.0 + log(1.0 + N / M) );
      } else {
        if(N > 0.0) sum -= N * (1.0 + log(1.0 + M / N) );
        if(M > 0.0) sum -= M * (1.0 + log(1.0 + N / M) );
      }
    }
//    return 2.0*sum + GetPrior();
//    return 2.0*sum;
    return sum;
  }



  // SingleRatioBB class
  SingleRatioBB::SingleRatioBB(double gamma_in,unsigned int bufferSize_in,unsigned int rebinning_in,std::vector<double>& alpha_in,bool ignore_zero_in,double mjd_limit_in,std::vector<double>* gammas,bool rising_in,size_t min_in,size_t max_in,double correction,bool quiet) : BayesianBuffer(gamma_in,bufferSize_in,rebinning_in,ignore_zero_in,mjd_limit_in,gammas,correction,quiet) {
    alpha_.resize(alpha_in.size());
    for(size_t i=0; i<alpha_in.size(); ++i) alpha_[i] = alpha_in[i];
    rising_ = rising_in;
    changePointMin_ = min_in;
    changePointMax_ = max_in;
    startCountsOn_.resize(alpha_.size(),0.0);
    startCountsOff_.resize(alpha_.size(),0.0);
  }

  void SingleRatioBB::OptimizeSingle(std::vector<std::vector<unsigned int> >& changes,std::vector<double>* fitnesses,std::vector<double>* bufferFitnesses,double* correction) {
//std::cout << "Entering OptimizeSingle() with changes size " << changes.size() << std::endl;
    if(points_.size() < 2) {
      for(unsigned int i=0; i<changes.size(); ++i) changes[i].clear();
      return;
    }
    size_t maxbin = 0;
    for(unsigned int i = 0; i < points_.size(); ++i) {
      size_t testbins = points_[i]->GetBins();
      if(testbins > maxbin) maxbin = testbins;
    }
    if(fitnesses) fitnesses->resize(maxbin+1,0.0);
    if(correction) (*correction) = 0.0;
    if(bufferFitnesses) bufferFitnesses->resize(points_.size()-1,0.0);
//std::cout << "Max bin is " << maxbin << std::endl;
    std::vector<double> H;
    std::vector<std::vector<double> > Hsub;
    H.resize(points_.size()-1,0.0);
    if(fitnesses) {
      Hsub.resize(maxbin,std::vector<double>());
      for(size_t bin=0; bin<maxbin; ++bin) Hsub[bin].resize(points_.size()-1,0.0);
    }
    size_t minC = (changePointMin_ <= changePointMax_ ? changePointMin_ : 0);
    size_t maxC = (changePointMin_ <= changePointMax_ ? (changePointMax_ < points_.size()-1 ? (changePointMax_+1) : points_.size()-1) : points_.size()-1);
    if(GetKeyword() == "sigma") {
      //std::vector<std::pair<double,double> > obs0;
      std::vector<std::pair<double,double> > obs1;
      std::vector<std::pair<double,double> > obs2;
      //double p0 = 0.0;
      //double t0 = 0.0;
      //double s0 = 0.0;
      std::vector<double> alphas;
      alphas.resize(maxbin,0.0);
      //obs0.resize(maxbin,std::make_pair(0.0,0.0));
      obs1.resize(maxbin,std::make_pair(0.0,0.0));
      obs2.resize(maxbin,std::make_pair(0.0,0.0));
      std::vector<double> N;
      std::vector<double> M;
      N.resize(maxbin,0.0);
      M.resize(maxbin,0.0);
      for(size_t bin=0; bin<maxbin; ++bin) {
        for(unsigned int i=0; i<points_.size(); ++i) {
          N[bin] = N[bin]+points_[i]->GetNon(bin);
          M[bin] = M[bin]+points_[i]->GetNoff(bin);
          alphas[bin] += points_[i]->GetAlpha(bin);
        }
        //obs0[bin].first = N;
        //obs0[bin].second = M;
        alphas[bin] = (points_.size() > 0 ? alphas[bin]/((double)points_.size()) : 0.0);
      }
      //get_significance(obs0,alphas,p0,t0,s0);
      for(size_t C=minC; C<maxC; ++C) {
        double p1 = 0.0;
        double t1 = 0.0;
        double s1 = 0.0;
        double p2 = 0.0;
        double t2 = 0.0;
        double s2 = 0.0;
        for(size_t bin=0; bin<maxbin; ++bin) {
          obs1[bin].first = 0.0;
          obs1[bin].second = 0.0;
          obs2[bin].first = 0.0;
          obs2[bin].second = 0.0;
          double N1 = 0.0;
          double M1 = 0.0;
          for(size_t i=0; i<=C; ++i) {
            N1 += points_[i]->GetNon(bin);
            M1 += points_[i]->GetNoff(bin);
          }
          obs1[bin].first = N1;
          obs1[bin].second = M1;
          obs2[bin].first = N[bin]-N1;
          obs2[bin].second = M[bin]-M1;
        }
//log_info("Point " << C);
//for(size_t bin=0; bin<maxbin; ++bin) log_info("  Bin " << bin << " obs2: (" << obs2[bin].first << "," << obs2[bin].second << ")  obs1: (" << obs1[bin].first << "," << obs1[bin].second << ")  alpha: " << alphas[bin]);
        get_significance(obs1,alphas,p1,t1,s1);
        get_significance(obs2,alphas,p2,t2,s2);
//log_info("  Significance difference for " << C << ": " << s2 << " - " << s1 << " = " << s2-s1);
        //H[C] = s2-s1;
        H[C] = s2;
        // can't really get individual contributions, so just put them all in the first bin
        if(fitnesses) {
          Hsub[0][C] = H[C];
          for(size_t bin=1; bin<maxbin; ++bin) Hsub[bin][C] = 0.0;
        }
      }
    } else if(GetKeyword() == "limited") {
      // get the average alpha in each bin
      std::vector<double> alphas;
      alphas.resize(maxbin,0.0);
      for(size_t bin=0; bin<maxbin; ++bin) {
        for(unsigned int i=0; i<points_.size(); ++i) {
          alphas[bin] += points_[i]->GetAlpha(bin);
        }
        alphas[bin] = (points_.size() > 0 ? alphas[bin]/((double)points_.size()) : 0.0);
      }
      // compute the fitness
      for(size_t bin=0; bin<maxbin; ++bin) {
        double baseN0 = (startCountsOn_.size() > bin ? startCountsOn_[bin] : 0.0);
        double baseM0 = (startCountsOff_.size() > bin ? startCountsOff_[bin] : 0.0);
        for(size_t i=0; i<(minC < points_.size() ? minC : points_.size()); ++i) {
          baseN0 += points_[i]->GetNon(bin);
          baseM0 += points_[i]->GetNoff(bin);
        }
        double N = baseN0;
        double M = baseM0;
        for(unsigned int i=(minC < points_.size() ? minC : points_.size()); i<points_.size(); ++i) {
          N += points_[i]->GetNon(bin);
          M += points_[i]->GetNoff(bin);
        }
        bool sit1 = false;
        if(N < alphas[bin]*M) sit1 = true;
        double term = (sit1 ? 0.0 : (N > 0.0 ? N*log(1.0+M/N) : 0.0)+(M > 0.0 ? M*log(1.0+N/M) : 0.0));
        for(size_t C=minC; C<maxC; ++C) {
          double N0 = baseN0;
          double M0 = baseM0;
          for(size_t i=(minC < points_.size() ? minC : points_.size()); i<=C; ++i) {
            N0 += points_[i]->GetNon(bin);
            M0 += points_[i]->GetNoff(bin);
          }
          double N1 = N-N0;
          double M1 = M-M0;
          bool add = true;
          if(rising_) {
            if(N0*M1 > N1*M0) add = false;
          }
          double aterm = 0.0;
          if(add) {
            if(sit1) {
              if(N0 < alphas[bin]*M0) {
                if(N1 < alphas[bin]*M1) {
                  aterm = 0.0;
                } else {
                  double Cterm = -(N1 > 0.0 ? N1*log(1.0+M1/N1) : 0.0)-(M1 > 0.0 ? M1*log(1.0+N1/M1) : 0.0);
                  double Dterm = (alphas[bin] > 0.0 ? N1*log(alphas[bin]/(1.0+alphas[bin]))-M1*log(1.0+alphas[bin]) : 0.0);
                  aterm = Cterm-Dterm;
                }
              } else {
                double Bterm = -(N0 > 0.0 ? N0*log(1.0+M0/N0) : 0.0)-(M0 > 0.0 ? M0*log(1.0+N0/M0) : 0.0);
                double Eterm = (alphas[bin] > 0.0 ? M0*log(1.0+alphas[bin])-N0*log(alphas[bin]/(1.0+alphas[bin])) : 0.0);
                aterm = Bterm+Eterm;
              }
            } else {
              if(N0 < alphas[bin]*M0) {
                double Cterm = -(N1 > 0.0 ? N1*log(1.0+M1/N1) : 0.0)-(M1 > 0.0 ? M1*log(1.0+N1/M1) : 0.0);
                double Eterm = (alphas[bin] > 0.0 ? M0*log(1.0+alphas[bin])-N0*log(alphas[bin]/(1.0+alphas[bin])) : 0.0);
                aterm = term+Cterm-Eterm;
              } else {
                double Bterm = -(N0 > 0.0 ? N0*log(1.0+M0/N0) : 0.0)-(M0 > 0.0 ? M0*log(1.0+N0/M0) : 0.0);
                if(N1 < alphas[bin]*M1) {
                  double Dterm = (alphas[bin] > 0.0 ? N1*log(alphas[bin]/(1.0+alphas[bin]))-M1*log(1.0+alphas[bin]) : 0.0);
                  aterm = term+Bterm+Dterm;
                } else {
                  double Cterm = -(N1 > 0.0 ? N1*log(1.0+M1/N1) : 0.0)-(M1 > 0.0 ? M1*log(1.0+N1/M1) : 0.0);
                  aterm = term+Bterm+Cterm;
                }
              }
            }
          }
          if(aterm < 0.0) aterm = 0.0; // fix rounding errors
          aterm *= GetWeightLLH(bin);
          H[C] = H[C]+aterm;
          if(fitnesses) Hsub[bin][C] = Hsub[bin][C]+aterm;
        }
      }
    } else if(GetKeyword() == "source-free") {
      // get the average alpha in each bin
      std::vector<double> alphas;
      std::vector<std::pair<double,double> > alogs;
      alphas.resize(maxbin,0.0);
      alogs.resize(maxbin,std::make_pair(0.0,0.0));
      for(size_t bin=0; bin<maxbin; ++bin) {
        for(unsigned int i=0; i<points_.size(); ++i) {
          alphas[bin] += points_[i]->GetAlpha(bin);
        }
        alphas[bin] = (points_.size() > 0 ? alphas[bin]/((double)points_.size()) : 0.0);
        alogs[bin] = std::make_pair(log(1.0+1.0/alphas[bin]),log(1.0+alphas[bin]));
      }
      // compute the fitness
      for(size_t bin=0; bin<maxbin; ++bin) {
        //double baseN0 = (startCountsOn_.size() > bin ? startCountsOn_[bin] : 0.0);
        //double baseM0 = (startCountsOff_.size() > bin ? startCountsOff_[bin] : 0.0);
        //for(size_t i=0; i<(minC < points_.size() ? minC : points_.size()); ++i) {
        //  baseN0 += points_[i]->GetNon(bin);
        //  baseM0 += points_[i]->GetNoff(bin);
        //}
        //double N = baseN0;
        //double M = baseM0;
        //for(unsigned int i=(minC < points_.size() ? minC : points_.size()); i<points_.size(); ++i) {
        //  N += points_[i]->GetNon(bin);
        //  M += points_[i]->GetNoff(bin);
        //}
        //double term = (N > 0.0 ? N*log(1.0+M/N) : 0.0)+(M > 0.0 ? M*log(1.0+N/M) : 0.0);
        for(size_t C=minC; C<maxC; ++C) {
          double N1 = 0.0;
          double M1 = 0.0;
          for(size_t i=(C+1); i<points_.size(); ++i) {
            N1 += points_[i]->GetNon(bin);
            M1 += points_[i]->GetNoff(bin);
          }
          //double N0 = baseN0;
          //double M0 = baseM0;
          //for(size_t i=(minC < points_.size() ? minC : points_.size()); i<=C; ++i) {
          //  N0 += points_[i]->GetNon(bin);
          //  M0 += points_[i]->GetNoff(bin);
          //}
          //double N1 = N-N0;
          //double M1 = M-M0;
          bool add = true;
          if(N1 <= alphas[bin]*M1) add = false;
          //if(rising_) {
          //  if(N0*M1 > N1*M0) add = false;
          //}
          double aterm = 0.0;
          //if(add) aterm = term-(N0 > 0.0 ? N0*log(1.0+M0/N0) : 0.0)-(M0 > 0.0 ? M0*log(1.0+N0/M0) : 0.0)-(N1 > 0.0 ? N1*log(1.0+M1/N1) : 0.0)-(M1 > 0.0 ? M1*log(1.0+N1/M1) : 0.0);
          if(add) aterm = N1*alogs[bin].first+M1*alogs[bin].second-(N1 > 0.0 ? N1*log(1.0+M1/N1) : 0.0)-(M1 > 0.0 ? M1*log(1.0+N1/M1) : 0.0);
          if(aterm < 0.0) aterm = 0.0; // fix rounding errors
          aterm *= GetWeightLLH(bin);
          H[C] = H[C]+aterm;
          if(fitnesses) Hsub[bin][C] = Hsub[bin][C]+aterm;
        }
      }
    } else {
      // get the average alpha in each bin
      //std::vector<std::pair<double,double> > obs1;
      //std::vector<std::pair<double,double> > obs2;
/*      std::vector<double> alphas;
      alphas.resize(maxbin,0.0);
      //obs1.resize(maxbin,std::make_pair(0.0,0.0));
      //obs2.resize(maxbin,std::make_pair(0.0,0.0));
      //std::vector<double> N;
      //std::vector<double> M;
      //N.resize(maxbin,0.0);
      //M.resize(maxbin,0.0);
      for(size_t bin=0; bin<maxbin; ++bin) {
        for(unsigned int i=0; i<points_.size(); ++i) {
          //N[bin] = N[bin]+points_[i]->GetNon(bin);
          //M[bin] = M[bin]+points_[i]->GetNoff(bin);
          alphas[bin] += points_[i]->GetAlpha(bin);
        }
        alphas[bin] = (points_.size() > 0 ? alphas[bin]/((double)points_.size()) : 0.0);
      }*/
      // compute the fitness
      for(size_t bin=0; bin<maxbin; ++bin) {
        double baseN0 = (startCountsOn_.size() > bin ? startCountsOn_[bin] : 0.0);
        double baseM0 = (startCountsOff_.size() > bin ? startCountsOff_[bin] : 0.0);
        for(size_t i=0; i<(minC < points_.size() ? minC : points_.size()); ++i) {
          baseN0 += points_[i]->GetNon(bin);
          baseM0 += points_[i]->GetNoff(bin);
        }
        double N = baseN0;
        double M = baseM0;
        for(unsigned int i=(minC < points_.size() ? minC : points_.size()); i<points_.size(); ++i) {
          N += points_[i]->GetNon(bin);
          M += points_[i]->GetNoff(bin);
        }
//std::cout << "Bin " << bin << " sums: " << N << " and " << M << std::endl;
        double term = (N > 0.0 ? N*log(1.0+M/N) : 0.0)+(M > 0.0 ? M*log(1.0+N/M) : 0.0);
//std::cout << "  term: " << term << std::endl;
        for(size_t C=minC; C<maxC; ++C) {
          double N0 = baseN0;
          double M0 = baseM0;
          for(size_t i=(minC < points_.size() ? minC : points_.size()); i<=C; ++i) {
            N0 += points_[i]->GetNon(bin);
            M0 += points_[i]->GetNoff(bin);
          }
          double N1 = N-N0;
          double M1 = M-M0;
//std::cout << "  C = " << C << " sums: " << N0 << "," << M0 << " and " << N1 << "," << M1 << std::endl;
          // suppress negative fluctuations (doesn't work...not sure why -TRW 25 May 2016)
          /*if(N0 < alphas[bin]*M0) {
            double delta = alphas[bin]*M0-N0;
            double NN = N-delta;
            term = (NN > 0.0 ? NN*log(1.0+M/NN) : 0.0)+(M > 0.0 ? M*log(1.0+NN/M) : 0.0);
            N0 = alphas[bin]*M0;
          } else {
            term = (N > 0.0 ? N*log(1.0+M/N) : 0.0)+(M > 0.0 ? M*log(1.0+N/M) : 0.0);
          }*/
          bool add = true;
          if(rising_) {
            if(N0*M1 > N1*M0) add = false;
          }
          //if(add) H[C] = H[C]+term-(N0 > 0.0 ? N0*log(1.0+M0/N0) : 0.0)-(M0 > 0.0 ? M0*log(1.0+N0/M0) : 0.0)-(N1 > 0.0 ? N1*log(1.0+M1/N1) : 0.0)-(M1 > 0.0 ? M1*log(1.0+N1/M1) : 0.0);
          double aterm = 0.0;
          if(add) aterm = term-(N0 > 0.0 ? N0*log(1.0+M0/N0) : 0.0)-(M0 > 0.0 ? M0*log(1.0+N0/M0) : 0.0)-(N1 > 0.0 ? N1*log(1.0+M1/N1) : 0.0)-(M1 > 0.0 ? M1*log(1.0+N1/M1) : 0.0);
          if(aterm < 0.0) aterm = 0.0; // fix rounding errors
          aterm *= GetWeightLLH(bin);
          H[C] = H[C]+aterm;
          if(fitnesses) Hsub[bin][C] = Hsub[bin][C]+aterm;
        }
      }
    }
    double Hmax = H[0]+GetPriorCorrection(0);
    size_t Cmax = 0;
    if(bufferFitnesses) bufferFitnesses->at(0) = H[0];
    for(size_t i=1; i<H.size(); i++) {
      if(bufferFitnesses) bufferFitnesses->at(i) = H[i];
      if(H[i]+GetPriorCorrection(i) > Hmax) {
        Hmax = H[i]+GetPriorCorrection(i);
        Cmax = i;
      }
    }
    // if the significance before is too negative, let's get rid of it!
    if(GetKeyword() == "sigma-check" || GetKeyword() == "limited") {
      std::vector<std::pair<double,double> > obs1;
      std::vector<std::pair<double,double> > obs2;
      std::vector<double> alphas;
      alphas.resize(maxbin,0.0);
      obs1.resize(maxbin,std::make_pair(0.0,0.0));
      obs2.resize(maxbin,std::make_pair(0.0,0.0));
      std::vector<double> N;
      std::vector<double> M;
      N.resize(maxbin,0.0);
      M.resize(maxbin,0.0);
      for(size_t bin=0; bin<maxbin; ++bin) {
        if(startCountsOn_.size() > bin) N[bin] = N[bin]+startCountsOn_[bin];
        if(startCountsOff_.size() > bin) M[bin] = M[bin]+startCountsOff_[bin];
        for(size_t i=0; i<points_.size(); ++i) {
          N[bin] = N[bin]+points_[i]->GetNon(bin);
          M[bin] = M[bin]+points_[i]->GetNoff(bin);
          alphas[bin] += points_[i]->GetAlpha(bin);
        }
        alphas[bin] = (points_.size() > 0 ? alphas[bin]/((double)points_.size()) : 0.0);
      }
      for(size_t bin=0; bin<maxbin; ++bin) {
        double N1 = (startCountsOn_.size() > bin ? startCountsOn_[bin] : 0.0);
        double M1 = (startCountsOff_.size() > bin ? startCountsOff_[bin] : 0.0);
        for(size_t i=0; i<=Cmax; ++i) {
          N1 += points_[i]->GetNon(bin);
          M1 += points_[i]->GetNoff(bin);
        }
        obs1[bin].first = N1;
        obs1[bin].second = M1;
        obs2[bin].first = N[bin]-N1;
        obs2[bin].second = M[bin]-M1;
      }
      double p1 = 0.0;
      double t1 = 0.0;
      double s1 = 0.0;
      double p2 = 0.0;
      double t2 = 0.0;
      double s2 = 0.0;
      get_significance(obs1,alphas,p1,t1,s1);
      get_significance(obs2,alphas,p2,t2,s2);
      if(s2 < -s1) {
log_debug("Killing significance transition " << s1 << " --> " << s2);
        Hmax = 0.0;
        for(size_t bin=0; bin<maxbin; ++bin) Hsub[bin][Cmax] = 0.0;
      }
    }
    if(fitnesses) {
      for(size_t bin=0; bin<maxbin; ++bin) {
        fitnesses->at(bin) = Hsub[bin][Cmax];
      }
      fitnesses->at(maxbin) = Hmax;
    }
    if(correction) (*correction) = GetPriorCorrection(Cmax);
    if(IsMultiPriored()) {
      for(unsigned int i=0; i<changes.size(); ++i) {
        if(Hmax+GetPrior(i) > 0.0) changes[i].push_back(Cmax+1);
      }
    } else {
      if(Hmax+GetPrior() > 0.0) changes[0].push_back(Cmax+1);
    }
  }



  // RisingRatioBB class
/*  RisingRatioBB::RisingRatioBB(double gamma_in,unsigned int bufferSize_in,unsigned int rebinning_in,std::vector<double>& alpha_in,bool ignore_zero_in,double mjd_limit_in,std::vector<double>* gammas) : BayesianBuffer(gamma_in,bufferSize_in,rebinning_in,ignore_zero_in,mjd_limit_in,gammas) {
    alpha_.resize(alpha_in.size());
    for(size_t i=0; i<alpha_in.size(); ++i) alpha_[i] = alpha_in[i];
  }

  void RisingRatioBB::OptimizeSingle(std::vector<std::vector<unsigned int> >& changes) {
    if(points_.size() < 2) {
      for(unsigned int i=0; i<changes.size(); ++i) changes[i].clear();
      return;
    }
    size_t maxbin = 0;
    for(unsigned int i = 0; i < points_.size(); ++i) {
      size_t testbins = points_[i]->GetBins();
      if(testbins > maxbin) maxbin = testbins;
    }
    std::vector<double> H;
    H.resize(points_.size()-1,0.0);
    for(size_t bin=0; bin<maxbin; ++bin) {
      double N = 0.0;
      double M = 0.0;
      for(unsigned int i=0; i<points_.size(); ++i) {
        N += points_[i]->GetNon(bin);
        M += points_[i]->GetNoff(bin);
      }
      double term = (N > 0.0 ? N*log(1.0+M/N) : 0.0)+(M > 0.0 ? M*log(1.0+N/M) : 0.0);
      for(unsigned int C=0; C<(points_.size()-1); ++C) {
        double N0 = 0.0;
        double M0 = 0.0;
        for(unsigned int i=0; i<=C; ++i) {
          N0 += points_[i]->GetNon(bin);
          M0 += points_[i]->GetNoff(bin);
        }
        double N1 = N-N0;
        double M1 = M-M0;
        // don't record the contribution if the ratio before is larger than the ratio after
        if(!(N0*M1 > N1*M0)) {
          H[C] = H[C]+term-(N0 > 0.0 ? N0*log(1.0+M0/N0) : 0.0)-(M0 > 0.0 ? M0*log(1.0+N0/M0) : 0.0)-(N1 > 0.0 ? N1*log(1.0+M1/N1) : 0.0)-(M1 > 0.0 ? M1*log(1.0+N1/M1) : 0.0);
        }
      }
    }
    double Hmax = H[0];
    size_t Cmax = 0;
    for(size_t i=1; i<H.size(); i++) {
      if(H[i] > Hmax) {
        Hmax = H[i];
        Cmax = i;
      }
    }
    if(IsMultiPriored()) {
      for(unsigned int i=0; i<changes.size(); ++i) {
        if(Hmax+GetPrior(i) > 0.0) changes[i].push_back(Cmax+1);
      }
    } else {
      if(Hmax+GetPrior() > 0.0) changes[0].push_back(Cmax+1);
    }
  }*/



  // TwoStepRatioBB class
  TwoStepRatioBB::TwoStepRatioBB(double gamma_in,unsigned int buffer_in,unsigned int binning_in,std::vector<double>& alpha_in,bool ignore_zero_in,double mjd_limit_in,std::vector<double>* gammas,double correction) : RatioBB(gamma_in,buffer_in,binning_in,alpha_in,ignore_zero_in,mjd_limit_in,gammas,correction) {
    log_prob_ = log(gamma_in);
  }

  // determine if splitting the buffer into two blocks is better than not splitting it
  //void TwoStepRatioBB::OptimizeTriggered(std::vector<unsigned int>& changes) {
  void TwoStepRatioBB::OptimizeTriggered(std::vector<std::vector<unsigned int> >& changes) {
    if(points_.size() < 2) {
      for(unsigned int i=0; i<changes.size(); ++i) changes[i].clear();
      return;
    }
    double base_fitness = GetBlockFitness(0,points_.size());
    unsigned int best_split = 1;
    double max_fitness = GetBlockFitness(0,1)+GetBlockFitness(1,points_.size());
    for(unsigned int i = 2; i< points_.size(); ++i) {
      double test_fitness = GetBlockFitness(0,i)+GetBlockFitness(i,points_.size());
      if(test_fitness > max_fitness) {
        max_fitness = test_fitness;
        best_split = i;
      }
    }
    if(IsMultiPriored()) {
      for(unsigned int i=0; i<changes.size(); ++i) {
        if(max_fitness+GetPrior(i) > base_fitness) changes[i].push_back(best_split);
      }
    } else {
      if(max_fitness > base_fitness) changes[0].push_back(best_split);
    }
/*    changes.clear();
    if(points_.size() < 2) return;
    double base_fitness = GetBlockFitness(0,points_.size());
    unsigned int best_split = 1;
    double max_fitness = GetBlockFitness(0,1)+GetBlockFitness(1,points_.size());
    for(unsigned int i = 2; i< points_.size(); ++i) {
      double test_fitness = GetBlockFitness(0,i)+GetBlockFitness(i,points_.size());
      if(test_fitness > max_fitness) {
        max_fitness = test_fitness;
        best_split = i;
      }
    }
    if(max_fitness > base_fitness) changes.push_back(best_split);*/
  }



  // BelieveBackgroundBB class
  BelieveBackgroundBB::BelieveBackgroundBB(double prob_in,unsigned int rebinning_in,std::vector<double>& alpha_in,double src_factor_in,double mjd_limit_in,std::vector<double>* gammas,double correction,bool quiet) : BayesianBuffer(prob_in,2,rebinning_in,true,mjd_limit_in,gammas,correction,quiet) {
    log_prob_ = log(prob_in);
    alpha_.resize(alpha_in.size());
    for(size_t i=0; i<alpha_in.size(); ++i) alpha_[i] = alpha_in[i];
    src_factor_.resize(alpha_in.size());
    for(size_t i=0; i<alpha_in.size(); ++i) src_factor_[i] = (1.0+src_factor_in)*alpha_[i];
  }

  BelieveBackgroundBB::~BelieveBackgroundBB() {
  }

  double BelieveBackgroundBB::Fitness(unsigned int idx1,unsigned int idx2) const {
    return 1.0;
  }

  void BelieveBackgroundBB::OptimizeTriggered(std::vector<std::vector<unsigned int> >& changes) {
    if(points_.size() < 2) {
      for(unsigned int i=0; i<changes.size(); ++i) changes[i].clear();
      return;
    }
    size_t maxbin = 0;
    for(unsigned int i = 0; i < points_.size(); ++i) {
      size_t testbins = points_[i]->GetBins();
      if(testbins > maxbin) maxbin = testbins;
    }
    double log_sum = 0.0;
    for(size_t bin = 0; bin < maxbin; ++bin) {
      double mu = src_factor_[bin]*GetLastNoff(bin);
      if(mu > 0.0) {
        double ni = GetLastNon(bin);
        double log_pi = ni*log(mu)-mu-SpecialFunctions::Gamma::lnG(ni+1.0);
        log_sum += log_pi;
      }
    }
    /*double base_fitness = GetBlockFitness(0,points_.size());
    unsigned int best_split = 1;
    double max_fitness = GetBlockFitness(0,1)+GetBlockFitness(1,points_.size());
    for(unsigned int i = 2; i< points_.size(); ++i) {
      double test_fitness = GetBlockFitness(0,i)+GetBlockFitness(i,points_.size());
      if(test_fitness > max_fitness) {
        max_fitness = test_fitness;
        best_split = i;
      }
    }*/
    if(IsMultiPriored()) {
      for(unsigned int i=0; i<changes.size(); ++i) {
        if(log_sum < GetPrior(i)) changes[i].push_back(GetLastIndex()-GetShifts());
      }
    } else {
      if(log_sum < log_prob_) changes[0].push_back(GetLastIndex()-GetShifts());
    }
/*    changes.clear();
    size_t maxbin = 0;
    for(unsigned int i = 0; i < points_.size(); ++i) {
      size_t testbins = points_[i]->GetBins();
      if(testbins > maxbin) maxbin = testbins;
    }
    double log_sum = 0.0;
    for(size_t bin = 0; bin < maxbin; ++bin) {
      double mu = src_factor_[bin]*GetLastNoff(bin);
      if(mu > 0.0) {
        double ni = GetLastNon(bin);
        double log_pi = ni*log(mu)-mu-SpecialFunctions::Gamma::lnG(ni+1.0);
        log_sum += log_pi;
      }
    }
    if(log_sum < log_prob_) changes.push_back(GetLastIndex()-GetShifts());*/
  }



  // LikelihoodWindowBB class
  /*LikelihoodWindowBB::LikelihoodWindowBB(double gamma_in,unsigned int buffer_in,unsigned int binning_in,double alpha_in,bool ignore_zero_in,double mjd_limit_in) : BayesianBuffer(gamma_in,buffer_in,binning_in,ignore_zero_in,mjd_limit_in) {
    alpha_ = alpha_in;
    for(unsigned int i=0; i<buffer_in; ++i) {
      N_.push_back(0.0);
      M_.push_back(0.0);
    }
  }

  void LikelihoodWindowBB::AddPoint(double non,double noff,double alpha,double mjd) {
    AddDataPoint(non,noff,alpha,mjd);
    if(IsNewBlockComplete()) {
      if(contribution_.size() > GetBufferSize()) {
        contribution_.pop_front();
        n_.pop_front();
        m_.pop_front();
      }
      double contribution = Contribution(contribution_.size());
      contribution_.push_back(contribution);
      n_.push_back(GetLastNon());
      m_.push_back(GetLastNoff());
      double
    }
  }

  void LikelihoodWindowBB::OptimizeTriggered(std::vector<unsigned int>& changes) {
    changes.clear();
    double mu = src_factor_*GetLastNoff();
    if(mu > 0.0) {
      double ni = GetLastNon();
      double log_pi = ni*log(mu)-mu-SpecialFunctions::Gamma::lnG(ni+1.0);
      if(log_pi < log_prob_) changes.push_back(GetLastIndex()-GetShifts());
    }
  }

  double LikelihoodWindowBB::Contribution(unsigned int idx) {
    double N = 0.0;
    double Noff = 0.0;
    double Non = 0.0;
    for(unsigned int i = idx1; i < idx2; ++i) {
      if(!points_[i]->IsZero()) {
        Non += points_[i]->GetNon();
        Noff += points_[i]->GetNoff();
        N += 1.0;
      }
    }
    if(N == 0.0) return 0.0;
    if(Noff == 0.0) return 0.0;
    double F = Non / (alpha_ * Noff) - 1.0;
    double t1 = 1.0/(1.0 + alpha_ + alpha_ * F);
    double H = 0.0;
    for(unsigned int i = idx1; i < idx2; ++i) {
      if(!points_[i]->IsZero()) {
        double sum = points_[i]->GetNon() + points_[i]->GetNoff();
        H -= sum * log(sum * t1);
        H += points_[i]->GetFactorialTerm();
      }
    }
    H += Non + Noff;
    H -= log(alpha_ + alpha_ * F) * Non;
//    return -H + GetPrior();
    return -H;
  }*/



  // RatioScargleErrorBB class
  RatioScargleErrorBB::RatioScargleErrorBB(double gamma_in,unsigned int bufferSize_in,unsigned int rebinning_in,bool ignore_zero_in,double mjd_limit_in,std::vector<double>* gammas,double correction,bool quiet) : BayesianBuffer(gamma_in,bufferSize_in,rebinning_in,ignore_zero_in,mjd_limit_in,gammas,correction,quiet) {
  }

  RatioScargleErrorBB::~RatioScargleErrorBB() {
  }

  double RatioScargleErrorBB::Fitness(unsigned int idx1,unsigned int idx2) const {
    size_t maxbin = 0;
    for(unsigned int i = idx1; i < idx2; ++i) {
      size_t testbins = points_[i]->GetBins();
      if(testbins > maxbin) maxbin = testbins;
    }
    //for(size_t bin = 0; bin < maxbin; ++bin) {
    double ak = 0.0;
    double bk = 0.0;
    for(unsigned int i = idx1; i < idx2; ++i) {
      double err = points_[i]->GetExcessError();
      if(err > 0.0) {
        ak += 1.0/(err*err);
        double non = 0.0;
        double noff = 0.0;
        for(size_t bin = 0; bin < maxbin; ++bin) {
          non += points_[i]->GetNon(bin);
          noff += points_[i]->GetNoff(bin);
        }
        log_fatal("Not implemented!");
        //bk += non/(points_[i]->GetAlpha(bin)*noff*err*err);
      }
    }
    ak *= 0.5;
    if(ak > 0.0) return 0.25*bk*bk/ak;
    return 0.0;
/*    double S1 = 0.0;
    double S2 = 0.0;
    for(unsigned int i = idx1; i < idx2; ++i) {
      if(!points_[i]->IsZero()) {
        double n = points_[i]->GetNon();
        double m = points_[i]->GetNoff();
        double alpha = points_[i]->GetAlpha();
        double m2sinv = m*m/(n+m);
        if(n > 0.0 && m > 0.0) {
          S1 += m2sinv;
          S2 += alpha*alpha*m*m2sinv/n;
        }
      }
    }
    if(S2 > 0.0) return 0.5*S1*S1/S2;
    return 0.0;*/
  }

}

