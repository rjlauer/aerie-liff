/*!
 * @file CatalogQuery.h
 * @brief Gives access to various catalogs (SIMBAD, NED, TeVCat, Fermi, etc)
 * @author Israel Martinez
 * @date 8 February 2017
 * @version $Id$
 */

#ifndef CATALOGQUERY_ASTROSERVICE_H_INCLUDED
#define CATALOGQUERY_ASTROSERVICE_H_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <utility>

#include <unistd.h>

#include <data-structures/astronomy/EquPoint.h>

class CatalogQuery{
  
  private:
    
    enum ErrorType{
      
      OK = 0,
      NOT_FOUND,
      PARSING_ERROR,
      FORMAT_ERROR,
      FORK_FAILED,
      CHILD_ERROR
      
    };

  public:
    
    //Holds a catalog
    class Catalog{

      private:
        
        class CatalogEntry{

          public:

            CatalogEntry(const std::string name,
                         const EquPoint& equP,
                         const std::vector<std::string>& associations):
              name_(name),
              equP_(equP),
              associations_(associations)
              { }

            std::string Name() const {return name_;}
            EquPoint EquCoord() const {return equP_;}
            double RA() const {return equP_.GetRA();}
            double Dec() const {return equP_.GetDec();}
            std::vector<std::string> Associations() const {return associations_;}
            
          private:
            
            std::string name_;
            EquPoint equP_;
            std::vector<std::string> associations_;
            
        };
        
      public:

        void Clear(){entries_.clear();}

        CatalogEntry operator[](int i) const {return entries_[i];}

        void AddEntry(CatalogEntry entry){entries_.push_back(entry);}
        
        void AddEntry(const std::string name,
                      const EquPoint& equP,
                      const std::vector<std::string>&
                      associations = std::vector<std::string>(0)){
          entries_.push_back(CatalogEntry(name, equP, associations));
        }
        
        int Size() const {return entries_.size();};
        
      private:
        
        std::vector<CatalogEntry> entries_;
        
    };
    
  public:
    
    /*Methods to obtain coordinates, in standard units, 
      of an identifier in a given catalog.
      Coordinates are saved in a EquPoint passed as reference
    */
      
    /*SIMBAD, catalog of extrasolar objects maintained by  
      Centre de données astronomiques de Strasbourg
      URL: http://simbad.u-strasbg.fr/
      Help with queries: How to query SIMBAD by URLs:
      http://simbad.u-strasbg.fr/Pages/guide/sim-url.htx */
    ErrorType GetCoordsSIMBAD(std::string id, EquPoint& equCoord);
    
    /*NED, catalog of extragalactic object maintained by NASA/IPAC
      URL: http://ned.ipac.caltech.edu/ 
      Help with queries: Web-based Tools—NED VO Services:
      http://www.aspbooks.org/publications/382/165.pdf */
    ErrorType GetCoordsNED(std::string id, EquPoint& equCoord);
    
    /*TeVCat, catalog of TeV sources maintained by 
      Scott Wakely & Deirdre Horan (U. Chicago)
      URL: http://tevcat.uchicago.edu/
      The file is simply the source code of the TeVCat webpage
      If no file is provided, download it
      If assoc, also consider associated sources.
    */
    ErrorType GetCoordsTeVCat(const std::string& file, std::string id,
                              EquPoint& equCoord, bool assoc = false);
    ErrorType GetCoordsTeVCat(std::string id, EquPoint& equCoord,
                              bool assoc = false);

    /*Fermi FHL or FGL catalog
      URL: https://fermi.gsfc.nasa.gov/ssc/data/access/lat/
      If assoc, also consider associated sources. */
    ErrorType GetCoordsFermicat(const std::string& file, std::string id,
                                EquPoint& equCoord, bool assoc = false);
    
    
  public:
    
    /*Methods to find sources between certain radius from a J2000 location.
      ra, dec, and radius are in standard units.
      Sources found are appended to variable sources*/
    
    /*TeVCat, catalog of TeV sources maintained by 
      Scott Wakely & Deirdre Horan (U. Chicago)
      URL: http://tevcat.uchicago.edu/
      The file is simply the source code of the TeVCat webpage
      If no file is provided, download it
    */
    ErrorType ConeSearchTeVCat(const std::string& file,
                               const EquPoint& coords,
                               double radius,
                               Catalog& sources);
    ErrorType ConeSearchTeVCat(const EquPoint& coords,
                               double radius,
                               Catalog& sources);

    /*Fermi FHL or FGL catalog
      URL: https://fermi.gsfc.nasa.gov/ssc/data/access/lat/ */
    ErrorType ConeSearchFermicat(const std::string& file,
                                 const EquPoint& coords,
                                 double radius,
                                 Catalog& sources);
    
  private:
    
    //Fetch TeVCat webpage
    //Saves the output path (the format is tevcat_data_YYYY-MM-DD.txt)
    ErrorType DownloadTeVCat(std::string& path);
    
    //Executes bash command and save stdout and stderr (each element is a line)
    ErrorType Exec(const std::vector<std::string>& argv,
                   std::vector<std::string>& stdoutput,
                   std::vector<std::string>& stderror);
    
    //Encodes special characters with percent code
    std::string URLencode(const std::string& str);
    
    //Parse TeVCat webpage and extrac sources
    ErrorType GetTeVCatSourceList(const std::string& file, Catalog& tevcat);

    //Extract sources from a Fermi catalog (FHL or FGL)
    ErrorType GetFermicatSourceList(const std::string& file, Catalog& fermicat);
    
    //Compare strings case insensitive and disregarding whitespaces
    bool SameNameQ(std::string id1, std::string id2);

    //Find first index in vector that matches a given value
    //Return true is found 
    template< class T >
    bool FindIndex(const std::vector<T>& vec,
                   const T& value, int& index){
      index = find(vec.begin(), vec.end(), value) - vec.begin();
      return index < vec.size();
    }
};



#endif // ASTROSERVICE_ASTROSERVICE_H_INCLUDED
