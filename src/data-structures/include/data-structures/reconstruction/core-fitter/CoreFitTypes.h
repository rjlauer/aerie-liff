/*!
 * @file CoreFitTypes.h 
 * @brief  Identifiers for different core fitters. Use with CoreFitResult
 *         so we can determine which core fitter it came from in SummarizeRec.
 * @author Josh Wood
 */ 

#ifndef DATA_STRUCTURES_RECO_CORE_FIT_TYPES_H_INCLUDED
#define DATA_STRUCTURES_RECO_CORE_FIT_TYPES_H_INCLUDED

namespace CoreFitTypes {

  const int COMCoreFit       =  1;
  const int FakeCoreFit      =  2;
  const int GaussCoreFit     =  3;
  const int GLMCoreFit       =  4;
  const int NKGCoreFit       =  5;
  const int SFCF             =  6;
  const int TankGaussCoreFit =  7;
  const int TankGLMCoreFit   =  8;

}

#endif // DATA_STRUCTURES_RECO_CORE_FIT_TYPES_H_INCLUDED
