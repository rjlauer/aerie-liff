/*!
 * @file BayesianBuffer.h
 * @brief Implementation of Bayesian Block algorithm with buffering
 * @author Tom Weisgarber
 * @date 28 April 2014
 * @version $Id: BayesianBuffer.h 38436 2017-03-29 20:07:37Z tweisgarber $
 *
 * The algorithm as implemented computes the optimal partitioning of
 * a buffer of DataPoint objects.
 */

#ifndef DATACLASSES_MATH_BAYESIANBLOCK_H_INCLUDED
#define DATACLASSES_MATH_BAYESIANBLOCK_H_INCLUDED

#include <algorithm>
#include <iostream>
#include <vector>
#include <deque>
#include <string>

class TF1;

namespace BayesianBuffer {

  // Handy functions related to Li Ma significance calculation
  // (probably should move these somewhere else!)
  double convert_chi2_value(double,double,double);
  double get_li_ma_statistic_contribution(double non,double noff,double alpha);
  double get_sigma(double Non,double Noff,double alpha);
  void get_significance(std::vector<std::pair<double,double> >& obs,double alpha,double& prob,double& ts,double& sigma_like,bool check_limit = true);
  void get_significance(std::pair<double,double> obs,double alpha,double& prob,double& ts,double& sigma_like,bool check_limit = true);
  void get_significance(std::vector<std::pair<double,double> >& obs,std::vector<double>& alpha,double& prob,double& ts,double& sigma_like,bool check_limit = true);
  void interpret_ts_value(double ts,double n,double weighted,double& prob,double& sigma_like,bool check_limit = true);
  double solve_chi2(double,double);

  /*!
   * @class DataPoint
   * @author Tom Weisgarber
   * @date 28 April 2014
   * @ingroup math
   * @brief Representation of data operated on by the Bayesian block algoritm
   *
   * This class is a rudimentary collection of data from an observation. It
   * should be replaced with something more general. Right now it has Non,
   * Noff, alpha, and an MJD timestamp for the observation.
   *
   * Updated 23 March 2015 to include multiple bins -TRW
   *
   * It also computes log(Non!) + log(Noff!) for use with the default Bayesian
   * block fitness.
   */
  class DataPoint {

    public:

      DataPoint(double, double, double, double, double,double = 0.0);
      DataPoint(std::vector<std::pair<double,double> >&, std::vector<double>&, double, double,double = 0.0);
      ~DataPoint() {}

    public:

      inline double GetMJD() const { return mjd_; }
      inline double GetAverageMJD() const { return average_mjd_; }
      inline double GetZenith() const { return zenith_; }
      inline double GetLiMaSig() const { return lima_sig_; }
      inline double GetExcessError() const { return excess_error_; }

      inline double GetAlpha(size_t i = 0) const { return (i < alpha_.size() ? alpha_[i] : 1.0); }
      inline double GetAlphaInverse(size_t i = 0) const { return (i < alphaInverse_.size() ? alphaInverse_[i] : 1.0); }
      inline double GetLogAlpha(size_t i = 0) const { return (i < alphaLog_.size() ? alphaLog_[i] : 0.0); }
      inline bool IsZero(size_t i = 0) const { return (i < zero_.size() ? zero_[i] : true); }
      inline double GetNon(size_t i = 0) const { return (i < N_.size() ? N_[i].first : 0.0); }
      inline double GetNoff(size_t i = 0) const { return (i < N_.size() ? N_[i].second : 0.0); }
      inline double GetFactorialTerm(size_t i = 0) const { return i < factorialTerm_.size() ? factorialTerm_[i] : 0.0; }
      inline double GetLogTerm(size_t i = 0) const { return i < logTerm_.size() ? logTerm_[i] : 0.0; }
      inline double GetSubLogTerm(size_t i = 0) const { return i < subLogTerm_.size() ? subLogTerm_[i] : 0.0; }

      inline size_t GetBins() const { return N_.size(); }

    private:

      void Compute();

    private:

      std::vector<bool> zero_;
      std::vector<std::pair<double,double> > N_;
      std::vector<double> alpha_;
      std::vector<double> alphaInverse_;
      std::vector<double> alphaLog_;
      std::vector<double> factorialTerm_;
      std::vector<double> logTerm_;
      std::vector<double> subLogTerm_;
      double mjd_;
      double average_mjd_;
      double zenith_;
      double lima_sig_;
      double lima_prob_;
      double lima_ts_;
      double excess_error_;

  };


  /*!
   * @class OptimizeInfo
   * @author Tom Weisgarber
   * @date 2 October 2014
   * @ingroup math
   * @brief Object to hold the results of a Bayesian Buffer Optimize() call
   *
   */

  class OptimizeInfo {

    public:
      OptimizeInfo();
      ~OptimizeInfo() {};

    public:
      //void AddChangePoint(unsigned int,unsigned int,double,double,double,double,unsigned int,unsigned int = 0);
      void AddChangePoint(unsigned int,unsigned int,std::vector<std::vector<double> >&,unsigned int = 0);
      inline void Reserve(unsigned int i) {
        change_.resize(i);
        bufferChange_.resize(i);
        beforeSignal_.resize(i);
        beforeBackground_.resize(i);
        beforeAverageAlpha_.resize(i);
        afterSignal_.resize(i);
        afterBackground_.resize(i);
        afterAverageAlpha_.resize(i);
        fitnesses_.clear();
        bufferFitnesses_.clear();
      }

      void AddSingleFitnesses(std::vector<double>* fitnesses,std::vector<double>* bufferFitnesses = NULL,double correction = 0.0) {
        if(fitnesses) {
          fitnesses_.clear();
          fitnesses_.resize(fitnesses->size(),0.0);
          for(size_t i=0; i<fitnesses->size(); i++) fitnesses_[i] = fitnesses->at(i);
          if(fitnesses_.size() > 0) fitnessDistance_ = fitnesses_[fitnesses_.size()-1];
        }
        if(bufferFitnesses) {
          bufferFitnesses_.resize(bufferFitnesses->size(),0.0);
          for(size_t i=0; i<bufferFitnesses->size(); i++) bufferFitnesses_[i] = bufferFitnesses->at(i);
        }
        correction_ = correction;
      }

    public:
      double fitnessDistance_;
      /*std::vector<unsigned int> change_;
      std::vector<unsigned int> bufferChange_;
      std::vector<double> beforeSignal_;
      std::vector<double> beforeBackground_;
      std::vector<double> afterSignal_;
      std::vector<double> afterBackground_;*/

      std::vector<std::vector<unsigned int> > change_;
      std::vector<std::vector<unsigned int> > bufferChange_;
      std::vector<std::vector<std::vector<double> > > beforeSignal_;
      std::vector<std::vector<std::vector<double> > > beforeBackground_;
      std::vector<std::vector<std::vector<double> > > beforeAverageAlpha_;
      std::vector<std::vector<std::vector<double> > > afterSignal_;
      std::vector<std::vector<std::vector<double> > > afterBackground_;
      std::vector<std::vector<std::vector<double> > > afterAverageAlpha_;
      std::vector<double> fitnesses_;
      double correction_;
      std::vector<double> bufferFitnesses_;
  };


  /*!
   * @class BayesianBuffer
   * @author Tom Weisgarber
   * @date 28 April 2014
   * @ingroup math
   * @brief Modified version of the Bayesian block algorithm
   *
   * Adapted from Scargle et al. ApJ. 764, 176 (2013).
   *
   * Maintains a buffer of DataPoint objects and caches their fitnesses
   * for the Bayesian block algorithm. Calls to Optimize() will find
   * the optimum partition of the buffer in terms of the locations of
   * the change points.
   *
   * You can inherit from this class and overload the Fitness()
   * function to define your own fitness algorithm.
   *
   * Added 22 May 2014 by TRW:
   *   Buffer now includes a rebinning integer, so that it waits for
   *   that number of data points to come in, then packages them into a
   *   single block.
   *
   * Added 2 October 2014 by TRW:
   *   Buffer now includes an option to ignore change points occuring
   *   when the MJD jumps by more than a certain amount
   *
   * Added 24 March 2015 by TRW:
   *   Modify for use with multiple bins
   */

  class BayesianBuffer {

    public:

      BayesianBuffer(double, unsigned int, unsigned int = 1,bool = true,double = -1.0,std::vector<double>* = NULL,double = 0.0,bool = false);
      virtual ~BayesianBuffer();
      // only implemented in SingleRatioBB class right now
      inline void SetStartCountsOn(size_t bin,double count) { if(startCountsOn_.size() > bin) startCountsOn_[bin] = count; }
      inline void SetStartCountsOff(size_t bin,double count) { if(startCountsOff_.size() > bin) startCountsOff_[bin] = count; }

    public:

      virtual void AddPoint(double non,double noff,double alpha,double mjd,double zenith = 0.0) { AddDataPoint(non,noff,alpha,mjd,zenith); }
      virtual void AddPoint(std::vector<std::pair<double,double> >& N,std::vector<double>& alphas,double mjd,double zenith = 0.0) { AddDataPoint(N,alphas,mjd,zenith); }
      // Optimize returns the difference between the fitness of the optimum
      // partition and the fitness of the 1-block partition
      double Optimize(std::vector<std::vector<unsigned int> >&);
      double OptimizeMJD(std::vector<std::vector<double> >&);
      OptimizeInfo OptimizeByInfo(std::vector<std::vector<unsigned int> >&);

      virtual double Fitness(unsigned int,unsigned int) const;
      //virtual bool IsLastPointTrigger() const { return false; }
      //virtual void OptimizeTriggered(std::vector<unsigned int>& changes) { changes.clear(); }
      virtual void OptimizeTriggered(std::vector<std::vector<unsigned int> >& changes) { changes.clear(); }
      virtual void OptimizeSingle(std::vector<std::vector<unsigned int> >& changes,std::vector<double>* vec = NULL,std::vector<double>* vec2 = NULL,double* = NULL) { changes.clear(); }
      virtual inline bool IsTriggeredSearch() const { return false; }
      virtual inline bool IsSingleSearch() const { return false; }
      virtual inline bool UseBlockFitness() const { return true; }
      double GetSignalSum(unsigned int,unsigned int) const;
      double GetBackgroundSum(unsigned int,unsigned int) const;
      double GetAverageAlpha(unsigned int,unsigned int) const;
      double GetSignalSum(unsigned int,unsigned int,unsigned int) const;
      double GetBackgroundSum(unsigned int,unsigned int,unsigned int) const;
      double GetAverageAlpha(unsigned int,unsigned int,unsigned int) const;

      //inline double GetPrior() const { return (multipriors_ ? 0.0 : prior_+basePriorCorrection_); }
      inline double GetPrior() const { return (multipriors_ ? 0.0 : prior_); }
      double GetPrior(unsigned int) const;
      inline unsigned int GetBufferSize() const { return bufferSize_; }
      inline bool IsNewBlockComplete() const { return newBlockComplete_; }
      inline bool IsBufferFull() const { return points_.size() >= bufferSize_; }
      inline bool IsMultiPriored() const { return multipriors_; }
      inline size_t GetFilledSize() const { return points_.size(); }

      double GetPointNon(unsigned int,size_t = 0) const;
      double GetPointNoff(unsigned int,size_t = 0) const;
      double GetPointAlpha(unsigned int,size_t = 0) const;
      double GetPointMJD(unsigned int) const;
      double GetPointAverageMJD(unsigned int) const;
      double GetPointZenith(unsigned int) const;
      double GetPointLiMaSig(unsigned int) const;
      unsigned int GetPointIndex(unsigned int) const;
      inline double GetLastNon(size_t bin = 0) const { return GetPointNon(points_.size()-1,bin); }
      inline double GetLastNoff(size_t bin = 0) const { return GetPointNoff(points_.size()-1,bin); }
      inline double GetLastAlpha(size_t bin = 0) const { return GetPointAlpha(points_.size()-1,bin); }
      inline double GetLastMJD() const { return GetPointMJD(points_.size()-1); }
      inline double GetLastAverageMJD() const { return GetPointAverageMJD(points_.size()-1); }
      inline double GetLastZenith() const { return GetPointZenith(points_.size()-1); }
      inline double GetLastLiMaSig() const { return GetPointLiMaSig(points_.size()-1); }
      inline unsigned int GetLastIndex() const { return GetPointIndex(points_.size()-1); }
      inline unsigned int GetShifts() const { return shifts_; }
      inline unsigned int GetRebinning() const { return rebinning_; }

      inline std::string GetKeyword() const { return keyword_; }
      inline void SetKeyword(std::string word) { keyword_ = word; }

      double GetBlockFitness(unsigned int,unsigned int) const;

      double GetFitnessDiff0to1(bool) const;

      void ReadPriorCorrection(std::string);
      //inline double GetPriorCorrection(size_t bin) const { return (bin < priorCorrectionValue_.size() ? priorCorrectionValue_[bin] : 0.0); }
      inline double GetPriorCorrection(size_t bin) const { return (bin < priorCorrectionValue_.size() ? priorCorrectionValue_[bin] : 0.0)+basePriorCorrection_; }
      void ReadWeightsLLH(std::string);
      inline double GetWeightLLH(size_t abin) const { return (abin < weightLLH_.size() ? weightLLH_[abin] : 1.0); }
      //double EvalPriorCorrection(size_t) const;
      //void SetPriorCorrection(std::string);

      inline void SetComputeBufferFitnesses(bool val) { computeBufferFitnesses_ = val; }
      inline bool GetComputeBufferFitnesses() const { return computeBufferFitnesses_; }

      inline void SetBuffered() { bufferedMode_ = true; }
      inline void UnsetBuffered() { bufferedMode_ = false; bufferSize_ = points_.size(); }
      inline bool IsBuffered() const { return bufferedMode_; }

      void ClearData();

    protected:
      void AddDataPoint(double, double, double, double, double = 0.0);
      void AddDataPoint(std::vector<std::pair<double,double> >&, std::vector<double>&, double, double = 0.0);

      std::vector<double> startCountsOn_;
      std::vector<double> startCountsOff_;

    private:

      double GetFitness(unsigned int, unsigned int) const;
      void ResetCache();
      void AddToCache(double,double,double,double,double = 0.0);
      void AddToCache(std::vector<std::pair<double,double> >&,std::vector<double>&,double,double = 0.0);

    protected:

      std::deque<DataPoint*> points_;

    private:

      std::deque<std::deque<double> > blockFitness_;
      double gamma_;
      double prior_;
      bool multipriors_;
      unsigned int bufferSize_;
      unsigned int shifts_;
      unsigned int rebinning_;
      unsigned int binctr_;
      double mjdJumpLimit_;

      // for prior correction
      //TF1* priorCorrection_;
      std::vector<double> priorCorrectionValue_;
      bool computeBufferFitnesses_;
      double basePriorCorrection_;

      // for weights
      std::vector<double> weightLLH_;

      // for running multiple priors at once
      std::vector<double> priors_;

      // true if a new block has been added since the last Optimize() call
      bool newBlockComplete_;

      std::vector<std::pair<double,double> > cache_;
      std::vector<double> cacheAlpha_;
      double cacheMJD_;
      double cacheAverageMJD_;
      double cacheAverageZenith_;
      int cacheCount_;
      double cacheNon_;
      double cacheNoff_;

      std::string keyword_;

      // true if we should ignore completely zero data points
      bool ignoreZero_;

      // run in buffered mode?
      bool bufferedMode_;

  };


  /*!
   * @class ExcessRatioBB
   * @author Tom Weisgarber
   * @date 29 April 2014
   * @ingroup math
   * @brief Modified version of the Bayesian block algorithm
   *
   * Child class of BayesianBuffer using excess as fitness.
   */

  class ExcessRatioBB : public BayesianBuffer {

    public:

      ExcessRatioBB(double, unsigned int, unsigned int, std::vector<double>&, bool = true, double = -1.0,std::vector<double>* = NULL,double = 0.0,bool = false);
      ~ExcessRatioBB();

    public:

      double Fitness(unsigned int,unsigned int) const;

    private:

      std::vector<double> alpha_;

  };


  /*!
   * @class RatioBB
   * @author Tom Weisgarber
   * @date 22 May 2014
   * @ingroup math
   * @brief Modified version of the Bayesian block algorithm
   *
   * Child class of BayesianBuffer using ratio of Non to alpha*Noff.
   */

  class RatioBB : public BayesianBuffer {

    public:

      RatioBB(double, unsigned int, unsigned int, std::vector<double>&, bool = true,double = -1.0,std::vector<double>* = NULL,double = 0.0,bool = false);
      ~RatioBB();

    public:

      double Fitness(unsigned int,unsigned int) const;

    private:

      std::vector<double> alpha_;

  };

  class SingleRatioBB : public BayesianBuffer {
    public:
      SingleRatioBB(double,unsigned int,unsigned int,std::vector<double>&,bool = true,double = -1.0,std::vector<double>* = NULL,bool = false,size_t = 1,size_t = 0,double = 0.0,bool = false);
      ~SingleRatioBB() { }
    public:
      double Fitness(unsigned int,unsigned int) const { return 0.0; }
      inline bool IsSingleSearch() const { return true; }
      inline bool UseBlockFitness() const { return false; }
      void OptimizeSingle(std::vector<std::vector<unsigned int> >&,std::vector<double>* = NULL,std::vector<double>* = NULL,double* = NULL);
    private:
      std::vector<double> alpha_;
      bool rising_;
      size_t changePointMin_;
      size_t changePointMax_;
  };

  class TwoStepRatioBB : public RatioBB {
    public:
      TwoStepRatioBB(double,unsigned int,unsigned int,std::vector<double>&,bool = true,double = -1.0,std::vector<double>* = NULL,double = 0.0);
      ~TwoStepRatioBB() { }
      inline bool IsTriggeredSearch() const { return true; }
      void OptimizeTriggered(std::vector<std::vector<unsigned int> >&);
    private:
      double log_prob_;
  };

  class BelieveBackgroundBB : public BayesianBuffer {
    public:
      BelieveBackgroundBB(double,unsigned int,std::vector<double>&,double,double = -1.0,std::vector<double>* = NULL,double = 0.0,bool = false);
      ~BelieveBackgroundBB();
      //bool IsLastPointTrigger() const;
      inline bool IsTriggeredSearch() const { return true; }
      //void OptimizeTriggered(std::vector<unsigned int>&);
      void OptimizeTriggered(std::vector<std::vector<unsigned int> >&);

    public:
      double Fitness(unsigned int,unsigned int) const;

    private:
      double log_prob_;
      std::vector<double> alpha_;
      std::vector<double> src_factor_;
  };


  /*!
   * @class RatioScargleErrorBB
   * @author Tom Weisgarber
   * @date 23 June 2014
   * @ingroup math
   * @brief Modified version of the Bayesian block algorithm
   *
   * Child class of BayesianBuffer using ratio of Non to alpha*Noff
   * and computing the fitness via Scargle 2013 eq. ??
   */

  class RatioScargleErrorBB : public BayesianBuffer {

    public:
      RatioScargleErrorBB(double, unsigned int, unsigned int, bool = true,double = -1.0,std::vector<double>* = NULL,double = 0.0,bool = false);
      ~RatioScargleErrorBB();

    public:
      double Fitness(unsigned int,unsigned int) const;
  };

}

#endif // DATACLASSES_MATH_BAYESIANBLOCK_H_INCLUDED

