/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBIdentifiedParticleCnv_H
#define TBIdentifiedParticleCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
#include "TBEvent/TBIdentifiedParticle.h"

typedef T_AthenaPoolCnv<TBIdentifiedParticle> TBIdentifiedParticleCnvBase;

class TBIdentifiedParticleCnv : public TBIdentifiedParticleCnvBase
{
  friend class CnvFactory<TBIdentifiedParticleCnv >;
 public:
  TBIdentifiedParticleCnv(ISvcLocator* svcloc);
  virtual ~TBIdentifiedParticleCnv();

  /// initialization
  virtual StatusCode initialize() override;

  /// Extend base-class conversion method to modify when reading in
  virtual StatusCode     PoolToDataObject(DataObject*& pObj,
                                          const Token* token,
                                          const std::string& key) override;
};

#endif


