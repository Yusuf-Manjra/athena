/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/RingerRings.h"

#include "TrigCaloEventTPCnv/RingerRings_p1.h"
#include "TrigCaloEventTPCnv/RingerRingsCnv_p1.h"

void RingerRingsCnv_p1::transToPers(const RingerRings *trans, 
                                    RingerRings_p1 *pers,
				    MsgStream &log) const {

  log << MSG::DEBUG << "RingerRingsCnv_p1::tranToPers" << endmsg;
  
  if ( (!trans) || (!pers) )
    return;

  pers->m_numberOfRings = trans->size();
  pers->m_rings.clear();
  for (unsigned int i = 0; i < trans->size(); ++i) { 
    pers->m_rings.push_back(trans->at(i));
  }
}

void RingerRingsCnv_p1::persToTrans(const RingerRings_p1 *pers, 
                                    RingerRings *trans,
				    MsgStream &log) const {

  log << MSG::DEBUG << "RingerRingsCnv_p1::persToTrans" << endmsg;

  if ( (!pers) || (!trans) )
    return;

  trans->rings().clear();
  for (unsigned int i = 0; i < pers->m_rings.size(); ++i){   
    trans->rings().push_back(pers->m_rings[i]);
  }
}

