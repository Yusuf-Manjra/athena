/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @class  : TrigTauClusterCnv_p3
 *
 *
 * @author M. Pilar Casado <casado@ifae.es>  - IFAE/UAB - Barcelona
 *
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCNV_P3_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigCaloEventTPCnv/TrigTauCluster_p3.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

class MsgStream;

class TrigTauClusterCnv_p3  : public T_AthenaPoolTPCnvConstBase<TrigTauCluster, TrigTauCluster_p3>  {
    
  public:
    using base_class::transToPers;
    using base_class::persToTrans;

    TrigTauClusterCnv_p3() {}

    virtual void persToTrans(const TrigTauCluster_p3* persObj,  TrigTauCluster* transObj,   MsgStream &log ) const override;
    virtual void transToPers(const TrigTauCluster* transObj, TrigTauCluster_p3* persObj, MsgStream &log ) const override;

 protected:

  // to persistify ElementLinks to cluster and track 
  ElementLinkCnv_p3< ElementLink<TrigTauClusterDetailsContainer> >   m_ELinkTauClusterDetailsCnv ;
};
#endif
