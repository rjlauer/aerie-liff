/*!
 * @file PMTChi2Result.h 
 * @brief PMTChi2 of a reconstructed air shower.
 * @author Jim Braun
 * @version $Id: PMTChi2Result.h 24730 2015-04-04 19:35:57Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_RECO_PMTChi2_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RECO_PMTChi2_RESULT_H_INCLUDED

#include <data-structures/reconstruction/RecoResult.h>
#include <hawcnest/HAWCNest.h>

/*!
 * @class PMTChi2Result
 * @author Jim Braun
 * @ingroup core_fitters 
 * @brief PMTChi2 calculated for a shower, given a particular core exclusion radius
 *
 * NOTE: this isn't really a chi2 calculation because it uses individual PMT measurements
 *       which are correlated within each tank
 *
 */
class PMTChi2Result : public RecoResult {

  public:

    PMTChi2Result() : radius_(0.),
                      pmtChi2_(0.) { }

    double GetRadius() const
    { return radius_; }

    double GetPMTChi2() const
    { return pmtChi2_; }

    int GetNCh() const
    { return nch_; }

    int GetNDOF() const
    { return ndof_; }

    void SetRadius(const double radius)
    { radius_ = radius; }

    void SetPMTChi2(const double pmtChi2)
    { pmtChi2_ = pmtChi2; }

    void SetNCh(const int nch)
    { nch_ = nch; }

    void SetNDOF(const int ndof)
    { ndof_ = ndof; }

  protected:

    int nch_;                   ///< number of channels
    int ndof_;                  ///< number of degrees of freedom
    double radius_;             ///< Exclusion radius used to cut away core
    double pmtChi2_;            ///< PMTChi2

};

SHARED_POINTER_TYPEDEFS(PMTChi2Result);

#endif // DATA_STRUCTURES_RECO_PMTChi2_RESULT_H_INCLUDED

