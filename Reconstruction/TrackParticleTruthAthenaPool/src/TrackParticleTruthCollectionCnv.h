/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLETRUTHCOLLECTIONCNV_H
#define TRACKPARTICLETRUTHCOLLECTIONCNV_H
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollection_p1.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionCnv_p1.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionCnv_p0.h"

namespace Trk { class TrackParticleTruthCollection_p1; }

typedef Rec::TrackParticleTruthCollection_p1 TrackParticleTruthCollectionPERS;

typedef T_AthenaPoolCustomCnv<TrackParticleTruthCollection, TrackParticleTruthCollectionPERS> TrackParticleTruthCollectionCnvBase;

class TrackParticleTruthCollectionCnv : public TrackParticleTruthCollectionCnvBase
{
  friend class CnvFactory<TrackParticleTruthCollectionCnv>;
protected:
   TrackParticleTruthCollectionCnv(ISvcLocator* svcloc);
  virtual TrackParticleTruthCollection* createTransient();
  virtual TrackParticleTruthCollectionPERS* createPersistent(TrackParticleTruthCollection*);
private:
  TrackParticleTruthCollectionCnv_p0 m_converter_p0;
  TrackParticleTruthCollectionCnv_p1 m_converter_p1;

  static pool::Guid p0_guid;
  static pool::Guid p1_guid;
};

#endif
