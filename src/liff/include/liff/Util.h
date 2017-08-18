/*!
 * @file Util.h
 * @author Patrick Younk
 * @date 25 Jul 2014
 * @brief Utilities
 * @version $Id: Util.h 40017 2017-07-28 19:56:58Z rlauer $
 */

#ifndef LIFF_UTIL_H
#define LIFF_UTIL_H

#include <TVector3.h>
#include <TGraph2D.h>

#include <pointing.h>

#include <liff/MultiSource.h>

//This class hold the sky position.

/*!
 * @class SkyPos
 * @author Patrick Younk
 * @date 25 Jul 2014
 * @ingroup 
 * @brief Holds Sky Position Information. Can return coordinates in different systems.
 */

class SkyPos {

  public:

    SkyPos() { };
    SkyPos(double RA, double Dec, bool Galactic = false); //RA and Dec in degrees
    SkyPos(pointing x);

    double RA() const;
    double Dec() const;

    ///Galactic Latitude, Not implemented
    //double B() {};

    ///Galactic Longitude, Not implemented
    //double L() {};

    double Angle(SkyPos x) const; //In degrees

    TVector3 GetVector() const { return pos_; };

    pointing GetPointing() const;

    void Dump() const;

  private:

    TVector3 pos_;
};


// Functions to load sources from string or files

// Makes a Func1Ptr for a cutoff power law using 4 doubles as input
// If logEmin or logEmax are not NaN, add sharp cut to the spectrum
Func1Ptr
MakeSpectrum(std::string name,
             double norm, double index, double pivotE, double cutoffE,
             double cutEMin=std::numeric_limits<double>::quiet_NaN(),
             double cutEMax=std::numeric_limits<double>::quiet_NaN());

// Makes a Func1Ptr for a cutoff power law using a string like
// 'CutOffPowerLaw,2.62e-11,2.29,42.7' as input
// If logEmin or logEmax are not NaN, add sharp cut to the spectrum
Func1Ptr
MakeSpectrum(std::string name, std::string spectrumString,
             double cutEMin=std::numeric_limits<double>::quiet_NaN(),
             double cutEMax=std::numeric_limits<double>::quiet_NaN(),
             double pivot=1.);

// CR: To do: Put pivot in source file. Eventually per source?
int
AddSourcesFromFile(threeML::MultiSource &sources,
                   std::string fileName,
                   double pivot=1.);

std::vector<std::vector<double> > GetSpectrum(std::string);

TGraph2D* GetScaling(std::string);

std::vector<double> CalculateFluxes(std::vector<double>, std::vector<double>, std::vector<double>);

//the following is a reimplementation of addThresholdMask and getMaskedPixels from map-maker/RoIMask.h
//this copying of code tries to avoid an otherwise unnecessary map-maker dependence
std::vector<int> maskPixels(std::string mapPath, float threshold, bool greater, int nSide);

#endif // LIFF_UTIL_H
