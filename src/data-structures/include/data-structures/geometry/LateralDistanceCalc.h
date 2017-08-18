/*!
 * @file LatDist.h
 * @brief Wrapper for lateral distance calculators.
 * @author Zig Hampel
 * @date 19 Dec 2016
 * @version $Id$
 */

#ifndef DATACLASSES_MATH_LATDISTCALC_H_INCLUDED
#define DATACLASSES_MATH_LATDISTCALC_H_INCLUDED


#include <hawcnest/PointerTypedefs.h>
#include <hawcnest/Logging.h>
#include <data-structures/geometry/R3Vector.h>

#include <string>
#include <vector>

/*!
 * @class LatDistCalc
 * @brief LatDistCalc lateral distance calculators
 * @author Zig Hampel
 * @ingroup math
 * @date 19 Dec 2016
 *
 * This class is a wrapper for the implementation lateral
 * distance algorithms, which calculate the distance
 * from a point to an axis
 *
 */
class LatDistCalc {

  public:

    LatDistCalc() : IsCoreSet_(false), IsAxisSet_(false), zenith_(0), azimuth_(0) { }
    LatDistCalc(R3Vector core, double zenith, double azimuth);

    virtual ~LatDistCalc() { };

    void SetCore(R3Vector core) { 
      core_ = core;
      IsCoreSet_ = true;
    };

    void SetAxis(double zenith, double azimuth) {
      zenith_ = zenith;
      azimuth_ = azimuth;
      axis_.SetRThetaPhi(1.,zenith_,azimuth_);
      IsAxisSet_ = true;
    };

    double RotLatDist(R3Vector position) const;
    double MinRLatDist(R3Vector position) const;

    void CheckSettings(void) const 
    {
      if (!IsCoreSet_) 
        log_fatal("LatDistCalc: CheckSettings: The core position is not defined. Please define with SetCore(R3Vector core) function");
      if (!IsAxisSet_) 
        log_fatal("LatDistCalc: CheckSettings: The axis is not defined. Please define with SetAxis(double zenith, double azimuth) function");
    };
    

  private:

    bool IsCoreSet_;
    bool IsAxisSet_;
    double zenith_;
    double azimuth_;
    R3Vector core_;
    R3Vector axis_;

};

SHARED_POINTER_TYPEDEFS(LatDistCalc);

#endif // DATACLASSES_MATH_LATDISTCALC_H_INCLUDED

