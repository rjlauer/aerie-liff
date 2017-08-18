/*
 * @file bayes-sigma.cc
 * @brief test significance calculations
 * @author Tom Weisgarber
 * @date 7 May 2015
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
#include <string>

int main(int argc, char* argv[])
{
  CommandLineConfigurator cl;
  cl.AddOption<double>("alpha,a",0.1,"Alpha parameter");
  cl.AddOption<double>("frac,f",0.1,"Excess fraction");
  cl.AddOption<double>("bkg,b",10.0,"Background rate");
  cl.AddOption<int>("steps,s",1000,"Number of steps");
  cl.AddOption<std::string>("root,r","test-bayes-sigma.root","Output ROOT file");

  if (!cl.ParseCommandLine(argc, argv))
    return 1;

  // get the arguments
  int steps = cl.GetArgument<int>("steps");
  double alpha = cl.GetArgument<double>("alpha");
  double fraction = cl.GetArgument<double>("frac");
  double bkg = cl.GetArgument<double>("bkg");
  std::string filename = cl.GetArgument<std::string>("root");
  double sig = (1.0+fraction)*alpha*bkg;

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
  double non = 0.0;
  double noff = 0.0;
  double sigma = 0.0;
  double sigma_mult = 0.0;
  double sigma_old = 0.0;
  double sigma_mult3 = 0.0;
  double sigma_old3 = 0.0;
  double prob = 0.0;
  double prob_old = 0.0;
  double prob3 = 0.0;
  double prob_old3 = 0.0;
  tree->Branch("non",&non,"non/D");
  tree->Branch("noff",&noff,"noff/D");
  tree->Branch("alpha",&alpha,"alpha/D");
  tree->Branch("sigma",&sigma,"sigma/D");
  tree->Branch("sigma_mult",&sigma_mult,"sigma_mult/D");
  tree->Branch("sigma_old",&sigma_old,"sigma_old/D");
  tree->Branch("sigma_mult3",&sigma_mult3,"sigma_mult3/D");
  tree->Branch("sigma_old3",&sigma_old3,"sigma_old3/D");
  tree->Branch("prob",&prob,"prob/D");
  tree->Branch("prob_old",&prob_old,"prob_old/D");
  tree->Branch("prob3",&prob3,"prob3/D");
  tree->Branch("prob_old3",&prob_old3,"prob_old3/D");

  // set up the multi-bin calculation
  std::vector<std::pair<double,double> > obs;
  obs.push_back(std::make_pair(non,noff));
  double ts = 0.0;

  // set up a second multi-bin calculation
  std::vector<std::pair<double,double> > obs3;
  obs3.push_back(std::make_pair(non,noff));
  obs3.push_back(std::make_pair(non,noff));
  obs3.push_back(std::make_pair(non,noff));
  std::vector<double> alphas;
  alphas.push_back(alpha);
  alphas.push_back(1.1*alpha);
  alphas.push_back(0.9*alpha);
  std::vector<double> nons;
  nons.push_back(0.0);
  nons.push_back(0.0);
  nons.push_back(0.0);

  // get the sigmas
  for(int i=0; i<steps; i++) {
    sigma = BayesianBuffer::get_sigma(non,noff,alpha);
    obs[0].first = non;
    obs[0].second = noff;
    BayesianBuffer::get_significance(obs,alpha,prob,ts,sigma_mult,true);
    BayesianBuffer::get_significance(obs,alpha,prob_old,ts,sigma_old,false);
    obs3[0].first = nons[0];
    obs3[0].second = noff;
    obs3[1].first = nons[1];
    obs3[1].second = noff;
    obs3[2].first = nons[2];
    obs3[2].second = noff;
    BayesianBuffer::get_significance(obs3,alphas,prob3,ts,sigma_mult3,true);
    BayesianBuffer::get_significance(obs3,alphas,prob_old3,ts,sigma_old3,false);
    tree->Fill();
    non += sig;
    noff += bkg;
    nons[0] = nons[0]+(1.0+fraction)*alphas[0]*bkg;
    nons[1] = nons[1]+(1.0+fraction)*alphas[1]*bkg;
    nons[2] = nons[2]+(1.0+fraction)*alphas[2]*bkg;
  }

  // clean up
  tree->Write("data",TObject::kOverwrite);
  tree->SetDirectory(0);
  delete tree;
  file->Close();
  delete file;

  return 0;
}

