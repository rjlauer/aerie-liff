/*!
 * @file SkyMapCollection.h
 * @author Robert Lauer
 * @date 10 Sep 2014
 * @brief A collection of SkyMap objects.
 * @version $Id: SkyMapCollection.h 36617 2017-01-01 19:06:17Z imc $
 */

#ifndef SKY_MAP_COLLECTION_H_INCLUDED
#define SKY_MAP_COLLECTION_H_INCLUDED

#include <vector>

#include <liff/skymaps/SkyMap.h>
#include <liff/BinDefinitions.h>
#include <liff/BinList.h>
#include <liff/Util.h>

#include <hawcnest/Logging.h>

#include <data-structures/time/UTCDate.h>

class SkyMapCollection {

  protected:

    static const double defaultTransits_;
    static const SkyPos defaultCenter_;
    static const double defaultRadius_;
    static const double defaultMinDec_;
    static const double defaultMaxDec_;

    static const double dontSetTransits_;
  
  public:
  
    /// Default constructor.
    SkyMapCollection()
        : transits_(defaultTransits_),
          dir_(""),
          center_(defaultCenter_),
          radius_(defaultRadius_),
          minDec_(defaultMinDec_),
          maxDec_(defaultMaxDec_) { }

    /// Constructor taking bin list.
    SkyMapCollection(const std::string& dir, const BinList& binList)
        : transits_(defaultTransits_),
          dir_(dir),
          center_(defaultCenter_),
          radius_(defaultRadius_),
          minDec_(defaultMinDec_),
          maxDec_(defaultMaxDec_) {
      SetBins(binList);
    }
    
    /// Loads list of bins.
    void SetBins(const BinList& binList);

    /// Sets (fractional) number of transits
    void SetTransits(double t);

    double GetTransits() const { return transits_; };

    /// Set map directory, with year/month/day sub-directories
    void SetDir(std::string directory);

    /// Sets center point and radius for partial SkyMap
    void SetDisc(SkyPos position, double radius);

    /// Sets dec range for partial SkyMap
    void SetDecBand(double dec1, double dec2);

    /// Sets polygon sky section
    void SetPolygon(const std::vector<SkyPos>& polygon);

    ///Returns a rangeset of all pixels loaded for a given bin
    rangeset<int> GetPixels(const BinName& nhbin);

    ///Returns loaded region as vector of boundary SkyPos, or as
    ///vector of 2 SkyPos, where first is center and second holds radius as RA
    std::vector<SkyPos> GetSkyPosVector(const BinName& nhbin);

    /// Returns vector of maptree names found for a given UTC range
    /// must set sky region and map directory first
    std::vector<std::string> FindSiderealDayMaps(UTCDate start, UTCDate stop, 
                                                 std::string prefix="maptree")
                                                 const;

    /// Deletes stored maps and loads maps for a given UTC range
    /// must set sky region and map directory first
    std::vector<std::string> LoadSiderealDayMaps(UTCDate start, UTCDate stop,
                                                 std::string prefix="maptree");

    /// Deletes stored maps and loads list of maps
    /// must set sky region and map directory first
    void LoadMapList(std::vector<std::string> maptrees);

    /// Deletes stored maps and loads maps from a MapTree file in a given
    /// analysis-bin range. Optionally set transit signal fraction.
    void LoadMaps(const std::string& file, const BinList& binList,
                  double transits = dontSetTransits_);

    /// Loads integration duration from BinInfo tree inside a MapTree file
    void LoadBinInfo(const std::string& mtFile, bool reset = true);
  
    /// Deletes stored maps and loads all maps from a MapTree file. Optionally
    /// set transit signal fraction.
    inline void LoadMaps(const std::string& file,
                         const double transits = dontSetTransits_) {
      LoadMaps(file, BinList(file), transits);
    }

    /// Deletes stored maps and (re-)initializes with empty maps
    /// must set sky region first
    void InitializeMaps(const BinList& binList, int nside = 512);

    /// Fills model maps with zeros
    void EmptyModelMaps();

    /// Enum of the various ways to write this SkyMapCollection to file
    enum WriteType {
      WRITE_STANDARD, // write event map, background map
      WRITE_MODEL,    // write (background + model) map, background map
      WRITE_RESIDUAL, // write event map, (background + model) map
      WRITE_INJECT,   // write (event + model), background map
    };

    /// Stores this SkyMapCollection to disk as MapTree
    void WriteMapTree(const std::string& filename,
                      WriteType writeType = WRITE_STANDARD,
                      bool poisson = false);

    /// Stores model maps to disk as MapTree
    void WriteModelMapTree(const std::string& filename, bool poisson = false) {
      WriteMapTree(filename, WRITE_MODEL, poisson);
    }

    /// Stores residual maps to disk as MapTree
    void WriteResidualMapTree(const std::string& filename) {
      WriteMapTree(filename, WRITE_RESIDUAL);
    }

    /// Update the event map using the model map. Similar to WriteMapTree(), but
    /// updating the map in memory instead.
    void UpdateWithModel(WriteType writeType = WRITE_MODEL,
                         bool poisson = false);

    /// Returns pointer to event-SkyMap for chosen bin
    SkyMap<double> *GetEventMap(const BinName& nhbin);

    /// Returns pointer to event-SkyMap for the first bin
    SkyMap<double> *GetFirstEventMap();

    /// Returns pointer to model-SkyMap for chosen bin
    SkyMap<double> *GetModelMap(const BinName& nhbin);

    /// Returns pointer to background-SkyMap for chosen bin
    SkyMap<double> *GetBackgroundMap(const BinName& nhbin);

    /// Returns nHit and g/h binning as map-typedef, see BinDefinitions.h
    AnalysisBinMap &GetBins() { return analysisBins_; }

    /// Returns average duration of Direct Integration for chosen bin
    double GetIntegrationDuration(const BinName& nhbin);

    /// Returns total duration of data in chosen bin
    double GetTotalDuration(const BinName& nhbin);

    /// Returns start MJD of of data in chosen bin
    double GetStartMJD(const BinName& nhbin);

    /// Returns stop MJD of of data in chosen bin
    double GetStopMJD(const BinName& nhbin);

    /// Returns number of events of data in chosen bin
    double GetNEvents(const BinName& nhbin);

    /// Returns maximum integration duration of data of chosen bin
    double GetMaxDur(const BinName& nhbin);

    /// Returns minimum integration duration of data of chosen bin
    double GetMinDur(const BinName& nhbin);

    /// Returns map type (sky, moon, etc.) of data of chosen bin
    std::string GetMaptype(const BinName& nhbin);

    /// Returns epoch of data (J2000, current, etc.) of chosen bin
    std::string GetEpoch(const BinName& nhbin);

    ////Obtain the values of all the keys in BinInfo for the chosen bin
    void GetBinInfo(const BinName& nhbin,
                    double &startMJD,
                    double &stopMJD,
                    double &nEvents,
                    double &totDur,
                    double &duration,
                    std::string &maptype,
                    double &maxDur,
                    double &minDur,
                    std::string &epoch);
  
    /// Prints info about SkyMapCollection instance
    void Info()  {
      log_info("center_ " << center_.RA() << " " << center_.Dec());
      log_info("radius_ " << radius_);
      log_info("polygon_ size " << polygon_.size());
      log_info("minDec_ " << minDec_);
      log_info("maxDec_ " << maxDec_);
      log_info("eventMaps_ size " << eventMaps_.size());
      log_info("modelMaps_ size " << modelMaps_.size());
      if (modelMaps_.size()>0) {
        log_info("modelMaps_[0] info: ");
        modelMaps_[0].Info();
      }
      log_info("backgroundMaps_ size " << backgroundMaps_.size());
    }

  private:

    /// Map setters
    void SetEventMap(const BinName& binName, const SkyMap<double> map) {
      eventMaps_[binName] = map;
    }
    void SetBackgroundMap(const BinName& binName, const SkyMap<double> map) {
      backgroundMaps_[binName] = map;
    }
    void SetModelMap(const BinName& binName, const SkyMap<double> map) {
      modelMaps_[binName] = map;
    }

    struct BinInfo {
      double startMJD;
      double stopMJD;
      double nEvents;
      double totalDuration;
      double duration;
      std::string maptype;
      double maxDur;
      double minDur;
      std::string epoch;
      
      BinInfo operator+(const BinInfo &b) {
        BinInfo bi;
        bi.startMJD = std::min(this->startMJD, b.startMJD);
        bi.stopMJD = std::max(this->stopMJD, b.stopMJD);

        if(this->nEvents == -1 || b.nEvents == -1)
          bi.nEvents = -1;
        else
          bi.nEvents = this->nEvents + b.nEvents;

        bi.totalDuration = this->totalDuration + b.totalDuration;
        bi.duration = b.duration; //should be averaged externally

        if(b.maptype == this->maptype)
          bi.maptype = b.maptype;
        else
          bi.maptype = "unknown";

        if(this->maxDur == -1 || b.maxDur == -1)
          bi.maxDur = -1;
        else
          bi.maxDur = std::max(this->maxDur, b.maxDur);

        if(this->minDur == -1 || b.minDur == -1)
          bi.minDur = -1;
        else
          bi.minDur = std::max(this->minDur, b.minDur);
        
        if(b.epoch == this->epoch)
          bi.epoch = b.epoch;
        else
          bi.epoch = "unknown";
        
        return bi;
      }

      void operator+=(const BinInfo &b) {
        BinInfo bi;
        bi = *this + b;
        *this = bi;
      }
    };
    typedef std::map<BinName, BinInfo> BinInfoMap;

    double transits_; // number of transits (not for daily maps)
    std::string dir_; // directory for daily maps, unused in other cases
    // Next 5 lines are for the GetPixels() (used by CalcBin to check the data
    // we need is indeed available) and GetSkyPosVector() (used as an ROI in a
    // call of an InternalModelBin cnostructor).
    // We use (center_ and radius_) or polygon_ or (minDec_ and maxDec_), checking
    // that values are in valid range IN THAT ORDER:
    // if (radius_>0) elif (polygon_.size()>0) else ...
    SkyPos center_;
    double radius_;
    std::vector<pointing> polygon_;
    double minDec_;
    double maxDec_;
    // Actual data: eventMaps_, modelMaps_, backgroundMaps_, analysisBins_,
    // binInfoMap_ MUST have all the same dimension.
    typedef std::map<BinName, SkyMap<double> > MapMap;
    
    MapMap eventMaps_;
    MapMap modelMaps_;
    MapMap backgroundMaps_;
    AnalysisBinMap analysisBins_; // contains the cuts
    BinInfoMap binInfoMap_; // contains the durations

};
#endif
