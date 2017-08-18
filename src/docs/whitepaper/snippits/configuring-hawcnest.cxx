#include <hawcnest/HAWCNest.h>
#include "STDRandomNumberSerice.h"
#include "DBCalibrationService.h"

HAWCNest nest;

nest.Service<STDRandomNumberService>("rand")
  ("Seed",12345);

nest.Service<DBCalibrationService>("calib")
  ("server","mildb.umd.edu")
  ("uname","milagro")
  ("password","secret");

nest.Configure();
