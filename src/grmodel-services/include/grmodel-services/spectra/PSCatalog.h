/*!
 * @file PSCatalog.h
 * @brief Build a list of gamma-ray point sources from an XML catalog
 * @author Segev BenZvi
 * @date 12 May 2012
 * @version $Id: PSCatalog.h 18862 2014-02-18 23:38:45Z sybenzvi $
 */

#ifndef GRMODEL_SERVICES_SPECTRA_PSCATALOG_H_INCLUDED
#define GRMODEL_SERVICES_SPECTRA_PSCATALOG_H_INCLUDED

#include <string>
#include <vector>

class HAWCNest;

/*!
 * @namespace PSCatalog
 * @brief Build a list of point sources (gamma-ray only) from a catalog
 * @author Segev BenZvi
 * @date 12 May 2012
 * @ingroup gr_models
 */
namespace PSCatalog {

  typedef std::vector<std::string> SourceList;
  typedef std::vector<std::string>::iterator SourceIterator;
  typedef std::vector<std::string>::const_iterator ConstSourceIterator;

  /// Parse a point source catalog and create a list of PointSource services
  SourceList Build(const std::string& catalogFile, const HAWCNest& nest,
                   bool validateXML=true);

}

#endif // GRMODEL_SERVICES_SPECTRA_PSCATALOG_H_INCLUDED

