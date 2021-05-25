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
    m_decisionId( HLT::Identifier::fromToolName( name ) ) 
{}

//==================================================================


StatusCode TrigEgammaFastElectronHypoTool::initialize()  {
  
  if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );

  ATH_MSG_DEBUG( "Initialization completed successfully:" );
  ATH_MSG_DEBUG( "AcceptAll            = " << ( m_acceptAll==true ? "True" : "False" ) ); 
  ATH_MSG_DEBUG( "TrackPt              = " << m_trackPt              ); 
  ATH_MSG_DEBUG( "CaloTrackdETA        = " << m_caloTrackDEta        );
  ATH_MSG_DEBUG( "CaloTrackdPHI        = " << m_caloTrackDPhi        );
  ATH_MSG_DEBUG( "CaloTrackdEoverPLow  = " << m_caloTrackdEoverPLow  );
  ATH_MSG_DEBUG( "CaloTrackdEoverPHigh = " << m_caloTrackdEoverPHigh );
  ATH_MSG_DEBUG( "TRTRatio             = " << m_trtRatio             );
  ATH_MSG_DEBUG( "Do_LRT               = " << m_doLRT                );
  ATH_MSG_DEBUG( "d0Cut                = " << m_d0                   );


  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId  );

  return StatusCode::SUCCESS;
}

//==================================================================

StatusCode TrigEgammaFastElectronHypoTool::decide( std::vector<ElectronInfo>& input)  const {
  for ( auto& i: input ) {
    if ( TCU::passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i.electron ) ) {
        TCU::addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}

//==================================================================


bool TrigEgammaFastElectronHypoTool::decide( const xAOD::TrigElectron* electron) const 
{

  if ( m_acceptAll ) {
    ATH_MSG_DEBUG( "AcceptAll property is set: taking all events" );
    return true;
  } else {
     ATH_MSG_DEBUG( "AcceptAll property not set: applying selection" );
   }

  auto cutCounter = Monitored::Scalar<int>( "CutCounter", -1 );  
  auto ptCalo     = Monitored::Scalar( "PtCalo", -999. );
  auto ptTrack    = Monitored::Scalar( "PtTrack", -999. );
  auto dEtaCalo   = Monitored::Scalar( "CaloTrackEta", -1. );
  auto dPhiCalo   = Monitored::Scalar( "CaloTrackPhi", -1. );
  auto eToverPt   = Monitored::Scalar( "CaloTrackEoverP", -1. );
  auto caloEta    = Monitored::Scalar( "CaloEta", -100. );
  auto caloPhi    = Monitored::Scalar( "CaloEta", -100. );
  auto trk_d0    = Monitored::Scalar( "d0 value", -1. );
  auto monitorIt  = Monitored::Group( m_monTool, 
                                      cutCounter,
                                      ptCalo, ptTrack,    
                                      dEtaCalo, dPhiCalo,   
                                      eToverPt,   
                                      caloEta, caloPhi, trk_d0);

  const xAOD::TrackParticle* trkIter = electron->trackParticle();

  if ( !trkIter ){  // disconsider candidates without track
     return false;
  }
  cutCounter++;

  // Retrieve all quantities
  dPhiCalo    = electron->trkClusDphi();
  dEtaCalo    = electron->trkClusDeta();
  ptCalo      = electron->pt();	  
  eToverPt    = electron->etOverPt();	  
  caloEta     = electron->caloEta();
  caloPhi     = electron->caloPhi();
  trk_d0      = std::abs(trkIter->d0());

  float NTRHits     = static_cast<float>( electron->nTRTHits() );
  float NStrawHits  = static_cast<float>( electron->nTRTHiThresholdHits() );
  float TRTHitRatio = NStrawHits == 0 ? 1e10 : NTRHits/NStrawHits;

  if ( ptCalo < m_trackPt ){ 
    ATH_MSG_DEBUG( "Fails pt cut" << ptCalo << " < " << m_trackPt );
    return  false;
  }
  cutCounter++;

  if ( dEtaCalo > m_caloTrackDEta ) {
    ATH_MSG_DEBUG( "Fails dEta cut " << dEtaCalo << " < " << m_caloTrackDEta );
    return  false;
  }

  cutCounter++;
  if ( dPhiCalo > m_caloTrackDPhi ) {
    ATH_MSG_DEBUG( "Fails dPhi cut " << dPhiCalo << " < " << m_caloTrackDPhi );
    return  false;
  }

  cutCounter++;
  if( eToverPt <  m_caloTrackdEoverPLow ) {
    ATH_MSG_DEBUG( "Fails eoverp low cut " << eToverPt << " < " <<  m_caloTrackdEoverPLow );
    return false;
  }


  cutCounter++;
  if ( eToverPt > m_caloTrackdEoverPHigh ) {
    ATH_MSG_DEBUG( "Fails eoverp high cut " << eToverPt << " < " << m_caloTrackdEoverPHigh );
    return false;
  }


  cutCounter++;
  if ( TRTHitRatio < m_trtRatio ){
    ATH_MSG_DEBUG( "Fails TRT cut " << TRTHitRatio << " < " << m_trtRatio );
    return false;
  }


  cutCounter++;
  if(m_doLRT){
   ATH_MSG_DEBUG( "doLRT: " <<m_doLRT);
   ATH_MSG_DEBUG( "Track d0: " <<trk_d0);
   
   if(trk_d0 < m_d0){
     ATH_MSG_DEBUG( "Fails d0 cut " <<trk_d0<< " < " <<m_d0 );
     return false;
   }
  }
  ATH_MSG_DEBUG( "Passed selection" );
  return  true;

}
