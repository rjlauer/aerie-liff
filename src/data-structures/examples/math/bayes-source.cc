/*
 * @file bayes-root.cc
 * @brief for testing BayesianBuffers and plotting results in a ROOT file
 * @author Tom Weisgarber
 * @date 17 June 2014
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

namespace BayesianBufferSource {

class Source {
  public:
    Source(double flux_in,double t0_in,double t01_in,double t12_in,double t23_in) {
      reset(flux_in,t0_in,t01_in,t12_in,t23_in);
    }
    ~Source();

    void reset(double flux_in,double t0_in,double t01_in,double t12_in,double t23_in) {
      static int last_uid = -1;
      flux = flux_in;
      t0 = t0_in;
      t1 = t01_in+t0;
      t2 = t12_in+t1;
      t3 = t23_in+t2;
      d1 = t01_in > 0.0 ? 1.0/t01_in : 0.0;
      d2 = t23_in > 0.0 ? 1.0/t23_in : 0.0;
      last_uid++;
      uid = last_uid;
    }

    double evaluate(double t) {
      if(t <= t0 || t >= t3) return 0.0;
      if(t >= t1 && t <= t2) return flux;
      if(t < t1) return flux*(t-t0)*d1;
      return flux*(t3-t)*d2;
    }

    inline double get_max_time() {return t3;}
    inline bool is_active(double t) {return t < t3;}

    inline int get_uid() {return uid;}
    inline double get_flux() {return flux;}
    inline double get_t0() {return t0;}
    inline double get_t1() {return t1;}
    inline double get_t2() {return t2;}
    inline double get_t3() {return t3;}

  private:
    double flux,t0,t1,t2,t3;
    double d1,d2;
    int uid;
};

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
  cl.AddOption<int>("period,T", -1, "Period of oscillation in timesteps (negative for no oscillation)");
  cl.AddOption<int>("pflare,P", -1, "Average number of timesteps in between random flares");
  cl.AddOption<double>("cospower,C", 1.0, "Power of cosine for oscillation");
  cl.AddOption<double>("flux,F", 0.0, "Source flux average");
  cl.AddOption<double>("fsigma,f", 0.0, "Source flux sigma");
  cl.AddOption<double>("t1", 0.0, "Source turn on time average [timesteps]");
  cl.AddOption<double>("s1", 0.0, "Source turn on time sigma [timesteps]");
  cl.AddOption<double>("t2", 0.0, "Source duration average [timesteps]");
  cl.AddOption<double>("s2", 0.0, "Source duration sigma [timesteps]");
  cl.AddOption<double>("t3", 0.0, "Source turn off time average [timesteps]");
  cl.AddOption<double>("s3", 0.0, "Source turn off time sigma [timesteps]");
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
  double flare_prob = 1.0/((double)(cl.GetArgument<int>("pflare")));
  double omega = (period > 0.0 ? HAWCUnits::twopi/period : 0.0);
  double power = cl.GetArgument<double>("cospower");
  double flux = cl.GetArgument<double>("flux");
  double sflux = cl.GetArgument<double>("fsigma");
  double t1 = cl.GetArgument<double>("t1") * tDelta;
  double t2 = cl.GetArgument<double>("t2") * tDelta;
  double t3 = cl.GetArgument<double>("t3") * tDelta;
  double s1 = cl.GetArgument<double>("s1") * tDelta;
  double s2 = cl.GetArgument<double>("s2") * tDelta;
  double s3 = cl.GetArgument<double>("s3") * tDelta;
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
  double tSignalAvg = 0.0;
  double tBackAvg = 0.0;
  double tNoffAvg = 0.0;
  double tWeight = 0.0;
  int tActiveSource = -1;
  data_tree->Branch("fraction",&tFraction,"fraction/D");
  data_tree->Branch("alpha",&tAlpha,"alpha/D");
  data_tree->Branch("Non",&tNon,"Non/D");
  data_tree->Branch("Noff",&tNoff,"Noff/D");
  data_tree->Branch("MJD",&tMJD,"MJD/D");
  data_tree->Branch("sigma",&tSigma,"sigma/D");
  data_tree->Branch("lnprior",&tLnPrior,"lnprior/D");
  data_tree->Branch("srcavg",&tSignalAvg,"srcavg/D");
  data_tree->Branch("bkgavg",&tBackAvg,"bkgavg/D");
  data_tree->Branch("noffavg",&tNoffAvg,"noffavg/D");
  data_tree->Branch("weight",&tWeight,"weight/D");
  data_tree->Branch("source",&tActiveSource,"source/I");

  // make a tree to hold information about the sources
  TTree* source_tree = new TTree("source","source");
  int sUID = -1;
  double sFlux = 0.0;
  double sT0 = 0.0;
  double sT1 = 0.0;
  double sT2 = 0.0;
  double sT3 = 0.0;
  source_tree->Branch("id",&sUID,"id/I");
  source_tree->Branch("flux",&sFlux,"flux/D");
  source_tree->Branch("mjd0",&sT0,"mjd0/D");
  source_tree->Branch("mjd1",&sT1,"mjd1/D");
  source_tree->Branch("mjd2",&sT2,"mjd2/D");
  source_tree->Branch("mjd3",&sT3,"mjd3/D");
  
  // Configure services and modules
  HAWCNest nest;
  nest.Service<StdRNGService>("rng")
    ("seed", cl.GetArgument<int>("seed"));
  nest.Configure();

  // Set up a Bayesian buffer
  BayesianBuffer::BayesianBuffer* bb;
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
  //int independents = 0;

  // deques to hold information we want to record later
  std::deque<double> nsignal_average;
  std::deque<double> nback_average;
  std::deque<double> noff_average;
  std::deque<double> weight_value;
  std::deque<double> weight_count;
  std::deque<int> active_source;
  nsignal_average.push_back(0.0);
  nback_average.push_back(0.0);
  noff_average.push_back(0.0);
  weight_value.push_back(0.0);
  weight_count.push_back(0.0);
  active_source.push_back(-1);

  // source class
  BayesianBufferSource::Source* source = new BayesianBufferSource::Source(0.0,t-2.0*tDelta,0.5*tDelta,0.5*tDelta,0.5*tDelta);

  // run, collecting change points and the fraction of the time they are identified while in the buffer
  for(int i = 0; i < runs; ++i) {
    t += tDelta;
    double src_avg = sig_avg;
    // add source here
    if(!source->is_active(t)) {
      if(rng.Uniform() < flare_prob) {
        source->reset(rng.Gaussian(flux,sflux),t,rng.Gaussian(t1,s1),rng.Gaussian(t2,s2),rng.Gaussian(t3,s3));
        sUID = source->get_uid();
        sFlux = source->get_flux();
        sT0 = source->get_t0() / HAWCUnits::day;
        sT1 = source->get_t1() / HAWCUnits::day;
        sT2 = source->get_t2() / HAWCUnits::day;
        sT3 = source->get_t3() / HAWCUnits::day;
        source_tree->Fill();
      }
      active_source[active_source.size()-1] = -1;
    } else {
      src_avg += source->evaluate(t);
      active_source[active_source.size()-1] = source->get_uid();
    }
    // sample the events
    double weight = (omega > 0.0 ? cos(omega*(t-T0)) : 1.0);
    if(weight < 0.0) weight = 0.0; // can't have negative average numbers of events!
    if(power != 1.0 && weight != 0.0) weight = pow(weight,power);
    nsignal_average[nsignal_average.size()-1] = nsignal_average[nsignal_average.size()-1]+src_avg;
    nback_average[nback_average.size()-1] = nback_average[nback_average.size()-1]+bkg_avg;
    noff_average[noff_average.size()-1] = noff_average[noff_average.size()-1]+noff_avg;
    weight_value[weight_value.size()-1] = weight_value[weight_value.size()-1]+weight;
    weight_count[weight_count.size()-1] = weight_count[weight_count.size()-1]+1.0;
    double sampled_on = rng.Poisson((bkg_avg+src_avg)*weight);
    double sampled_off = rng.Poisson(noff_avg*weight);
    bb->AddPoint(sampled_on,sampled_off,alpha,t / HAWCUnits::day);
    //bb->AddPoint(rng.Poisson((bkg_avg+src_avg)*weight),rng.Poisson(noff_avg*weight),alpha,t / HAWCUnits::day);
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
      if(bb->IsBufferFull()) {
        unsigned int point = bb->GetPointIndex(0);
        tFraction = (changePointMap.find(point) == changePointMap.end() ? 0.0 : changePointMap[point]);
        tAlpha = bb->GetPointAlpha(0);
        tNon = bb->GetPointNon(0);
        tNoff = bb->GetPointNoff(0);
        tMJD = bb->GetPointMJD(0);
        tSigma = bb->GetPointLiMaSig(0);
        tSignalAvg = nsignal_average[0];
        tBackAvg = nback_average[0];
        tNoffAvg = noff_average[0];
        tWeight = (weight_count[0] > 0.0 ? weight_value[0]/weight_count[0] : 0.0);
        tActiveSource = active_source[0];
        data_tree->Fill();
      }
      nsignal_average.push_back(0.0);
      nback_average.push_back(0.0);
      noff_average.push_back(0.0);
      weight_value.push_back(0.0);
      weight_count.push_back(0.0);
      active_source.push_back(-1);
      if(nsignal_average.size() > buffer) nsignal_average.pop_front();
      if(nback_average.size() > buffer) nback_average.pop_front();
      if(noff_average.size() > buffer) noff_average.pop_front();
      if(weight_value.size() > buffer) weight_value.pop_front();
      if(weight_count.size() > buffer) weight_count.pop_front();
      if(active_source.size() > buffer) active_source.pop_front();
    }
  }

  // clean up the ROOT file
  data_tree->Write("data",TObject::kOverwrite);
  data_tree->SetDirectory(0);
  delete data_tree;
  source_tree->Write("source",TObject::kOverwrite);
  source_tree->SetDirectory(0);
  delete source_tree;
  file->Close();
  delete file;
  
  return 0;
}

