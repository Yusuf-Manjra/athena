/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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


#include "ITrigJetHypoToolNoGrouperConfig.h"
#include "./RepeatedConditionsDefs.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "./IJetsMatcherMT.h"
#include "./ConditionsDefsMT.h"
#include "./ITrigJetRepeatedConditionConfig.h"
#include "./ConditionFilter.h"

class TrigJetHypoToolConfig_fastreduction:
public extends<AthAlgTool, ITrigJetHypoToolNoGrouperConfig> {

 public:
  
  TrigJetHypoToolConfig_fastreduction(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);
  virtual ~TrigJetHypoToolConfig_fastreduction();

  virtual StatusCode initialize() override;
  virtual std::unique_ptr<IJetsMatcherMT> getMatcher() const override;

  virtual std::vector<std::unique_ptr<ConditionFilter>>
  getConditionFilters() const override;

  virtual std::size_t requiresNJets() const override;
  virtual StatusCode checkVals() const override;


 private:
  ToolHandleArray<ITrigJetRepeatedConditionConfig> m_conditionMakers{
    this, "conditionMakers", {}, "hypo tree Condition builder AlgTools"};

  ToolHandleArray<ITrigJetRepeatedConditionConfig>
  m_antiConditionMakers{this, "antiConditionMakers", {},
    "hypo tree AntiCondition builder AlgTools"};


  ToolHandleArray<ITrigJetRepeatedConditionConfig> m_filtConditionMakers{
    this, "filtConditionsMakers", {},
    "hypo tree Condition builder AlgTools for Condition filters"};

  
  Gaudi::Property<std::vector<std::size_t>> m_treeVec{
    this, "treeVector", {}, "integer sequence representation of jet hypo tree"};

  Gaudi::Property<std::vector<int>> m_leafNodes{
    this, "leafVector", {}, "node ids for leaf nodes"};

  ConditionPtrs getRepeatedConditions() const;


};
#endif
