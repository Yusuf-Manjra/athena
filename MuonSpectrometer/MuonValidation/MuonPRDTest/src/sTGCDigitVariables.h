/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCDIGITVARIABLES_H
#define STGCDIGITVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class sTGCDigitVariables : public ValAlgVariables
{
 public:
  sTGCDigitVariables(StoreGateSvc* evtStore,
                     const MuonGM::MuonDetectorManager* detManager,
                     const MuonIdHelper* idhelper,
                     TTree* tree,
							std::string containername,
							MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_NSWsTGC_nDigits(0),
    m_NSWsTGC_nPadDigits(0),
    m_NSWsTGC_dig_time(0),
    m_NSWsTGC_dig_bctag(0),
    m_NSWsTGC_dig_charge(0),
		m_NSWsTGC_dig_isDead(0),
		m_NSWsTGC_dig_isPileup(0),
    m_NSWsTGC_dig_stationName(0),
    m_NSWsTGC_dig_stationEta(0),
    m_NSWsTGC_dig_stationPhi(0),
    m_NSWsTGC_dig_multiplet(0),
    m_NSWsTGC_dig_gas_gap(0),
    m_NSWsTGC_dig_channel_type(0),
    m_NSWsTGC_dig_channel(0),
    m_NSWsTGC_dig_stationEtaMin(0),
    m_NSWsTGC_dig_stationEtaMax(0),
    m_NSWsTGC_dig_stationPhiMin(0),
    m_NSWsTGC_dig_stationPhiMax(0),
    m_NSWsTGC_dig_gas_gapMin(0),
    m_NSWsTGC_dig_gas_gapMax(0),
    m_NSWsTGC_dig_padEta(0),
    m_NSWsTGC_dig_padPhi(0),
    m_NSWsTGC_dig_numberOfMultilayers(0),
    m_NSWsTGC_dig_multilayerMin(0),
    m_NSWsTGC_dig_multilayerMax(0),
    m_NSWsTGC_dig_channelTypeMin(0),
    m_NSWsTGC_dig_channelTypeMax(0),
    m_NSWsTGC_dig_channelMin(0),
    m_NSWsTGC_dig_channelMax(0),
    m_NSWsTGC_dig_channelNumber(0),
    m_NSWsTGC_dig_channelPosX(0),
    m_NSWsTGC_dig_channelPosY(0),
    m_NSWsTGC_dig_localPosX(0),
    m_NSWsTGC_dig_localPosY(0),
    m_NSWsTGC_dig_globalPosX(0),
    m_NSWsTGC_dig_globalPosY(0),
    m_NSWsTGC_dig_globalPosZ(0),
    m_NSWsTGC_dig_PadglobalCornerPosX(0),
    m_NSWsTGC_dig_PadglobalCornerPosY(0),
    m_NSWsTGC_dig_PadglobalCornerPosZ(0)
  {
    setHelper(idhelper);
  }

  ~sTGCDigitVariables()
  {
    deleteVariables();
  }

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_sTgcIdHelper = dynamic_cast<const sTgcIdHelper*>(idhelper);
    if(m_sTgcIdHelper == 0) {
       ATH_MSG_ERROR("casting IdHelper to sTgcIdhelper failed");
       throw;
    }
  }

  void deleteVariables();
  StatusCode clearVariables();

  const sTgcIdHelper* m_sTgcIdHelper;

  int m_NSWsTGC_nDigits;
  int m_NSWsTGC_nPadDigits;
  std::vector<double> *m_NSWsTGC_dig_time;
  std::vector<int> *m_NSWsTGC_dig_bctag;
  std::vector<double> *m_NSWsTGC_dig_charge;
  std::vector<bool> *m_NSWsTGC_dig_isDead;
  std::vector<bool> *m_NSWsTGC_dig_isPileup;
  std::vector<std::string> *m_NSWsTGC_dig_stationName;
  std::vector<int> *m_NSWsTGC_dig_stationEta;
  std::vector<int> *m_NSWsTGC_dig_stationPhi;
  std::vector<int> *m_NSWsTGC_dig_multiplet;
  std::vector<int> *m_NSWsTGC_dig_gas_gap;
  std::vector<int> *m_NSWsTGC_dig_channel_type;
  std::vector<int> *m_NSWsTGC_dig_channel;
  std::vector<int> *m_NSWsTGC_dig_stationEtaMin;
  std::vector<int> *m_NSWsTGC_dig_stationEtaMax;
  std::vector<int> *m_NSWsTGC_dig_stationPhiMin;
  std::vector<int> *m_NSWsTGC_dig_stationPhiMax;
  std::vector<int> *m_NSWsTGC_dig_gas_gapMin;
  std::vector<int> *m_NSWsTGC_dig_gas_gapMax;
  std::vector<int> *m_NSWsTGC_dig_padEta;
  std::vector<int> *m_NSWsTGC_dig_padPhi;
  std::vector<int> *m_NSWsTGC_dig_numberOfMultilayers;
  std::vector<int> *m_NSWsTGC_dig_multilayerMin;
  std::vector<int> *m_NSWsTGC_dig_multilayerMax;
  std::vector<int> *m_NSWsTGC_dig_channelTypeMin;
  std::vector<int> *m_NSWsTGC_dig_channelTypeMax;
  std::vector<int> *m_NSWsTGC_dig_channelMin;
  std::vector<int> *m_NSWsTGC_dig_channelMax;
  std::vector<int> *m_NSWsTGC_dig_channelNumber;

  std::vector< double > *m_NSWsTGC_dig_channelPosX;
  std::vector< double > *m_NSWsTGC_dig_channelPosY;
  std::vector< double > *m_NSWsTGC_dig_localPosX;
  std::vector< double > *m_NSWsTGC_dig_localPosY;
  std::vector< double > *m_NSWsTGC_dig_globalPosX;
  std::vector< double > *m_NSWsTGC_dig_globalPosY;
  std::vector< double > *m_NSWsTGC_dig_globalPosZ;
  std::vector< double > *m_NSWsTGC_dig_PadglobalCornerPosX;
  std::vector< double > *m_NSWsTGC_dig_PadglobalCornerPosY;
  std::vector< double > *m_NSWsTGC_dig_PadglobalCornerPosZ;


};

#endif // STGCDIGITVARIABLES_H
