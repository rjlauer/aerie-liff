// The Logging header
#include <hawcnest/Logging.h>

// Exhibiting how to set the logging threshold to TRACE
// This should only be done in end-user code and not in 
// Modules or Services
LoggingConfig::Instance().SetDefaultLogLevel(TRACE);

log_trace("An example at TRACE level. Note that there is no trailing newline");
log_trace("Since log_trace statements are compiled out at high optimization "
          "levels, you can use them as an alternative to comments where it is "
          "appropriate");

int i = 5;
log_warn("An example at WARN level. Note that we can use standard printf "
         "syntax to print %d",i);

log_fatal("Example FATAL call. NB that the call does not throw. It is "
          "up to you to actually generate an error via throwing an "
          "exception or calling exit");
