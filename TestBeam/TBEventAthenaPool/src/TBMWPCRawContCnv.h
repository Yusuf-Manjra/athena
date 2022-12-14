/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBMWPCRawContCnv_H
#define TBMWPCRawContCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
#include "TBEvent/TBMWPCRawCont.h"

typedef T_AthenaPoolCnv<TBMWPCRawCont> TBMWPCRawContCnvBase;

class TBMWPCRawContCnv : public TBMWPCRawContCnvBase
{
  friend class CnvFactory<TBMWPCRawContCnv >;
 public:
  TBMWPCRawContCnv(ISvcLocator* svcloc);
  virtual ~TBMWPCRawContCnv();

  /// initialization
  virtual StatusCode initialize() override;

  /// Extend base-class conversion method to modify when reading in
  virtual StatusCode     PoolToDataObject(DataObject*& pObj,
                                          const Token* token,
                                          const std::string& key) override;
};

#endif


