/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Gaudi/Athena include(s):
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigObjectMatching/TrigMatchTool.h"

/**********************************************************************
 *
 * Constructors and destructors
 *
 **********************************************************************/

TrigMatchTool::TrigMatchTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
   : AthAlgTool(type, name, parent),
     m_trigDecisionTool( "Trig::TrigDecisionTool/TrigDecisionTool" ) {

   declareInterface<TrigMatchTool>(this);

   declareProperty( "TrigDecisionTool", m_trigDecisionTool );
}

/**********************************************************************
 *
 * Initialization and Finalization 
 *
 **********************************************************************/

StatusCode TrigMatchTool::initialize() {

   ATH_MSG_DEBUG( "TrigMatchTool::Intialize()" );

   // load the required tool(s):
   CHECK( m_trigDecisionTool.retrieve() );
   ATH_MSG_INFO( "Successfully retrieved the TrigDecisionTool!" );
   this->setTDT( &( *m_trigDecisionTool ) );

   // register to hear end event incidents so we can clear
   // the cache
   ServiceHandle<IIncidentSvc> p_incSvc("IncidentSvc", this->name());
   p_incSvc->addListener( this, "EndEvent" );
   p_incSvc->addListener( this, "TrigConf" );

   return StatusCode::SUCCESS;
}

/**********************************************************************
 *
 * Private functions
 *
 **********************************************************************/

std::vector<std::string>
TrigMatchTool::getConfiguredChainNames() const
{
  if (!m_trigDecisionTool) {
    return std::vector<std::string>();
  }
  return m_trigDecisionTool->getListOfTriggers();
}

std::string TrigMatchTool::lowerChainName( const std::string& chainName ) const {

   // enable expert methods in tdt
   Trig::ExpertMethods *em = m_trigDecisionTool->ExperimentalAndExpertMethods();
   em->enable();

   const TrigConf::HLTChain *hltChain = em->getChainConfigurationDetails(chainName);
   if( hltChain ) {
      return hltChain->lower_chain_name();
   }

   return "";
}

// issue a clear at the beginning of each event
void TrigMatchTool::handle( const Incident &inc ) {

   if (inc.type() == "EndEvent") {
      this->endEvent();
   }
   if( inc.type() == "TrigConf" ) {
      ATH_MSG_INFO( "Switching to a new trigger configuration" );
      // Clear and re-build all cached information. (The LVL1->LVL2 mapping
      // is re-built the next time it is needed.)
      clearChainIndex();
   }

   return;
}
