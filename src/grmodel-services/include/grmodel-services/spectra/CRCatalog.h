/*!
 * @file CRCatalog.h
 * @brief Create a list of cosmic ray source spectra from an XML file
 * @author Segev BenZvi
 * @date 31 Dec 2012
 * @version $Id: CRCatalog.h 19531 2014-04-17 01:43:06Z sybenzvi $
 */

#ifndef GRMODEL_SERVICES_SPECTRA_CRCATALOG_H_INCLUDED
#define GRMODEL_SERVICES_SPECTRA_CRCATALOG_H_INCLUDED

#include <string>
#include <map>
#include <vector>

#include <grmodel-services/spectra/CosmicRaySource.h>

class HAWCNest;

/*!
 * @namespace CRCatalog
 * @brief Build a list of cosmic ray spectra from an XML catalog.  E.g., the
 *        list will contain spectral properties for each cosmic ray species
 * @author Segev BenZvi
 * @date 31 Dec 2012
 * @ingroup gr_models
 */
namespace CRCatalog {

  typedef std::map<ParticleType, std::string> SourceMap;
  typedef SourceMap::iterator SourceMapIterator;
  typedef SourceMap::const_iterator ConstSourceMapIterator;

  /// Parse a list of cosmic ray spectra and create a CosmicRayService list
  SourceMap Build(const std::string& catalogFile, const HAWCNest& nest,
                  bool validateXML=true);

}

#endif // GRMODEL_SERVICES_SPECTRA_CRCATALOG_H_INCLUDED

