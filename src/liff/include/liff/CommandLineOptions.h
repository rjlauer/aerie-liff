/*!
 * @file CommandLineOptions.h
 * @author Robert Lauer
 * @date 14 Apr 2017
 * @brief Centralized Command Line Options implementation
 * @version $Id$
 */

#ifndef LIFF_COMMAND_LINE_OPTIONS_H_INCLUDED
#define LIFF_COMMAND_LINE_OPTIONS_H_INCLUDED

#include <string>
#include <hawcnest/CommandLineConfigurator.h>

/*************************************
 * EBL
 * ***********************************/

/// Set up command-line arguments specifying redshift and EBL model.
template<typename Parser>
void AddEBLOptions(Parser& cl) {

  cl.template AddOption<double>(
      "redshift",
      0, 
      "Redshift for EBL correction"
  );

  cl.template AddOption<std::string>(
      "eblmodel",
      "Gilmore12FiducialEBLModel",
      "EBL model, names are class names in grmodel-services/src/ebl"
  );

}

/// Get redshift from CL parser
inline double GetRedshift(const CommandLineConfigurator& cl) {
  return cl.GetArgument<double>("redshift");
}

/// Get EBL model from CL parser
inline std::string GetEBLModel(const CommandLineConfigurator& cl) {
  return cl.GetArgument<std::string>("eblmodel");
}

#endif
