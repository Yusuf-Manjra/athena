/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "MuonSimData/CscSimData.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonDigitContainer/CscSimDataCnv_p1.h"

void CscSimDataCnv_p1::persToTrans(const Muon::CscSimData_p1 * persObj, CscSimData * transObj, MsgStream & log){
   log << MSG::DEBUG << "CscSimDataCnv_p1::persToTrans" << endreq;
   transObj->m_word = persObj->m_word;
   log << MSG::VERBOSE << "\tconverting m_word:\t" << persObj->m_word << "\tto\t" << transObj->m_word << endreq;
   transObj->m_deposits.resize(persObj->m_deposits.size());
   for(unsigned int ic=0; ic<persObj->m_deposits.size(); ic++){
      HepMcParticleLink transMcPartLink;
      m_mcpartlinkCnv.persToTrans(&persObj->m_deposits[ic].first, &transMcPartLink, log);
      CscMcData transMcData;
      m_mcdataCnv.persToTrans(&persObj->m_deposits[ic].second, &transMcData, log);
      transObj->m_deposits[ic] = std::make_pair(transMcPartLink, transMcData);
   }
}

void CscSimDataCnv_p1::transToPers( const CscSimData * transObj, Muon::CscSimData_p1 * persObj, MsgStream & log){
   log << MSG::DEBUG << "CscSimDataCnv_p1::transToPers" << endreq;
   persObj->m_word = transObj->m_word;
   log << MSG::VERBOSE << "\tconverting m_word:\t" << transObj->m_word << "\tto\t" << persObj->m_word << endreq;
   persObj->m_deposits.resize(transObj->m_deposits.size());
   for(unsigned int ic=0; ic<transObj->m_deposits.size(); ic++){
      HepMcParticleLink_p1 persMcPartLink;
      m_mcpartlinkCnv.transToPers(&transObj->m_deposits[ic].first, &persMcPartLink, log);
      Muon::CscMcData_p1 persMcData;
      m_mcdataCnv.transToPers(&transObj->m_deposits[ic].second, &persMcData, log);
      persObj->m_deposits[ic] = std::make_pair(persMcPartLink, persMcData);
   }
}
