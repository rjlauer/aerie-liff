#ifndef Features_NN_Features_NN_RESULT_H_INCLUDED
#define Features_NN_Features_NN_RESULT_H_INCLUDED

#include <data-structures/reconstruction/RecoResult.h>
#include <hawcnest/HAWCNest.h>

/*!
 * @class FeaturesNNResult
 * @author Tomas Capistran
 * @ingroup Gamma / Hadron Separation
 * @date Oct 20, 2015 
 * @brief 
 */
class FeaturesNNResult : public RecoResult {

  public:

    FeaturesNNResult() :	MaxPE_(0.),
							MaxPEO_(0.),
							MaxPEOChannelID_(0),
							MaxPEChannelID_(0),
							HSM_(0.),
							DisMax_(0.) { }

    double GetMaxPE () const
    { return MaxPE_; }

    double GetMaxPEO () const
    { return MaxPEO_; }

    double GetHSM () const
    { return HSM_; }

    double GetDisMax () const
    { return DisMax_; }

    unsigned GetMaxPEOChannelID () const
    { return MaxPEOChannelID_; }
  
    unsigned GetMaxPEChannelID () const
    { return MaxPEChannelID_; }

    void SetMaxPE (const double MaxPE)
    { MaxPE_ = MaxPE; }

    void SetMaxPEO (const double MaxPEO)
    { MaxPEO_ = MaxPEO; }

    void SetHSM(const double HSM)
    { HSM_ = HSM; }

    void SetDisMax (const double DisMax)
    { DisMax_ = DisMax; }

    void SetMaxPEOChannelID (const unsigned MaxPEOChannelID)
    { MaxPEOChannelID_ = MaxPEOChannelID; }

    void SetMaxPEChannelID (const unsigned MaxPEChannelID)
    { MaxPEChannelID_ = MaxPEChannelID; }

  protected:

	double MaxPE_;				///< Maximum PE in all events
	double MaxPEO_;				///< Maximum PE outside exclusion 40 meter
	unsigned MaxPEOChannelID_;	///< Channel with max PE outside exclusion 40 meter
	unsigned MaxPEChannelID_;	///< Channel with max PE in all events
	double HSM_;				///< Feature MaxPEO_/MaxPE
	double DisMax_;				///< Distants between two PMTs

};

SHARED_POINTER_TYPEDEFS(FeaturesNNResult);

#endif // Features_NN_Features_NN_RESULT_H_INCLUDED
