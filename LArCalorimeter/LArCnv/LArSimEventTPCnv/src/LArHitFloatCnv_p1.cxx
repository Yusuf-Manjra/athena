/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "LArSimEvent/LArHitFloat.h"
#undef private
#undef protected
#include "Identifier/Identifier.h"

#include "LArSimEventTPCnv/LArHit_p1.h"
#include "LArSimEventTPCnv/LArHitFloatCnv_p1.h"


void
LArHitFloatCnv_p1::persToTrans(const LArHit_p1* persObj, LArHitFloat* transObj, MsgStream &log)
{
   log << MSG::DEBUG << "LArHitFloatCnv_p1::persToTrans called " << endreq;

   transObj->m_ID          = Identifier(persObj->m_channelID);
   transObj->m_energy      =  persObj->m_energy;
   transObj->m_time        =  persObj->m_time;
}


void
LArHitFloatCnv_p1::transToPers(const LArHitFloat* transObj, LArHit_p1* persObj, MsgStream &log)
{
   log << MSG::DEBUG << "LArHitFloatCnv_p1::transToPers called " << endreq;
   persObj->m_channelID         = transObj->m_ID.get_identifier32().get_compact();
   persObj->m_energy            =  transObj->m_energy;
   persObj->m_time              =  transObj->m_time;
}
