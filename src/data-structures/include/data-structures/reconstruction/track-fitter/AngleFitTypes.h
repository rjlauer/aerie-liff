/*!
 * @file AngleFitTypes.h 
 * @brief  Identifiers for different angle fitters. Use with AngleFitResult
 *         so we can determine which angle fitter it came from in SummarizeRec.
 * @author Josh Wood
 */ 

#ifndef DATA_STRUCTURES_RECO_ANGLE_FIT_TYPES_H_INCLUDED
#define DATA_STRUCTURES_RECO_ANGLE_FIT_TYPES_H_INCLUDED

namespace AngleFitTypes {

  const int GaussPlaneFit = 1;
  const int LHAngleFit    = 2;
  const int MultiPlaneFit = 3;
  const int PDFAngleFit   = 4;

}

#endif // DATA_STRUCTURES_RECO_ANGLE_FIT_TYPES_H_INCLUDED
