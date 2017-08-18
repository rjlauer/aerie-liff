/*!
 * @file Grid.h
 * @brief Multidimensional array with dimensionality determined at runtime.
 * @author Sam Marinelli
 * @date 27 Jun 2016
 * @version $Id: $
 */

#ifndef DATA_STRUCTURES_CONTAINER_GRID_H_INCLUDED
#define DATA_STRUCTURES_CONTAINER_GRID_H_INCLUDED

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <numeric>
#include <vector>

/*!
 * @class Grid
 * @author Sam Marinelli
 * @ingroup containers
 * @brief Multidimensional array with dimensionality determined at runtime.
 */
template<typename T>
class Grid {

  public:
  
    // Constructor.
    template<typename Iterator>
    Grid(Iterator begin,
         const Iterator end) {
    
      for (; begin != end; ++begin)
        extent_.push_back(*begin);
        
      nDimensions_ = extent_.size();
        
      offset_.resize(nDimensions_ + 1);
      offset_[0] = 1;
      std::partial_sum(extent_.begin(), extent_.end(), offset_.begin() + 1,
        std::multiplies<std::size_t>());
      
      data_.resize(offset_[nDimensions_]);
    
    }
    
    // Element access (non-const).
    template<typename Iterator>
    inline T& Get(const Iterator begin) {
      return data_[Get1DIndex(begin)];
    }
    
    // Element access (const).
    template<typename Iterator>
    inline const T& Get(const Iterator begin) const {
      return data_[Get1DIndex(begin)];
    }
    
    // Get dimensionality.
    inline std::size_t GetNDimensions() const {
      return nDimensions_;
    }
    
    // Get extent in a particular direction.
    inline std::size_t GetExtent(const size_t direction) const {
      return extent_[direction];
    }
    
  protected:
  
    // Data.
    std::size_t nDimensions_;
    std::vector<std::size_t> extent_, offset_;
    std::vector<T> data_;
    
    // Get one-dimensional index.
    template<typename Iterator>
    std::size_t Get1DIndex(Iterator begin) const {
    
      std::size_t ret = 0;
      
      for (std::size_t i = 0; i < nDimensions_; ++i, ++begin)
        ret += *begin * offset_[i];
        
      return ret;
    
    }

};

#endif
