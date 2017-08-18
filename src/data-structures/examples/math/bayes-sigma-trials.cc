/*
 * @file bayes-sigma-trials.cc
 * @brief test significance calculations
 * @author Tom Weisgarber
 * @date 11 May 2015
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
#include <TMath.h>

#include <vector>
#include <string>

// get the inverse of the error function on 1-p, accurate for small p
double inverse_erf_one_minus_arg(double p) {
  if(p > 1.0e-4) return TMath::ErfInverse(1.0-p);
  static const double p1 = log(2.0/HAWCUnits::pi);
  double p2 = p1-2.0*log(p);
  return sqrt(0.5*p2-0.5*log(p2));
}

int main(int argc, char* argv[])
{
  CommandLineConfigurator cl;
  cl.AddOption<double>("alpha,a",0.1,"Alpha parameter");
  cl.AddOption<int>("bins,b",10,"Number of bins");
  cl.AddOption<int>("non,n",100,"Number of on events per bin");
  cl.AddOption<double>("sigma,s",5.0,"Approximate significance in one bin");
  cl.AddOption<int>("runs,r",1000,"Number of runs");
  cl.AddOption<int>("seed",0,"Random number seed");
  cl.AddOption<std::string>("root,R","test-bayes-sigma-trials.root","Output ROOT file");

  if (!cl.ParseCommandLine(argc, argv)) return 1;
  
  // Configure services and modules
  HAWCNest nest;
  nest.Service<StdRNGService>("rng")
    ("seed", cl.GetArgument<int>("seed"));
  nest.Configure();
  const RNGService& rng = GetService<RNGService>("rng");

  // get the arguments
  double alpha = cl.GetArgument<double>("alpha");
  int bins = cl.GetArgument<int>("bins");
  double non = (double)cl.GetArgument<int>("non");
  double sigma = cl.GetArgument<double>("sigma");
  int runs = cl.GetArgument<int>("runs");
  std::string filename = cl.GetArgument<std::string>("root");

  double noff = non/alpha;
  double nsig = non+sigma*sqrt(non);
  double dbins = (double)bins;
  double pmin = 1.0e-3/dbins;

  // set up the ROOT file and write the command line to it
  TFile* file = new TFile(filename.c_str(),"RECREATE");
  std::string full_command_line = argv[0];
  for(int i=1; i<argc; i++) full_command_line = full_command_line+" "+std::string(argv[i]);
  std::cout << "Command line: " << full_command_line << std::endl;
  TNamed* command_line = new TNamed("command line",full_command_line.c_str());
  command_line->Write("command_line",TObject::kOverwrite);
  delete command_line;

  // make a tree to hold information about the data points
  TTree* tree = new TTree("data","data");
  double sigma_best = 0.0;
  double sigma_trials = 0.0;
  double sigma_mult = 0.0;
  tree->Branch("sigma_best",&sigma_best,"sigma_best/D");
  tree->Branch("sigma_trials",&sigma_trials,"sigma_trials/D");
  tree->Branch("sigma_mult",&sigma_mult,"sigma_mult/D");

  // set up the multi-bin calculation
  std::vector<std::pair<double,double> > obs;
  for(int i=0; i<bins; i++) obs.push_back(std::make_pair(0.0,0.0));

  // get the sigmas
  double prob = 0.0;
  double ts = 0.0;
  for(int i=0; i<runs; i++) {
    double best_ts = 0.0;
    int best_idx = 0;
    for(int j=0; j<bins; j++) {
      obs[j].second = rng.Poisson(noff);
      obs[j].first = rng.Poisson((j == 0 ? nsig : non));
      double test_ts = BayesianBuffer::get_li_ma_statistic_contribution(obs[j].first,obs[j].second,alpha);
      if(j == 0 || test_ts > best_ts) {
        best_ts = test_ts;
        best_idx = j;
      }
    }
    // multi-bin significance
    BayesianBuffer::get_significance(obs,alpha,prob,ts,sigma_mult,true);
    // best possible significance
    ts = BayesianBuffer::get_li_ma_statistic_contribution(obs[best_idx].first,obs[best_idx].second,alpha);
    BayesianBuffer::interpret_ts_value(ts,1.0,obs[best_idx].first-alpha*obs[best_idx].second,prob,sigma_best,true);
    // correct for trials
    prob = (prob < pmin ? dbins*prob : 1.0-pow(1.0-prob,dbins));
    ts = 2.0*pow(inverse_erf_one_minus_arg(prob),2.0);
    BayesianBuffer::interpret_ts_value(ts,1.0,obs[best_idx].first-alpha*obs[best_idx].second,prob,sigma_trials,true);
    tree->Fill();
  }

  // clean up
  tree->Write("data",TObject::kOverwrite);
  tree->SetDirectory(0);
  delete tree;
  file->Close();
  delete file;

  return 0;
}

