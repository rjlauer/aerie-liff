/*!
 * @file SkyMap.cc
 * @author Robert Lauer
 * @date 10 Sep 2014
 * @brief Implementation of a HEALPix map covering only part of the sky.
 * @version $Id: SkyMap.cc 35073 2016-10-07 22:33:07Z criviere $
 */

#include <boost/random.hpp>
#include <boost/random/poisson_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/random_device.hpp>

#include <typeinfo>

#include <liff/skymaps/SkyMap.h>

#include <hawcnest/Logging.h>

using namespace std;

template<typename T>
void SkyMap<T>::SetPixelRange
    (rangeset<int> &pixset, int nside, Healpix_Ordering_Scheme scheme) {
  SetNside(nside, scheme);
  pixels_ = pixset;
  tsize ranges = pixels_.size();
  for (unsigned i = 0; i < ranges; i++) {
    int length = pixels_.ivlen(i);
    arr<T> mappart(length, 0);
    maps_.push_back(mappart);
  }
}

template<typename T>
void SkyMap<T>::SetFromMap(Healpix_Map<T> &map, rangeset<int> &pixset) {
  SetNside(map.Nside(), map.Scheme());
  pixels_ = pixset;
  tsize ranges = pixels_.size();
  for (unsigned i = 0; i < ranges; i++) {
    int length = pixels_.ivlen(i);
    arr<T> mappart(length);
    for (int p = 0; p < length; p++) {
      mappart[p] = map[pixels_.ivbegin(i) + p];
    }
    maps_.push_back(mappart);
  }
}

template<typename T>
void SkyMap<T>::Empty() {
  for (unsigned i = 0; i < pixels_.size(); i++) {
    for (int p = 0; p < pixels_.ivlen(i); p++) {
      maps_[i][p] = 0;
    }
  }
}

template<typename T>
void SkyMap<T>::Add(const SkyMap map) {
#if HEALPIX_VERSION < 330
  if (!(this->GetPixelRange().equals(map.GetPixelRange()))) {
#else
  // CR, 2016-10-09: Untested, I think == started at 3.30, cf:
  // http://healpix.sourceforge.net/downloads.php
  if (!(this->GetPixelRange() == map.GetPixelRange())) {
#endif
    log_fatal("Can not add skymaps with different rangesets");
  }
  if (this->GetOutsideValue() != map.GetOutsideValue()) {
    log_warn("Adding SkyMaps with different outside values, keeping the "
             "first one");
  }
  for (unsigned i = 0; i < pixels_.size(); i++) {
    for (int p = 0; p < pixels_.ivlen(i); p++) {
      maps_[i][p] += map[pixels_.ivbegin(i) + p];
    }
  }
}

template<typename T>
void SkyMap<T>::PoissonFluctuate() {
  boost::mt19937 rndGen;
  typedef boost::variate_generator<boost::mt19937,
                                   boost::poisson_distribution<> > RndPoissonType;
  RndPoissonType rndPoisson(rndGen, boost::poisson_distribution<>(1.));
  for (unsigned i = 0; i < pixels_.size(); i++) {
    for (int p = 0; p < pixels_.ivlen(i); p++) {
      // Re-use the random generator from AddHealpixMap(). Use aerie's instead?
      rndPoisson = RndPoissonType(rndGen, boost::poisson_distribution<>(maps_[i][p]));
      rndPoisson.engine().seed(boost::random::random_device()());
      maps_[i][p] = rndPoisson();
    }
  }
}

template<typename T>
void SkyMap<T>::AddHealpixMap(Healpix_Map<T> &map, bool poisson) {
  if (nside_ != map.Nside()) {
    log_fatal("Nside " << map.Nside() << " of referenced Healpix_Map" <<
        " does not match Nside " << nside_ << " of SkyMap.")
  }
  if (poisson) {
    boost::mt19937 rndGen;
    typedef boost::variate_generator<boost::mt19937,
                                     boost::poisson_distribution<> > RndPoissonType;
    RndPoissonType rndPoisson(rndGen, boost::poisson_distribution<>(1.));
    if (scheme_ != map.Scheme()) {
      swapfunc swapper = (scheme_ == RING) ?
                         &Healpix_Base::ring2nest : &Healpix_Base::nest2ring;
      for (unsigned i = 0; i < pixels_.size(); i++) {
        for (int p = 0; p < pixels_.ivlen(i); p++) {
          rndPoisson = RndPoissonType(rndGen, boost::poisson_distribution<>((T) map[(this->*swapper)(pixels_.ivbegin(i) + p)]));
          rndPoisson.engine().seed(boost::random::random_device()());
          maps_[i][p] += rndPoisson();
        }
      }
    }
    else {
      for (unsigned i = 0; i < pixels_.size(); i++) {
        for (int p = 0; p < pixels_.ivlen(i); p++) {
          rndPoisson = RndPoissonType(rndGen, boost::poisson_distribution<>(map[pixels_.ivbegin(i) + p]));
          rndPoisson.engine().seed(boost::random::random_device()());
          maps_[i][p] += rndPoisson();
        }
      }
    }
  }
  else { // No Poisson fluctuation
    if (scheme_ != map.Scheme()) {
      swapfunc swapper = (scheme_ == RING) ?
                         &Healpix_Base::ring2nest : &Healpix_Base::nest2ring;
      for (unsigned i = 0; i < pixels_.size(); i++) {
        for (int p = 0; p < pixels_.ivlen(i); p++) {
          maps_[i][p] += (T) map[(this->*swapper)(pixels_.ivbegin(i) + p)];
        }
      }
    }
    else {
      for (unsigned i = 0; i < pixels_.size(); i++) {
        for (int p = 0; p < pixels_.ivlen(i); p++) {
          maps_[i][p] += map[pixels_.ivbegin(i) + p];
        }
      }
    }
  }
}

template<typename T>
void SkyMap<T>::SubHealpixMap(Healpix_Map<T> &map) {
  if (nside_ != map.Nside()) {
    log_fatal("Nside " << map.Nside() << " of referenced Healpix_Map" <<
        " does not match Nside " << nside_ << " of SkyMap.")
  }
  else if (scheme_ != map.Scheme()) {
    swapfunc swapper = (scheme_ == RING) ?
                       &Healpix_Base::ring2nest : &Healpix_Base::nest2ring;
    for (unsigned i = 0; i < pixels_.size(); i++) {
      for (int p = 0; p < pixels_.ivlen(i); p++) {
        maps_[i][p] -= (T) map[(this->*swapper)(pixels_.ivbegin(i) + p)];
      }
    }
  }
  else {
    for (unsigned i = 0; i < pixels_.size(); i++) {
      for (int p = 0; p < pixels_.ivlen(i); p++) {
        maps_[i][p] -= map[pixels_.ivbegin(i) + p];
      }
    }
  }
}

template<typename T>
void SkyMap<T>::SetFromTree(MapTree &tree, rangeset<int> &pixset) {
  SetNside(tree.Nside(), tree.Scheme());
  pixels_ = pixset;
  tsize ranges = pixels_.size();
  for (unsigned i = 0; i < ranges; i++) {
    int length = pixels_.ivlen(i);
    arr<T> mappart(length);
    for (int p = 0; p < length; p++) {
      mappart[p] = (T) tree.GetPixel(pixels_.ivbegin(i) + p);
    }
    maps_.push_back(mappart);
  }
}

template<typename T>
void SkyMap<T>::AddMapTree(MapTree &tree) {
  if (nside_ != tree.Nside()) {
    log_fatal("Nside " << tree.Nside() << " of referenced MapTree" <<
        " does not match Nside " << nside_ << " of SkyMap.")
  }
  else if (scheme_ != tree.Scheme()) {
    swapfunc swapper = (scheme_ == RING) ?
                       &Healpix_Base::ring2nest : &Healpix_Base::nest2ring;
    for (unsigned i = 0; i < pixels_.size(); i++) {
      for (int p = 0; p < pixels_.ivlen(i); p++) {
        maps_[i][p] += (T) tree.GetPixel((this->*swapper)(pixels_.ivbegin(i) + p));
      }
    }
  }
  else {
    for (unsigned i = 0; i < pixels_.size(); i++) {
      for (int p = 0; p < pixels_.ivlen(i); p++) {
        maps_[i][p] += (T) tree.GetPixel(pixels_.ivbegin(i) + p);
      }
    }
  }
}

template<typename T>
void SkyMap<T>::SubMapTree(MapTree &tree) {
  if (nside_ != tree.Nside()) {
    log_fatal("Nside " << tree.Nside() << " of referenced MapTree" <<
        " does not match Nside " << nside_ << " of SkyMap.")
  }
  else if (scheme_ != tree.Scheme()) {
    swapfunc swapper = (scheme_ == RING) ?
                       &Healpix_Base::ring2nest : &Healpix_Base::nest2ring;
    for (unsigned i = 0; i < pixels_.size(); i++) {
      for (int p = 0; p < pixels_.ivlen(i); p++) {
        maps_[i][p] -= (T) tree.GetPixel((this->*swapper)(pixels_.ivbegin(i) + p));
      }
    }
  }
  else {
    for (unsigned i = 0; i < pixels_.size(); i++) {
      for (int p = 0; p < pixels_.ivlen(i); p++) {
        maps_[i][p] -= (T) tree.GetPixel(pixels_.ivbegin(i) + p);
      }
    }
  }
}

template<typename T>
void SkyMap<T>::Scale(T val) {
  for (unsigned i = 0; i < pixels_.size(); i++) {
    for (int p = 0; p < pixels_.ivlen(i); p++) {
      maps_[i][p] *= val;
    }
  }
}

template<typename T>
void SkyMap<T>::SetInsideValue(T val) {
  for (unsigned i = 0; i < pixels_.size(); i++) {
    for (int p = 0; p < pixels_.ivlen(i); p++) {
      maps_[i][p] = val;
    }
  }
}

template<typename T>
const T &SkyMap<T>::operator[](int pix) const {
  // const: pixel values can not be changed directly
  tdiff i = pixels_.findInterval(pix);
  return (i != -1) ? maps_[i][pix - pixels_.ivbegin(i)] : outside_;
}

template<typename T>
void SkyMap<T>::SetPixel(int pix, T value) {
  tdiff i = pixels_.findInterval(pix);
  if (i == -1) log_fatal("Pixel ID outside SkyMap range.")
  maps_[i][pix - pixels_.ivbegin(i)] = value;
}

template<typename T>
void SkyMap<T>::AddToPixel(int pix, T value) {
  tdiff i = pixels_.findInterval(pix);
  if (i == -1) log_fatal("Pixel ID outside SkyMap range.")
  maps_[i][pix - pixels_.ivbegin(i)] += value;
}

template<typename T>
const int SkyMap<T>::FillHealpixMap(Healpix_Map<T> &fullmap, bool poisson) const {
  fullmap.SetNside(nside_, scheme_);
  fullmap.fill(T(Healpix_undef));
  int n = 0;
  if (poisson) {
    boost::mt19937 rndGen;
    typedef boost::variate_generator<boost::mt19937,
                                     boost::poisson_distribution<> > RndPoissonType;
    RndPoissonType rndPoisson(rndGen, boost::poisson_distribution<>(1.));
    for (unsigned i = 0; i < pixels_.size(); i++) {
      for (int p = 0; p < pixels_.ivlen(i); p++) {
        if (maps_[i][p] > 0.) {
          rndPoisson = RndPoissonType(rndGen,
                                      boost::poisson_distribution<>(maps_[i][p]));
          rndPoisson.engine().seed(boost::random::random_device()());
          fullmap[pixels_.ivbegin(i) + p] = rndPoisson();
        } else {
          fullmap[pixels_.ivbegin(i) + p] = maps_[i][p];
        }
        n++;
      }
    }
  } else {
    for (unsigned i = 0; i < pixels_.size(); i++) {
      for (int p = 0; p < pixels_.ivlen(i); p++) {
        fullmap[pixels_.ivbegin(i) + p] = maps_[i][p];
        n++;
      }
    }
  }
  return n;
}

template<typename T>
const int SkyMap<T>::WriteTreeFile
    (string filename, string treename) const {
  Healpix_Map<T> fullmap;
  int n = FillHealpixMap(fullmap);
  MapTree tree(fullmap);
  tree.Write(filename, treename);
  tree.CloseFile();
  return n;
}

template<typename T>
void SkyMap<T>::Info() const {
  log_info("Printing info of SkyMap of type "<<typeid(T).name()<<":");
  log_info(" - outside_: "<<outside_);
  log_info(" - maps_.size(): "<<maps_.size());
  log_info(" - pixels_: "<<pixels_);
  log_info(" - sum: "<<this->GetSum());
  return;
}

template<typename T>
T SkyMap<T>::GetSum() const {
  T sum = 0;
  for (unsigned i = 0; i < pixels_.size(); i++) {
    for (int p = 0; p < pixels_.ivlen(i); p++) {
      sum += maps_[i][p];
    }
  }
  return sum;
}

template
class SkyMap<int>;
template
class SkyMap<double>;
template
class SkyMap<float>;
