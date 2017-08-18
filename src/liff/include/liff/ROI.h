/*!
 * @file ROI.H
 * @author Chang Rho
 * @date 18 Sep 2016
 */

#ifndef ROI_H
#define ROI_H

#include <liff/SkyMapCollection.h>

class ROI {

  public:
  
    virtual ~ROI();

    virtual void CalcROI(const SkyMap<double> *skymap, std::vector<SkyPos> roi, rangeset<int>& skymapPixels) = 0;

    const rangeset<int>& GetPixelList() const
    { return pixels_; }

  protected:
  
    rangeset<int> pixels_;
};

class DiscROI : public ROI {

  public:
  
    virtual ~DiscROI();

    virtual void CalcROI(const SkyMap<double> *skymap, std::vector<SkyPos> roi, rangeset<int>& skymapPixels);
};

class PolygonROI : public ROI {

  public:
  
    virtual ~PolygonROI();
  
    virtual void CalcROI(const SkyMap<double> *skymap, std::vector<SkyPos> roi, rangeset<int>& skymapPixels);
};

//Galactic plane diffuse model
class GPDROI: public ROI {

  public:
  
    virtual ~GPDROI();
  
    virtual void CalcROI(const SkyMap<double> *skymap, std::vector<SkyPos> roi, rangeset<int>& skymapPixels);
};

#endif