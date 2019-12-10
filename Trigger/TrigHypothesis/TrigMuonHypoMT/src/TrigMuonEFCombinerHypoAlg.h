/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONHYPOMT_TRIGMUONEFCOMBINERHYPOALG_H
#define TRIGMUONHYPOMT_TRIGMUONEFCOMBINERHYPOALG_H 1

#include "TrigMuonEFCombinerHypoTool.h"
#include "DecisionHandling/HypoBase.h"


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TrigMuonEFCombinerHypoAlg
   : public ::HypoBase
{
  public:

   TrigMuonEFCombinerHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );   

   virtual StatusCode  initialize() override;
   virtual StatusCode  execute( const EventContext& context ) const override;

  private:
 
    ToolHandleArray<TrigMuonEFCombinerHypoTool> m_hypoTools {this, "HypoTools", {}, "Tools to perform selection"}; 

    SG::ReadHandleKey<xAOD::MuonContainer> m_muonKey{
	this, "MuonDecisions", "MuonEFCombiner_MuonData", "Name of the input data on xAOD::MuonContainer produced by MuonCreatorAlg"};

};

#endif
