/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASMONITORING_FWDAFPJETMONITORINGALG_H
#define TRIGMINBIASMONITORING_FWDAFPJETMONITORINGALG_H

// Framework include(s):
#include <AthenaMonitoring/AthMonitorAlgorithm.h>
#include <AthenaMonitoringKernel/Monitored.h>

// xAOD
#include <xAODForward/AFPTrackContainer.h>
#include <xAODJet/JetContainer.h>

class FwdAFPJetMonitoringAlg : public AthMonitorAlgorithm {
 public:
  FwdAFPJetMonitoringAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms(const EventContext& context) const override;

 private:
  Gaudi::Property<std::vector<std::string>> m_chains{this, "chains", {}, "Chains to monitor"};
  Gaudi::Property<SG::ReadHandleKey<xAOD::JetContainer>> m_jetKey{this, "jetContainer", "AntiKt4EMTopoJets",
                                                                  "SG key for the jet container"};
};

#endif
