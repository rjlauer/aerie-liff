/*!
 * @file Geopack-2008.h
 * @brief C++ wrapper for Fortran IGRF11 routines in Geopack-2008.f
 * @author Segev BenZvi
 * @date 10 Jan 2013
 * @version $Id: Geopack-2008.h 13809 2013-01-17 23:00:21Z sybenzvi $
 */

#ifdef COMPILE_FORTRAN_COMPONENTS

#ifndef ASTRO_SERVICE_GEOPACK2008_H_INCLUDED
#define ASTRO_SERVICE_GEOPACK2008_H_INCLUDED

extern "C" {

  /// Set up internal coordinate transformations (from Geopack-2008.f)
  extern void recalc_08_(int* year, int* day, int* hour, int* min, int* sec,
                         float* Vgsex, float* Vgsey, float* Vgsez);

  /// Convert to/from geocentric to geocentric solar wind coordinates:
  /// output in GSW if j>0, or GEO if j<0
  extern void geogsw_08_(float* xgeo, float* ygeo, float* zgeo,
                         float* xgsw, float* ygsw, float* zgsw, int* j);

  /// Convert spherical coords to cartesian values (j>0) or vice-versa (j<0)
  extern void sphcar_08_(float* r, float* theta, float* phi,
                         float* x, float* y, float* z, int* j);

  /// Calculate geocentric solar-wind (GSW) components of geomagnetic dipole 
  extern void dip_08_(float* xgsw, float* ygsw, float* zgsw,
                      float* Bxgsw, float* Bygsw, float* Bzgsw);

  /// Given spherical geocentric coordinates, calculate the components of the
  /// geomagnetic field using the IAGA International Geomagnetic Reference
  /// (IGRF) Model spherical harmonic coefficients.
  extern void igrf_geo_08_(float* r, float* theta, float* phi,
                           float* Br, float* Btheta, float* Bphi);

}

#endif // ASTRO_SERVICE_GEOPACK2008_H_INCLUDED

#endif // COMPILE_FORTRAN_COMPONENTS

