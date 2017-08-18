
#include <rng-service/RNGService.h>

#include <hawcnest/HAWCNest.h>
#include <hawcnest/CommandLineConfigurator.h>

#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char* argv[])
{
  CommandLineConfigurator cl;
  cl.AddOption<double>("xmin,a", -3., "Histogram low edge");
  cl.AddOption<double>("xmax,b", 3., "Histogram high edge");
  cl.AddOption<int>("nbins,n", 20, "Number of histogram bins");
  cl.AddOption<int>("nrand,N", 10000, "Size of random number set");
  cl.AddOption<double>("mean,u", 0., "Distribution mean");
  cl.AddOption<double>("width,w", 1., "Distribution width");
  cl.AddOption<int>("type,t", 0, "Distribution type: 0=Gaussian, 1=Rician");
  cl.AddOption<int>("seed,S", 5489, "RNG seed; 0=seed with system clock");

  if (!cl.ParseCommandLine(argc, argv))
    return 1;

  HAWCNest nest;
  nest.Configure();

  nest.Service("StdRNGService", "rng")
    ("seed", cl.GetArgument<int>("seed"));

  nest.Configure();

  const RNGService& rng = GetService<RNGService>("rng");

  // Sample N normal random numbers and histogram them
  const int nbins = cl.GetArgument<int>("nbins");
  const int nrand = cl.GetArgument<int>("nrand");
  const double a = cl.GetArgument<double>("xmin");
  const double b = cl.GetArgument<double>("xmax"); 
  const double mu = cl.GetArgument<double>("mean");
  const double sigma = cl.GetArgument<double>("width");
  const int type = cl.GetArgument<int>("type");

  int idx = 0;
  vector<int> buckets(nbins+1, 0);

  for (int i = 0; i < nrand; ++i) {
    switch (type) {
      case 0:
        idx = int(nbins * (rng.Gaussian(mu, sigma) - a) / (b - a));
        break;
      case 1:
        idx = int(nbins * (rng.Rician(mu, sigma) - a) / (b - a));
        break;
      default:
        log_fatal("Unknown distribution type " << type);
        break;
    }
    if (idx >= 0 && idx < nbins)
      ++buckets[idx];
  }

  switch (type) {
    case 0:
      cout << "Gaussian of mean " << mu << " and width " << sigma << ":\n\n";
      break;
    case 1:
      cout << "Rician of mean " << mu << " and width " << sigma << ":\n\n";
      break;
    default:
      break;
  }

  // Print the histogram of random numbers (rescale the histogram if needed)
  int countMax = *max_element(buckets.begin(), buckets.end());
  double scale = 1.;
  if (countMax > 74)
    scale = 74. / countMax;

  double x;
  for (int i = 0; i < nbins; ++i) {
    x = a + (b - a) * (i + 0.5) / nbins;
    cout << setw(5) << setprecision(2) << fixed << x << ' ';
    for (int j = 0; j < int(scale*buckets[i]); ++j)
      cout << '#';
    cout << endl;
  }

  nest.Finish();

  return 0;
}

