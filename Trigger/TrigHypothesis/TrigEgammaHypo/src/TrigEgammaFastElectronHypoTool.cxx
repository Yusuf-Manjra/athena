/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include <cmath>
#include "TrigEgammaFastElectronHypoTool.h"

namespace TCU = TrigCompositeUtils;

TrigEgammaFastElectronHypoTool::TrigEgammaFastElectronHypoTool( const std::string& type, 
						const std::string& name, 
						const IInterface* parent ) 
  : AthAlgTool( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode TrigEgammaFastElectronHypoTool::initialize()  {
  
  if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );

  ATH_MSG_DEBUG( "Initialization completed successfully:" );
  ATH_MSG_DEBUG( "AcceptAll            = " 
		<< ( m_acceptAll==true ? "True" : "False" ) ); 
  ATH_MSG_DEBUG( "TrackPt              = " << m_trackPt           ); 
  ATH_MSG_DEBUG( "CaloTrackdETA        = " << m_caloTrackDEta        );
  ATH_MSG_DEBUG( "CaloTrackdPHI        = " << m_caloTrackDPhi        );
  ATH_MSG_DEBUG( "CaloTrackdEoverPLow  = " << m_caloTrackdEoverPLow );
  ATH_MSG_DEBUG( "CaloTrackdEoverPHigh = " << m_caloTrackdEoverPHigh );
  ATH_MSG_DEBUG( "TRTRatio = " << m_trtRatio );
  ATH_MSG_DEBUG( "Do_LRT = "  <<m_doLRT);
  ATH_MSG_DEBUG( "d0Cut = " << m_d0 );

  std::vector<size_t> sizes( {m_trackPt.size(), m_caloTrackDEta.size(), m_caloTrackDPhi.size(), m_caloTrackdEoverPLow.size(), m_caloTrackdEoverPHigh.size(), m_trtRatio.size() } );


  if ( *std::min_element( sizes.begin(), sizes.end() ) != *std::max_element( sizes.begin(), sizes.end() )  ) {
    ATH_MSG_ERROR( "Missconfiguration, cut properties listed above ( when DEBUG ) have different dimensions shortest: " <<  *std::min_element( sizes.begin(), sizes.end() ) << " longest " << *std::max_element( sizes.begin(), sizes.end() ) );
    return StatusCode::FAILURE;
  }

  m_multiplicity = m_trackPt.size();
  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId  );

  return StatusCode::SUCCESS;
}



bool TrigEgammaFastElectronHypoTool::decideOnSingleObject( const xAOD::TrigElectron* electron, 
						   size_t cutIndex ) const {
  bool pass=false;

  if ( m_acceptAll ) {
    pass = true;
    ATH_MSG_DEBUG( "AcceptAll property is set: taking all events" );
    return pass;
   } else {
     pass = false;
     ATH_MSG_DEBUG( "AcceptAll property not set: applying selection" );
   }

  auto cutCounter = Monitored::Scalar<int>( "CutCounter", -1 );  
  auto cutIndexM  = Monitored::Scalar<int>( "CutIndex", cutIndex );  // one can do 2D plots for each cut independently
  auto ptCalo     = Monitored::Scalar( "PtCalo", -999. );
  auto ptTrack    = Monitored::Scalar( "PtTrack", -999. );
  auto dEtaCalo   = Monitored::Scalar( "CaloTrackEta", -1. );
  auto dPhiCalo   = Monitored::Scalar( "CaloTrackPhi", -1. );
  auto eToverPt   = Monitored::Scalar( "CaloTrackEoverP", -1. );
  auto caloEta    = Monitored::Scalar( "CaloEta", -100. );
  auto caloPhi    = Monitored::Scalar( "CaloEta", -100. );
  auto trk_d0    = Monitored::Scalar( "d0 value", -1. );
  auto monitorIt  = Monitored::Group( m_monTool, 
					     cutCounter, cutIndexM,
					     ptCalo, ptTrack,    
					     dEtaCalo, dPhiCalo,   
					     eToverPt,   
					     caloEta, caloPhi, trk_d0);

  const xAOD::TrackParticle* trkIter = electron-> trackParticle();
  if ( trkIter == 0 ){  // disconsider candidates without track
     pass = false;
     return pass;
  }
  cutCounter++;

  // Retrieve all quantities
  dPhiCalo    = electron->trkClusDphi();
  dEtaCalo    = electron->trkClusDeta();
  ptCalo      = electron->pt();	  
  eToverPt    = electron->etOverPt();	  

  caloEta     = electron->caloEta();
  caloPhi     = electron->caloPhi();

  trk_d0      = std::fabs(trkIter->d0());

  float NTRHits     = ( float )( electron->nTRTHits() );
  float NStrawHits  = ( float )( electron->nTRTHiThresholdHits() );
  float TRTHitRatio = NStrawHits == 0 ? 1e10 : NTRHits/NStrawHits;

  ATH_MSG_DEBUG( "Cut index " << cutIndex );
  if ( ptCalo < m_trackPt[cutIndex] ){ 
    ATH_MSG_DEBUG( "Fails pt cut" << ptCalo << " < " << m_trackPt[cutIndex] );
    pass = false;
    return  pass;
  }
  cutCounter++;

  if ( dEtaCalo > m_caloTrackDEta[cutIndex] ) {
    ATH_MSG_DEBUG( "Fails dEta cut " << dEtaCalo << " < " << m_caloTrackDEta[cutIndex] );
    
    pass = false;
    return  pass;
  }
  cutCounter++;
  if ( dPhiCalo > m_caloTrackDPhi[cutIndex] ) {
    ATH_MSG_DEBUG( "Fails dPhi cut " << dPhiCalo << " < " << m_caloTrackDPhi[cutIndex] );
    
    pass = false;
    return  pass;
  }

  cutCounter++;
  if( eToverPt <  m_caloTrackdEoverPLow[cutIndex] ) {
    ATH_MSG_DEBUG( "Fails eoverp low cut " << eToverPt << " < " <<  m_caloTrackdEoverPLow[cutIndex] );

    pass = false;
    return  pass;
  }
  cutCounter++;
  if ( eToverPt > m_caloTrackdEoverPHigh[cutIndex] ) {
    ATH_MSG_DEBUG( "Fails eoverp high cut " << eToverPt << " < " << m_caloTrackdEoverPHigh[cutIndex] );

    pass = false;
    return  pass;
  }
  cutCounter++;
  if ( TRTHitRatio < m_trtRatio[cutIndex] ){
    ATH_MSG_DEBUG( "Fails TRT cut " << TRTHitRatio << " < " << m_trtRatio[cutIndex] );

    pass = false;
    return  pass;
  }
  cutCounter++;
  if(m_doLRT){
   ATH_MSG_DEBUG( "doLRT: " <<m_doLRT);
   ATH_MSG_DEBUG( "Track d0: " <<trk_d0);
   
   if(trk_d0 < m_d0[cutIndex]){
     ATH_MSG_DEBUG( "Fails d0 cut " <<trk_d0<< " < " <<m_d0[cutIndex] );
     pass = false;
     return pass;
   }
  }
  ATH_MSG_DEBUG( "Passed selection" );
  pass = true;
  return  pass;

}

StatusCode TrigEgammaFastElectronHypoTool::inclusiveSelection( std::vector<ElectronInfo>& input ) const {
    for ( auto i: input ) {
      if ( i.previousDecisionIDs.count( m_decisionId.numeric() ) == 0 ) continue; // the decision was negative or not even made in previous stage

      auto objDecision = decideOnSingleObject( i.electron, 0 );
      if ( objDecision == true ) {
        TCU::addDecisionID( m_decisionId.numeric(), i.decision );
      }
    }
    return StatusCode::SUCCESS;
}


StatusCode TrigEgammaFastElectronHypoTool::markPassing( std::vector<ElectronInfo>& input, const std::set<size_t>& passing ) const {

  for ( auto idx: passing ) 
    TCU::addDecisionID( m_decisionId.numeric(), input[idx].decision );
  return StatusCode::SUCCESS;
}


StatusCode TrigEgammaFastElectronHypoTool::multiplicitySelection( std::vector<ElectronInfo>& input ) const {
  HLT::Index2DVec passingSelection( m_multiplicity );
  
  for ( size_t cutIndex = 0; cutIndex < m_multiplicity; ++ cutIndex ) {
    size_t elIndex{ 0 };
    for ( auto elIter =  input.begin(); elIter != input.end(); ++elIter, ++elIndex ) {
      if ( TCU::passed( m_decisionId.numeric(), elIter->previousDecisionIDs ) ) {
	if ( decideOnSingleObject( elIter->electron, cutIndex ) ) {
	  passingSelection[cutIndex].push_back( elIndex );
	}
      }
    }
    // checking if by chance none of the objects passed the single obj selection, if so there will be no valid combination and we can skip
    if ( passingSelection[cutIndex].empty() ) {
      ATH_MSG_DEBUG( "No object passed selection " << cutIndex << " rejecting" );
      return StatusCode::SUCCESS;
    }
  }
  
  // go to the tedious counting
  std::set<size_t> passingIndices;
  if ( m_decisionPerCluster ) {            
    // additional constrain has to be applied for each combination
    // from each combination we extract set of clustusters associated to it
    // if all are distinct then size of the set should be == size of combination, 
    // if size of clusters is smaller then the combination consists of electrons from the same RoI
    // and ought to be ignored
    auto notFromSameRoI = [&]( const HLT::Index1DVec& comb ) {
      std::set<const xAOD::TrigEMCluster*> setOfClusters;
      for ( auto index: comb ) {
	setOfClusters.insert( input[index].cluster );
      }
      return setOfClusters.size() == comb.size();
    };

    HLT::elementsInUniqueCombinations( passingSelection, passingIndices, notFromSameRoI );    
  } else {
    HLT::elementsInUniqueCombinations( passingSelection, passingIndices );
  }
  
  return markPassing( input, passingIndices );
}

StatusCode TrigEgammaFastElectronHypoTool::decide(  std::vector<ElectronInfo>& input )  const {
  // handle the simplest and most common case ( multiplicity == 1 ) in easiest possible manner
  if ( m_trackPt.size() == 1 ) {
    return inclusiveSelection( input );

  } else {    
    return multiplicitySelection( input );    
  }

  return StatusCode::SUCCESS;
}


