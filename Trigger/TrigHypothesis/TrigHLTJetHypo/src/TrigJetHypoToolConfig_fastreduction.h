/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPOTOOLCONFIG_FASTREDUCTION_H
#define TRIGJETHYPOTOOLCONFIG_FASTREDUCTION_H
/********************************************************************
 *
 * NAME:     TrigJetHypoToolConfig_fastreduction.h
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
#include "./ITrigJetConditionConfig.h"

class TrigJetHypoToolConfig_fastreduction:
public extends<AthAlgTool, ITrigJetHypoToolConfig> {

 public:
  
  TrigJetHypoToolConfig_fastreduction(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);
  virtual ~TrigJetHypoToolConfig_fastreduction();

  virtual StatusCode initialize() override;
  virtual std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  virtual std::unique_ptr<IJetGrouper> getJetGrouper() const override;
  virtual std::unique_ptr<IGroupsMatcherMT> getMatcher() const override;
  virtual std::optional<ConditionsMT> getConditions() const override;
  virtual std::size_t requiresNJets() const override;
  virtual StatusCode checkVals() const override;


 private:
  ToolHandleArray<ITrigJetConditionConfig> m_conditionMakers{
    this, "conditionMakers", {}, "hypo tree node to conditiionMaker map"};

  Gaudi::Property<std::vector<std::size_t>> m_treeVec{
    this, "treeVector", {}, "integer sequence representation of jet hypo tree"};

  Gaudi::Property<std::vector<int>> m_sharedNodesVec{
    this, "sharedVector", {}, "nodeID groups for nodes that see input jets"};

  std::vector<std::vector<int>> m_sharedNodes{};
 
};
#endif
