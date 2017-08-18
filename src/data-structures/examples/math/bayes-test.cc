/*
 * @file bayes-test.cc
 * @brief test lots of things
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
#include <TGraph.h>

#include <vector>
#include <string>
#include <iostream>

int main(int argc, char* argv[])
{
  CommandLineConfigurator cl;
  cl.AddOption<double>("alpha,a",0.1,"Alpha parameter");
  cl.AddOption<int>("bins,b",1,"Number of analysis bins");
  cl.AddOption<int>("tbins,t",1,"Number of time bins");
  //cl.AddOption<int>("t2bins,2",0,"Number of raised time bins");
  cl.AddOption<double>("non,n",10,"Number of on events per bin");
  cl.AddOption<double>("sigma,s",0.0,"Approximate significance in one bin");
  //cl.AddOption<double>("sigma2,S",0.0,"Approximate significance of raised time bins");
  cl.AddOption<int>("runs,r",1000,"Number of runs");
  cl.AddOption<int>("seed",0,"Random number seed");
  cl.AddOption<double>("period,T",-1,"Period of modulation (in bins)");
  cl.AddOption<double>("scale,S",1.0,"Scale from bin N to bin N+1");
  cl.AddOption<int>("hbins",200,"Number of fitting histogram bins");
  cl.AddOption<double>("chi2",-1.0,"Frozen chi2 value (set to number of analysis bins if <= 0)");
  cl.AddOption<std::string>("root,R","test-bayes-test.root","Output ROOT file");
  cl.AddOption<int>("samples",0,"Number of samples for samples histogram");
  cl.AddFlag("buffer","Run in buffered mode");
  cl.AddOption<int>("watch,w",-1,"Buffer point to watch (instead of searching for the best fitness)");
  cl.AddOption<int>("qcount,q",15,"Quantile count for computing trials plot");
  cl.AddFlag("rts","Use rate of TS increase as fitness");

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
  double non = cl.GetArgument<double>("non");
  double sigma_target = cl.GetArgument<double>("sigma");
  int runs = cl.GetArgument<int>("runs");
  int tbins = cl.GetArgument<int>("tbins");
  //int t2bins = cl.GetArgument<int>("t2bins");
  //double sigma2_target = cl.GetArgument<double>("sigma2");
  double period = cl.GetArgument<double>("period");
  double scale = cl.GetArgument<double>("scale");
  int hbins = cl.GetArgument<int>("hbins");
  double chi2param = cl.GetArgument<double>("chi2");
  std::string filename = cl.GetArgument<std::string>("root");
  int watch = cl.GetArgument<int>("watch");
  int qcount = cl.GetArgument<int>("qcount");
  if(qcount < 1) qcount = 1;

  bool is_buffered = cl.HasFlag("buffer");
  bool is_rts = cl.HasFlag("rts");

  // set up the rates
  double noff = non/alpha;
  double nsig = non+sigma_target*sqrt(non);
  //double nsig2 = non+sigma2_target*sqrt(non);
  double dbins = (double)bins;
  if(chi2param <= 0.0) chi2param = dbins;

  // set up the quantiles
  std::vector<double> qprobs;
  std::vector<double> quantiles;
  std::vector<double> pprobs;
  std::vector<double> qNs;
  qprobs.resize(qcount,0.0);
  quantiles.resize(qcount,0.0);
  pprobs.resize(qcount,0.0);
  qNs.resize(qcount,0.0);
  for(int i=0; i<qcount; i++) qprobs[i] = ((double)(i+1))/((double)(qcount+1));

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
  double fitness1 = 0.0;
  double fitness2 = 0.0;
  double fitness_total = 0.0;
  double sigma1 = 0.0;
  double sigma2 = 0.0;
  double sigma_total = 0.0;
  double on_mean = 0.0;
  double off_mean = 0.0;
  int best = 0;
  tree->Branch("fitness1",&fitness1,"fitness1/D");
  tree->Branch("fitness2",&fitness2,"fitness2/D");
  tree->Branch("fitness_total",&fitness_total,"fitness_total/D");
  tree->Branch("sigma1",&sigma1,"sigma1/D");
  tree->Branch("sigma2",&sigma2,"sigma2/D");
  tree->Branch("sigma_total",&sigma_total,"sigma_total/D");
  tree->Branch("on_mean",&on_mean,"on_mean/D");
  tree->Branch("off_mean",&off_mean,"off_mean/D");
  tree->Branch("best",&best,"best/I");

  // set up the multi-bin calculation
  std::vector<std::pair<double,double> > obs_total;
  std::vector<std::vector<std::vector<std::pair<double,double> > > > obs;
  std::vector<std::vector<double> > fitness;
  std::vector<std::vector<std::pair<double,double> > > buffer;
  std::vector<std::vector<std::pair<double,double> > > buffer_mean;
  std::vector<double> scales;
  scales.resize(bins,1.0);
  obs.resize(tbins);
  fitness.resize(tbins);
  for(int l=0; l<(tbins); l++) {
    fitness[l].resize(2,0.0);
    obs[l].resize(2);
    for(int j=0; j<bins; j++) {
      if(j > 0) scales[j] = scales[j-1]*scale;
      obs[l][0].push_back(std::make_pair(0.0,0.0));
      obs[l][1].push_back(std::make_pair(0.0,0.0));
    }
  }
  for(int j=0; j<bins; j++) obs_total.push_back(std::make_pair(0.0,0.0));
  buffer.resize(bins);
  buffer_mean.resize(bins);

  // fill the buffer with initial values
  for(int j=0; j<bins; j++) {
    buffer[j].resize(tbins);
    buffer_mean[j].resize(tbins);
    for(int k=0; k<tbins; k++) {
      double factor = (period > 0.0 ? cos(2.0*HAWCUnits::pi*(double)(k)/period) : 1.0)*scales[j];
      if(factor < 0.0) factor = 0.0;
      buffer_mean[j][k] = std::make_pair(nsig*factor,noff*factor);
      double n = rng.Poisson(nsig*factor);
      double m = rng.Poisson(noff*factor);
      buffer[j][k] = std::make_pair(n,m);
    }
  }
  int buffer_time = tbins;

  // run the simulation
  for(int i=0; i<runs; i++) {
    double prob = 0.0;
    double ts = 0.0;
    fitness_total = 0.0;
    for(int l=0; l<tbins; l++) {
      fitness[l][0] = 0.0;
      fitness[l][1] = 0.0;
    }
    on_mean = 0.0;
    off_mean = 0.0;
    for(int j=0; j<bins; j++) {
      double sum = 0.0;
      obs_total[j].first = 0.0;
      obs_total[j].second = 0.0;
      for(int l=0; l<tbins; l++) {
        obs[l][0][j].first = 0.0;
        obs[l][0][j].second = 0.0;
        obs[l][1][j].first = 0.0;
        obs[l][1][j].second = 0.0;
      }
      for(int k=0; k<tbins; k++) {
        double n = 0.0;
        double m = 0.0;
        if(is_buffered) {
          on_mean += buffer_mean[j][k].first;
          off_mean += buffer_mean[j][k].second;
          n = buffer[j][k].first;
          m = buffer[j][k].second;
        } else {
          double factor = (period > 0.0 ? cos(2.0*HAWCUnits::pi*(double)(k+i*tbins)/period) : 1.0)*scales[j];
          if(factor < 0.0) factor = 0.0;
          on_mean += nsig*factor;
          off_mean += noff*factor;
          n = rng.Poisson(nsig*factor);
          m = rng.Poisson(noff*factor);
        }
        obs_total[j].first = obs_total[j].first+n;
        obs_total[j].second = obs_total[j].second+m;
        double term = 0.0;
        if(!is_rts) {
          term = (n+m > 0.0 ? (n+m)*log(n+m) : 0.0)-SpecialFunctions::Gamma::lnG(n+1.0)-SpecialFunctions::Gamma::lnG(m+1.0);
        }
        sum += term;
        for(int l=0; l<tbins; l++) {
          int idx = (k > l ? 1 : 0);
          obs[l][idx][j].first = obs[l][idx][j].first+n;
          obs[l][idx][j].second = obs[l][idx][j].second+m;
          if(!is_rts) fitness[l][idx] = fitness[l][idx]+term;
        }
      }
      if(!is_rts) {
        fitness_total += sum;
        fitness_total -= (obs_total[j].first > 0.0 ? obs_total[j].first*(1.0+log(1.0+obs_total[j].second/obs_total[j].first)) : 0.0);
        fitness_total -= (obs_total[j].second > 0.0 ? obs_total[j].second*(1.0+log(1.0+obs_total[j].first/obs_total[j].second)) : 0.0);
        for(int l=0; l<tbins; l++) {
          fitness[l][0] = fitness[l][0]-(obs[l][0][j].first > 0.0 ? obs[l][0][j].first*(1.0+log(1.0+obs[l][0][j].second/obs[l][0][j].first)) : 0.0);
          fitness[l][0] = fitness[l][0]-(obs[l][0][j].second > 0.0 ? obs[l][0][j].second*(1.0+log(1.0+obs[l][0][j].first/obs[l][0][j].second)) : 0.0);
          fitness[l][1] = fitness[l][1]-(obs[l][1][j].first > 0.0 ? obs[l][1][j].first*(1.0+log(1.0+obs[l][1][j].second/obs[l][1][j].first)) : 0.0);
          fitness[l][1] = fitness[l][1]-(obs[l][1][j].second > 0.0 ? obs[l][1][j].second*(1.0+log(1.0+obs[l][1][j].first/obs[l][1][j].second)) : 0.0);
        }
      }
    }
    if(is_rts) {
      for(int l=0; l<tbins; l++) {
        double ts1 = 0.0;
        double ts2 = 0.0;
        double r1 = 0.0;
        double r2 = 0.0;
        for(int j=0; j<bins; j++) {
          double s1 = obs[l][0][j].first+obs[l][0][j].second;
          double s2 = obs[l][1][j].first+obs[l][1][j].second;
          ts1 += 2.0*(log((1.0+alpha)*obs[l][0][j].first/(alpha*s1))*s1-obs[l][0][j].second*log(obs[l][0][j].first/(alpha*obs[l][0][j].second)));
          ts2 += 2.0*(log((1.0+alpha)*obs[l][1][j].first/(alpha*s2))*s2-obs[l][1][j].second*log(obs[l][1][j].first/(alpha*obs[l][1][j].second)));
          r1 += s1;
          r2 += s2;
        }
        //ts1 = (r1 > 0.0 ? ts1/r1 : 0.0);
        //ts2 = (r2 > 0.0 ? ts2/r2 : 0.0);
        fitness[l][0] = 0.0;
        fitness[l][1] = ts2;
      }
    }
    double fitbest = fitness[0][0]+fitness[0][1];
    int idx = 0;
    fitness1 = fitness[0][0];
    fitness2 = fitness[0][1];
    if(watch >= 0 && watch < int(fitness.size())) {
      idx = watch;
      fitness1 = fitness[watch][0];
      fitness2 = fitness[watch][1];
      fitbest = fitness[watch][0]+fitness[watch][1];
    } else {
      for(int l=1; l<tbins; l++) {
        if(fitness[l][0]+fitness[l][1] > fitbest) {
          fitness1 = fitness[l][0];
          fitness2 = fitness[l][1];
          fitbest = fitness[l][0]+fitness[l][1];
          idx = l;
        }
      }
    }
    best = idx;
    BayesianBuffer::get_significance(obs_total,alpha,prob,ts,sigma_total,true);
    BayesianBuffer::get_significance(obs[idx][0],alpha,prob,ts,sigma1,true);
    BayesianBuffer::get_significance(obs[idx][1],alpha,prob,ts,sigma2,true);
    tree->Fill();
    // shift the buffer
    if(is_buffered) {
      for(int j=0; j<bins; j++) {
        for(int k=1; k<tbins; k++) {
          buffer[j][k-1] = buffer[j][k];
          buffer_mean[j][k-1] = buffer_mean[j][k];
        }
        double factor = (period > 0.0 ? cos(2.0*HAWCUnits::pi*(double)(buffer_time)/period) : 1.0)*scales[j];
        if(factor < 0.0) factor = 0.0;
        buffer_mean[j][tbins-1] = std::make_pair(nsig*factor,noff*factor);
        buffer[j][tbins-1] = std::make_pair(rng.Poisson(nsig*factor),rng.Poisson(noff*factor));
      }
      buffer_time++;
    }
  }

  // report
  Long_t entries = tree->Draw("2.0*(fitness1+fitness2-fitness_total)","(on_mean!=0.0)||(off_mean!=0.0)","goff");
  double max = tree->GetV1()[0];
  for(Long_t i=1; i<entries; i++) {
    if(max < tree->GetV1()[i]) max = tree->GetV1()[i];
  }
  std::cout << "Max: " << max << std::endl;
  TH1D* hfit = new TH1D("hfit","",hbins,0.0,max);
  hfit->GetXaxis()->SetTitle("Test Statistic");
  tree->Draw("2.0*(fitness1+fitness2-fitness_total)>>hfit","(on_mean!=0.0)||(off_mean!=0.0)","goff");
  //TH1F* hfit = (TH1F*)gDirectory->Get("hfit");
  std::cout << "Mean: " << hfit->GetMean() << "  RMS: " << hfit->GetRMS() << std::endl;
  std::cout << "Fit with degrees of freedom free:" << std::endl;
  TF1* chi = new TF1("chi","[0]*pow(x,0.5*[1]-1.0)*exp(-0.5*x)",0.0,max);
  hfit->Fit(chi);
  std::cout << "Fit with degrees of freedom fixed to " << chi2param << ":" << std::endl;
  TF1* chifixed = new TF1("chifixed","[0]*pow(x,0.5*[1]-1.0)*exp(-0.5*x)",0.0,max);
  chifixed->FixParameter(1,chi2param);
  hfit->Fit(chifixed);
  chi->SetNpx(5000);
  chi->SetLineColor(2);
  chifixed->SetNpx(5000);
  chifixed->SetLineColor(4);
  hfit->Write("hfit",TObject::kOverwrite);
  chi->Write("chi",TObject::kOverwrite);
  chifixed->Write("chifixed",TObject::kOverwrite);
  hfit->GetQuantiles(qcount,&quantiles[0],&qprobs[0]);
  for(int i=0; i<qcount; i++) {
    pprobs[i] = SpecialFunctions::Gamma::P(0.5,0.5*quantiles[i]);
    qNs[i] = log(qprobs[i])/log(pprobs[i]);
  }
for(int i=0; i<qcount; i++) std::cout << "Probability " << i << ": " << qprobs[i] << "   quantile: " << quantiles[i] << "   prime: " << pprobs[i] << "   N: " << qNs[i] << std::endl;
  TGraph* graph = new TGraph();
  for(int i=0; i<qcount; i++) graph->SetPoint(i,quantiles[i],qNs[i]);
  graph->Write("trials",TObject::kOverwrite);

  // make the sampled histogram
  TH1D* hsampled = new TH1D("hsampled","",hbins,0.0,max);
  hsampled->GetXaxis()->SetTitle("Test Statistic");
  int samples = cl.GetArgument<int>("samples");
  double sample_weight = ((double)runs)/((double)samples);
  for(int i=0; i<samples; i++) {
    double max = 0.0;
    for(int j=0; j<(tbins-1); j++) {
      double test = BayesianBuffer::solve_chi2(chi2param,rng.Uniform());
      if(test > max) max = test;
    }
    hsampled->Fill(max,sample_weight);
  }
  hsampled->SetLineColor(2);
  hsampled->Write("hsampled",TObject::kOverwrite);

  std::cout << "KS test result: " << hfit->KolmogorovTest(hsampled) << std::endl;

  // clean up
  tree->Write("data",TObject::kOverwrite);
  tree->SetDirectory(0);
  delete tree;
  file->Close();
  delete file;

  return 0;
}

