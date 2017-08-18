
#ifndef DATA_STRUCTURES_RECO_MULTI_PLANE_FIT_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RECO_MULTI_PLANE_FIT_RESULT_H_INCLUDED

#include <data-structures/reconstruction/RecoResult.h>

#include <data-structures/reconstruction/track-fitter/PropagationPlane.h>

#include <hawcnest/HAWCNest.h>

#include <vector>

/*!
 * @class MultiPlaneFitResult
 * @author Jim Braun
 * @ingroup data_structures
 * @brief Holds the result of the MultiPlaneFit module
 */
class MultiPlaneFitResult : public RecoResult {

  public:

    MultiPlaneFitResult() { }

    class PlaneResult {

      public:

        double errorSize, weight, nx, ny, nz, t;
    };

    void AddPlane(double errorSize, double weight, const PropagationPlane& p) {

      PlaneResult r;
      r.errorSize = errorSize;
      r.weight = weight;
      r.nx = p.GetAxis().GetX();
      r.ny = p.GetAxis().GetY();
      r.nz = p.GetAxis().GetZ();
      r.t = p.GetReferenceTime();
      results_.push_back(r);
    }

    void SetConstantWeight(double constantWeight) {
      constantWeight_ = constantWeight;
    }

    double GetConstantWeight() const {return constantWeight_;}
    double GetConstantFrac() const {

      if (nFit_ == 0) {
        return 0;
      } else {
        return constantWeight_/nFit_;
      }
    }
    const std::vector<PlaneResult>& GetResults() const {return results_;}

  private:

    double constantWeight_;
    std::vector<PlaneResult> results_;
};

SHARED_POINTER_TYPEDEFS(MultiPlaneFitResult);

#endif // DATA_STRUCTURES_RECO_MULTI_PLANE_FIT_RESULT_H_INCLUDED
