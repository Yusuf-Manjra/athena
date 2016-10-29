/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     conditionsFactory2.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//


#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/EtaEtCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/DijetDEtaMassCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HTCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TLACondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory2.h"

Conditions conditionsFactoryEtaEt(const std::vector<double>& etaMins,
                                  const std::vector<double>& etaMaxs,
                                  const std::vector<double>& thresholds){

  Conditions conditions;
  for (std::size_t i = 0; i != thresholds.size(); ++i){
    
    std::shared_ptr<ICondition> pCondition(new EtaEtCondition(etaMins[i],
                                                              etaMaxs[i], 
                                                              thresholds[i]));
    
    conditions.push_back(ConditionBridge(pCondition));
  }
  return conditions;
}


Conditions conditionsFactoryDijetEtaMass(const std::vector<double>& etaMins,
                                         const std::vector<double>& etaMaxs,
                                         const std::vector<double>& etMins,
                                         const std::vector<double>& dEtaMins,
                                         const std::vector<double>& dEtaMaxs,
                                         const std::vector<double>& massMins,
                                         const std::vector<double>& massMaxs){
  
  Conditions conditions;
  std::shared_ptr<ICondition> 
    pCondition(new DijetDEtaMassCondition(etaMins,
                                          etaMaxs, 
                                          etMins,
                                          dEtaMins,
                                          dEtaMaxs,
                                          massMins,
                                          massMaxs));
  
  conditions.push_back(ConditionBridge(pCondition));
  return conditions;
}


Conditions conditionsFactoryTLA(const std::vector<double>& etaMins,
                                const std::vector<double>& etaMaxs,
                                const std::vector<double>& ystarMins,
                                const std::vector<double>& ystarMaxs,
                                const std::vector<double>& massMins,
                                const std::vector<double>& massMaxs){

  Conditions conditions;
  std::shared_ptr<ICondition> 
    pCondition(new TLACondition(etaMins,
                                etaMaxs, 
                                ystarMins,
                                ystarMaxs,
                                massMins,
                                massMaxs));
  
  conditions.push_back(ConditionBridge(pCondition));
  return conditions;
}



Conditions conditionsFactoryHT(double etaMin,
                               double etaMax,
                               double etMin,
                               double htMin){
  
  Conditions conditions;
  
  std::shared_ptr<ICondition> pCondition(new HTCondition(etaMin,
                                                         etaMax,
                                                         etMin,
                                                         htMin));
  
  conditions.push_back(ConditionBridge(pCondition));
  return conditions;
}



