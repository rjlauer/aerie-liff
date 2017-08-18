/*!
 * @file CatalogQuery.h
 * @brief Gives access to various catalogs (SIMBAD, NED, TeVCat, Fermi, etc)
 * @author Israel Martinez
 * @date 8 February 2017
 * @version $Id$
 *
 * @todo Right now it call system's curl (POSIX) using execvp. 
 * It would be better to remove this to make it more system independent
 * Also, cJSON and base64 are included in AERIE's, 
 * but they should part of the externals 
 */

#include <astro-service/catalogs/CatalogQuery.h>
#include <astro-service/catalogs/cJSON.h>
#include <astro-service/catalogs/base64.h>

#include <hawcnest/HAWCUnits.h>

#include <data-structures/astronomy/AstroCoords.h>
#include <data-structures/astronomy/EquPoint.h>
#include <data-structures/math/Math.h>

#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <time.h>

#include <fitshandle.h>

#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace HAWCUnits;
using namespace boost::assign;
using namespace boost::algorithm;

CatalogQuery::ErrorType
CatalogQuery::GetCoordsSIMBAD(string id, EquPoint& equCoord){

  //**** Query *****

  //Run ADQL query using TAP URL
  vector<string> command;
  command += "curl";
  command += "http://simbad.u-strasbg.fr/simbad/sim-tap/sync?request=doQuery"
    "&lang=adql&format=text&query="
    "SELECT%20ra,%20dec%20FROM%20basic%20JOIN%20ident%20ON%20oid%20=%20oidref"
    "%20WHERE%20id%20=%20'" + URLencode(id) + "';";
  
  vector<string> stdoutput;
  vector<string> stderror;
  if(Exec(command, stdoutput, stderror)){
      
    log_error("SIMBAD query error:\n"
              << join(stderror,"\n"));
    return CHILD_ERROR;
    
  }
  
  sleep(1);//Comply with SIMBAD automatic query frequency
  
  //***** Parsing *****
  
  //Check if only the header is present
  if(stdoutput.size() < 3)
    return NOT_FOUND;
  
  //Extract values
  double ra;
  double dec;
  if(sscanf(stdoutput[2].c_str(), "%lf|%lf", &ra, &dec ) != 2){
    
    log_error("Unexpected problem while parsing SIMBAD output:\n"
              << join(stdoutput,"\n"));
    
    return PARSING_ERROR;
    
  }

  //Save
  equCoord.SetRADec( ra * degree, dec * degree );
  
  return OK;
}

CatalogQuery::ErrorType
CatalogQuery::GetCoordsNED(string id, EquPoint& equCoord){

  //***** Query *****
  
  //Run URL object search using "of=ascii_bar" 
  vector<string> command;
  command += "curl";
  command += "http://ned.ipac.caltech.edu/cgi-bin/nph-objsearch?"
    "extend=no&of=ascii_bar&objname=" + URLencode(id) ;
  
  vector<string> stdoutput;
  vector<string> stderror;
  if(Exec(command, stdoutput, stderror)){
    
    log_error("NED query error:\n"
              << join(stderror,"\n"));
    return CHILD_ERROR;
    
  }
  
  sleep(1);//Comply with NED automatic query frequency
  
  //***** Parsing *****

  //Error message starting with "Error:" is received if not found
  int nlines = stdoutput.size();
  
  for(int i = 0; i < nlines; ++i){
    if(stdoutput[i].substr(0, 6) == "Error:"){
      return NOT_FOUND; //Not found
    }
  }

  /*For some unknown reasons it also sometimes throws a server error
    when not found (e.g. 'mrk421s', '1ES 1727+501' ). 
    These next 5 lines should be removed if they fix this issue */ 
  for(int i = 0; i < nlines; ++i){
    if(stdoutput[i] == "<title>500 Internal Server Error</title>"){
      return NOT_FOUND; //Not found
    }
  }
  
  //Check if header is as expected
  if(nlines < 2){
    
    log_error("Could not parse NED query result. "
              "Format might have changed. NED output:\n"
              << join(stdoutput,"\n"));
    
    return PARSING_ERROR;
    
  }

  vector<string> header;
  split(header, stdoutput[nlines-2], is_any_of("|"));

  if(header.size() < 4 ||
     header[2] != "RA(deg)" ||
     header[3] != "DEC(deg)") {

    log_error("Could not parse NED query result. "
              "Format might have changed. NED output:\n"
              << join(stdoutput,"\n"));
    
    return PARSING_ERROR;

  } 
  
  //Extract values from final row

  vector<string> results;
  split(results, stdoutput[nlines-1], is_any_of("|"));

  if(results.size() < 4){
    
    log_error("Could not parse NED query result. "
              "Format might have changed. NED output:\n"
              << join(stdoutput,"\n"));
    
    return PARSING_ERROR;
    
  }

  equCoord.SetRADec( atof(results[2].c_str()) * degree ,
                     atof(results[3].c_str()) * degree );
  
  return OK;
}

CatalogQuery::ErrorType
CatalogQuery::GetCoordsTeVCat(const string& file, string id,
                              EquPoint& equCoord, bool assoc){

  Catalog tc;
  ErrorType status = GetTeVCatSourceList(file, tc);

  if(status)
    return status;

  for(int i = 0; i < tc.Size(); ++i){

    string name = tc[i].Name();
    vector<string> associations = tc[i].Associations();

    bool found = false;
    
    if(SameNameQ(id, name))
      found = true;

    if(assoc){
      for(int s = 0; s < associations.size(); ++s){
        if(SameNameQ(id, associations[s]))
          found = true;
      }
    }
    
    if(found) {
      
      equCoord.SetRADec( tc[i].RA() , tc[i].Dec() );
      
      return OK;
    }
    
  }
  
  return NOT_FOUND;
  
}

CatalogQuery::ErrorType
CatalogQuery::GetCoordsFermicat(const string& file, string id,
                                EquPoint& equCoord, bool assoc){

  Catalog fermicat;
  ErrorType status = GetFermicatSourceList(file, fermicat);

  if(status)
    return status;
  
  for(int i = 0; i < fermicat.Size(); ++i){

    string name = fermicat[i].Name();
    vector<string> associations = fermicat[i].Associations();

    bool found = false;
    
    if(SameNameQ(id, name))
      found = true;

    if(assoc){
      for(int s = 0; s < associations.size(); ++s){
        if(SameNameQ(id, associations[s]))
          found = true;
      }
    }
    
    if(found) {
      
      equCoord.SetRADec( fermicat[i].RA() , fermicat[i].Dec() );
      
      return OK;
    }
    
  }
  
  return NOT_FOUND;
  
}

CatalogQuery::ErrorType
CatalogQuery::GetCoordsTeVCat(string id, EquPoint& equCoord, bool assoc){

  //Download
  string file;
  ErrorType status = DownloadTeVCat(file);

  if(status)
    return status;
  
  //Call real function
  return GetCoordsTeVCat(file, id, equCoord, assoc);
}

CatalogQuery::ErrorType
CatalogQuery::Exec(const vector<string>& argv,
                   vector<string>& stdoutput,
                   vector<string>& stderror){

  //Create pipes
  int pipeOut[2];
  pipe(pipeOut);

  int pipeErr[2];
  pipe(pipeErr);

  //Fork
  pid_t pid = fork();

  if (pid < 0){
    log_error("Fork failed");
    return FORK_FAILED;
  }
    
  //*** Done by child ***
  if(pid == 0){

    //Redirect pipes
    close(pipeOut[0]);
    dup2(pipeOut[1], STDOUT_FILENO);
    
    close(pipeErr[0]);
    dup2(pipeErr[1], STDERR_FILENO);
    
    //Put in char argv format
    int argc = argv.size();

    char* cargv[argc+1];

    for(int i=0; i<argc; ++i)
      cargv[i] = const_cast<char*>(argv[i].c_str());
    
    cargv[argc] = NULL;

    //Execute
    execvp(cargv[0], cargv);
    
  }
    
  //*** Done by parent ***

  //Save to stdout and stderr to vector
  close(pipeOut[1]);
  close(pipeErr[1]);

  FILE* fileOut = fdopen(pipeOut[0], "r");
  FILE* fileErr = fdopen(pipeErr[0], "r");

  stdoutput.clear();
  stderror.clear();
  
  char line[256];

  bool lastHasEOL = true;
  while(fgets(line, sizeof(line), fileOut)){

    //Check if has the end of the line
    char *newlinePtr = strchr(line, '\n');
    bool hasEOL = false;
    if (newlinePtr != NULL){
      *newlinePtr = '\0'; //Remove newline character
      hasEOL = true;
    }

    //Add element or append to last one
    if(lastHasEOL)
      stdoutput.push_back(line);
    else
      stdoutput.back() += line;

    lastHasEOL = hasEOL;
      
  }

  
  lastHasEOL = true;
  while(fgets(line, sizeof(line), fileErr)){

    //Check if has the end of the line
    char *newlinePtr = strchr(line, '\n');
    bool hasEOL = false;
    if (newlinePtr != NULL){
      *newlinePtr = '\0'; //Remove newline character
      hasEOL = true;
    }

    //Add element or append to last one
    if(lastHasEOL)
      stderror.push_back(line);
    else
      stderror.back() += line;

    lastHasEOL = hasEOL;
    
  }
  
  //Wait for child and check exit code
  int childStatus = 0;
  waitpid(pid, &childStatus, 0);
  
  if (!WIFEXITED(childStatus) || WEXITSTATUS(childStatus) != 0)
    return CHILD_ERROR;

  return OK;
}

//By @xperroni in stack overflow
string CatalogQuery::URLencode(const std::string& str) {

  ostringstream escaped;
  escaped.fill('0');
  escaped << hex;

  for (string::const_iterator i = str.begin(), n = str.end(); i != n; ++i) {
    string::value_type c = (*i);

    // Keep alphanumeric and other accepted characters intact
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      escaped << c;
      continue;
    }

    // Any other characters are percent-encoded
    escaped << uppercase;
    escaped << '%' << setw(2) << int((unsigned char) c);
    escaped << nouppercase;
  }

  return escaped.str();
}


CatalogQuery::ErrorType
CatalogQuery::GetTeVCatSourceList(const string& file,
                                  CatalogQuery::Catalog& tevcat){

  //***** Get data from webpage *****
  
  ifstream tevcatFile(file.c_str());

  string data = "";
  
  if (tevcatFile.is_open()){

    string line;
    
    while ( getline (tevcatFile,line) ) {

      if (line.find("var dat  =") != string::npos) {
        
        vector<string> lineParts;
        split(lineParts, line, is_any_of("\""));

        data = lineParts[1];

        break;
        
      }
      
    }

    tevcatFile.close();
    
  }else{
    log_fatal("Unable to open " << file);
  }

  if(data == ""){

    log_error("Unable to parse TeVCat file. Format might have changed");

    return PARSING_ERROR;
    
  }
  
  //***** Extract sources from data ******

  //Clean and decode
  int nd = data.size();
  nd -= nd % 4 ? nd % 4 : 4;
  data = data.substr(0, nd);
  data = base64_decode(data);
  data = data.substr(0, data.rfind("}") + 1);

  //Fill map
  tevcat.Clear();
  
  cJSON* jdata = cJSON_Parse(data.c_str());
  cJSON* sources = cJSON_GetObjectItem(jdata, "sources");

  if(!sources){

    log_error("Unable to parse TeVCat file. Format might have changed");
    
    return PARSING_ERROR;
    
  }else{

    for(int i = 0; i < cJSON_GetArraySize(sources); ++i) {

      cJSON* source = cJSON_GetArrayItem(sources, i);

      cJSON* jName = cJSON_GetObjectItem(source, "canonical_name"); 
      cJSON* jAssociations = cJSON_GetObjectItem(source, "other_names"); 
      cJSON* jRA = cJSON_GetObjectItem(source, "coord_ra");
      cJSON* jDec = cJSON_GetObjectItem(source, "coord_dec");

      if(jName && jAssociations && jRA && jDec){
        
        string name(jName->valuestring);
        trim(name);

        string raTrad(jRA->valuestring);
        vector<string> raSplit;
        split(raSplit, raTrad, is_any_of(" "));
        
        string decTrad(jDec->valuestring);
        vector<string> decSplit;
        split(decSplit, decTrad, is_any_of(" "));

        if(raSplit.size() < 3 || decSplit.size() < 3){

          log_error("Unable to parse TeVCat file. Format might have changed");
          
          return PARSING_ERROR;
          
        }
        
        double ra = HrMinSec(atof(raSplit[0].c_str()) * hour,
                             atof(raSplit[1].c_str()) * minute,
                             atof(raSplit[2].c_str()) * second);

        double decDeg = atof(decSplit[0].c_str());
        
        double dec = sign(decDeg) *
                            DegMinSec(abs(decDeg) * degree,
                                      atof(decSplit[1].c_str()) * arcmin,
                                      atof(decSplit[2].c_str()) * arcsec);
        
        char* cAssociations = jAssociations->valuestring;
        vector<string> associations;
          
        if(cAssociations){

          string sAssociations(cAssociations);

          replace_all(sAssociations, "<br>", ",");
          
          split(associations, sAssociations, is_any_of(","));

          for(int s = 0; s < associations.size(); ++s)
            trim(associations[s]);
          
        }


        tevcat.AddEntry(name, EquPoint(ra, dec), associations);
        
      }else{

        log_error("Unable to parse TeVCat file. Format might have changed");
        
        return PARSING_ERROR;
    
      }
      
    }

  }

  cJSON_Delete(jdata);
  
  return OK;
  
}

CatalogQuery::ErrorType
CatalogQuery::DownloadTeVCat(string& path){

  log_info("Fetching TeVCat webpage...");

  //Construct file name with date
  time_t     now = time(0);
  struct tm  tstruct;
  char       ctcFile[80];
  tstruct = *localtime(&now);
  strftime(ctcFile, sizeof(ctcFile), "tevcat_data_%Y-%m-%d.txt", &tstruct);
  
  path = ctcFile;

  //Download
  vector<string> command;
  command += "curl", "http://tevcat.uchicago.edu/", "-o", path;

  vector<string> stdoutput;
  vector<string> stderror;
  if(Exec(command, stdoutput, stderror)){
    
    log_error("Error fetching TeVCat page:\n"
              << join(stderror,"\n"));
    return CHILD_ERROR;
    
  }

  return OK;

}

bool CatalogQuery::SameNameQ(string id1, string id2){

  replace_all(id1," ","");
  replace_all(id2," ","");

  return iequals(id1,id2);

}

CatalogQuery::ErrorType
CatalogQuery::ConeSearchTeVCat(const EquPoint& coords,
                               double radius,
                               Catalog& sources){

  //Download
  string file;
  ErrorType status = DownloadTeVCat(file);

  if(status)
    return status;
  
  //Call real function
  return ConeSearchTeVCat(file, coords, radius, sources);
  
}

CatalogQuery::ErrorType
CatalogQuery::ConeSearchTeVCat(const string& file,
                               const EquPoint& coords,
                               double radius,
                               Catalog& sources){

  Catalog tc;
  ErrorType status = GetTeVCatSourceList(file, tc);
  
  if(status)
    return status;
  
  for(int i = 0; i < tc.Size(); ++i){

    EquPoint sCoords(tc[i].RA(), tc[i].Dec());

    if(coords.Angle(sCoords) < radius){

      sources.AddEntry(tc[i]);
      
    }
    
  }

  return OK;

}


CatalogQuery::ErrorType
CatalogQuery::ConeSearchFermicat(const string& file,
                                 const EquPoint& coords,
                                 double radius,
                                 Catalog& sources){
  
  Catalog fermicat;
  ErrorType status = GetFermicatSourceList(file, fermicat);

  if(status)
    return status;
  
  for(int i = 0; i < fermicat.Size(); ++i){
    
    EquPoint sCoords(fermicat[i].RA(), fermicat[i].Dec());
    
    if(coords.Angle(sCoords) < radius){
      
      sources.AddEntry(fermicat[i]);
      
    }
    
  }
  
  return OK;

}

CatalogQuery::ErrorType
CatalogQuery::GetFermicatSourceList(const string& file,
                                    Catalog& fermicat){

  //Open FITS file
  fitshandle handle;
  handle.open(file);
  handle.goto_hdu(2);

  //Get column names
  vector<string> colnames;
  for(int c = 1; c <= handle.ncols(); ++c)
    colnames.push_back( handle.colname(c) );

  //Read name and coordinates
  int colnum;

  arr<string> name;
  if(FindIndex(colnames, string("Source_Name"), colnum))
    handle.read_entire_column(colnum+1, name);
  else{
    log_error("'Source_Name' column not found");
    return FORMAT_ERROR;
  }
      
  arr<double> ra;
  if(FindIndex(colnames, string("RAJ2000"), colnum))
    handle.read_entire_column(colnum+1, ra);
  else{
    log_error("'RAJ2000' column not found");
    return FORMAT_ERROR;
  }
  
  arr<double> dec;
  if(FindIndex(colnames, string("DEJ2000"), colnum))
    handle.read_entire_column(colnum+1, dec);
  else{
    log_error("'DEJ2000' column not found");
    return FORMAT_ERROR;
  }

  int nentries = name.size();

  //Add associations
  vector<vector<string> > associations(nentries);

  if(FindIndex(colnames, string("ASSOC_GAM"), colnum)){

    arr<string> assoc;
    handle.read_entire_column(colnum+1, assoc);

    for(int i = 0; i < nentries; ++i){

      trim(assoc[i]);

      if(assoc[i] != "")
        associations[i].push_back(assoc[i]);
        
    }
    
  }

  if(FindIndex(colnames, string("ASSOC_GAM1"), colnum)){

    arr<string> assoc;
    handle.read_entire_column(colnum+1, assoc);

    for(int i = 0; i < nentries; ++i){

      trim(assoc[i]);

      if(assoc[i] != "")
        associations[i].push_back(assoc[i]);
        
    }
    
  }

  if(FindIndex(colnames, string("ASSOC_GAM2"), colnum)){

    arr<string> assoc;
    handle.read_entire_column(colnum+1, assoc);

    for(int i = 0; i < nentries; ++i){

      trim(assoc[i]);

      if(assoc[i] != "")
        associations[i].push_back(assoc[i]);
        
    }
    
  }

  if(FindIndex(colnames, string("ASSOC_GAM3"), colnum)){

    arr<string> assoc;
    handle.read_entire_column(colnum+1, assoc);

    for(int i = 0; i < nentries; ++i){

      trim(assoc[i]);

      if(assoc[i] != "")
        associations[i].push_back(assoc[i]);
        
    }
    
  }
  
  if(FindIndex(colnames, string("ASSOC_TEV"), colnum)){

    arr<string> assoc;
    handle.read_entire_column(colnum+1, assoc);

    for(int i = 0; i < nentries; ++i){

      trim(assoc[i]);

      if(assoc[i] != "")
        associations[i].push_back(assoc[i]);
        
    }
    
  }


  if(FindIndex(colnames, string("ASSOC1"), colnum)){

    arr<string> assoc;
    handle.read_entire_column(colnum+1, assoc);

    for(int i = 0; i < nentries; ++i){

      trim(assoc[i]);

      if(assoc[i] != "")
        associations[i].push_back(assoc[i]);
        
    }
    
  }

  if(FindIndex(colnames, string("ASSOC2"), colnum)){

    arr<string> assoc;
    handle.read_entire_column(colnum+1, assoc);

    for(int i = 0; i < nentries; ++i){

      trim(assoc[i]);

      //Unidentified point sources inside extended ones
      //are marked by “xxx field” in the ASSOC2
      if(assoc[i].size()>5 && assoc[i].substr( assoc[i].size()-5 ) == "field")
        assoc[i] = assoc[i].substr(0, assoc[i].size()-5);
      
      if(assoc[i] != "")
        associations[i].push_back(assoc[i]);
        
    }
    
  }

  handle.close();
  
  //Fill catalog
  fermicat.Clear();
  
  for(int i = 0; i < nentries; ++i){

    //Delete duplicates
    sort( associations[i].begin(), associations[i].end() );
    associations[i].erase(
      unique( associations[i].begin(), associations[i].end() ),
      associations[i].end() );

    //Add entry
    fermicat.AddEntry(name[i],
                      EquPoint(ra[i] * degree, dec[i] * degree),
                      associations[i]);
    
  }
  
  return OK;
  
}
