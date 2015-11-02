/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
//  file:   TgcCoinDataCnv_p1.cxx
//* Takashi Kubota - June 30, 2008 */
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonTrigCoinData/TgcCoinData.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinDataCnv_p1.h"
#include <algorithm>
#include "DataModel/tools/IdentContIndex.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

void TgcCoinDataCnv_p1::
persToTrans( const Muon::TgcCoinData_p1 *persObj, Muon::TgcCoinData *transObj,MsgStream &log ) 
{
   //log << MSG::DEBUG << "TgcCoinDataCnv_p1::persToTrans" << endreq;
   
   transObj->m_channelIdIn = persObj->m_channelIdIn; 
   transObj->m_channelIdOut = persObj->m_channelIdOut; 
   transObj->m_collectionIdHash = persObj->m_collectionIdHash;
   transObj->m_indexAndHash.setHashAndIndex(persObj->m_indexAndHash);
   
   switch(persObj->m_type){
   case 0: transObj->m_type=Muon::TgcCoinData::TYPE_TRACKLET; break;
   case 1: transObj->m_type=Muon::TgcCoinData::TYPE_HIPT; break;
   case 2: transObj->m_type=Muon::TgcCoinData::TYPE_SL; break;
   case 3: transObj->m_type=Muon::TgcCoinData::TYPE_UNKNOWN; break;
   case 4: transObj->m_type=Muon::TgcCoinData::TYPE_TRACKLET_EIFI; break;
   default: break;
   }
   //   transObj->m_type = persObj->m_type; 
   transObj->m_isAside = persObj->m_isAside; 
   transObj->m_phi = persObj->m_phi; 
   transObj->m_isForward = persObj->m_isForward; 
   transObj->m_isStrip = persObj->m_isStrip; 
   transObj->m_trackletId = persObj->m_trackletId; 
   transObj->m_trackletIdStrip = persObj->m_trackletIdStrip; 
   transObj->m_widthIn = persObj->m_widthIn; 
   transObj->m_widthOut = persObj->m_widthOut; 

   transObj->m_posIn = createTransFromPStore( &m_localPosCnv, persObj->m_posIn, log );
   transObj->m_posOut = createTransFromPStore( &m_localPosCnv, persObj->m_posOut, log );
   Trk::ErrorMatrix dummy;
   fillTransFromPStore( &m_errorMxCnv, persObj->m_errMat, &dummy, log );
   Amg::MatrixX* tempMat = new Amg::MatrixX;
   EigenHelpers::vectorToEigenMatrix(dummy.values, *tempMat, "TgcCoinDataCnv_p1");
   transObj->m_errMat =  tempMat;

   transObj->m_delta = persObj->m_delta; 
   transObj->m_roi = persObj->m_roi; 
   transObj->m_pt = persObj->m_pt; 
}

void TgcCoinDataCnv_p1::
transToPers( const Muon::TgcCoinData *transObj, Muon::TgcCoinData_p1 *persObj, MsgStream &log )
{
   //log << MSG::DEBUG << "TgcCoinDataCnv_p1::transToPers" << endreq;
 
   persObj->m_channelIdIn = transObj->m_channelIdIn.get_identifier32().get_compact(); 
   persObj->m_channelIdOut = transObj->m_channelIdOut.get_identifier32().get_compact(); 
   persObj->m_collectionIdHash = transObj->m_collectionIdHash;
   persObj->m_indexAndHash=transObj->m_indexAndHash.hashAndIndex();
   switch(transObj->m_type){
   case Muon::TgcCoinData::TYPE_TRACKLET: persObj->m_type=0; break;
   case Muon::TgcCoinData::TYPE_HIPT: persObj->m_type=1;  break;
   case Muon::TgcCoinData::TYPE_SL: persObj->m_type=2; break;
   case Muon::TgcCoinData::TYPE_UNKNOWN: persObj->m_type=3; break;
   case Muon::TgcCoinData::TYPE_TRACKLET_EIFI: persObj->m_type=4; break;
   default: break;
   }  
   //persObj->m_type = transObj->m_type; 
   persObj->m_isAside = transObj->m_isAside; 
   persObj->m_phi = transObj->m_phi; 
   persObj->m_isForward = transObj->m_isForward; 
   persObj->m_isStrip = transObj->m_isStrip; 
   persObj->m_trackletId = transObj->m_trackletId; 
   persObj->m_trackletIdStrip = transObj->m_trackletIdStrip; 
   persObj->m_widthIn = transObj->m_widthIn; 
   persObj->m_widthOut = transObj->m_widthOut; 
   persObj->m_posIn = toPersistent( &m_localPosCnv, transObj->m_posIn, log );
   persObj->m_posOut = toPersistent( &m_localPosCnv, transObj->m_posOut, log );
   // persObj->m_errMat = toPersistent( &m_errorMxCnv, transObj->m_errMat, log );
   if (transObj->m_errMat){
     Trk::ErrorMatrix pMat;
     EigenHelpers::eigenMatrixToVector(pMat.values, *transObj->m_errMat, "TgcCoinDataCnv_p1");
     persObj->m_errMat = toPersistent( &m_errorMxCnv, &pMat, log );
   }
   persObj->m_delta = transObj->m_delta; 
   persObj->m_roi = transObj->m_roi; 
   persObj->m_pt = transObj->m_pt; 
}


