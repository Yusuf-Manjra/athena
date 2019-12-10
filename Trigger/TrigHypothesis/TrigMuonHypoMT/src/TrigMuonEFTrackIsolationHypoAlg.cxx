/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFTrackIsolationHypoAlg.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils; 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMuonEFTrackIsolationHypoAlg::TrigMuonEFTrackIsolationHypoAlg( const std::string& name,
				      ISvcLocator* pSvcLocator ) :
   ::HypoBase( name, pSvcLocator )
{}


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuonEFTrackIsolationHypoAlg::initialize()
{
  ATH_CHECK( m_hypoTools.retrieve() );

  renounce( m_muonKey );
  ATH_CHECK( m_muonKey.initialize() );

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigMuonEFTrackIsolationHypoAlg::execute( const EventContext& context ) const
{

  ATH_MSG_DEBUG("Executing ...");

  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  if ( not previousDecisionsHandle.isValid() ) { //implicit
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
    return StatusCode::SUCCESS;
  }  
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions");

  // prepare output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();

  std::vector<TrigMuonEFTrackIsolationHypoTool::EFIsolationMuonInfo> toolInput;
  size_t counter = 0;
  for ( const auto previousDecision: *previousDecisionsHandle ) {
    // get RoIs
    auto roiInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( previousDecision, initialRoIString()  );
    auto roiEL = roiInfo.link;
    ATH_CHECK( roiEL.isValid() );
    const TrigRoiDescriptor *roi = *roiEL;

    // get View
    auto viewEL = previousDecision->objectLink<ViewContainer>(viewString());
    ATH_CHECK( viewEL.isValid() );

    // get Muon
    auto muonHandle = ViewHelper::makeHandle( *viewEL, m_muonKey, context );
    ATH_CHECK( muonHandle.isValid() );
    ATH_MSG_DEBUG( "Muinfo handle size: " << muonHandle->size() << " ..." );

    // It is posisble that no muons are found, in this case we go to the next decision
    if ( muonHandle->size()==0 ) continue;

    // this code only gets muon 0. The EF algorithms can potentially make more than 1 muon, so may need to revisit this
    auto muonEL = ViewHelper::makeLink( *viewEL, muonHandle, 0 );
    ATH_CHECK( muonEL.isValid() );
    const xAOD::Muon* muon = *muonEL;

    // create new dicions
    auto newd = newDecisionIn( decisions );

    toolInput.emplace_back( newd, roi, muon, previousDecision );

    newd -> setObjectLink( featureString(), muonEL );
    TrigCompositeUtils::linkToPrevious( newd, previousDecision, context );


    ATH_MSG_DEBUG("REGTEST: " << m_muonKey.key() << " pT = " << (*muonEL)->pt() << " GeV");
    ATH_MSG_DEBUG("REGTEST: " << m_muonKey.key() << " eta/phi = " << (*muonEL)->eta() << "/" << (*muonEL)->phi());
    ATH_MSG_DEBUG("REGTEST:  RoI  = eta/phi = " << (*roiEL)->eta() << "/" << (*roiEL)->phi());
    ATH_MSG_DEBUG("Added view, roi, feature, previous decision to new decision "<<counter <<" for view "<<(*viewEL)->name()  );

    counter++;
  }

  ATH_MSG_DEBUG("Found "<<toolInput.size()<<" inputs to tools");

  StatusCode sc = StatusCode::SUCCESS;
  for ( auto& tool: m_hypoTools ) {
    ATH_MSG_DEBUG("Go to " << tool );
    sc = tool->decide(toolInput);
    if (!sc.isSuccess()) {
      ATH_MSG_ERROR("MuonHypoTool is failed");
      return StatusCode::FAILURE;
    }
  } // End of tool algorithm

  ATH_CHECK(hypoBaseOutputProcessing(outputHandle));

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

