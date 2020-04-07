/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPOTOOLConfig_SIMPLE_PARTITION_H
#define TRIGJETHYPOTOOLConfig_SIMPLE_PARTITION_H
/********************************************************************
 *
 * NAME:     TrigJetHypoToolConfig_simple_partition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *
 *********************************************************************/


#include "ITrigJetHypoToolConfig.h"
#include "./ConditionsDefsMT.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "./ConditionsDefsMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerBridge.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigJetHypoToolConfig_simple_partition:
public extends<AthAlgTool, ITrigJetHypoToolConfig> {

 public:
  
  TrigJetHypoToolConfig_simple_partition(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  virtual std::unique_ptr<IJetGrouper> getJetGrouper() const override;
  virtual std::unique_ptr<IGroupsMatcherMT> getMatcher() const override;
  virtual std::optional<ConditionsMT> getConditions() const override;
  virtual std::size_t requiresNJets() const override;

 private:
  
  Gaudi::Property<std::vector<double>>
    m_EtThresholds{this, "EtThresholds", {}, "Etthresholds by eta region"};
  
  Gaudi::Property<std::vector<double>>
    m_etaMins{this, "eta_mins", {}, "Eta min for eta regions"};
  
  Gaudi::Property<std::vector<double>>
    m_etaMaxs{this, "eta_maxs", {}, "Eta max for eta regions"};

  Gaudi::Property<std::vector<int>>
    m_asymmetricEtas{this, "asymmetricEtas", {}, "Apply asym. eta cuts"};


  virtual StatusCode checkVals()  const override;
 
};
#endif
