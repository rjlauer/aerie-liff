/**
 * FILE ExampleComponents.h
 */

#ifndef EXAMPLE_MODULES_H
#define EXAMPLE_MODULES_H

#include <cstdio>
#include <hawcnest/processing/Module.h>
#include <hawcnest/Service.h>
#include <hawcnest/processing/Source.h>

// some structures for the bag
struct EventHeader : public Baggable{
  int event_number;
  int run_number;
  int hit_channels;
};

typedef boost::shared_ptr<EventHeader> EventHeaderPtr;
typedef boost::shared_ptr<const EventHeader> EventHeaderConstPtr;

struct HitData : public Baggable{
  std::vector<int> hit_channels;
  std::vector<float> hit_times;
};

typedef boost::shared_ptr<HitData> HitDataPtr;
typedef boost::shared_ptr<const HitData> HitDataConstPtr;

struct CoreReconstruction : public Baggable{
  float corex;
  float corey;
};

typedef boost::shared_ptr<CoreReconstruction> CoreReconstructionPtr;
typedef boost::shared_ptr<const CoreReconstruction> CoreReconstructionConstPtr;

// In this simple example, I have implemented all of the components (services
// and modules) used in this example.  For each service, I have two 
// implementations

// First are the services.  The services come in at least two parts.  The first
// is the interface.  This is a pure virtual base class which just defines
// what it is that the service provides.  The pure virtual base class
// is akin to the Java 'Interface' construct.  The second part is the
// actual implementation which inherits from the interface and implements
// the methods defined in the interface as well as the methods required
// of the framework.

// The interface for the RandomNumberService.  Note that the client code
// calls RandomNumberService& random = GetService<RandomNumberService()
// So this is the only thing that they know about. 
class RandomNumberService
{
 public:
  // give a number drawn from a uniform distribution
  virtual double Uniform(double low,double high) = 0;
  
  virtual ~RandomNumberService(){}
};

// The first implementation of this service uses std::rand 
class STDRandomNumberService : public RandomNumberService
{
 public:
  // This typedef tells the framework that this class will be used
  // to satisfy the RandomNumberService interface
  typedef RandomNumberService Interface;

  // This method is where you declare your default configuration
  // and all the configuration parameters that this component will take.
  // This random number generator takes one parameter, a 'seed'.
  Configuration DefaultConfiguration(){
    Configuration config;
    config.Parameter<int>("Seed",0);
    return config;
  }

  // This method is what is used to actually configure the module.  
  // The user has presumably set all the parameters and now we take
  // them and do something useful with them.
  void Initialize(const Configuration& config){
    config.GetParameter("Seed",seed_);
    log_info("seeding random number generator with seed " << seed_);
    srand(seed_);
  }

  // and here is implementing the methods promised by the interface
  double Uniform(double low,double high){
    double rnd = (double)rand() /(double)RAND_MAX;
    double toReturn = rnd *(high - low) + low;
    return toReturn;
  }

 private:
  int seed_;
};

// This is a data structure which I fancy holds the calibration constants
class Calibration
{
 public:
  std::vector<float> fakeCalibrationConstants_;
};

// Here is a service which retrieves the calibration keyed off the
// event run number
class CalibrationService
{
 public:
  virtual Calibration& GetCalibration(unsigned run) = 0;

  virtual ~CalibrationService(){}
};

// This is a dummy implementation, except I fancy that it goes off
// and fetches the calibration constants from a DB
class DBCalibrationService : public CalibrationService
{
 public:
  typedef CalibrationService Interface;
  Configuration DefaultConfiguration(){
    Configuration config;
    config.Parameter<std::string>("server","mildb.umd.edu");
    config.Parameter<std::string>("uname","milagro");
    config.Parameter<std::string>("password","milagro");
    return config;
  }
  void Initialize(const Configuration& config){
    config.GetParameter("server",server_);
    config.GetParameter("uname",uname_);
    config.GetParameter("password",password_);
  }
  void Finish(){
    log_info("closing DB connection");
  }

  Calibration& GetCalibration(unsigned run){
    log_info("Pretend I'm reading calibration from a DB here");
    log_info("Connecting to server:" << server_
             << " with uname:" << uname_
             << " and password:" << password_);
    return calibration_; // pretend we fetch it from a DB here
  }
 private:
  Calibration calibration_;
  std::string server_;
  std::string uname_;
  std::string password_;
};

// Here, I fancy that we are getting the calibration constants from 
// a flat file
class FlatFileCalibrationService : public CalibrationService
{
 public:
  typedef CalibrationService Interface;

  Configuration DefaultConfiguration(){
    Configuration config;
    config.Parameter<std::string>("filename","calibration.dat");
    return config;
  }

  Calibration& GetCalibration(unsigned run){
    log_info("pretend I'm reading calibration from a flat file here");
    return calibration_; // pretend we fetch it from a DB here
  }
 private:
  Calibration calibration_;
};


// This is an example 'Source' which is the first Module added to the 
// framework.  This source fills the data structure.  Typically the source
// would get data from a file or a socket, but this one makes it up
// from scratch.  The difference between a source and a Module is just
// function 'Next'.  The framework processes until Next returns a null
// pointer
class ExampleSource : public Source
{
public:
  typedef Source Interface;

  ExampleSource(){events_ = 0;}

  Configuration DefaultConfiguration(){
    Configuration config;

    // not supplying a default value means the framework will complain
    // if the user doesn't set a value for 'input'
    config.Parameter<std::string>("input");
    config.Parameter<int>("maxevents",10);
    return config;
  }

  // This function is called before 'Process' is called.  This eventually
  // will be where the module gets configured
  void Initialize(const Configuration& config)
  {
    config.GetParameter("input",infile_);
    config.GetParameter("maxevents",maxEvents_);
    log_info("opening the imaginary file '" << infile_ << "'");
    log_info("processing " << maxEvents_ << " events");
  }

  // This method gets called for each event.  The EventPtr structure 
  // already points somewhere valid and we just fill it.
  BagPtr Next(){
    if(events_ >= maxEvents_)
      return BagPtr();
    BagPtr b = make_shared<Bag>();
    log_info("========================================\n"
             << "'reading' event " << events_ 
             << " from file " << infile_); 
    EventHeaderPtr header = make_shared<EventHeader>();
    header->event_number = events_;
    header->run_number = 12345; 

    b->Put("Header",header);

    HitDataPtr hitData = make_shared<HitData>();
    int hitChannels = rand() % 50;
    hitData->hit_channels.resize(hitChannels);
    hitData->hit_times.resize(hitChannels);
    for(int i = 0 ; i < hitChannels ; i++){
      hitData->hit_channels[i] = rand() % 900;
      hitData->hit_times[i] = (float)rand()/(float)RAND_MAX;
    }
    
    b->Put("RawHitData",hitData);

    events_ += 1; 

    return b;
  }

  // This method is called when all is done
  void Finish(){
    log_info("closing the imaginary file");
  }
private:
  int events_;
  int maxEvents_;
  std::string infile_;
};

// Another dummy example.  This one does a fake calibration
class CalibrationModule : public Module
{
 public:
  typedef Module Interface;

  Result Process(BagPtr b){
    // This shows how to fetch the calibration serice and use it.  
    // N.B. The complete ignorance of whether this comes from a DB
    // or a file.
    const EventHeader& head = b->Get<EventHeader>("Header");
    Calibration& calib =  
      GetService<CalibrationService>("calib").GetCalibration(head.run_number);
    
    log_info("calibrating the event with "
             << calib.fakeCalibrationConstants_.size() << " constants" )
    return Continue;
  }
};

// A fake Reconstruction module... COM technique
class ReconstructionModule_COM : public Module
{
 public:
  typedef Module Interface;

  Result Process(BagPtr e){
    RandomNumberService& random = GetService<RandomNumberService>("rand"); 
    log_info("reconstructing the event with COM technique"); 

    CoreReconstructionPtr core = make_shared<CoreReconstruction>();
    core->corex =random.Uniform(-2000,2000); 
    core->corey =random.Uniform(-2000,2000); 

    e->Put("Core_COM",core);
    
    return Continue;
  }
};

// A fake Reconstruction module... Gauss technique
class ReconstructionModule_Gauss : public Module
{
 public:
  typedef Module Interface;

  Result Process(BagPtr e){
    RandomNumberService& random = GetService<RandomNumberService>("rand"); 
    log_info("reconstructing the event with Gauss technique"); 

    CoreReconstructionPtr core = make_shared<CoreReconstruction>();
    core->corex =random.Uniform(-2000,2000); 
    core->corey =random.Uniform(-2000,2000); 

    e->Put("Core_Gauss",core);

    return Continue;
  }
};

// A module which prints the values in the event structure
class PrintingModule : public Module
{
 public:
  typedef Module Interface;

  Result Process(BagPtr b){
    log_info("--------------------------------\n" << *b
             << "--------------------------------\n");

    return Continue;
  }
};

#endif

