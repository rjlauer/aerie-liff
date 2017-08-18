/*!
 * @file PnPoly.h
 * @brief Wrapper for pnpoly inclusion test.
 * @author Zig Hampel
 * @date 18 Sep 2015
 * @version $Id$
 */

#ifndef DATACLASSES_MATH_PNPOLY_H_INCLUDED
#define DATACLASSES_MATH_PNPOLY_H_INCLUDED


#include <hawcnest/PointerTypedefs.h>
#include <hawcnest/Logging.h>

#include <string>
#include <vector>

/*!
 * @class PnPoly
 * @brief PnPoly polygon inclusion test
 * @author Zig Hampel
 * @ingroup math
 * @date 18 Sep 2015
 *
 * This class is a wrapper for the implementation of the pnpoly
 * algorithm, which tests the inclusion of a 2d point inside 
 * a polygon defined by its vertices.
 *
 * PnPoly code modified from
 * PNPOLY - Point Inclusion in Polygon Test
 * W. Randolph Franklin (WRF)
 * http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
 *
 */
class PnPoly {

  private:
    
    typedef std::vector<double> vecD;

  public:

    PnPoly() : AreVertsSet_(false), nvert_(0) { }
    PnPoly(vecD x, vecD y);

    virtual ~PnPoly() { };

    void SetPoly(vecD x, vecD y);

    int Eval(double testx, double testy) const;

    const vecD& GetVertX() const 
    {
      if (!AreVertsSet_) 
        log_fatal("PnPoly: GetVerX: The input point vectors are empty. Please fill.");
      return vertx_;
    };
    
    const vecD& GetVertY() const 
    {
      if (!AreVertsSet_) 
        log_fatal("PnPoly: GetVertY: The input point vectors are empty. Please fill.");
      return verty_;
    };

  private:

    bool AreVertsSet_;
    int nvert_;
    vecD vertx_;
    vecD verty_;

};

SHARED_POINTER_TYPEDEFS(PnPoly);

#endif // DATACLASSES_MATH_PNPOLY_H_INCLUDED

