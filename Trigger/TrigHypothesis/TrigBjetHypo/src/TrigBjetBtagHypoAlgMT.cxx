/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBjetBtagHypoAlgMT.h"


TrigBjetBtagHypoAlgMT::TrigBjetBtagHypoAlgMT( const std::string& name, 
						ISvcLocator* pSvcLocator ) : 
  TrigBjetHypoAlgBaseMT( name, pSvcLocator ) {}


StatusCode TrigBjetBtagHypoAlgMT::initialize() {

  ATH_CHECK( m_hypoTools.retrieve() );
  if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );

  ATH_CHECK( m_bTaggedJetKey.initialize() );
  ATH_CHECK( m_bTagKey.initialize() );
  ATH_CHECK( m_trackKey.initialize() );
  ATH_CHECK( m_inputPrmVtx.initialize() );

  renounce( m_bTaggedJetKey );
  renounce( m_bTagKey );
  renounce( m_trackKey );
  renounce( m_inputPrmVtx );

  return StatusCode::SUCCESS;
}


StatusCode TrigBjetBtagHypoAlgMT::execute( const EventContext& context ) const {
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  // ========================================================================================================================== 
  //    ** Retrieve Ingredients 
  // ========================================================================================================================== 

  // Read in previous Decisions made before running this Hypo Alg.
  const TrigCompositeUtils::DecisionContainer *prevDecisionContainer = nullptr;
  CHECK( retrievePreviousDecisionContainer( context,prevDecisionContainer ) );
  ATH_MSG_DEBUG( "Running with "<< prevDecisionContainer->size() <<" previous decisions");

  // Retrive Precision tracks from Event Views. We get them all in this way!
  ElementLinkVector< xAOD::TrackParticleContainer > trackELs;
  CHECK( retrieveObjectFromEventView( context,trackELs,m_trackKey,prevDecisionContainer ) );
  ATH_MSG_DEBUG( "Retrieved " << trackELs.size() << " precision tracks..." );

  // online monitoring for tracks
  auto monitor_for_track_count = Monitored::Scalar( "track_count", trackELs.size() );
  CHECK( monitor_tracks( trackELs ) );

  for ( const ElementLink< xAOD::TrackParticleContainer >& trackLink : trackELs )
    ATH_MSG_DEBUG( "   * pt=" << (*trackLink)->p4().Et() << 
		   " eta=" << (*trackLink)->eta() <<
		   " phi=" << (*trackLink)->phi() );


  // Retrieve Jets
  ElementLinkVector< xAOD::JetContainer > jetELs;
  CHECK( retrieveCollectionFromNavigation( TrigCompositeUtils::featureString(), jetELs, prevDecisionContainer ) );  
  ATH_MSG_DEBUG( "Retrieved " << jetELs.size() << " Jets of key " << TrigCompositeUtils::featureString() );

  // online monitoring for jets
  auto monitor_for_jet_count = Monitored::Scalar( "jet_count", jetELs.size() );
  CHECK( monitor_jets( jetELs ) );


  // Retrieve Vertices
  ElementLinkVector< xAOD::VertexContainer > vertexELs;
  CHECK( retrieveObjectFromStoreGate( context,vertexELs,m_inputPrmVtx ) );
  ATH_MSG_DEBUG( "Retrieved " << vertexELs.size() <<" vertices..." );

  // opnline monitoring for vertex
  auto monitor_for_vertex_count = Monitored::Scalar( "vertex_count", vertexELs.size() );  


  auto monitor_group_for_events = Monitored::Group( m_monTool, monitor_for_track_count, monitor_for_jet_count, monitor_for_vertex_count );


  // ==========================================================================================================================
  //    ** Prepare Outputs
  // ==========================================================================================================================

  // Decisions
  SG::WriteHandle< TrigCompositeUtils::DecisionContainer > handle = TrigCompositeUtils::createAndStore( decisionOutput(), context );
  TrigCompositeUtils::DecisionContainer *outputDecisions = handle.ptr();

  // ========================================================================================================================== 
  //    ** Prepare Decisions
  // ========================================================================================================================== 
  
  const unsigned int nDecisions = prevDecisionContainer->size();

  // Create output decisions
  ATH_MSG_DEBUG("Creating Output Decisions and Linking Stuff to it");
  std::vector< TrigCompositeUtils::Decision* > newDecisions;
  for ( unsigned int index(0); index<nDecisions; index++ ) {
    TrigCompositeUtils::Decision *toAdd = TrigCompositeUtils::newDecisionIn( outputDecisions,
									     prevDecisionContainer->at(index),
									     "", context );

    // Attaching links to the output decisions
    // Retrieved jets from view on which we have run flavour tagging
    ElementLinkVector< xAOD::JetContainer > bTaggedJetEL;
    CHECK( retrieveCollectionFromView( context,
				       bTaggedJetEL,
				       m_bTaggedJetKey,
				       prevDecisionContainer->at(index) ) );

    if ( bTaggedJetEL.size() != 1 ) {
      ATH_MSG_ERROR( "Did not find only 1 b-tagged jet object from View!" );
      return StatusCode::FAILURE;
    }

    toAdd->setObjectLink< xAOD::JetContainer >( TrigCompositeUtils::featureString(),bTaggedJetEL.front() );     



    // Retrieve Flavour Tagging object from view
    ElementLinkVector< xAOD::BTaggingContainer > bTaggingEL;
    CHECK( retrieveCollectionFromView( context,
				       bTaggingEL,
				       m_bTagKey,
				       prevDecisionContainer->at(index) ) );
    
    if ( bTaggingEL.size() != 1 ) {
      ATH_MSG_ERROR( "Did not find only 1 b-tagging object from View!" );
      return StatusCode::FAILURE;
    }

    toAdd->setObjectLink< xAOD::BTaggingContainer >( m_bTaggingLink.value(),bTaggingEL.front() );



    // Add to Decision collection
    newDecisions.push_back( toAdd );
  }


  // ==========================================================================================================================
  //    ** Prepare input to Hypo Tools  
  // ==========================================================================================================================


  std::vector< TrigBjetBtagHypoTool::TrigBjetBtagHypoToolInfo > bTagHypoInputs;

  for ( unsigned int index(0); index<nDecisions; index++ ) {

    // Extract the IDs of the b-jet chains which are active.
    // Previous decision IDs.
    const TrigCompositeUtils::Decision *previousDecision = prevDecisionContainer->at(index);

    TrigCompositeUtils::DecisionIDContainer previousDecisionIDs {
      TrigCompositeUtils::decisionIDs( previousDecision ).begin(),
      TrigCompositeUtils::decisionIDs( previousDecision ).end() };

    
    // Retrieve PV from navigation
    ElementLink< xAOD::VertexContainer > vertexEL;
    CHECK( retrieveObjectFromNavigation(  m_prmVtxLink.value(), vertexEL, previousDecision ) );

    // Retrieve b-tagging code
    ElementLinkVector< xAOD::BTaggingContainer > bTaggingELs;
    CHECK( retrieveCollectionFromView< xAOD::BTaggingContainer >( context,
								  bTaggingELs,
								  m_bTagKey,
								  previousDecision ) );
    CHECK( bTaggingELs.size() == 1 );
    
    // Put everything in place
    TrigBjetBtagHypoTool::TrigBjetBtagHypoToolInfo infoToAdd;
    infoToAdd.previousDecisionIDs = previousDecisionIDs;
    infoToAdd.btaggingEL = bTaggingELs.front();
    infoToAdd.vertexEL = vertexEL;
    infoToAdd.decision = newDecisions.at( index );
    bTagHypoInputs.push_back( infoToAdd );
  }

  // ==========================================================================================================================

  // Run on Trigger Chains
  for ( const ToolHandle< TrigBjetBtagHypoTool >& tool : m_hypoTools ) 
    CHECK( tool->decide( bTagHypoInputs ) ); 
  
  ATH_MSG_DEBUG( "Exiting with " << handle->size() << " decisions" );
  ATH_MSG_DEBUG( "CHECKING OUTPUT DECISION HANDLE" );
  CHECK( hypoBaseOutputProcessing(handle) );

  // ==========================================================================================================================  

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetBtagHypoAlgMT::monitor_jets( const ElementLinkVector<xAOD::JetContainer >& jetELs ) const {

  auto monitor_for_jet_pt = Monitored::Collection( "jet_pt", jetELs,
    [](const ElementLink< xAOD::JetContainer >& jetLink) { return (*jetLink)->pt(); } );
  auto monitor_for_jet_eta = Monitored::Collection( "jet_eta", jetELs,
    [](const ElementLink< xAOD::JetContainer >& jetLink) { return (*jetLink)->eta(); } );
  auto monitor_for_jet_phi = Monitored::Collection( "jet_phi", jetELs,
    [](const ElementLink< xAOD::JetContainer >& jetLink) { return (*jetLink)->phi(); } );

  auto monitor_group_for_jets = Monitored::Group( m_monTool, 
     monitor_for_jet_pt, monitor_for_jet_eta, monitor_for_jet_phi
  );

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetBtagHypoAlgMT::monitor_tracks( const ElementLinkVector< xAOD::TrackParticleContainer >& trackELs ) const {

  auto monitor_for_track_Et = Monitored::Collection( "track_Et", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) { return (*trackLink)->p4().Et(); } );
  auto monitor_for_track_eta = Monitored::Collection( "track_eta", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) { return (*trackLink)->eta(); } ); 
  auto monitor_for_track_phi = Monitored::Collection( "track_phi", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) { return (*trackLink)->phi(); } );

  // Monitors for d0 and z0 track impact parameter variables
  auto monitor_for_track_d0 = Monitored::Collection( "track_d0", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) { return (*trackLink)->d0(); } );

  auto monitor_for_track_d0err = Monitored::Collection( "track_d0err", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) {
      return (*trackLink)->definingParametersCovMatrix()( Trk::d0, Trk::d0 );
    } );

  auto monitor_for_track_d0sig = Monitored::Collection( "track_d0sig", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) {
      return (*trackLink)->d0() / (*trackLink)->definingParametersCovMatrix()( Trk::d0, Trk::d0 );
    } );

  auto monitor_for_track_z0 = Monitored::Collection( "track_z0", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) { return (*trackLink)->z0(); } );

  auto monitor_for_track_z0err = Monitored::Collection( "track_z0err", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) {
      return (*trackLink)->definingParametersCovMatrix()( Trk::z0, Trk::z0 );
    } );

  auto monitor_for_track_z0sig = Monitored::Collection( "track_z0sig", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) {
      return (*trackLink)->z0() / (*trackLink)->definingParametersCovMatrix()( Trk::z0, Trk::z0 );
    } );

  auto monitor_group_for_tracks = Monitored::Group( m_monTool, 
    monitor_for_track_Et, monitor_for_track_eta, monitor_for_track_phi,
    monitor_for_track_d0, monitor_for_track_d0err, monitor_for_track_d0sig,
    monitor_for_track_z0, monitor_for_track_z0err, monitor_for_track_z0sig
  );

  return StatusCode::SUCCESS;
}


