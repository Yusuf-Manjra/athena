/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_RawEvent/LUCID_Digit.h"
#include "LUCID_EventTPCnv/LUCID_Digit_p2.h"
#include "LUCID_EventTPCnv/LUCID_DigitCnv_p2.h"

void LUCID_DigitCnv_p2::persToTrans(const LUCID_Digit_p2* persObj, LUCID_Digit* transObj, MsgStream &log) {

  log << MSG::DEBUG << " In LUCID_DigitCnv_p2::persToTrans " << endreq;

  *transObj = LUCID_Digit (persObj->m_tubeID,
                           persObj->m_npe,
                           persObj->m_npeGas,
                           persObj->m_npePmt,
                           persObj->m_qdc,
                           persObj->m_tdc,
                           persObj->m_isHit);
}

void LUCID_DigitCnv_p2::transToPers(const LUCID_Digit* transObj, LUCID_Digit_p2* persObj, MsgStream& log) {

  log << MSG::DEBUG << " In LUCID_DigitCnv_p2::transToPers " << endreq;
  
  persObj->m_tubeID = transObj->getTubeID();
  persObj->m_npe    = transObj->getNpe();
  persObj->m_npeGas = transObj->getNpeGas();
  persObj->m_npePmt = transObj->getNpePmt();
  persObj->m_qdc    = transObj->getQDC();
  persObj->m_tdc    = transObj->getTDC();
  persObj->m_isHit  = transObj->isHit();
}
