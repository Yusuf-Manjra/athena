// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGTRUTHEVENTTPCNV_TRIGINDETTRACKTRUTHMAP_OLD
#define TRIGTRUTHEVENTTPCNV_TRIGINDETTRACKTRUTHMAP_OLD

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetTruthEvent/TrigInDetTrackTruth.h"
#include "AthLinks/ElementLink.h"
#include <vector>

class TrigInDetTrackTruthMap_old
{
public:    
  std::vector< ElementLink< TrigInDetTrackCollection > > m_elink_vec;
  std::vector< TrigInDetTrackTruth >                     m_truth_vec;
};


#endif // not TRIGTRUTHEVENTTPCNV_TRIGINDETTRACKTRUTHMAP_OLD
