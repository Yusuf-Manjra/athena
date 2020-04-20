/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONMONITORINGMT_L2MUONSAMONMT_H
#define TRIGMUONMONITORINGMT_L2MUONSAMONMT_H

#include "TrigMuonMonitorAlgorithm.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"

/*
This is a class for monitoring L2MuonSA.
 */
class L2MuonSAMonMT : public TrigMuonMonitorAlgorithm{

 public:
  L2MuonSAMonMT(const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode initialize() override;

 protected:
  virtual StatusCode fillVariables(const EventContext &ctx) const override;


 private:
  SG::ReadHandleKey<xAOD::L2StandAloneMuonContainer> m_L2MuonSAContainerKey {this, "L2StandAloneMuonContainerName", "HLT_MuonL2SAInfo", "L2MuonSA container"};
  

};

#endif //TRIGMUONMONITORINGMT_L2MUONSAMONMT_H
