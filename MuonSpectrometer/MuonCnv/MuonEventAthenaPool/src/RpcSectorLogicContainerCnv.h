/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_RPCSECTORLOGICCNV_H
#define MUONEVENTATHENAPOOL_RPCSECTORLOGICCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonRDO/RpcSectorLogicContainer.h"
#include "RpcSectorLogicContainerCnv_p1.h"

// the latest persistent representation type of RpcSectorLogicContainer
typedef  RpcSectorLogicContainer_p1  PERS;
typedef  T_AthenaPoolCustomCnv<RpcSectorLogicContainer, PERS >   RpcSectorLogicContainerCnvBase;

class RpcSectorLogicContainerCnv : public RpcSectorLogicContainerCnvBase {
friend class CnvFactory<RpcSectorLogicContainerCnv >;
protected:
public:
  RpcSectorLogicContainerCnv (ISvcLocator* svcloc) : RpcSectorLogicContainerCnvBase(svcloc) {}
protected:
  virtual PERS*   createPersistent (RpcSectorLogicContainer* transObj);
  virtual RpcSectorLogicContainer*  createTransient ();
};

#endif
