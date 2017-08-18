/*
 * @file bayes-test-quick.cc
 * @brief test lots of things quickly
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
#include <TH1F.h>
#include <TH1D.h>
#include <TF1.h>

#include <vector>
#include <string>
#include <iostream>

int main(int argc, char* argv[])
{
  CommandLineConfigurator cl;
  cl.AddOption<double>("onmean,n",100.0,"Mean of on counts");
  cl.AddOption<double>("offmean,m",100.0,"Mean of off counts");
  cl.AddOption<int>("runs,r",1000,"Number of runs");
  cl.AddOption<int>("bins,b",500,"Number of bins for fit");
  cl.AddOption<std::string>("root,R","test-bayes-test-quick.root","Output ROOT file");
  cl.AddOption<int>("seed",0,"Random number seed");

  if (!cl.ParseCommandLine(argc, argv)) return 1;
  
  // Configure services and modules
  HAWCNest nest;
  nest.Service<StdRNGService>("rng")
    ("seed", cl.GetArgument<int>("seed"));
  nest.Configure();
  const RNGService& rng = GetService<RNGService>("rng");

  std::vector<std::pair<double,double> > chi1;
  chi1.push_back(std::make_pair(-log(1.0e-5),2.0));
  chi1.push_back(std::make_pair(-log(1.0e-5),3.0));
  chi1.push_back(std::make_pair(-log(1.0e-5),4.0));
  chi1.push_back(std::make_pair(-log(1.0e-5),8.0));
  chi1.push_back(std::make_pair(0.1,3.0));
  chi1.push_back(std::make_pair(1.0e-2,3.0));
  chi1.push_back(std::make_pair(1.0e-3,3.0));
  chi1.push_back(std::make_pair(1.0e-5,3.0));
  chi1.push_back(std::make_pair(1.0,3.0));
  chi1.push_back(std::make_pair(10.0,3.0));
  chi1.push_back(std::make_pair(1.0e2,3.0));
  chi1.push_back(std::make_pair(1.0e4,3.0));
  chi1.push_back(std::make_pair(1.0e-50,3.0));
  chi1.push_back(std::make_pair(1.0e50,3.0));
  std::cout << "Conversions:" << std::endl;
  log_info("Conversions:");
  for(size_t i=0; i<chi1.size(); i++) {
    double val = BayesianBuffer::convert_chi2_value(chi1[i].first,chi1[i].second,1.0);
    log_info("  " << chi1[i].second << " -> 1:   " << chi1[i].first << " -> " << val << "    check: " << SpecialFunctions::Gamma::P(0.5,0.5*val) << " " << SpecialFunctions::Gamma::P(0.5*chi1[i].second,0.5*chi1[i].first));
  }
  for(size_t i=0; i<chi1.size(); i++) {
    double val = BayesianBuffer::convert_chi2_value(chi1[i].first,1.0,chi1[i].second);
    log_info("  1 -> " << chi1[i].second << ":   " << chi1[i].first << " -> " << val << "    check: " << SpecialFunctions::Gamma::P(0.5*chi1[i].second,0.5*val) << " " << SpecialFunctions::Gamma::P(0.5,0.5*chi1[i].first));
  }
  for(size_t i=0; i<chi1.size(); i++) {
    double val = BayesianBuffer::convert_chi2_value(chi1[i].first,2.0,chi1[i].second+1.0);
    log_info("  2 -> " << (chi1[i].second+1.0) << ":   " << chi1[i].first << " -> " << val << "    check: " << SpecialFunctions::Gamma::P(0.5*(chi1[i].second+1.0),0.5*val) << " " << SpecialFunctions::Gamma::P(1.0,0.5*chi1[i].first));
  }
  for(size_t i=0; i<chi1.size(); i++) {
    double val = BayesianBuffer::convert_chi2_value(chi1[i].first,2.0,chi1[i].second);
    log_info("  2 -> " << (chi1[i].second) << ":   " << chi1[i].first << " -> " << val << "    check: " << SpecialFunctions::Gamma::P(0.5*(chi1[i].second),0.5*val) << " " << SpecialFunctions::Gamma::P(1.0,0.5*chi1[i].first));
  }

  // get the arguments
  double nu = cl.GetArgument<double>("onmean");
  double mu = cl.GetArgument<double>("offmean");
  int runs = cl.GetArgument<int>("runs");
  int bins = cl.GetArgument<int>("bins");
  std::string filename = cl.GetArgument<std::string>("root");

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
  double delta_nll = 0.0;
  double n1 = 0.0;
  double n2 = 0.0;
  double m1 = 0.0;
  double m2 = 0.0;
  tree->Branch("delta_nll",&delta_nll,"delta_nll/D");
  tree->Branch("n1",&n1,"n1/D");
  tree->Branch("n2",&n2,"n2/D");
  tree->Branch("m1",&m1,"m1/D");
  tree->Branch("m2",&m2,"m2/D");

  // run the tests
  for(int i=0; i<runs; ++i) {
    n1 = rng.Poisson(nu);
    n2 = rng.Poisson(nu);
    m1 = rng.Poisson(mu);
    m2 = rng.Poisson(mu);
    double sum = n1+n2+m1+m2;
    double q1 = (n1+m1)*(n1+n2)/(n1*sum);
    double q2 = (n1+m1)*(m1+m2)/(m1*sum);
    double q3 = (n2+m2)*(n1+n2)/(n2*sum);
    double q4 = (n2+m2)*(m1+m2)/(m2*sum);
    delta_nll = -(n1*log(q1)+m1*log(q2)+n2*log(q3)+m2*log(q4));
    tree->Fill();
  }

  // report
  double max = 2.0*tree->GetMaximum("delta_nll");
  std::cout << "Max: " << max << std::endl;
  TH1D* hfit = new TH1D("hfit","",bins,0.0,max);
  tree->Draw("2.0*delta_nll>>hfit");
  std::cout << "Mean: " << hfit->GetMean() << "  RMS: " << hfit->GetRMS() << std::endl;
  std::cout << "Fitting with degrees of freedom free" << std::endl;
  TF1* chi = new TF1("chi","[0]*pow(x,0.5*[1]-1.0)*exp(-0.5*x)",0.0,max);
  hfit->Fit(chi);
  std::cout << "Fitting with degrees of freedom fixed to 1" << std::endl;
  chi->FixParameter(1,1.0);
  hfit->Fit(chi);

  // clean up
  tree->Write("data",TObject::kOverwrite);
  tree->SetDirectory(0);
  delete tree;
  file->Close();
  delete file;

  return 0;
}

