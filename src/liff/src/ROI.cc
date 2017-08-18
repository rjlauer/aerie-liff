/*!
 * @file ROI.cc
 * @author Chang Rho
 * @date 22 Sep 2016
 * @version $Id: ROI.cc 38538 2017-04-03 18:05:52Z criviere $
 */

#include <hawcnest/HAWCUnits.h>
#include <vector>
#include <liff/ROI.h>
#include <cmath>
#include <data-structures/geometry/R3Transform.h>
#include <data-structures/astronomy/EquPoint.h>
#include <data-structures/astronomy/GalPoint.h>
#include <data-structures/astronomy/AstroCoords.h>

using namespace HAWCUnits;
using std::vector;

// Define rotation matrix for galactic-equatorial transformations at the level
// of this C++ translation unit. This is a hack to get the code independent of
// astro-service so that it can work with python bindings in 3ML. Will think of
// a better approach in the future.
namespace {

  // Galactic north pole in J2000.0 equatorial coordinates
  const EquPoint gnpJ2000(HrMinSec(12*hour, 51*minute, 26.28*second),
                          DegMinSec(27*degree, 7*arcminute, 41.70*arcsecond));

  // Galactic center in J2000.0 equatorial coordinates
  const EquPoint gcnJ2000(HrMinSec(17*hour, 45*minute, 37.2*second),
                         -DegMinSec(28*degree, 56*arcminute, 10.22*arcsecond));

  // Equatorial to galactic rotation matrix
  const Rotate e2gMtx(gcnJ2000.GetPoint(), gnpJ2000.GetPoint(),
                      XAXIS, ZAXIS);

  // Galactic to equatorial rotation matrix
  const Rotate g2eMtx(XAXIS, ZAXIS,
                      gcnJ2000.GetPoint(), gnpJ2000.GetPoint());

  void equ2gal(const EquPoint& equ, GalPoint& gal) {
    gal.SetPoint(e2gMtx * equ.GetPoint());
  }

  void gal2equ(const GalPoint& gal, EquPoint& equ) {
    equ.SetPoint(g2eMtx * gal.GetPoint());
  }

}

ROI::~ROI()
{
}

DiscROI::~DiscROI()
{
}

void DiscROI::CalcROI(const SkyMap<double> *skymap,
                      vector<SkyPos> roi, rangeset<int>& skymapPixels)
{
  skymap->query_disc(roi[0].GetPointing(), roi[1].RA() * HAWCUnits::degree, pixels_);
}

PolygonROI::~PolygonROI()
{
}

void
PolygonROI::CalcROI(const SkyMap<double> *skymap,
                    vector<SkyPos> roi, rangeset<int>& skymapPixels)
{
  vector<pointing> pol;
  for (vector<SkyPos>::iterator p = roi.begin();
         p != roi.end(); p++) {
      pol.push_back(p->GetPointing());
  }

  skymap->query_polygon(pol, pixels_);
}

GPDROI::~GPDROI() {}

void
GPDROI::CalcROI(const SkyMap<double> *skymap,
                vector<SkyPos> roi, rangeset<int>& skymapPixels)
{
  EquPoint cel;
  GalPoint gal;
  pixels_=skymapPixels;
  //cout<<"before filter: "<<pixels_.nval()<<' '<<endl;
  for (int i=0; i<skymapPixels.size(); ++i) {
    for (int j=skymapPixels.ivbegin(i); j<skymapPixels.ivend(i); ++j) {
      int pix = j;
      //cout<<pix<<' '<<endl;
      double RA = SkyPos(skymap->pix2ang(pix)).RA();
      double dec = SkyPos(skymap->pix2ang(pix)).Dec();
      //cout<<"RA is "<<RA<<' '<<endl;
      //cout<<"Dec is "<<dec<< ' '<<endl;
      cel.SetRADec(RA*HAWCUnits::degree, dec*HAWCUnits::degree);
      equ2gal(cel, gal);
      //cout<<"gal is "<<gal<<' '<<endl;
      //cout<<"lat is "<<gal.GetB() / HAWCUnits::degree<<' '<<endl;
      if (abs(gal.GetB() / HAWCUnits::degree) > 1.2) {
        pixels_.remove(pix);
      }
    }
  }
  //cout<<"after filter: "<<pixels_.nval()<<' '<<endl;
}
