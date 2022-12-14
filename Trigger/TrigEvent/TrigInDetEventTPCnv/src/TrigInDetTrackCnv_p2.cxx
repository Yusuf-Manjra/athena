/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p2.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p2.h"

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigInDetTrackCnv_p2::persToTrans( const TrigInDetTrack_p2 *persObj,
			       TrigInDetTrack    *transObj,
			       MsgStream       &log )
{

  log << MSG::DEBUG << "TrigInDetTrackCnv_p2::persToTrans called " << endmsg;

  transObj->algorithmId( ( TrigInDetTrack::AlgoId)persObj->m_algId );
  transObj->chi2        (persObj->m_chi2)       ;
  transObj->StrawHits   (persObj->m_NStrawHits) ;
  transObj->Straw       (persObj->m_NStraw)     ;
  transObj->StrawTime   (persObj->m_NStrawTime) ;
  transObj->TRHits      (persObj->m_NTRHits)    ;  

  transObj->NPixelSpacePoints (persObj->m_NPixelSpacePoints);
  transObj->NSCT_SpacePoints  (persObj->m_NSCT_SpacePoints);
  transObj->HitPattern        (persObj->m_HitPattern);

  transObj->param      (createTransFromPStore( &m_fpCnv, persObj->m_param, log ));
  transObj->endParam   (createTransFromPStore( &m_fpCnv, persObj->m_endParam, log ));
  
}

//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigInDetTrackCnv_p2::transToPers( const TrigInDetTrack    *transObj,
			       TrigInDetTrack_p2 *persObj,
			       MsgStream       &log )
{

  log << MSG::DEBUG << "TrigInDetTrackCnv_p2::transToPers called " << endmsg;

  persObj->m_algId       = transObj->algorithmId()      ;
  persObj->m_chi2        = transObj->chi2()       ;
  persObj->m_NStrawHits  = transObj->NStrawHits() ;
  persObj->m_NStraw      = transObj->NStraw()     ;
  persObj->m_NStrawTime  = transObj->NStrawTime() ;
  persObj->m_NTRHits     = transObj->NTRHits()    ;  
  persObj->m_NPixelSpacePoints = transObj->NPixelSpacePoints();
  persObj->m_NSCT_SpacePoints = transObj->NSCT_SpacePoints();
  persObj->m_HitPattern = transObj->HitPattern();

  persObj->m_param       = toPersistent( &m_fpCnv, transObj->param(), log );
  persObj->m_endParam    = toPersistent( &m_fpCnv, transObj->endParam(), log );
}
