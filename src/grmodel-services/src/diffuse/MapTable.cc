/*!
 * @file MapTable.cc
 * @brief Implementation of multi-dimensional flux table.
 * @author Segev BenZvi
 * @date 26 Dec 2012
 * @version $Id: MapTable.cc 13661 2012-12-30 19:40:13Z sybenzvi $
 */

#include <grmodel-services/diffuse/MapTable.h>

#include <fitsio.h>
#include <fitsio2.h>

#include <cstring>
#include <stdexcept>

using namespace std;

namespace {

  bool
  greater_than(const float* d1, const float* d2) { return *d1 > *d2; }

  int
  bisect(const float* xa, const int n, const float x)
  {
    int klo = 0;
    int khi = n-1;
    int k;
    while (khi - klo > 1) {
      k = (khi+klo) >> 1;
      if (xa[k] > x)
        khi = k;
      else
        klo = k;
    }
    return klo;
  }

}

MapTable::MapTable(const std::string& file) :
  ndim_(0),
  naxes_(0),
  data_(0),
  strides_(0),
  ncoord_(0),
  xc_(0),
  xe_(0)
{
  int rc = ReadFITS(file.c_str());
  if (rc) {
    Cleanup();
    throw runtime_error("Could not initialize MapTable");
  }
}

MapTable::~MapTable()
{
  Cleanup();
}

void
MapTable::Cleanup()
{
  if (naxes_)
    delete [] naxes_;
  if (data_)
    delete [] data_;
  if (strides_)
    delete [] strides_;

  if (xc_) {
    for (int i = 0; i < ncoord_; ++i)
      delete [] xc_[i];
    delete [] xc_;
  }

  if (xe_) {
    for (int i = 0; i < ncoord_; ++i)
      delete [] xe_[i];
    delete [] xe_;
  }
}

int
MapTable::Index(int* indices)
  const
{
  int idx = 0;
  for (int i = 0; i < ndim_; ++i)
    idx += indices[i] * strides_[i];
  return idx;
}

int
MapTable::ReadFITS(const char* file)
{
  fitsfile* fits;
  int rc = 0;

  // Open the file
  fits_open_file(&fits, file, READONLY, &rc);
  if (rc)
    return rc;

  // Move to the first (and only) HDU
  int type = 0;
  int hdus;
  fits_get_num_hdus(fits, &hdus, &rc);
  fits_movabs_hdu(fits, 1, &type, &rc);
  if (rc)
    return rc;

  // Read the header and estimate the shape of the table.  Note that the axis
  // counts need to be transposed from the way it is written in the FITS file
  // due to FORTRAN/C array differences.
  fits_get_img_dim(fits, &ndim_, &rc);
  if (rc)
    return rc;

  naxes_ = new long [ndim_];
  fits_get_img_size(fits, ndim_, naxes_, &rc);

  {
    long* naxesTmp = new long [ndim_];
    for (int i = 0; i < ndim_; ++i)
      naxesTmp[i] = naxes_[ndim_ - i - 1];

    delete [] naxes_;
    naxes_ = naxesTmp;
  }

  // Compute the total array size and the strides into each array dimension
  strides_ = new unsigned long [ndim_];
  size_t arraySize;
  strides_[ndim_ - 1] = arraySize = 1;

  for (int i = ndim_ - 1; i >= 0; --i) {
    arraySize *= naxes_[i];
    if (i > 0)
      strides_[i-1] = arraySize;
  }
  data_ = new float [arraySize];

  // Read the data
  long* fpixel = new long [ndim_];
  for (int i = 0; i < ndim_; ++i)
    fpixel[i] = 1;

  fits_read_pix(fits, TFLOAT, fpixel, arraySize, NULL, data_, NULL, &rc);
  delete [] fpixel;
  if (rc)
    return rc;

  // Set up the coordinate axis table
  ncoord_ = ndim_ - 1;
  long* n = naxes_ + 1;

  float xc, x0, dx;
  char buffer[80];

  xc_ = new float* [ncoord_];   // Coordinate centers
  xe_ = new float* [ncoord_];   // Coordinate edges

  for (int i = 0; i < ncoord_; ++i) {
    sprintf(buffer, "CRVAL%d", ndim_-i-1);
    fits_read_key(fits, TFLOAT, buffer, &xc, NULL, &rc);
    if (rc)
      return rc;
    sprintf(buffer, "CDELT%d", ndim_-i-1);
    fits_read_key(fits, TFLOAT, buffer, &dx, NULL, &rc);
    if (rc)
      return rc;

    // Coordinate centers
    xc_[i] = new float [n[i]];
    for (int j = 0; j < n[i]; ++j)
      xc_[i][j] = xc + j*dx;

    // Coordinate edges
    x0 = xc - 0.5*dx;

    xe_[i] = new float [n[i] + 1];
    for (int j = 0; j <= n[i]; ++j)
      xe_[i][j] = x0 + j*dx;
  }

  return rc;
}

int
MapTable::GetBin(const int i, const double x)
  const
{
  long* nc = naxes_ + 1;
  return bisect(xe_[i], nc[i]+1, x);
}

float
MapTable::GetBinCenter(const int i, const double x)
  const
{
  return xc_[i][GetBin(i, x)];
}

double
MapTable::GetMaximum(const double* coord, const int n)
  const
{
  int indices[ndim_];
  memset(indices, 0, sizeof(indices));

  if (n > 0 && coord) {
    long* nc = naxes_ + 1;
    for (int i = 0; i < n; ++i)
      indices[i+1] = bisect(xe_[i], nc[i]+1, coord[i]);
  }

  int idx1 = Index(indices);
  indices[n]++;
  int idx2 = Index(indices);

  return *max_element(data_ + idx1, data_ + idx2);
}

double
MapTable::GetTableValue(const double* coord)
  const
{
  int indices[ndim_];
  memset(indices, 0, sizeof(indices));

  long* nc = naxes_ + 1;
  for (int i = 0; i < ncoord_; ++i)
    indices[i+1] = bisect(xe_[i], nc[i]+1, coord[i]);

  return data_[Index(indices)];
}

double
MapTable::Interpolate(const double* coord, const int n)
  const
{
  long* nc = naxes_ + 1;

  // Simplex-based linear interpolation of the n-D coordinate space in the
  // table.  From IEEE Trans. Comp. 47:894-899 (1998).
  int indices[ndim_];
  memset(indices, 0, sizeof(indices));
  for (int i = 0; i < n; ++i)
    indices[i+1] = bisect(xe_[i], nc[i]+1, coord[i]);

  // Find coordinate location in table and normalize it to a unit cell
  int K[ncoord_-n];
  for (int i = n; i < ncoord_; ++i)
    K[i-n] = bisect(xc_[i], nc[i], coord[i]);

  float x[ncoord_-n];
  for (int i = n; i < ncoord_; ++i)
    x[i-n] = (coord[i] - xc_[i][K[i-n]]) / (xc_[i][K[i-n]+1] - xc_[i][K[i-n]]);

  // Construct a permutation p such that x[p[i]] >= x[p[i+1]]
  float* pp[ncoord_-n];
  int p[ncoord_-n];
  for (int i = 0; i < ncoord_-n; ++i)
    pp[i] = &x[i];
  stable_sort(pp, pp + ncoord_-n, greater_than);
  for (int i = 0; i < ncoord_-n; ++i)
    p[i] = pp[i] - x;

  // Get the D+1 vertices of the n-D simplex containing the input coordinate
  int A[ncoord_-n+1][ncoord_-n];
  memset(A, 0, sizeof(A));
  for (int i = 0; i <= ncoord_-n; ++i) {
    memset(A[i], 0, ncoord_+1);
    if (i == 0) {
      for (int j = 0; j < ncoord_-n; ++j)
        A[i][j] = 0;
    }
    else {
      for (int j = i-1; j >= 0; --j)
        A[i][p[j]] = 1;
    }
  }

  // Interpolate the function using the simplex vertices
  double density = 0.;
  float dx;
  int idx;

  for (int i = n; i <= ncoord_; ++i) {
    for (int j = n; j < ncoord_; ++j)
      indices[j+1] = (i == 0) ? K[j-n] : K[j-n] + A[i-n][j-n];

    if (i == n)
      dx = 1. - x[p[i-n]];
    else if (i == ncoord_)
      dx = x[p[i-n-1]];
    else
      dx = x[p[i-n-1]] - x[p[i-n]];

    idx = Index(indices);
    density += dx * data_[idx];
  }

  return density;
}

