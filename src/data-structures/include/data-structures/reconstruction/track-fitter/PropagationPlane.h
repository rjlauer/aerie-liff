/*!
 * @file PropagationPlane.h
 * @brief Calculations of arrival times for a plane propagating at the speed of
 *        light.
 * @author Jim Braun
 * @version $Id: PropagationPlane.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_RECO_PROPAGATION_PLANE_H_INCLUDED
#define DATA_STRUCTURES_RECO_PROPAGATION_PLANE_H_INCLUDED

#include <data-structures/geometry/Vector.h>
#include <data-structures/geometry/Point.h>
#include <data-structures/physics/PhysicsConstants.h>

/*!
 * @class PropagationPlane
 * @author Jim Braun
 * @ingroup track_fitters
 * @brief Class describing a plane propagating at the speed of light, which
 *        could potentially be modified for any speed.  Arrival time and 
 *        translation calculations are provided.
 */
class PropagationPlane {

  public:

    PropagationPlane() : axis_(0.,0.,0.),
                         point_(0.,0.,0.),
                         t0_(0.),
                         d_(0.),
                         dValid_(false) { }

    virtual ~PropagationPlane() { }

    /// The plane propagation axis
    const Vector& GetAxis() const {return axis_;};

    /// A point lying in the plane at time t0_;
    const Point& GetReferencePoint() const {return point_;}

    /// The reference time of the plane
    const double GetReferenceTime() const {return t0_;}

    /// Set the reference point
    void SetReferencePoint(const double x, const double y, const double z) {
      dValid_ = false;
      point_.SetXYZ(x,y,z);
    }

    /// Set the reference time
    void SetReferenceTime(const double t0) {t0_ = t0;}

    /// Set the propagation axis
    void SetAxis(const double x, const double y, const double z) {
      dValid_ = false;
      axis_.SetXYZ(x,y,z);
      axis_.Normalize();
    }

    void SetAxis(const Vector& axis) {axis_ = axis;}

    /// Set the propagation axis via spherical coordinates
    void SetThetaPhi(const double theta, const double phi) {
      dValid_ = false;
      axis_.SetRThetaPhi(1., theta, phi);
    }

    /// Calculate arrival time of plane at a given point, in nanoseconds,
    /// relative to the start of the event
    double ArrivalTime(const Point& p) const {
      return ArrivalTime(p.GetX(), p.GetY(), p.GetZ());
    }
    double ArrivalTime(const double x, const double y, const double z) const {

      // Check if plane scalar has already been calculated
      if (!dValid_) {
        d_ = -1.*(axis_.GetX()*point_.GetX() +
                  axis_.GetY()*point_.GetY() +
                  axis_.GetZ()*point_.GetZ());
        dValid_ = true;
      }

      // Axis points in direction opposite to propagation
      double dist = axis_.GetX()*x + axis_.GetY()*y + axis_.GetZ()*z + d_;
      return t0_ - dist/PhysicsConstants::c;
    }

    /// Calculate the angular difference between planes, to avoid verbosity
    double Angle(const PropagationPlane& p) {return axis_.Angle(p.axis_);}

    /// Translate the plane by a vector
    void Translate(const Vector& v) {
      point_  += v;
      dValid_ = false;
    }

    void Translate(const double x, const double y, const double z) {
      Vector v(x,y,z);
      Translate(v);
    }

  protected:

    Vector axis_;
    Point  point_;
    double t0_;

  private:

    /// Cache the plane scalar value for fast arrival time calculations
    mutable double d_;
    mutable bool   dValid_;
};

SHARED_POINTER_TYPEDEFS(PropagationPlane);

#endif // DATA_STRUCTURES_RECO_PROPAGATION_PLANE_H_INCLUDED

