/*
 * @file bayes-root.cc
 * @brief for testing BayesianBuffers and plotting results in a ROOT file
 * @author Tom Weisgarber
 * @date 11 June 2014
 */

#include <hawcnest/HAWCNest.h>
#include <hawcnest/HAWCUnits.h>
#include <hawcnest/Logging.h>
#include <hawcnest/CommandLineConfigurator.h>

#include <rng-service/StdRNGService.h>

#include <data-structures/math/BayesianBuffer.h>
#include <data-structures/math/SpecialFunctions.h>

#include <TFile.h>
#include <TNamed.h>
#include <TTree.h>

#include <vector>
#include <deque>

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
//  double prob = SpecialFunctions::Beta::I((double)(k+1),(double)(n-k+1),high)-SpecialFunctions::Beta::I((double)(k+1),(double)(n-k+1),low);
  double prob = get_beta_difference(k+1,n-k+1,low,high);
  while(abs(prob-conf) > 1.0e-6 && lhigh-llow > 1.0e-14) {
    high = match_low_guess(low,(double)k,(double)n);
    //prob = SpecialFunctions::Beta::I((double)(k+1),(double)(n-k+1),high)-SpecialFunctions::Beta::I((double)(k+1),(double)(n-k+1),low);
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
  cl.AddOption<int>("binning,B", 1, "Rebinning");
  cl.AddOption<double>("alpha,a", 0.1, "Alpha value");
  cl.AddOption<double>("signal,s", 0.0, "Average signal in source region");
  cl.AddOption<double>("background,b", 200.0, "Average background in source region");
  cl.AddOption<double>("prior,p", 1.0e-5, "Prior parameter");
  cl.AddOption<double>("timestep,t", 1.0, "Time step in minutes");
  cl.AddOption<std::string>("method,m", "standard", "Method of running");
  cl.AddOption<std::string>("root,R", "test-bayes.root", "Output ROOT file");
  cl.AddOption<double>("conf,c", 0.68, "Confidence level for limits");
  cl.AddOption<int>("period,T", -1, "Period of oscillation in timesteps (negative for no oscillation)");
  cl.AddOption<double>("cospower,C", 1.0, "Power of cosine for oscillation");
  cl.AddOption<int>("Tsource,P", -1, "Average time in between sources turning on (in timesteps)");
  cl.AddOption<int>("duration,d", -1, "Average length of time a source is on for (in timesteps)");
  cl.AddFlag("report", "Make a detailed change point report");
  cl.AddFlag("diffs", "Report differences between optimum partition fitness and 1-block partition fitness");
  cl.AddFlag("basediff", "Report fitness differences for 0 vs. 1 change point");
  cl.AddFlag("basefixed", "Fix first change point for basediff option");
  cl.AddOption<int>("seed", 4537, "RNG seed");

  if (!cl.ParseCommandLine(argc, argv)) return 1;

  // parse the arguments
  int runs = cl.GetArgument<int>("runs");
  unsigned int buffer = (unsigned int)cl.GetArgument<int>("lookback");
  double alpha = cl.GetArgument<double>("alpha");
  double sig_avg = cl.GetArgument<double>("signal");
  double bkg_avg = cl.GetArgument<double>("background");
  double gamma = cl.GetArgument<double>("prior");
  int binning = cl.GetArgument<int>("binning");
  double tDelta = cl.GetArgument<double>("timestep") * HAWCUnits::minute;
  double period = (double)(cl.GetArgument<int>("period")) * tDelta;
  double omega = (period > 0.0 ? HAWCUnits::twopi/period : 0.0);
  double duration = (double)(cl.GetArgument<int>("duration")) * tDelta;
  double source_period = (double)(cl.GetArgument<int>("Tsource")) * tDelta;
  double power = cl.GetArgument<double>("cospower");
  if(binning < 1) {
    std::cerr << "Warning! Changing your ridiculous rebinning of " << binning << " to 1" << std::endl;
    binning = 1;
  }
  unsigned int rebinning = (unsigned int)binning;
  std::string method = cl.GetArgument<std::string>("method");

  // set up the ROOT file and write the command line to it
  TFile* file = new TFile(cl.GetArgument<std::string>("root").c_str(),"RECREATE");
  std::string full_command_line = argv[0];
  for(int i=1; i<argc; i++) full_command_line = full_command_line+" "+std::string(argv[i]);
  std::cout << "Command line: " << full_command_line << std::endl;
  TNamed* command_line = new TNamed("command line",full_command_line.c_str());
  command_line->Write("command_line",TObject::kOverwrite);
  delete command_line;

  // make a tree to hold information about the data points
  TTree* data_tree = new TTree("data","data");
  double tFraction = 0.0;
  double tAlpha = 0.0;
  double tNon = 0.0;
  double tNoff = 0.0;
  double tMJD = 0.0;
  double tSigma = 0.0;
  double tLnPrior = log(gamma);
  double tSourceAvg = 0.0;
  double tBackAvg = 0.0;
  double tNoffAvg = 0.0;
  double tWeight = 0.0;
  data_tree->Branch("fraction",&tFraction,"fraction/D");
  data_tree->Branch("alpha",&tAlpha,"alpha/D");
  data_tree->Branch("Non",&tNon,"Non/D");
  data_tree->Branch("Noff",&tNoff,"Noff/D");
  data_tree->Branch("MJD",&tMJD,"MJD/D");
  data_tree->Branch("sigma",&tSigma,"sigma/D");
  data_tree->Branch("lnprior",&tLnPrior,"lnprior/D");
  data_tree->Branch("srcavg",&tSourceAvg,"srcavg/D");
  data_tree->Branch("bkgavg",&tBackAvg,"bkgavg/D");
  data_tree->Branch("noffavg",&tNoffAvg,"noffavg/D");
  data_tree->Branch("weight",&tWeight,"weight/D");
  
  // Configure services and modules
  HAWCNest nest;
  nest.Service<StdRNGService>("rng")
    ("seed", cl.GetArgument<int>("seed"));
  nest.Configure();

  // Set up a Bayesian buffer
  std::vector<double> binAlpha;
  binAlpha.push_back(alpha);
  BayesianBuffer::BayesianBuffer* bb;
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
  //double non_avg = bkg_avg + sig_avg;
  std::map<unsigned int,double> changePointMap;
  std::map<unsigned int,std::vector<double> > changePointDiffMap;
  const RNGService& rng = GetService<RNGService>("rng");

  // start us at MJD = 56000
  double T0 = 56000.0 * HAWCUnits::day;
  double t = T0;
  double frac = 1.0/((double)buffer);

  // keep track of the number of times we look for change points
  int searches = 0;
  int independents = 0;

  // get the probability that a source turns on at the given point
  double source_on_prob = tDelta/source_period;
  double source_off_prob = tDelta/duration;
  unsigned int source_on_count = 0;
  std::cout << "Source probabilities: ON: " << source_on_prob << "  OFF: " << source_off_prob << std::endl;

  // deques to hold information we want to record later
  std::deque<double> source_average;
  std::deque<double> back_average;
  std::deque<double> noff_average;
  std::deque<double> weight_value;

  // run, collecting change points and the fraction of the time they are identified while in the buffer
  unsigned int buffer_flush = buffer;
  for(int i = 0; i < runs; ++i) {
    t += tDelta;
    double src_avg = sig_avg;
    if(duration > 0.0 && source_period > 0.0) {
      if(rng.Uniform() < source_on_prob) source_on_count++;
      unsigned int loops = source_on_count;
      for(unsigned int j=0; j<loops; j++) {
        if(rng.Uniform() < source_off_prob) source_on_count--;
      }
      src_avg = sig_avg*((double)source_on_count);
    }
    // sample the events
    double weight = (omega > 0.0 ? cos(omega*(t-T0)) : 1.0);
    if(weight < 0.0) weight = 0.0; // can't have negative average numbers of events!
    if(power != 1.0 && weight != 0.0) weight = pow(weight,power);
    source_average.push_back(src_avg);
    back_average.push_back(bkg_avg);
    noff_average.push_back(noff_avg);
    weight_value.push_back(weight);
    if(source_average.size() > buffer) source_average.pop_front();
    if(back_average.size() > buffer) back_average.pop_front();
    if(noff_average.size() > buffer) noff_average.pop_front();
    if(weight_value.size() > buffer) weight_value.pop_front();
    bb->AddPoint(rng.Poisson((bkg_avg+src_avg)*weight),rng.Poisson(noff_avg*weight),binAlpha[0],t / HAWCUnits::day);
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
      if(bb->IsBufferFull()) {
        unsigned int point = bb->GetPointIndex(0);
        tFraction = (changePointMap.find(point) == changePointMap.end() ? 0.0 : changePointMap[point]);
        tAlpha = bb->GetPointAlpha(0);
        tNon = bb->GetPointNon(0);
        tNoff = bb->GetPointNoff(0);
        tMJD = bb->GetPointMJD(0);
        tSigma = bb->GetPointLiMaSig(0);
        tSourceAvg = source_average[0];
        tBackAvg = back_average[0];
        tNoffAvg = noff_average[0];
        tWeight = weight_value[0];
        data_tree->Fill();
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

  // clean up the ROOT file
  data_tree->Write("data",TObject::kOverwrite);
  data_tree->SetDirectory(0);
  delete data_tree;
  file->Close();
  delete file;
  
  return 0;
}

