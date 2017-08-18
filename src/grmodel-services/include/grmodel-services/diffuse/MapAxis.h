
#ifndef GRMODEL_SERVICES_DIFFUSE_MAPAXIS_H_INCLUDED
#define GRMODEL_SERVICES_DIFFUSE_MAPAXIS_H_INCLUDED

#include <hawcnest/PointerTypedefs.h>

/*!
 * @class MapAxis
 * @brief Class that handles binning of 2D and 3D maps along one dimension
 * @author Segev BenZvi
 * @date 5 Apr 2011
 */
class MapAxis {
  
  public:

    MapAxis() :
      nx_(0), x0_(0), x1_(0) { }

    /// Construct a map with nx bins, x0 start value, and x1 end value
    MapAxis(const long nx, const float x0, const float x1) :
      nx_(nx), x0_(x0), x1_(x1), dx_((x1-x0)/nx) { }

   ~MapAxis() { }

    /// Construct a map with nx bins, x0 start value, and x1 end value
    void SetAxis(const long nx, const float x0, const float x1) {
      nx_ = nx;
      x0_ = x0;
      x1_ = x1;
      dx_ = (x1-x0) / nx;
    }

    /// Get the bin number corresponding to a particular axis value x
    int GetBin(const float x) const { return int(nx_ * (x-x0_)/(x1_-x0_)); }

    /// Get the central value of bin i
    float GetBinCenter(const int i) const { return x0_ + (i+0.5)*dx_; }

    long GetN() const { return nx_; }
    float GetXMin() const { return x0_; }
    float GetXMax() const { return x1_; }
    float GetDX() const { return dx_; }

  private:

    long nx_;     ///< Number of bins
    float x0_;    ///< Start value (lower edge of lowest bin)
    float x1_;    ///< End value (upper edge of highest bin)
    float dx_;    ///< Distance between bin centers

};

SHARED_POINTER_TYPEDEFS(MapAxis);

#endif // GRMODEL_SERVICES_DIFFUSE_MAPAXIS_H_INCLUDED

