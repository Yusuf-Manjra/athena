/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimEvent/TRTUncompressedHit.h"
#include "Identifier/Identifier.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"

#include "InDetSimEventTPCnv/InDetHits/TRT_Hit_p2.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCnv_p2.h"


void
TRT_HitCnv_p2::persToTrans(const TRT_Hit_p2* persObj, TRTUncompressedHit* transObj,
                           MsgStream &log)
{
  HepMcParticleLinkCnv_p2 HepMcPLCnv;
  HepMcParticleLink link;
  HepMcPLCnv.persToTrans(&(persObj->m_partLink),&link, log);

  *transObj = TRTUncompressedHit (persObj-> hitID,
                                  link,
                                  persObj->particleEncoding,
                                  persObj->kineticEnergy,
                                  persObj->energyDeposit,
                                  persObj->preStepX,
                                  persObj->preStepY,
                                  persObj->preStepZ,
                                  persObj->postStepX,
                                  persObj->postStepY,
                                  persObj->postStepZ,
                                  persObj->globalTime);
}


void
TRT_HitCnv_p2::transToPers(const TRTUncompressedHit* transObj, TRT_Hit_p2* persObj,
                           MsgStream &log)
{
  //    if (log.level() <= MSG::DEBUG)  log << MSG::DEBUG << "TRT_HitCnv_p2::transToPers called " << endreq;
  HepMcParticleLinkCnv_p2 HepMcPLCnv;
  persObj->hitID         = transObj-> GetHitID();
  HepMcPLCnv.transToPers(&(transObj->particleLink()),&(persObj->m_partLink), log);
  persObj->particleEncoding = transObj->GetParticleEncoding();
  persObj->kineticEnergy = transObj->GetKineticEnergy();
  persObj->energyDeposit = transObj->GetEnergyDeposit();

  persObj->preStepX      = transObj->GetPreStepX();
  persObj->preStepY      = transObj->GetPreStepY();
  persObj->preStepZ      = transObj->GetPreStepZ();

  persObj->postStepX     = transObj->GetPostStepX();
  persObj->postStepY     = transObj->GetPostStepY();
  persObj->postStepZ     = transObj->GetPostStepZ();
  persObj->globalTime    = transObj->GetGlobalTime();
}
