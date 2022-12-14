/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGTAUCLUSTERDETAILSCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGTAUCLUSTERDETAILSCONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigCaloEvent/TrigTauClusterDetailsContainer.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsContainer_p2.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsContainerCnv_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsContainerCnv_p2.h"

typedef TrigTauClusterDetailsContainer_p2   TrigTauClusterDetailsContainer_PERS;

typedef T_AthenaPoolCustomCnv<TrigTauClusterDetailsContainer, TrigTauClusterDetailsContainer_PERS> TrigTauClusterDetailsContainerCnvBase;

class TrigTauClusterDetailsContainerCnv : public TrigTauClusterDetailsContainerCnvBase {

  friend class CnvFactory<TrigTauClusterDetailsContainerCnv>;

 protected:

public:
  TrigTauClusterDetailsContainerCnv( ISvcLocator *svcloc ): TrigTauClusterDetailsContainerCnvBase(svcloc){}
protected:

  virtual TrigTauClusterDetailsContainer_PERS *createPersistent( TrigTauClusterDetailsContainer *transObj);
  virtual TrigTauClusterDetailsContainer      *createTransient();

private:
  TrigTauClusterDetailsContainerCnv_tlp1   m_converter1;
  TrigTauClusterDetailsContainerCnv_p2     m_converter;

};
 
#endif 
