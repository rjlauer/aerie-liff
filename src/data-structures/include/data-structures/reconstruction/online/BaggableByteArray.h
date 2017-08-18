/*!
 * @file BaggableByteArray.h 
 * @brief Typedefs for Baggable vectors of byte-sized data.
 * @author Jim Braun
 * @version $Id: BaggableByteArray.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */ 

#ifndef DATA_STRUCTURES_RECO_ONLINE_BAGGABLEBYTEARRAY_H_INCLUDED
#define DATA_STRUCTURES_RECO_ONLINE_BAGGABLEBYTEARRAY_H_INCLUDED

#include <hawcnest/processing/BaggableVector.h>
#include <hawcnest/HAWCNest.h>

typedef BaggableVector<char> BaggableByteArray;
SHARED_POINTER_TYPEDEFS(BaggableByteArray);

typedef BaggableVector<BaggableByteArray> BaggableByteArrayVector;
SHARED_POINTER_TYPEDEFS(BaggableByteArrayVector);

#endif // DATA_STRUCTURES_RECO_ONLINE_BAGGABLEBYTEARRAY_H_INCLUDED
