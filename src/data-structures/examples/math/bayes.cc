/*
 * @file bayes.cc
 * @brief example use of BayesianBuffer object
 * @author Tom Weisgarber
 * @date 28 Apr 2014
 * @version $Id: bayes.cc 35081 2016-10-10 12:59:42Z lukas $
 */

#include <hawcnest/HAWCNest.h>
#include <hawcnest/HAWCUnits.h>
#include <hawcnest/Logging.h>
#include <hawcnest/CommandLineConfigurator.h>

#include <rng-service/StdRNGService.h>

#include <data-structures/math/BayesianBuffer.h>
#include <data-structures/math/SpecialFunctions.h>

#include <vector>

using std::abs;

double match_low_guess(double guess_low,double k,double n) {
  double mode = k/n;
  double llmatch = k*log(guess_low)+(n-k)*log(1.0-guess_low);
  double hlow = mode;
  double hhigh = 1.0;
  double hmid = 0.5*(hhigh+hlow);
  double lhmatch = k*log(hmid)+(n-k)*log(1.0-hmid);
  while(abs(lhmatch-llmatch) > 1.0e-8 && hhigh-hlow > 1.0e-14) {
    lhmatch = k*log(hmid)+(n-k)*log(1.0-hmid);
    if(lhmatch > llmatch) hlow = hmid;
    else if(lhmatch < llmatch) hhigh = hmid;
    else return hmid;
    hmid = 0.5*(hhigh+hlow);
  }
  return hmid;
}

double get_beta_difference(unsigned int n,unsigned int m,double x1,double x2) {
  double g1 = SpecialFunctions::Gamma::lnG((double)(n+m))-SpecialFunctions::Gamma::lnG((double)n);
  double sum = 0.0;
  for(unsigned int i=0; i<m; i++) {
    unsigned int j = m-i-1;
    double dnpj = (double)(n+j);
    double g = exp(g1-SpecialFunctions::Gamma::lnG((double)(j+1))-SpecialFunctions::Gamma::lnG((double)(m-j)));
    double t = pow(-1.0,(double)j)/dnpj;
    sum += g*t*(pow(x2,dnpj)-pow(x1,dnpj));
  }
  return sum;
}

void get_limits(int k,int n,double conf,double& low,double& high) {
  if(n == 0) {
    low = 0.5-1.0/12.0;
    high = 0.5+1.0/12.0;
    return;
  } else if(k == 0) {
    double power = 1.0/(1.0+(double)n);
    low = 0.0;
    high = 1.0-pow(1.0-conf,power);
    return;
  } else if(k == n) {
    double power = 1.0/(1.0+(double)n);
    low = pow(1.0-conf,power);
    high = 1.0;
    return;
  }
  double mode = ((double)k)/((double)n);
  double llow = 0.0;
  double lhigh = mode;
  low = 0.5*(lhigh+llow);
  high = match_low_guess(low,(double)k,(double)n);
  double prob = get_beta_difference(k+1,n-k+1,low,high);
  while(abs(prob-conf) > 1.0e-6 && lhigh-llow > 1.0e-14) {
    high = match_low_guess(low,(double)k,(double)n);
    prob = get_beta_difference(k+1,n-k+1,low,high);
    if(prob > conf) llow = low;
    else if(prob < conf) lhigh = low;
    else return;
    low = 0.5*(lhigh+llow);
  }
  return;
}

int main(int argc, char* argv[])
{
  CommandLineConfigurator cl;
  cl.AddOption<int>("runs,r", 3000, "Number of data points to run");
  cl.AddOption<int>("lookback,l", 300, "Lookback buffer size");
  cl.AddOption<int>("binning", 1, "Rebinning");
  cl.AddOption<double>("alpha,a", 0.1, "Alpha value");
  cl.AddOption<double>("signal,s", 0.0, "Average signal in source region");
  cl.AddOption<double>("background,b", 200.0, "Average background in source region");
  cl.AddOption<double>("prior,p", 1.0e-5, "Prior parameter");
  cl.AddOption<std::string>("method,m", "standard", "Method of running");
  cl.AddOption<double>("conf,c", 0.68, "Confidence level for limits");
  cl.AddFlag("report", "Make a detailed change point report");
  cl.AddFlag("diffs", "Report differences between optimum partition fitness and 1-block partition fitness");
  cl.AddFlag("lima", "Report Li-Ma significance of each added block");
  cl.AddFlag("basediff", "Report fitness differences for 0 vs. 1 change point");
  cl.AddFlag("basefixed", "Fix first change point for basediff option");
  cl.AddOption<int>("seed", 4537, "RNG seed");

  if (!cl.ParseCommandLine(argc, argv))
    return 1;

  int runs = cl.GetArgument<int>("runs");
  unsigned int buffer = (unsigned int)cl.GetArgument<int>("lookback");
  double alpha = cl.GetArgument<double>("alpha");
  double sig_avg = cl.GetArgument<double>("signal");
  double bkg_avg = cl.GetArgument<double>("background");
  double gamma = cl.GetArgument<double>("prior");
  int binning = cl.GetArgument<int>("binning");
  if(binning < 1) {
    std::cerr << "Warning! Changing your ridiculous rebinning of " << binning << " to 1" << std::endl;
    binning = 1;
  }
  unsigned int rebinning = (unsigned int)binning;
  std::string method = cl.GetArgument<std::string>("method");
  
  // Configure services and modules
  HAWCNest nest;
  nest.Service<StdRNGService>("rng")
    ("seed", cl.GetArgument<int>("seed"));
  nest.Configure();

  // Set up a Bayesian buffer
  BayesianBuffer::BayesianBuffer* bb;
  //BayesianBuffer::BayesianBuffer* bb = new BayesianBuffer::BayesianBuffer(gamma,buffer);
  std::vector<double> binAlpha;
  binAlpha.push_back(alpha);
  if(method == "signal") {
    bb = new BayesianBuffer::BayesianBuffer(gamma,buffer,rebinning);
  } else if(method == "excess") {
    bb = new BayesianBuffer::ExcessRatioBB(gamma,buffer,rebinning,binAlpha);
  } else if(method == "scargle") {
    bb = new BayesianBuffer::RatioScargleErrorBB(gamma,buffer,rebinning);
  } else {
    bb = new BayesianBuffer::RatioBB(gamma,buffer,rebinning,binAlpha);
  }
  double noff_avg = bkg_avg / alpha;
  double non_avg = bkg_avg + sig_avg;
  std::map<unsigned int,double> changePointMap;
  std::map<unsigned int,std::vector<double> > changePointDiffMap;
  const RNGService& rng = GetService<RNGService>("rng");

  // start us at MJD = 56000
  double t = 56000.0;
  double tDelta = HAWCUnits::minute / HAWCUnits::day;
  double frac = 1.0/((double)buffer);

  // keep track of the number of times we look for change points
  int searches = 0;
  int independents = 0;

  // run, collecting change points and the fraction of the time they are identified while in the buffer
  unsigned int buffer_flush = buffer;
  for(int i = 0; i < runs; ++i) {
    t += tDelta;
    bb->AddPoint(rng.Poisson(non_avg),rng.Poisson(noff_avg),binAlpha[0],t);
    if(cl.HasFlag("lima")) std::cout << "LiMa: " << bb->GetLastLiMaSig() << std::endl;
    std::vector<std::vector<unsigned int> > changes;
    if(bb->IsNewBlockComplete()) {
      if(buffer_flush > 0) buffer_flush--;
      searches++;
      double diff = bb->Optimize(changes);
      for(unsigned int j = 0; j < changes[0].size(); ++j) {
        if(changePointMap.find(changes[0][j]) == changePointMap.end())
          changePointMap.insert(std::make_pair(changes[0][j],0.0));
        changePointMap[changes[0][j]] = changePointMap[changes[0][j]] + frac;
      }
      // if we record a diff, make sure the next one is totally independent
      if(buffer_flush == 0) {
        if(cl.HasFlag("basediff")) std::cout << bb->GetFitnessDiff0to1(cl.HasFlag("basefixed")) << std::endl;
        if(changes[0].size() > 0) {
          if(changePointDiffMap.find(changes[0].size()) == changePointDiffMap.end())
            changePointDiffMap.insert(std::make_pair(changes[0].size(),std::vector<double>()));
          changePointDiffMap[changes[0].size()].push_back(diff);
          buffer_flush = buffer;
          independents++;
        }
      }
    }
  }

  // do some reporting
  log_info("Detected " << changePointMap.size() << " change point" << (changePointMap.size() == 1 ? "" : "s") << " (" << independents << " independent)");
  double low = 0.0;
  double high = 0.0;
  get_limits((int)changePointMap.size(),searches,cl.GetArgument<double>("conf"),low,high);
  log_info("Searches, change points, low limit, high limit: " << searches << " " << changePointMap.size() << " " << low << " " << high);
  if(cl.HasFlag("report")) {
    for(std::map<unsigned int,double>::iterator it = changePointMap.begin(); it != changePointMap.end(); it++) {
      log_info("  Change point " << (*it).first << " fraction: " << (*it).second);
    }
  }
  if(cl.HasFlag("diffs")) {
    for(std::map<unsigned int,std::vector<double> >::iterator it = changePointDiffMap.begin(); it != changePointDiffMap.end(); it++) {
      for(unsigned int i=0; i<(*it).second.size(); i++)
        std::cout << (*it).first << " " << (*it).second[i] << std::endl;
    }
  }
  
  return 0;
}

