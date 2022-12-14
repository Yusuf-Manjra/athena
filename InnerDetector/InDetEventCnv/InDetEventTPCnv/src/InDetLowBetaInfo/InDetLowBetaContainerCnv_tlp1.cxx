/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaContainerCnv_tlp1.h"



InDetLowBetaContainerCnv_tlp1::InDetLowBetaContainerCnv_tlp1() {
  // add the "main" base class converter
  addMainTPConverter();

  // Add all converters defined in this top level converter:
  addTPConverter(&m_lowBetaCandidateCnv); 
}

void InDetLowBetaContainerCnv_tlp1::setPStorage(
         InDetLowBetaContainer_tlp1 *storage) {
  setMainCnvPStorage(&storage->m_lowBetaContainers);
  m_lowBetaCandidateCnv.setPStorage(&storage->m_lowBetaCandidate);
}

void T_TPCnv<InDet::InDetLowBetaContainer, InDetLowBetaContainer_tlp1>::persToTrans
            (const InDetLowBetaContainer_tlp1 *pers,
             InDet::InDetLowBetaContainer *trans,
             MsgStream &msg) {
  // FIXME: TPConverter uses the same non-const member m_pStorage
  // for both reading and writing, but we want it to be const
  // in the former case.
  InDetLowBetaContainer_tlp1 *pers_nc ATLAS_THREAD_SAFE =
    const_cast<InDetLowBetaContainer_tlp1*>(pers);
  this->setTLPersObject(pers_nc);
  m_mainConverter.pstoreToTrans(0, trans, msg);
  this->clearTLPersObject();

#if 0
  if (log.level() <= MSG::DEBUG) 
  {
    InDet::InDetLowBetaContainer::iterator it = trans->begin();
    while (it != trans->end()) {
	  if (!(*it)){
		  msg<< MSG::DEBUG << "InDetLowBetaContainer TP container with NULL element "
		     << endmsg;
	  }
	  it++;
    }
  }
#endif
}

void T_TPCnv<InDet::InDetLowBetaContainer, InDetLowBetaContainer_tlp1>::
transToPers(const InDet::InDetLowBetaContainer *trans,
            InDetLowBetaContainer_tlp1 *pers,
            MsgStream &msg) {
//  if (log.level() <= MSG::DEBUG)  msg << MSG::DEBUG << "::transToPers called " << endmsg;
  this->setTLPersObject(pers);
  m_mainConverter.virt_toPersistent(trans, msg);
  this->clearTLPersObject();
}
