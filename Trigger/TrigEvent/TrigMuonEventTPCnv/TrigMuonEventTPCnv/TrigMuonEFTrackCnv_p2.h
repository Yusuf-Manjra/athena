/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @class  : TrigMuonEFTrackCnv_p2
 *
 * @brief transient-persistent converter for TrigMuonEFTrack_p2
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it>  - U.Salento/INFN Le
 * @author Alexander Oh <alexander.oh@cern.ch>         - U. Manchester
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFTRACKCNV_P2_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFTRACKCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "EventCommonTPCnv/P4IPtCotThPhiMCnv_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p2.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"

class MsgStream;


class TrigMuonEFTrackCnv_p2 : public T_AthenaPoolTPCnvBase<TrigMuonEFTrack, TrigMuonEFTrack_p2>
{

 public:

  TrigMuonEFTrackCnv_p2() : m_P4IPtCotThPhiMCnv(0) {}

  virtual void persToTrans(const TrigMuonEFTrack_p2* persObj, TrigMuonEFTrack* transObj, MsgStream &log);
  virtual void transToPers(const TrigMuonEFTrack* transObj, TrigMuonEFTrack_p2* persObj, MsgStream &log);

  protected:
  P4IPtCotThPhiMCnv_p1      *m_P4IPtCotThPhiMCnv;
};


#endif
