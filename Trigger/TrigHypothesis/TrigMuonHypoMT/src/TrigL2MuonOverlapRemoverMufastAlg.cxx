/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonOverlapRemoverMufastAlg.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils; 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonOverlapRemoverMufastAlg::TrigL2MuonOverlapRemoverMufastAlg( const std::string& name,
				      ISvcLocator* pSvcLocator ) :
   ::HypoBase( name, pSvcLocator )
{} 

TrigL2MuonOverlapRemoverMufastAlg::~TrigL2MuonOverlapRemoverMufastAlg() 
{}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonOverlapRemoverMufastAlg::initialize()
{
  ATH_MSG_DEBUG( "Initializing " << name() << "..." );
  ATH_CHECK(m_OverlapRemoverTools.retrieve());

  renounce(m_OverlapRemoverKey);
  ATH_CHECK(m_OverlapRemoverKey.initialize());

  ATH_MSG_DEBUG( "Initialization completed successfully" );
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonOverlapRemoverMufastAlg::execute(const EventContext& context) const
{
   ATH_MSG_DEBUG("StatusCode TrigL2MuonOverlapRemoverMufast::execute start"); 
   auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context);
   ATH_MSG_DEBUG("Runnung with " << previousDecisionsHandle->size() << " implicit ReadHandles for previous decision");

   // new output decisions
   SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
   auto decisions = outputHandle.ptr();
   // end of common   


   std::vector<TrigL2MuonOverlapRemoverTool::L2SAMuonOverlapInfo> toolInput;
   // loop over previous decisions
   size_t counter = 0; 
   
   for ( const auto previousDecision: *previousDecisionsHandle ) {
     //get RecRoI
     auto RecRoIInfo = TrigCompositeUtils::findLink< DataVector< LVL1::RecMuonRoI > >( previousDecision, initialRecRoIString() );
     auto RecRoIEL = RecRoIInfo.link;
     ATH_CHECK( RecRoIEL.isValid() );
     const LVL1::RecMuonRoI* RecRoI = *RecRoIEL;
 
     // get View
     auto viewELInfo = TrigCompositeUtils::findLink< ViewContainer >( previousDecision, viewString(), /*suppressMultipleLinksWarning = */ true  );
     ATH_CHECK( viewELInfo.isValid() );
     auto viewEL = viewELInfo.link;

     // get info
     auto L2MuonOverlapRemoverHandle = ViewHelper::makeHandle( *viewEL, m_OverlapRemoverKey, context );
     ATH_CHECK( L2MuonOverlapRemoverHandle.isValid() );
     ATH_MSG_DEBUG( "Muinfo handle size: " << L2MuonOverlapRemoverHandle->size() << "...");

     auto overlapEL = ViewHelper::makeLink( *viewEL, L2MuonOverlapRemoverHandle, 0 );
     ATH_CHECK( overlapEL.isValid() );
     const xAOD::L2StandAloneMuon* overlap = *overlapEL;

     // create new decisions
     auto newd = newDecisionIn( decisions );
     
     if ( decisionIDs( previousDecision ).size() != 0 ) { 
        toolInput.emplace_back( TrigL2MuonOverlapRemoverTool::L2SAMuonOverlapInfo{ newd, RecRoI, overlap, previousDecision} );
     }     

     // set objectLink
     newd->setObjectLink( featureString(), overlapEL );
     TrigCompositeUtils::linkToPrevious( newd, previousDecision, context);
     
     // DEBUG
     ATH_MSG_DEBUG("REGTEST: L2MuonSATrack pt in " << m_OverlapRemoverKey.key() << " = " << (*overlapEL)->pt() << " GeV");
     ATH_MSG_DEBUG("REGTEST: L2MuonSATrack eta/phi in " << m_OverlapRemoverKey.key() << " = " << (*overlapEL)->eta() << "/" << (*overlapEL)->phi());
     ATH_MSG_DEBUG("Added view, features, previous decision to new decision "<<counter <<" for view "<<(*viewEL)->name()  );
     ATH_MSG_DEBUG("Number of positive previous hypo Decision::" << decisionIDs( previousDecision ).size() );
     
     counter++;
   }

   ATH_MSG_DEBUG("Found " << toolInput.size() << " input to tools");

   // to TrigL2MuonOverlapRemoverTool
   StatusCode sc = StatusCode::SUCCESS;
   for ( auto& tool: m_OverlapRemoverTools ) {
     ATH_MSG_DEBUG("Go to " << tool );
     sc = tool->decide(toolInput);
     if (!sc.isSuccess()) {
       ATH_MSG_ERROR("MuonHypoTool is failed");
       return StatusCode::FAILURE;
     }
   } // End of tool algorithms */

   ATH_CHECK(hypoBaseOutputProcessing(outputHandle));

   ATH_MSG_DEBUG("StatusCode TrigL2MuonOverlapRemoverAlg::execute success");
   return StatusCode::SUCCESS;

}
