/*
 * @file bayes-prior.cc
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
#include <fstream>

int main(int argc, char* argv[])
{
  CommandLineConfigurator cl;
  // specify with config file whose lines are: buffer_size binning alpha signal background prior
  cl.AddOption<int>("runmin", 2000, "Minimum number of runs per config point");
  cl.AddOption<int>("runmax,r", 1000000, "Maximum number of runs per config point");
  cl.AddOption<double>("limit,l", 0.01, "Range limit");
  cl.AddOption<std::string>("config", "", "Config file");
  cl.AddOption<double>("timestep,t", 1.0, "Time step in minutes");
  cl.AddOption<std::string>("method,m", "standard", "Method of running");
  cl.AddOption<std::string>("root,R", "test-bayes.root", "Output ROOT file");
  cl.AddOption<int>("period,T", -1, "Period of oscillation in timesteps (negative for no oscillation)");
  cl.AddOption<double>("cospower,C", 1.0, "Power of cosine for oscillation");
  cl.AddOption<int>("seed", 4537, "RNG seed");

  if (!cl.ParseCommandLine(argc, argv)) return 1;

  // parse the arguments
  int runmin = cl.GetArgument<int>("runmin");
  if(runmin < 1) runmin = 1;
  int runmax = cl.GetArgument<int>("runmax");
  double rangelimit = cl.GetArgument<double>("limit");
  std::string config = cl.GetArgument<std::string>("config");
  double tDelta = cl.GetArgument<double>("timestep") * HAWCUnits::minute;
  std::string method = cl.GetArgument<std::string>("method");
  double period = (double)(cl.GetArgument<int>("period")) * tDelta;
  double omega = (period > 0.0 ? HAWCUnits::twopi/period : 0.0);
  double power = cl.GetArgument<double>("cospower");

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
  int tBuffer = 0;
  int tBinning = 0;
  double tAlpha = 0.0;
  double tSignal = 0.0;
  double tBackground = 0.0;
  double tPrior = 0.0;
  double tLow = 0.0;
  double tHigh = 0.0;
  int tRuns = 0;
  double tMode = 0.0;
  data_tree->Branch("buffer",&tBuffer,"buffer/I");
  data_tree->Branch("binning",&tBinning,"binning/I");
  data_tree->Branch("alpha",&tAlpha,"alpha/D");
  data_tree->Branch("signal",&tSignal,"signal/D");
  data_tree->Branch("background",&tBackground,"background/D");
  data_tree->Branch("prior",&tPrior,"prior/D");
  data_tree->Branch("elow",&tLow,"low/D");
  data_tree->Branch("ehigh",&tHigh,"high/D");
  data_tree->Branch("runs",&tRuns,"runs/I");
  data_tree->Branch("mode",&tMode,"mode/D");

  // Configure services and modules
  HAWCNest nest;
  nest.Service<StdRNGService>("rng")
    ("seed", cl.GetArgument<int>("seed"));
  nest.Configure();

  const RNGService& rng = GetService<RNGService>("rng");
  BayesianBuffer::BayesianBuffer* bb;

  // Open the config file and read it
  std::ifstream fconfig;
  fconfig.open(config.c_str());
  while(fconfig.good()) {
    fconfig >> tBuffer >> tBinning >> tAlpha >> tSignal >> tBackground >> tPrior;
    std::vector<double> binAlpha;
    binAlpha.push_back(tAlpha);
    if(method == "signal") {
      bb = new BayesianBuffer::BayesianBuffer(tPrior,tBuffer,tBinning);
    } else if(method == "excess") {
      bb = new BayesianBuffer::ExcessRatioBB(tPrior,tBuffer,tBinning,binAlpha);
    } else if(method == "scargle") {
      bb = new BayesianBuffer::RatioScargleErrorBB(tPrior,tBuffer,tBinning);
    } else {
      bb = new BayesianBuffer::RatioBB(tPrior,tBuffer,tBinning,binAlpha);
    }
    if(!fconfig.good()) continue;
    std::cout << "Running (buffer,binning,alpha,signal,background,prior) = " << tBuffer << "," << tBinning << "," << tAlpha << "," << tSignal << "," << tBackground << "," << tPrior << ")" << std::endl;
    int counts = 0;
    int countstep = runmin;
    double range = 1.0;
    double noff_avg = tBackground / tAlpha;
    double non_avg = tBackground+tSignal;
    double T0 = 56000.0 * HAWCUnits::day;
    double t = T0;
    double frac = 1.0/((double)tBuffer);
    std::map<unsigned int,double> changePointMap;
    int searches = 0;
    tRuns = 0;
    while(counts < runmax && range > rangelimit) {
      if(counts + countstep > runmax) countstep = runmax-counts;
      counts += countstep;
      for(int i=0; i<countstep; i++) {
        tRuns++;
        t += tDelta;
        double weight = (omega > 0.0 ? cos(omega*(t-T0)) : 1.0);
        if(weight < 0.0) weight = 0.0; // can't have negative average numbers of events!
        if(power != 1.0 && weight != 0.0) weight = pow(weight,power);
        bb->AddPoint(rng.Poisson(non_avg*weight),rng.Poisson(noff_avg*weight),tAlpha,t / HAWCUnits::day);
        std::vector<std::vector<unsigned int> > changes;
        if(bb->IsNewBlockComplete()) {
          searches++;
          double diff = bb->Optimize(changes);
          (void)diff;
          for(unsigned int j = 0; j < changes[0].size(); ++j) {
            if(changePointMap.find(changes[0][j]) == changePointMap.end())
              changePointMap.insert(std::make_pair(changes[0][j],0.0));
            changePointMap[changes[0][j]] = changePointMap[changes[0][j]] + frac;
          }
        }
      }
      double n = (double)searches;
      double k = (double)changePointMap.size();
      double var = (k+1.0)*(k+2.0)/((n+2.0)*(n+3.0))-(k+1.0)*(k+1.0)/((n+2.0)*(n+2.0));
      tMode = k/n;
      tLow = tMode-sqrt(var);
      tHigh = tMode+sqrt(var);
      if(tLow < 0.0) {
        tHigh -= tLow;
        tLow = 0.0;
      }
      if(tHigh > 1.0) {
        tLow -= (tHigh-1.0);
        tHigh = 1.0;
      }
      range = (tHigh-tLow)/tHigh;
      std::cout << " -> limits for " << changePointMap.size() << " out of " << searches << ": " << tLow << " to " << tHigh << " (relative range " << range << ")" << std::endl;
      countstep *= 2;
    }
    std::cout << "Filling the tree..." << std::endl;
    data_tree->Fill();
    delete bb;
  }

  // clean up the ROOT file
  data_tree->Write("data",TObject::kOverwrite);
  data_tree->SetDirectory(0);
  delete data_tree;
  file->Close();
  delete file;
  
  return 0;
}

