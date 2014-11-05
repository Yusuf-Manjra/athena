/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4SIMCNVDICT_H
#define G4SIMCNVDICT_H

#include "G4SimTPCnv/TrackRecordCollectionCnv_p2.h"
#include "G4SimTPCnv/TrackRecord_p1.h"
#include "G4SimTPCnv/TrackRecordCollection_p1.h"
#include "G4SimTPCnv/TrackRecordCollection_p2.h"

// For Root streamer:
// for ROOT streamer
#include "G4SimTPCnv/TrackRecord_p0.h"
#include "G4SimTPCnv/TrackRecordStreamer_p0.h"

struct GCCXML_DUMMY_INSTANTIATION_G4SIMTPCNV {
  T_TPCnv<TrackRecordCollection,TrackRecordCollection_p2> m_cnv;
};

#endif

