///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TileCosmicMuonCnv_p1.cxx 
// Implementation file for class TileCosmicMuonCnv_p1
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 

#include "TileEvent/TileCosmicMuon.h"
#include "TileTPCnv/TileCosmicMuonCnv_p1.h"


void TileCosmicMuonCnv_p1::transToPers(const TileCosmicMuon* transObj, TileCosmicMuon_p1* persObj, MsgStream &/*log*/) const {

  persObj->m_time = transObj->GetTime();
  persObj->m_positionX = transObj->GetPositionX();
  persObj->m_positionY = transObj->GetPositionY();
  persObj->m_positionZ = transObj->GetPositionZ();
  persObj->m_directionPhi = transObj->GetDirectionPhi();
  persObj->m_directionTheta = transObj->GetDirectionTheta();
  persObj->m_fitQuality = transObj->GetFitQuality();
  persObj->m_fitNCells = transObj->GetFitNCells();

  size_t sz = transObj->GetNSamples();
  persObj->m_pathTop.resize(sz);
  persObj->m_pathBottom.resize(sz);
  persObj->m_energyTop.resize(sz);
  persObj->m_energyBottom.resize(sz);
  for (size_t i = 0; i < sz; i++) {
    persObj->m_pathTop[i] = transObj->GetPathTop(i);
    persObj->m_pathBottom[i] = transObj->GetPathBottom(i);
    persObj->m_energyTop[i] = transObj->GetEnergyTop(i);
    persObj->m_energyBottom[i] = transObj->GetEnergyBottom(i);
  }

  size_t ncells = transObj->GetTrackNCells();
  persObj->m_trackCellHash.resize(ncells);
  for (size_t i = 0; i < ncells; i++) {
    persObj->m_trackCellHash[i] = transObj->GetTrackCellHash(i);
  }
}


void TileCosmicMuonCnv_p1::persToTrans(const TileCosmicMuon_p1* persObj, TileCosmicMuon* transObj, MsgStream &/*log*/) const {

  transObj->SetTime (persObj->m_time);
  transObj->SetPositionX (persObj->m_positionX);
  transObj->SetPositionY (persObj->m_positionY);
  transObj->SetPositionZ (persObj->m_positionZ);
  transObj->SetDirectionPhi (persObj->m_directionPhi);
  transObj->SetDirectionTheta (persObj->m_directionTheta);
  transObj->SetFitQuality (persObj->m_fitQuality);
  transObj->SetFitNCells (persObj->m_fitNCells);

  transObj->SetPathTop (std::vector<double> (persObj->m_pathTop.begin(),
                                             persObj->m_pathTop.end()));
  
  transObj->SetPathBottom (std::vector<double> (persObj->m_pathBottom.begin(),
                                                persObj->m_pathBottom.end()));
  
  transObj->SetEnergyTop (std::vector<double> (persObj->m_energyTop.begin(),
                                               persObj->m_energyTop.end()));
  
  transObj->SetEnergyBottom (std::vector<double> (persObj->m_energyBottom.begin(),
                                                  persObj->m_energyBottom.end()));
  
  transObj->SetTrackCellHash (std::vector<IdentifierHash> (persObj->m_trackCellHash.begin(),
                                                           persObj->m_trackCellHash.end()));

  transObj->SetSegmentPath (std::vector<double>());
  transObj->SetSegmentPartition (std::vector<int>());
  transObj->SetSegmentModule (std::vector<int>());
  transObj->SetSegmentSampling (std::vector<int>());
}
