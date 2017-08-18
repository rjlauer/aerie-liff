/*!
 * @file SkyMap.h
 * @author Robert Lauer
 * @date 10 Sep 2014
 * @brief A HEALPix map covering only part of the sky.
 * @version $Id: SkyMap.h 35073 2016-10-07 22:33:07Z criviere $
 */

#ifndef SKY_MAP_H
#define SKY_MAP_H

#include <healpix_base.h>
#include <healpix_map.h>

#include <liff/skymaps/MapTree.h>

const int outpix = -1;

/// A (partial) sky map of a given datatype
template<typename T>
class SkyMap: public Healpix_Base {

 public:

  SkyMap() : outside_(outpix) { }

  /// Constructs an empty SkyMap for a rangeset of pixels
  SkyMap(rangeset<int> &pixset, int nside, Healpix_Ordering_Scheme scheme)
      : outside_(outpix) { SetPixelRange(pixset, nside, scheme); }

  /// Constructs a partial map from a healpix map and a rangeset of pixels
  SkyMap(Healpix_Map<T> &map, rangeset<int> &pixset)
      : outside_(outpix) { SetFromMap(map, pixset); }

  /// Constructs a map of a disc for given center/radius from a healpix map
  SkyMap(Healpix_Map<T> &map, pointing ptg, double radius)
      : outside_(outpix) { DiscFromMap(map, ptg, radius); }

  /// Constructs a partial map from a MapTree and a rangeset of pixels
  SkyMap(MapTree &tree, rangeset<int> &pixset)
      : outside_(outpix) { SetFromTree(tree, pixset); }

  /// Constructs a map of a disc for given center/radius from a MapTree
  SkyMap(MapTree &tree, pointing ptg, double radius)
      : outside_(outpix) { DiscFromTree(tree, ptg, radius); }

  /// Deletes the old map and creates a new map with a given order/scheme.
  void Set(int order, Healpix_Ordering_Scheme scheme) {
    Healpix_Base::Set(order, scheme);
    maps_.clear();
    pixels_.clear();
  }

  /// Deletes the old map and creates a new map with a given nside/scheme
  void SetNside(int nside, Healpix_Ordering_Scheme scheme) {
    Healpix_Base::SetNside(nside, scheme);
    maps_.clear();
    pixels_.clear();
  }

  /// Deletes old map and creates an empty map with a given pixel range.
  void SetPixelRange
      (rangeset<int> &pixset, int nside, Healpix_Ordering_Scheme scheme);

  /// Deletes old map and copies a Healpix_Map for a given pixel range.
  void SetFromMap(Healpix_Map<T> &map, rangeset<int> &pixset);

  /// Deletes the old map and copies pixels in a disc from a Healpix_Map
  void DiscFromMap(Healpix_Map<T> &map, pointing ptg, double radius) {
    SetNside(map.Nside(), map.Scheme());
    rangeset<int> disc;
    query_disc(ptg, radius, disc);
    SetFromMap(map, disc);
  }

  /// Keep the same rangeset, fill map with zeros
  void Empty();

  /// Add values from another SkyMap, must have same rangeset
  void Add(const SkyMap map);

  /// Fluctuate the map using Poisson statistics
  void PoissonFluctuate();

  /// Adds Healpix_Map values for pixels in the SkyMap rangeset.
  void AddHealpixMap(Healpix_Map<T> &map, bool poisson=false);

  /// Subtracts Healpix_Map values from all pixels in the SkyMap rangeset.
  void SubHealpixMap(Healpix_Map<T> &map);

  /// Deletes the old map and copies given pixel range from a MapTree.
  void SetFromTree(MapTree &tree, rangeset<int> &pixset);

  /// Deletes the old map and copies pixels in a disc from a MapTree.
  void DiscFromTree(MapTree &tree, pointing ptg, double radius) {
    SetNside(tree.Nside(), tree.Scheme());
    rangeset<int> disc;
    query_disc(ptg, radius, disc);
    SetFromTree(tree, disc);
  }

  /// Adds MapTree values for pixels in the SkyMap rangeset.
  void AddMapTree(MapTree &tree);

  /// Subtracts MapTree values for pixels in the SkyMap rangeset.
  void SubMapTree(MapTree &tree);

  /// Multiplies pixels in the SkyMap rangeset by val
  void Scale(T val);

  /// Defines the pixel value returned for points outside the SkyMap range
  void SetOutsideValue(T val) { outside_ = val; }
  T GetOutsideValue() const { return outside_; }

  /// Sets all pixels inside SkyMap range to a common value
  void SetInsideValue(T val);

  /// Returns a constant reference to the pixel indexed by its healpix number
  const T &operator[](int pix) const;

  /// Changes the value of the pixel indexed by its healpix number
  void SetPixel(int pix, T value);

  /// Add the value to the pixel indexed by its healpix number
  void AddToPixel(int pix, T value);

  /// Fills Healpix_Map with SkyMap values and returns number of pixels
  /// Only use poisson fluctuation for event maps
  const int FillHealpixMap(Healpix_Map<T> &fullmap, bool poisson = false) const;

  /// Returns the defined pixel rangeset
  rangeset<int> GetPixelRange() const { return pixels_; }

  /// Returns the number of defined pixels
  int GetPixelNumber() const { return pixels_.nval(); }

  /// Writes SkyMap as a MapTree into a TFile and returns number of pixels
  const int WriteTreeFile
      (std::string filename, std::string treename) const;

  /// Prints info about the class instance
  void Info() const;

  /// Returns the sum of all pixels
  T GetSum() const;

 private:
  std::vector<arr<T> > maps_;
  rangeset<int> pixels_;
  T outside_;

};
#endif
