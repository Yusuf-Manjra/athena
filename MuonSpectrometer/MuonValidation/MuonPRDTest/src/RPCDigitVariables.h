/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonPRDTEST_RPCDigitVARIABLES_H
#define MuonPRDTEST_RPCDigitVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class RpcDigitVariables : public ValAlgVariables
{
 public:
  RpcDigitVariables(StoreGateSvc* evtStore,
                     const MuonGM::MuonDetectorManager* detManager,
                     const MuonIdHelper* idhelper,
                     TTree* tree,
							const std::string & containername,
							MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl)
  {
    setHelper(idhelper);
  }

  ~RpcDigitVariables() = default;

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:

  void setHelper(const MuonIdHelper* idhelper){
    m_RpcIdHelper = dynamic_cast<const RpcIdHelper*>(idhelper);
    if (!m_RpcIdHelper){
     throw std::runtime_error("Could not cast the RPCIdHelper");
    }
  }
  
  void deleteVariables(){};
  StatusCode clearVariables();
  
  const RpcIdHelper* m_RpcIdHelper{};

  int m_RPC_nDigits{};
  std::vector<std::string> m_RPC_dig_stationName;
  std::vector<int> m_RPC_dig_stationIndex; // stationIndex = 1 for BIS
  std::vector<double> m_RPC_dig_time;
  std::vector<int> m_RPC_dig_stationEta;
  std::vector<int> m_RPC_dig_stationPhi;
  std::vector<int> m_RPC_dig_gas_gap;
  std::vector<int> m_RPC_dig_doubletR;
  std::vector<int> m_RPC_dig_doubletZ;
  std::vector<int> m_RPC_dig_doubletPhi;
  std::vector<int> m_RPC_dig_strip; 
  std::vector<int> m_RPC_dig_stripNumber;
  std::vector<bool> m_RPC_dig_stripPosition;
  std::vector<bool> m_RPC_dig_measuresPhi;
  
  std::vector< double > m_RPC_dig_localPosX;
  std::vector< double > m_RPC_dig_localPosY;
  std::vector< double > m_RPC_dig_globalPosX;
  std::vector< double > m_RPC_dig_globalPosY;
  std::vector< double > m_RPC_dig_globalPosZ;  
};

#endif // MuonPRDTEST_RPCDigitVARIABLES_H
