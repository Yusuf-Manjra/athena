/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEGAMMAHYPO_TRIGEGAMMADPHIHYPOTOOL_H
#define TRIGEGAMMAHYPO_TRIGEGAMMADPHIHYPOTOOL_H

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/src/TrigEgammaDPhiHypoTool.h
 **
 **   Description: - Hypothesis Tool: search for photon pairs with 
 **                deltaPhi more than a threshold value; intended for H->gg
 **
 **   Author: Debottam BakshiGupta  <debottam.bakshigupta@cern.ch>
 **
 **************************************************************************/ 

#include <string>
#include <vector>

#include "DecisionHandling/ComboHypoToolBase.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"


/**
 * \class TrigEgammaDPhiHypoTool
 * \brief TrigEgammaDPhiHypoTool is a ComboHypoTool that calculates the deltaPhi distance 
 * Apply the threshold cut and accepts the event if condition is 
 * satisfied
 *
 */


class TrigEgammaDPhiHypoTool:  public ComboHypoToolBase {

 public:
  
  TrigEgammaDPhiHypoTool(const std::string& type,
                    const std::string& name,
                    const IInterface* parent);
  
  virtual StatusCode initialize() override;


 private:
  
  virtual bool executeAlg(std::vector<LegDecision>& thecomb) const override;
  
  // flags
  Gaudi::Property< bool > m_acceptAll {this, "AcceptAll", false, "Ignore selection" };
  
  // cuts
  Gaudi::Property<float> m_thresholdDPhiCut {this,"ThresholdDPhiCut", 1.5, "minimum deltaPhi required between two photons"};
  
  // monitoring
  ToolHandle<GenericMonitoringTool> m_monTool { this, "MonTool", "", "Monitoring tool" };

}; // TRIGEGAMMAHYPO_TRIGEGAMMADPHIHYPOTOOL_H
#endif



