/*!
 * @file LoadProject.cc 
 * @brief Load an AERIE project library into memory.
 * @author Segev BenZvi 
 * @date 9 Jul 2011 
 * @version $Id: LoadProject.cc 14881 2013-04-28 14:02:42Z sybenzvi $
 */

#include <hawcnest/LoadProject.h>
#include <hawcnest/Logging.h>

#include <dlfcn.h>
#include <sstream>

using namespace std;

int
load_project(const std::string& proj, bool doVerbose)
{
  if (doVerbose)
    log_info("Loading library ... " << proj);

  // Make sure path name is not of form libXYZ.so or libXYZ.dylib, to ensure
  // that all calls to load_project are platform-independent
  if (proj.find(".dylib") != string::npos || proj.find(".so") != string::npos)
    log_fatal("Failure loading " << proj
              << "; load external libraries without prefix or extension");

  stringstream path;

  // Prepend lib to the path name
  if (proj.find("lib") != 0)
    path << "lib" << proj;

  // Append suffix to the path name
#ifdef __APPLE_CC__
  string ext(".dylib");
#else
  string ext(".so");
#endif
  path << ext;

  // Attempt a first search in LD_LIBRARY_PATH
  void* v = dlopen(path.str().c_str(), RTLD_NOW | RTLD_GLOBAL);

  // If not found, try again in HAWC_INSTALL
  if (!v) {
    char* libdir = getenv("HAWC_INSTALL");
    if (libdir) {
      path.str("");
      path << libdir << "/lib/lib" << proj << ext;
      v = dlopen(path.str().c_str(), RTLD_NOW | RTLD_GLOBAL);
    }
  }

  // Last resort: try in HAWC_BUILD
  if (!v) {
    char* libdir = getenv("HAWC_BUILD");
    if (libdir) {
      path.str("");
      path << libdir << "/" << proj << "/lib" << proj << ext;
      v = dlopen(path.str().c_str(), RTLD_NOW | RTLD_GLOBAL);
    }
  }

  if (!v)
    log_fatal(proj << ": dlopen loading error");

  log_info(proj << " loaded");

  return 0;
}

