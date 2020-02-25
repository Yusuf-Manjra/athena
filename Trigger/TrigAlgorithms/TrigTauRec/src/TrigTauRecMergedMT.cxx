
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/SystemOfUnits.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

#include "CaloEvent/CaloCellContainer.h"
#include "Particle/TrackParticleContainer.h"
#include "VxVertex/VxContainer.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetTrigAuxContainer.h"
#include "xAODJet/JetContainer.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauJet.h"

#include "xAODTau/TauDefs.h"
#include "xAODTau/TauTrack.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauTrackAuxContainer.h"

#include "LumiBlockComps/ILumiBlockMuTool.h"

#include "TrigTauRecMergedMT.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "AthAnalysisBaseComps/AthAnalysisHelper.h"

#include <iterator>
#include <algorithm>


// Invokes base class constructor.
TrigTauRecMergedMT::TrigTauRecMergedMT(const std::string& name,ISvcLocator* pSvcLocator)
  :AthAlgorithm(name, pSvcLocator),
   m_tools(this),
   m_endtools(this),
   m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool")
{
  declareProperty("Tools", m_tools, "List of ITauToolBase tools" );
  declareProperty("EndTools", m_endtools, "List of End ITauToolBase tools" );
  declareProperty("LumiBlockMuTool", m_lumiBlockMuTool, "Luminosity Tool" );
}

TrigTauRecMergedMT::~TrigTauRecMergedMT()
{ 
}

StatusCode TrigTauRecMergedMT::initialize()
{
  ATH_MSG_DEBUG("TrigTauRecMergedMT::initialize()");

  if ( m_tools.begin() == m_tools.end() ) {
    ATH_MSG_DEBUG(" no tools given for this algorithm.");
    return StatusCode::FAILURE;
  }

  ToolHandleArray<ITauToolBase> ::iterator p_itT = m_tools.begin();
  ToolHandleArray<ITauToolBase> ::iterator p_itTE = m_tools.end();
  ATH_MSG_DEBUG("List of tools in execution sequence:");

  for(; p_itT != p_itTE; ++p_itT ) {
    // make sure the key of the container in tauRecTool are the same
    // need to set the property before the initialization of tools
    if (p_itT->name().find("VertexFinder") != std::string::npos) {
      ATH_CHECK( AAH::setProperty(*p_itT, "Key_trackPartInputContainer",m_tracksKey.key()) );
      ATH_CHECK( AAH::setProperty(*p_itT, "Key_vertexInputContainer",m_vertexKey.key()) );
    }
    else if (p_itT->name().find("TrackFinder") != std::string::npos) {
      ATH_CHECK( AAH::setProperty(*p_itT, "Key_trackPartInputContainer",m_tracksKey.key()) );
    }

    StatusCode p_sc = p_itT->retrieve();
    if( p_sc.isFailure() ) {
      ATH_MSG_DEBUG("Cannot find tool named <" << *p_itT << ">");
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_DEBUG("Add timer for tool "<< ( *p_itT )->type() <<" "<< ( *p_itT )->name());
    }
  }

  ToolHandleArray<ITauToolBase> ::iterator p_itTe = m_endtools.begin();
  ToolHandleArray<ITauToolBase> ::iterator p_itTEe = m_endtools.end();

  for(; p_itTe != p_itTEe; ++p_itTe ) {
    StatusCode p_sc = p_itTe->retrieve();
    if( p_sc.isFailure() ) {
      ATH_MSG_DEBUG("Cannot find tool named <" << *p_itTe << ">");
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_DEBUG(" Add time for end tool "<< ( *p_itTe )->type() <<" "<< ( *p_itTe )->name());
    }
  }

  if (m_lumiBlockMuTool.retrieve().isFailure()) {
    ATH_MSG_WARNING("Unable to retrieve LumiBlockMuTool");
  } 
  else {
    ATH_MSG_DEBUG("Successfully retrieved LumiBlockMuTool");
  }

  // Retrieve beam conditions
  CHECK(m_beamSpotKey.initialize());

  if ( not m_monTool.name().empty() ) {
    ATH_CHECK( m_monTool.retrieve() );
  }

  ATH_MSG_DEBUG( "Initialising HandleKeys" );
  CHECK( m_roIInputKey.initialize()        );
  CHECK( m_L1RoIKey.initialize()           );
  CHECK( m_clustersKey.initialize(!m_clustersKey.key().empty()));
  CHECK( m_tracksKey.initialize(!m_tracksKey.key().empty()));
  CHECK( m_vertexKey.initialize(!m_vertexKey.key().empty()));
  CHECK( m_trigTauJetKey.initialize(!m_trigTauJetKey.key().empty()));
  CHECK( m_trigTauTrackInKey.initialize(!m_trigTauTrackInKey.key().empty()));
  CHECK( m_trigtauSeedOutKey.initialize()  );
  CHECK( m_trigtauRecOutKey.initialize()   );
  CHECK( m_trigtauTrkOutKey.initialize()   );

  return StatusCode::SUCCESS;
}

StatusCode TrigTauRecMergedMT::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode TrigTauRecMergedMT::execute()
{

  ATH_MSG_DEBUG("Execution");

  auto ctx = getContext();

  // variables to initialize and keep values for monitoring variables
  std::vector<unsigned char> calo_errors(0);
  std::vector<unsigned char> track_errors(0);

  auto nCells             = Monitored::Scalar<int>("nRoI_EFTauCells",    0);
  auto nTracks            = Monitored::Scalar<int>("nRoI_EFTauTracks", -10);
  auto dEta               = Monitored::Scalar<float>("dEtaEFTau_RoI",    -10.);
  auto dPhi               = Monitored::Scalar<float>("dPhiEFTau_RoI",    -10.);
  auto emRadius           = Monitored::Scalar<float>("EMRadius", -0.099);
  auto hadRadius          = Monitored::Scalar<float>("EMRadius", -0.099);
  auto EtFinal            = Monitored::Scalar<float>("EtFinal", 0.);
  auto Et                 = Monitored::Scalar<float>("Et", 0.);
  auto EtHad              = Monitored::Scalar<float>("EtHad",-10.);
  auto EtEm               = Monitored::Scalar<float>("EtEm",-10.);
  auto EMFrac             = Monitored::Scalar<float>("EMFrac",-10.);
  auto IsoFrac            = Monitored::Scalar<float>("IsoFrac",-1.);
  auto centFrac           = Monitored::Scalar<float>("centFrac",-10.);
  auto nWideTrk           = Monitored::Scalar<int>("EF_nWideTrk",0);
  auto ipSigLeadTrk       = Monitored::Scalar<float>("ipSigLeadTrk",-1000.);
  auto trFlightPathSig    = Monitored::Scalar<float>("trFlightPathSig",-10.);
  auto massTrkSys         = Monitored::Scalar<float>("massTrkSys",-10.);
  auto dRmax              = Monitored::Scalar<float>("dRmax",-10.);
  auto numTrack           = Monitored::Scalar<int>("EF_NTrk", -10);
  auto trkAvgDist         = Monitored::Scalar<float>("TrkAvgDist",-1.0);
  auto etovPtLead         = Monitored::Scalar<float>("EtovPtLead",-10.);
  auto PSSFraction        = Monitored::Scalar<float>("PSSFraction",-999.9);
  auto EMPOverTrkSysP     = Monitored::Scalar<float>("EMPOverTrkSysP",-999.9);
  auto ChPiEMEOverCaloEME = Monitored::Scalar<float>("ChPiEMEOverCaloEME",-999.9);
  auto SumPtTrkFrac       = Monitored::Scalar<float>("SumPtTrkFrac",-999.9);
  auto innerTrkAvgDist    = Monitored::Scalar<float>("innerTrkAvgDist",-1.0);
  auto Ncand              = Monitored::Scalar<int>("nCand",0);
  auto ActualInteractions = Monitored::Scalar<float>("ActualInteractions",-999.9);
  auto AvgInteractions    = Monitored::Scalar<float>("AvgInteractions",-999.9);
  auto beamspot_x         = Monitored::Scalar<float>("beamspot_x",-999.9);
  auto beamspot_y         = Monitored::Scalar<float>("beamspot_y",-999.9);
  auto beamspot_z         = Monitored::Scalar<float>("beamspot_z",-999.9);
  auto EtaL1              = Monitored::Scalar<float>("EtaL1",-99.9);
  auto PhiL1              = Monitored::Scalar<float>("PhiL1",-99.9);
  auto EtaEF              = Monitored::Scalar<float>("EtaEF",-99.9);
  auto PhiEF              = Monitored::Scalar<float>("PhiEF",-99.9);

  auto EF_calo_errors     = Monitored::Collection("calo_errors",calo_errors);
  auto EF_track_errors    = Monitored::Collection("track_errors",track_errors);

  auto monitorIt = Monitored::Group( m_monTool, nCells, nTracks, dEta, dPhi, emRadius, hadRadius,
                                     EtFinal, Et, EtHad, EtEm, EMFrac, IsoFrac, centFrac, nWideTrk, ipSigLeadTrk, trFlightPathSig, massTrkSys,
                                     dRmax, numTrack, trkAvgDist, etovPtLead, PSSFraction, EMPOverTrkSysP, ChPiEMEOverCaloEME, SumPtTrkFrac,
                                     innerTrkAvgDist, Ncand, ActualInteractions, AvgInteractions, beamspot_x, beamspot_y, beamspot_z, EtaL1,
                                     PhiL1, EtaEF, PhiEF );

  // Retrieve store.
  ATH_MSG_DEBUG("Executing TrigTauRecMergedMT");

  // Get RoiDescriptor
  SG::ReadHandle< TrigRoiDescriptorCollection > roisHandle = SG::makeHandle( m_roIInputKey, ctx );
  if ( not roisHandle.isValid() ) {
    calo_errors.push_back(NoROIDescr);
    return StatusCode::SUCCESS;
  }

  if(roisHandle->size() == 0){
    ATH_MSG_DEBUG("RoIHandle size = Zero");
    return StatusCode::SUCCESS;
  }
  const TrigRoiDescriptor *roiDescriptor = roisHandle->at(0);

  if ( roiDescriptor != nullptr ) {
    ATH_MSG_DEBUG(" RoI " << *roiDescriptor);
  }
  else {
    ATH_MSG_DEBUG("Failed to find RoiDescriptor ");
    calo_errors.push_back(NoROIDescr);
    return StatusCode::SUCCESS;
  }

  double mu = 0.0;
  double avg_mu = 0.0;
  if(m_lumiBlockMuTool){
    mu     = m_lumiBlockMuTool->actualInteractionsPerCrossing(); // (retrieve mu for the current BCID)
    avg_mu = m_lumiBlockMuTool->averageInteractionsPerCrossing();
    ActualInteractions = mu;
    AvgInteractions    = avg_mu;
    ATH_MSG_DEBUG(" Retrieved Mu Value : " << mu);
    ATH_MSG_DEBUG(" Average Mu Value   : " << avg_mu);
  }
	

  //-------------------------------------------------------------------------
  // Get beamspot
  //-------------------------------------------------------------------------

  // Copy the first vertex from a const object
  xAOD::Vertex theBeamspot;
  theBeamspot.makePrivateStore();

  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
  if(beamSpotHandle.isValid()){
	
    // Alter the position of the vertex
    theBeamspot.setPosition(beamSpotHandle->beamPos());
	
    beamspot_x=theBeamspot.x();
    beamspot_y=theBeamspot.y();
    beamspot_z=theBeamspot.z();

    // Create a AmgSymMatrix to alter the vertex covariance mat.
    const auto& cov = beamSpotHandle->beamVtx().covariancePosition();
    theBeamspot.setCovariancePosition(cov);
  }

  // get TauJetContainer from SG
  const xAOD::TauJetContainer *pTauContainer = nullptr;
  const xAOD::TauTrackContainer *pTauTrackContainer = nullptr;

  if (!m_trigTauJetKey.key().empty() && m_clustersKey.key().empty()) {
    SG::ReadHandle<xAOD::TauJetContainer> tauInputHandle(m_trigTauJetKey);
    pTauContainer = tauInputHandle.cptr();
    ATH_MSG_DEBUG("Tau Calo Only Container Size" << pTauContainer->size());
  }

  if (!m_trigTauTrackInKey.key().empty() && m_clustersKey.key().empty()) {
    SG::ReadHandle<xAOD::TauTrackContainer> tauTrackInputHandle(m_trigTauTrackInKey);
    pTauTrackContainer = tauTrackInputHandle.cptr();
    ATH_MSG_DEBUG("Tau Track Container Size" << pTauTrackContainer->size());
  }


  // Make new container which is deep copy of that
  xAOD::TauJetContainer*    pContainer    = nullptr;
  xAOD::TauJetAuxContainer* pAuxContainer = nullptr;
  xAOD::TauJet* tau(0);
  xAOD::TauJet* p_tau(0);

  xAOD::TauTrackContainer*    pTrackContainer    = nullptr;
  xAOD::TauTrackAuxContainer* pTrackAuxContainer = nullptr;
  xAOD::TauTrack* tautrack(0);

  ATH_CHECK(deepCopy(pContainer, pAuxContainer, tau, pTauContainer));
  if(pContainer->size()>0) p_tau = pContainer->back();

  ATH_CHECK(deepCopy(pTrackContainer, pTrackAuxContainer, tautrack, pTauTrackContainer));

  if(!m_trigTauTrackInKey.key().empty() && m_clustersKey.key().empty()){
    p_tau->clearTauTrackLinks();
    if(pTrackContainer != nullptr){
      ATH_MSG_DEBUG("TauTrackContainer size: " << pTrackContainer->size());
      for( xAOD::TauTrack* track : *pTrackContainer){
        ElementLink<xAOD::TauTrackContainer> linkToTauTrack;
        linkToTauTrack.toContainedElement(*pTrackContainer, track);
        p_tau->addTauTrackLink(linkToTauTrack);
      }
    }
  }

  const xAOD::CaloClusterContainer *RoICaloClusterContainer = nullptr;

  if(!m_clustersKey.key().empty()){
    // Retrieve Calocluster container
    SG::ReadHandle< xAOD::CaloClusterContainer > CCContainerHandle = SG::makeHandle( m_clustersKey,ctx );
    CHECK( CCContainerHandle.isValid() );

    RoICaloClusterContainer = CCContainerHandle.get();

    if(RoICaloClusterContainer != nullptr) {
      ATH_MSG_DEBUG( "CaloCluster container found of size: " << RoICaloClusterContainer->size());
      //If size is zero, don't stop just continue to produce empty TauJetCollection
    }
    else {
      ATH_MSG_ERROR( "no CaloCluster container found " );
      return StatusCode::FAILURE;
    }

    if(p_tau==nullptr){
      p_tau = new xAOD::TauJet();
      pContainer->push_back(p_tau);
      p_tau->setROIWord(roiDescriptor->roiWord());
    }

    std::unique_ptr<xAOD::JetContainer> theJetContainer{std::make_unique<xAOD::JetContainer>()};
    std::unique_ptr<xAOD::JetTrigAuxContainer> theTrigJetAuxContainer{std::make_unique<xAOD::JetTrigAuxContainer>()};

    theJetContainer->setStore(theTrigJetAuxContainer.get());

    xAOD::Jet *aJet = new xAOD::Jet;

    theJetContainer->push_back(aJet);
	
    // Build the jet, also keep track of the kinematics by hand
    // We are using calibrated clusters, we need to keep track of this
    aJet->setConstituentsSignalState(xAOD::JetConstitScale::CalibratedJetConstituent);
	
    // Make a minimal effort to speed things up ;)
    // Eventually, want to use FastJet here?
    TLorentzVector myCluster;
    TLorentzVector TauBarycenter(0., 0., 0., 0.);

    xAOD::CaloClusterContainer::const_iterator clusterIt;
    for (clusterIt=RoICaloClusterContainer->begin(); clusterIt != RoICaloClusterContainer->end(); ++clusterIt) {
      ATH_MSG_DEBUG(" Cluster (e, eta, phi) : ("<< (*clusterIt)->e() << " , " <<(*clusterIt)->eta()<<" , "<<(*clusterIt)->phi()<< " )");
    
      if((*clusterIt)->e() < 0)
        {
	  ATH_MSG_DEBUG(" Negative energy cluster is rejected");
	  continue;
        }
    
      myCluster.SetPtEtaPhiE((*clusterIt)->pt(), (*clusterIt)->eta(), (*clusterIt)->phi(), (*clusterIt)->e());
      aJet->addConstituent(*clusterIt);

      TauBarycenter += myCluster;
    }
	 
    aJet->setJetP4(xAOD::JetFourMom_t(TauBarycenter.Pt(), TauBarycenter.Eta(), TauBarycenter.Phi(), TauBarycenter.M() ) ); 

    ATH_MSG_DEBUG("jet formed"<< aJet->eta() <<" , " << aJet->phi() <<" , " << aJet->pt() << " , "<< aJet->e() );

    //-------------------------------------------------------------------------
    // using Jet collection
    // setup tau candidate structure
    //-------------------------------------------------------------------------

    p_tau->setJet(theJetContainer.get(), aJet);

    if(aJet->e()<=0) {
      ATH_MSG_DEBUG( "Roi: changing eta due to energy " << aJet->e() );
      p_tau->setP4(p_tau->pt(), roiDescriptor->eta(), roiDescriptor->phi(), p_tau->m());		
      ATH_MSG_DEBUG( "Roi: " << roiDescriptor->roiId() << " Tau eta: " << p_tau->eta() << " Tau phi: " << p_tau->phi() );
    }

    // Save Outputs
    SG::WriteHandle< xAOD::JetContainer > outTauSeedHandle = SG::makeHandle( m_trigtauSeedOutKey,ctx );
    CHECK( outTauSeedHandle.record( std::move( theJetContainer ), std::move( theTrigJetAuxContainer ) ) );
  }

  // get TrackContainer
  if(!m_tracksKey.key().empty()){
    SG::ReadHandle< xAOD::TrackParticleContainer > TPContainerHandle = SG::makeHandle( m_tracksKey,ctx );

    const xAOD::TrackParticleContainer *RoITrackParticleContainer = nullptr;

    if ( !TPContainerHandle.isValid() ) {
      ATH_MSG_DEBUG(" No Track container found.");
      track_errors.push_back(NoTrkCont);
    }
    else {
      RoITrackParticleContainer = TPContainerHandle.get();
      ATH_MSG_DEBUG(" Size of vector Track container is " << RoITrackParticleContainer->size());
      if(RoITrackParticleContainer != nullptr) nTracks = RoITrackParticleContainer->size();
    }
  }

  // get Vertex Container
  if(!m_vertexKey.key().empty()){
    SG::ReadHandle< xAOD::VertexContainer > VertexContainerHandle = SG::makeHandle( m_vertexKey,ctx );

    const xAOD::VertexContainer* RoIVxContainer = nullptr;

    if( !VertexContainerHandle.isValid() ) {
      ATH_MSG_DEBUG(" No VxContainers retrieved for the trigger element");
      track_errors.push_back(NoVtxCont);
    }
    else {
      RoIVxContainer = VertexContainerHandle.get();
      ATH_MSG_DEBUG(" Size of vector Vertex  container " << RoIVxContainer->size());
    }
  }

  // This sets one track and link. Need to have at least 1 track linked to retrieve track container
  // Can't we instead implement in the EDM a function to retrieve the track container of a tau?
  if(!m_clustersKey.key().empty() || pTrackContainer->size()==0) setEmptyTauTrack(p_tau, pTrackContainer);

  ATH_MSG_DEBUG(" roidescriptor roiword " << roiDescriptor->roiWord() << " saved " << p_tau->ROIWord() );

  StatusCode processStatus    = StatusCode::SUCCESS;

  //-------------------------------------------------------------------------
  // eventInitialize tauRec colls
  //-------------------------------------------------------------------------
  ToolHandleArray<ITauToolBase> ::iterator firstTool = m_tools.begin();
  ToolHandleArray<ITauToolBase> ::iterator lastTool  = m_tools.end();
  for ( ; firstTool != lastTool; firstTool++ ) {
    processStatus = (*firstTool)->eventInitialize();

    if( processStatus != StatusCode :: SUCCESS ) {
      ATH_MSG_ERROR("tool "<<(*firstTool)->name()<< "failed in eventInitialize");
      return StatusCode::FAILURE;
    }
  }
  ATH_MSG_DEBUG(" initialize all good ");

  //-------------------------------------------------------------------------
  // loop over booked tau tools
  //-------------------------------------------------------------------------
  int toolnum = 0;
  firstTool = m_tools.begin();
  lastTool  = m_tools.end();
  processStatus    = StatusCode::SUCCESS;
  
  // dummy container passed to TauVertexVariables, not used in trigger though
  xAOD::VertexContainer dummyVxCont;
  
  ATH_MSG_DEBUG("Starting tool loop with seed jet");
  while ( ! processStatus.isFailure() && firstTool != lastTool ) {
    // loop stops only when Failure indicated by one of the tools
    ATH_MSG_DEBUG("Starting Tool: " <<  (*firstTool)->name() );
    // time in the various tools
    ++toolnum;

    if ((*firstTool)->type() == "TauVertexFinder" ) {
      processStatus = (*firstTool)->executeVertexFinder(*p_tau);
    }
    else if ( (*firstTool)->type() == "TauTrackFinder") {
      processStatus = (*firstTool)->executeTrackFinder(*p_tau);
    }
    else if ( (*firstTool)->type() == "TauVertexVariables" ) {
      processStatus = (*firstTool)->executeVertexVariables(*p_tau, dummyVxCont);
    }
    else {
      processStatus = (*firstTool)->execute( *p_tau );
    }
    
    if ( !processStatus.isFailure() ) {
      ATH_MSG_DEBUG(" "<< (*firstTool)->name() << " executed successfully ");
      ATH_MSG_DEBUG(" Roi: " << roiDescriptor->roiId()
		    << " Tau eta: " << p_tau->eta() << " Tau phi: " << p_tau->phi()
		    << " Tau pT : "<< p_tau->pt());
    }
    else {
      ATH_MSG_DEBUG(" "<< (*firstTool)->name() << " execution failed ");
    }
    ++firstTool;
  }

  //check status
  if ( !processStatus.isSuccess() )  {   // some problem
    ATH_MSG_DEBUG("The tau object has NOT been registered in the tau container");

    xAOD::TauJet* bad_tau = pContainer->back();
    ATH_MSG_DEBUG("Deleting " << bad_tau->nAllTracks() << " tracks associated with tau");
    pTrackContainer->erase(pTrackContainer->end()-bad_tau->nAllTracks(), pTrackContainer->end());

    pContainer->pop_back();

    ATH_MSG_DEBUG("Clean up done after jet seed");
  }
  else {

    float fJetEnergy = (*p_tau->jetLink())->e();
    ATH_MSG_DEBUG("Roi: jet e "<< fJetEnergy);
	  
    if( fJetEnergy < 0.00001 ) {
      ATH_MSG_DEBUG("Roi: changing eta phi to L1 ones due to energy negative (PxPyPzE flips eta and phi)");
      ATH_MSG_DEBUG("Roi: this is probably not needed anymore, method PxPyPzE has been corrected");
	    
      p_tau->setP4(p_tau->pt(), roiDescriptor->eta(), roiDescriptor->phi(), p_tau->m());
	    
      ATH_MSG_DEBUG("Roi: " << roiDescriptor->roiId()
		    << " Tau eta: " << p_tau->eta()
		    << " Tau phi: " << p_tau->phi()
		    << " Tau pT : "<< p_tau->pt());
    }
	  
    // loop over end tools
    ToolHandleArray<ITauToolBase> ::iterator p_itET = m_endtools.begin();
    ToolHandleArray<ITauToolBase> ::iterator p_itETE = m_endtools.end();
    for (; p_itET != p_itETE; ++p_itET ) {
      ATH_MSG_VERBOSE("Invoking endTool " << ( *p_itET )->name() );
	    
      processStatus = ( *p_itET )->execute( *p_tau);
      if( processStatus.isFailure() ) break;
    }

    // Get L1 RoiDescriptor
    SG::ReadHandle< TrigRoiDescriptorCollection > L1roisHandle = SG::makeHandle( m_L1RoIKey, ctx );

    if ( L1roisHandle.isValid() && (L1roisHandle->size() != 0) ) {
      const TrigRoiDescriptor *roiL1Descriptor = L1roisHandle->at(0);      
      EtaL1 = roiL1Descriptor->eta();
      PhiL1 = roiL1Descriptor->phi();
    }
    else {
      ATH_MSG_WARNING("Failed to retrieve L1 RoI descriptor!");
    }

    // get tau detail variables for Monitoring
    numTrack = p_tau->nTracks();
    nWideTrk = p_tau->nTracksIsolation();
    p_tau->detail(xAOD::TauJetParameters::numCells, nCells);
    p_tau->detail(xAOD::TauJetParameters::nChargedTracks, nTracks);
    p_tau->detail(xAOD::TauJetParameters::trkAvgDist, trkAvgDist);
    p_tau->detail(xAOD::TauJetParameters::etOverPtLeadTrk, etovPtLead);
    p_tau->detail(xAOD::TauJetParameters::EMRadius, emRadius);
    p_tau->detail(xAOD::TauJetParameters::hadRadius, hadRadius);
    p_tau->detail(xAOD::TauJetParameters::isolFrac, IsoFrac);
    p_tau->detail(xAOD::TauJetParameters::centFrac, centFrac);
    p_tau->detail(xAOD::TauJetParameters::ipSigLeadTrk, ipSigLeadTrk);
    p_tau->detail(xAOD::TauJetParameters::trFlightPathSig, trFlightPathSig);
    p_tau->detail(xAOD::TauJetParameters::dRmax, dRmax);
    p_tau->detail(xAOD::TauJetParameters::massTrkSys, massTrkSys);
	  
    p_tau->detail(xAOD::TauJetParameters::PSSFraction, PSSFraction);
    p_tau->detail(xAOD::TauJetParameters::EMPOverTrkSysP, EMPOverTrkSysP);
    p_tau->detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, ChPiEMEOverCaloEME);
    p_tau->detail(xAOD::TauJetParameters::innerTrkAvgDist, innerTrkAvgDist);	 
    p_tau->detail(xAOD::TauJetParameters::SumPtTrkFrac, SumPtTrkFrac);

    massTrkSys /= Gaudi::Units::GeV;
    p_tau->detail(xAOD::TauJetParameters::etEMAtEMScale, EtEm);
    EtEm /= Gaudi::Units::GeV;
    p_tau->detail(xAOD::TauJetParameters::etHadAtEMScale, EtHad);
    EtHad /= Gaudi::Units::GeV;
    Et            = EtEm + EtHad;
    EtFinal       = p_tau->pt()/Gaudi::Units::GeV;
    
    EtaEF = p_tau->eta();
    PhiEF = p_tau->phi();
	  
    if( Et !=0) EMFrac =  EtEm/ Et ;
	  
    dEta =  EtaEF - roiDescriptor->eta();
    dPhi =  PhiEF - roiDescriptor->phi();
    if(dPhi<-M_PI) dPhi += 2.0*M_PI;
    if(dPhi>M_PI)  dPhi -= 2.0*M_PI;

    //std::vector<const xAOD::TauJetContainer*> tempCaloOnlyContVec;

    ATH_MSG_DEBUG(" Roi: " << roiDescriptor->roiId()
		  << " Tau being saved eta: " << EtaEF << " Tau phi: " << PhiEF
		  << " wrt L1 dEta "<< dEta<<" dPhi "<<dPhi
		  << " Tau Et (GeV): "<< EtFinal);
	  
    ++Ncand;
  }

	
  // call eventFinalize on the booked tau tools
  for ( firstTool = m_tools.begin(); firstTool != lastTool; firstTool++ ) {
    processStatus = (*firstTool)->eventFinalize();
    if( processStatus != StatusCode :: SUCCESS ) {
      ATH_MSG_DEBUG("tool "<<(*firstTool)->name()<< "failed in eventFinalize");
      return StatusCode::FAILURE;
    }
  }
  ATH_MSG_DEBUG("Tools succeed in eventFinalize");	
	
  //-------------------------------------------------------------------------
  // all done, register the tau Container in TDS.
  //-------------------------------------------------------------------------
  
  ATH_MSG_DEBUG("Output TauJetContainer size:"<< pContainer->size());
  ATH_MSG_DEBUG("Output TauTrackJetContainer size:"<< pTrackContainer->size());
  
  
  ATH_MSG_DEBUG("Recorded a tau container: HLT_TrigTauRecMergedMT");
  ATH_MSG_DEBUG("the tau object has been registered in the tau container");

  SG::WriteHandle<xAOD::TauTrackContainer> tauTrackHandle( m_trigtauTrkOutKey );
  ATH_MSG_DEBUG("  write: " << tauTrackHandle.key() << " = " << "..." );
  ATH_CHECK(tauTrackHandle.record(std::unique_ptr<xAOD::TauTrackContainer>(pTrackContainer), 
                       std::unique_ptr<xAOD::TauTrackAuxContainer>(pTrackAuxContainer)));

  // Write final taujets container
  SG::WriteHandle<xAOD::TauJetContainer> outputTauHandle(m_trigtauRecOutKey);
  ATH_CHECK(outputTauHandle.record(std::unique_ptr<xAOD::TauJetContainer>(pContainer),
                       std::unique_ptr<xAOD::TauJetAuxContainer>(pAuxContainer)));
  
  return StatusCode::SUCCESS;
}

void TrigTauRecMergedMT::setEmptyTauTrack(xAOD::TauJet*pTau,
					  xAOD::TauTrackContainer* tauTrackContainer)
{
  // Make a new tau track, add to container
  xAOD::TauTrack* pTrack = new xAOD::TauTrack();
  tauTrackContainer->push_back(pTrack);

  // Create an element link for that track
  ElementLink<xAOD::TauTrackContainer> linkToTauTrack;
  linkToTauTrack.toContainedElement(*tauTrackContainer, pTrack);
  pTau->addTauTrackLink(linkToTauTrack);
}

