/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigMuisoHypo.h
// PACKAGE:  Trigger/TrigHypotheis/TrigMuonHypoMT/TrigMuisoHypo
//
// PURPOSE:  LVL2 Muon Isolation Hypothesis Algorithm: V4.0 
//           for athenaMT
// ********************************************************************
#ifndef TRIGMUONHYPOMT_TRIGMUISOHYPOALG_H
#define TRIGMUONHYPOMT_TRIGMUISOHYPOALG_H 1

#include "DecisionHandling/HypoBase.h"
#include "TrigMuisoHypoTool.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigMuisoHypoAlg
   : public ::HypoBase
{
  public:

    TrigMuisoHypoAlg( const std::string& name, 
                      ISvcLocator* pSvcLocator );

    virtual StatusCode  initialize() override;
    virtual StatusCode  execute( const EventContext& context ) const override;

  private:

  ToolHandleArray<TrigMuisoHypoTool> m_hypoTools {this, "HypoTools", {}, "Tools to perform selection"};
 
  SG::ReadHandleKey<xAOD::L2IsoMuonContainer> m_muIsoKey {
	this, "MuonL2ISInfoName", "MuonL2ISInfo", "Name of the input data produced by muIso"};


};

#endif

