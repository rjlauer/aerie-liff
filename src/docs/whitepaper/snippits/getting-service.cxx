#include <hawcnest/Service.h>

RandomNumberService& random = GetService<RandomNumberService>("rand");

CoreReconstructionPtr core = make_shared<CoreReconstruction>();
core->corex = random.Uniform(-2000,2000);
core->corex = random.Uniform(-2000,2000);
