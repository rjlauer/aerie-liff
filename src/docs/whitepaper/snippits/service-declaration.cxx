#include <hawcnest/Configuration.h>

class RandomNumberService
{
 public:
  virtual double Uniform(double low,double high) = 0;

  virtual ~RandomNumberService(){}
};

class STDRandomNumberService : public RandomNumberService
{
 public:
  typedef RandomNumberService Interface;

  Configuration DefaultConfiguration(){
    Configuration config;
    config.Parameter<int>("Seed",0);
    return config;
  }

  void Initialize(const Configuration& config){
    config.GetParameter("Seed",seed_);
    printf("seeding random number generator with seed %d\n",seed_);
    srand(seed_);
  }

  double Uniform(double low,double high){
    double rnd = (double)rand() /(double)RAND_MAX;
    double toReturn = rnd *(high - low) + low;
    return toReturn;
  }

 private:
  int seed_;
};
