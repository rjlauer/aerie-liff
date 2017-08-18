#include "hawcnest/impl/Dump.h"

#include <hawcnest/Logging.h>
#include <hawcnest/RegisterService.h>

REGISTER_SERVICE(Dump);

Module::Result
Dump::Process(BagPtr b)
{
  log_info("----------------------- Bag content: ----------------------");
  log_info(*b);
  log_info("-----------------------------------------------------------");
  return Module::Continue;
}

