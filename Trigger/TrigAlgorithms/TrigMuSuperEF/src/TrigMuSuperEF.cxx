/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>
#include <cmath>

#include "TrigMuSuperEF.h"


#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/System.h"

#include "Particle/TrackParticleContainer.h"
#include "MuonRecToolInterfaces/IMuonPatternSegmentAssociationTool.h"
#include "MuidEvent/MuidTrackContainer.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "TrkSegment/SegmentCollection.h"
#include "CaloEvent/CaloClusterContainer.h"

#include "TrigNavigation/TriggerElement.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigMuonEvent/CachingFeatureCollection.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

#include "TrigMuonToolInterfaces/ITrigMuonStandaloneTrackTool.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

// xAOD classes
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

// xAOD related tools
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "MuonCombinedToolInterfaces/IMuonCreatorTool.h"

// offline muon reco EDM wrapper classes
#include "MuonCombinedEvent/MuonCandidate.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/TagBase.h"
#include "MuonCombinedEvent/CaloTag.h"

// offline reco tools
#include "MuonCombinedToolInterfaces/IMuonCombinedTool.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedInDetExtensionTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"

// utility functions
#include "TrigMuSuperEFUtils.icc"
#include "CxxUtils/make_unique.h"

class ISvcLocator;

// ----------------------------------------
TrigMuSuperEF::TrigMuSuperEF(const std::string& name, ISvcLocator* pSvcLocator) :
  FexAlgo(name, pSvcLocator),
  m_TrigMuonEF_saTrackTool("TrigMuonEFStandaloneTrackTool/TrigMuonEFStandaloneTrackTool"),
  m_Roi_StillToBeAttached(0),
  m_totalTime(0),
  m_ctTrackParticleContainer(0),
  m_combTrkTrackColl(0),
  m_tracksCache(),
  m_doInsideOut(true),
  m_doOutsideIn(true),
  m_insideOutFirst(false),
  m_forceBoth(false),
  m_singleContainer(true),
  m_standaloneOnly(false),
  m_combinerOnly(false),
  m_fullScan(false),
  m_recordSegments(false),
  m_recordPatterns(false),
  m_recordTracks(false),
  m_dEtaRoI(0.2),
  m_dPhiRoI(0.2),
  m_idTrackParticlesName(""),
  m_saTrackParticleContName("MuonEFInfo_ExtrapTrackParticles"),
  m_msTrackParticleContName("MuonEFInfo_MSonlyTrackParticles"),
  m_cbTrackParticleContName("MuonEFInfo_CombTrackParticles"),
  m_ctTrackParticleContName("MuonEFInfo_CaloTagTrackParticles"),
  m_muonContName("MuonEFInfo"),
  m_debug(false),
  m_verbose(false),
  m_muonCombinedTool("MuonCombined::MuonCombinedTool/MuonCombinedTool"),
  m_muGirlTool("MuonCombined::MuGirlTagTool/MuGirlTagTool"),
  //  m_caloTagTool("TrigMuSuperEF_MuonCaloTagTool"),
  m_caloTagTool(""), //replace when configdb file is updated
  m_TrackToTrackParticleConvTool("TrackToTrackParticleConvTool",this),
  m_muonCreatorTool("MuonCreatorTool"),
  m_stauCreatorTool(""),
  m_doMuonFeature(false)
{

  declareProperty("doInsideOut",    m_doInsideOut,    "Run TrigMuGirl or not (starts from ID, then goes to MS)");
  declareProperty("doOutsideIn",    m_doOutsideIn,    "Run TrigMuonEF or not (starts from MS, then goes to ID)");
  declareProperty("insideOutFirst", m_insideOutFirst, "Run TrigMuGirl first (true) or run TrigMuonEF first (false)");
  declareProperty("forceBoth",      m_forceBoth,      "Always run both algorithms");
  declareProperty("SingleContainer", m_singleContainer, "Write muons of both algorithms in the same TrigMuonEFInfoContainer");
  declareProperty("StandaloneOnly", m_standaloneOnly, "Run only up to Muon Standalone tracks (extrapolated to IP)");
  declareProperty("CombinerOnly",  m_combinerOnly, "Run only the combiner (MS tracks must already by attached to the trigger element");
  declareProperty("CaloTagOnly", m_caloTagOnly, "Use calo-tagging to identify muon tracks using ID only");
  declareProperty("fullScan",       m_fullScan, "Search for tracks in the full Muon Spectrometer");

  declareProperty("TMEF_standaloneTrackTool",    m_TrigMuonEF_saTrackTool);
  declareProperty("MuGirlTool",              m_muGirlTool);
  declareProperty("CaloTagTool", m_caloTagTool);

  // tool to run offline combiner
  declareProperty("MuonCombinedTool", m_muonCombinedTool);
  // tool to create xAOD::TrackParticles
  declareProperty("TrkToTrackParticleConvTool", m_TrackToTrackParticleConvTool);
  // tool to create xAOD::Muons
  declareProperty("MuonCreatorTool", m_muonCreatorTool);
  declareProperty("StauCreatorTool", m_stauCreatorTool);

  declareProperty("recordSegmentCombinations",  m_recordSegments);
  declareProperty("recordPatternCombinations",  m_recordPatterns);
  declareProperty("recordTrackCollection",     m_recordTracks);

  declareProperty("doMuGirlMuonFeature",        m_doMuonFeature);

  declareProperty("deltaEtaRoI", m_dEtaRoI);
  declareProperty("deltaPhiRoI", m_dPhiRoI);   

  declareProperty("IdTrackParticles",           m_idTrackParticlesName);

  declareProperty("ExtrapolatedTrackParticleContName", m_saTrackParticleContName);
  declareProperty("MSonlyTrackParticleContName", m_msTrackParticleContName);
  declareProperty("CBTrackParticleContName", m_cbTrackParticleContName);
  declareProperty("CTTrackParticleContName", m_ctTrackParticleContName);
  declareProperty("MuonContName", m_muonContName);

  declareMonitoredStdContainer ("EF_trks_n",               m_monVars.numberOfTracks, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_type",            m_monVars.type,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_charge",          m_monVars.charge,         IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_chi2",            m_monVars.chi2,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_chi2Prob",        m_monVars.chi2Prob,       IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_match_chi2",      m_monVars.matchChi2,      IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_pt",              m_monVars.pt,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_phi",             m_monVars.phi,            IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_eta",             m_monVars.eta,            IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_d0",              m_monVars.d0,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_z0",              m_monVars.z0,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_nMdt",            m_monVars.nMdt,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_nRpcEta",         m_monVars.nRpcEta,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_nRpcPhi",         m_monVars.nRpcPhi,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_nCscEta",         m_monVars.nCscEta,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_nCscPhi",         m_monVars.nCscPhi,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_nTgcEta",         m_monVars.nTgcEta,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_nTgcPhi",         m_monVars.nTgcPhi,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_nIdSctHit",       m_monVars.nSct,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_nIdPixelHit",     m_monVars.nPixel,         IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_nIdTrtHit",       m_monVars.nTrt,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_trks_nHit",            m_monVars.nHit,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_IDtrks_n",             m_monVars.nIDTrks,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_IDtrks_phi",           m_monVars.IDTrk_phi,      IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_IDtrks_eta",           m_monVars.IDTrk_eta,      IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_IDtrks_pT",            m_monVars.IDTrk_pT,       IMonitoredAlgo::AutoClear);
  
  declareCombinedMonitoringVariables(m_TMEF_monVars.CB);
  declareCaloTagMonitoringVariables(m_TrigCaloTag_monVars);
}
// ----------------------------------------
TrigMuSuperEF::~TrigMuSuperEF()
{
}

//Function to clear the RoI cache (per RoI) used in the hltExecute function
void TrigMuSuperEF::clearRoiCache(){
  m_roi = 0;
  //m_Roi_StillToBeAttached = new TrigRoiDescriptor();
  delete m_Roi_StillToBeAttached;
  m_Roi_StillToBeAttached = 0;
  //clear the RoI cache (per RoI) in the standalone track tool
  if(m_doOutsideIn && !m_combinerOnly){
   m_TrigMuonEF_saTrackTool->clearRoiCache();
 }
 
}

HLT::ErrorCode 
TrigMuSuperEF::prepareRobRequests(const HLT::TriggerElement* input){

  ATH_MSG_DEBUG( "PREPARE ROB REQUESTS CALLED");

  HLT::RobRequestInfo* RRInfo = config()->robRequestInfo();
  if(m_doOutsideIn){
    const IRoiDescriptor* muonRoI = getRoiDescriptor(input, 0);
    const std::vector<uint32_t>& MdtRobList = m_TrigMuonEF_saTrackTool->getMdtRobList(muonRoI);
    const std::vector<uint32_t>& RpcRobList = m_TrigMuonEF_saTrackTool->getRpcRobList(muonRoI);
    const std::vector<uint32_t>& TgcRobList = m_TrigMuonEF_saTrackTool->getTgcRobList(muonRoI);
    const std::vector<uint32_t>& CscRobList = m_TrigMuonEF_saTrackTool->getCscRobList(muonRoI);

    RRInfo->addRequestScheduledRobIDs(MdtRobList);
    RRInfo->addRequestScheduledRobIDs(RpcRobList);
    RRInfo->addRequestScheduledRobIDs(TgcRobList);
    RRInfo->addRequestScheduledRobIDs(CscRobList);

    ATH_MSG_DEBUG( "Calling prepareRobRequests, find "<<MdtRobList.size()<<" Mdt Rob's, "<<RpcRobList.size()<<" Rpc Rob's, "<<TgcRobList.size()<< "Tgc Rob's, and "<<CscRobList.size()<< " Csc Rob's");

  }
return HLT::OK;
}
// ----------------------------------------
HLT::ErrorCode
TrigMuSuperEF::hltInitialize()
{
  m_debug   = msgLvl() <= MSG::DEBUG;
  m_verbose = msgLvl() <= MSG::VERBOSE;

  ATH_MSG_DEBUG( "Start of Initializing TrigMuSuperEF" );
  ATH_MSG_DEBUG( "package version = " << PACKAGE_VERSION );
  
  if( m_fullScan ) {
    ATH_MSG_INFO("In full scan mode");
    if(m_doInsideOut) {
      ATH_MSG_ERROR("doInsideOut is set to true, but we are in full scan - please fix the setup");
      return HLT::BAD_JOB_SETUP;
    }
    if(!m_doOutsideIn) {
      ATH_MSG_ERROR("doOutsideIn is set to false, but we are in full scan - please fix the setup");
      return HLT::BAD_JOB_SETUP;
    }

  }//full scan      

  if ( m_doInsideOut && m_doOutsideIn ) {
    if ( m_forceBoth ) {
      ATH_MSG_INFO("TrigMuGirl and TrigMuonEF always run both");
    } else if ( m_insideOutFirst ) {
      ATH_MSG_INFO("TrigMuGirl runs first and TrigMuonEF runs only if no muon found by TrigMuGirl");
    } else {
      ATH_MSG_INFO("TrigMuonEF runs first and TrigMuGirl runs only if no muon found by TrigMuonEF");
    }
  } else if ( m_doInsideOut ) {
      ATH_MSG_INFO("Only TrigMuGirl runs");
  } else if ( m_doOutsideIn ) {
      ATH_MSG_INFO("Only TrigMuonEF runs");
  }

  if ( m_standaloneOnly ) {
    ATH_MSG_INFO("Running in Muon Standalone Mode");
  }

  if( m_combinerOnly ) {
    ATH_MSG_INFO("Running in Combiner only Mode");
    if( m_standaloneOnly ) {
      ATH_MSG_ERROR("Cannot run in only MS-only and CB-only modes");
      return HLT::BAD_JOB_SETUP;
    }
  }
  
  if( m_caloTagOnly) { 
    ATH_MSG_INFO("Running in Calotag-only Mode");
    if( m_standaloneOnly ) {
      ATH_MSG_ERROR("Cannot run in Calotag-only and MS-only modes");
    }
    if ( m_combinerOnly ) { 
      ATH_MSG_ERROR("Cannot run in Calotag-only and Combined modes");
    }
  }

  if(!m_doOutsideIn && !m_doInsideOut) {
    ATH_MSG_ERROR( "Both MuonEF and MuGirl disabled" );
    return HLT::BAD_JOB_SETUP;
  }

  ATH_MSG_INFO("doTiming is set to: "<< doTiming());    
  if(doTiming()) m_totalTime = addTimer("TrigMuSuperEF_totalTime");

  // for now always loading TrigMuonEF tools, or get errors when running MuGirl only (9-1-15)
  //if ( m_doOutsideIn ) {
    // TrigMuonEF algtools
    if(!m_combinerOnly) {
      StatusCode sc = m_TrigMuonEF_saTrackTool.retrieve();
      if (sc.isSuccess()){
	ATH_MSG_INFO( "Retrieved " << m_TrigMuonEF_saTrackTool );
      }else{
	ATH_MSG_FATAL( "Could not get " << m_TrigMuonEF_saTrackTool );
	return HLT::BAD_JOB_SETUP;
      }

      if (doTiming()) m_TrigMuonEF_saTrackTool->setExtrapolatedTimers(this, m_TMEF_SATimers);
      m_TrigMuonEF_saTrackTool->declareExtrapolatedMonitoringVariables(this, m_TMEF_monVars);
    }

    if ( !m_standaloneOnly && !m_caloTagOnly ) {
      // Retrieve combiner tool
      if (m_muonCombinedTool.retrieve().isSuccess()){
	msg() << MSG::INFO << "Retrieved " << m_muonCombinedTool << endreq;
      }else{
	msg() << MSG::FATAL << "Could not get " << m_muonCombinedTool << endreq;
	return StatusCode::FAILURE;
      }
      if (doTiming()) setCombinedTimers(this, m_TMEF_CBTimers);
    }
    // }//setup TrigMuonEF

  // retrieve Trk::Track -> TrackParticle converter
  if (m_TrackToTrackParticleConvTool.retrieve().isSuccess()){
    msg() << MSG::INFO << "Retrieved " << m_TrackToTrackParticleConvTool << endreq;
  }else{
    msg() << MSG::FATAL << "Could not get " << m_TrackToTrackParticleConvTool << endreq;
    return StatusCode::FAILURE;
  }
  
  // retrieve the tool for making xAOD muons
  if(m_muonCreatorTool.retrieve().isSuccess()){
    msg() << MSG::INFO << "Retrieved " << m_muonCreatorTool << endreq;
  } else {
    msg() << MSG::FATAL << "Could not get " << m_muonCreatorTool << endreq;
    return StatusCode::FAILURE;
  }
  if(!m_stauCreatorTool.empty() ){
    if( m_stauCreatorTool.retrieve().isSuccess()){
      msg() << MSG::INFO << "Retrieved " << m_stauCreatorTool << endreq;
    } else {
      msg() << MSG::FATAL << "Could not get " << m_stauCreatorTool << endreq;
      return StatusCode::FAILURE;
    }
  }
  
  if ( m_doInsideOut ) {
    // Retrieve TrigMuGirl tool
    ATH_MSG_INFO("Try to retrieve " << m_muGirlTool);
    StatusCode sc = m_muGirlTool.retrieve();
    if (sc.isSuccess()){
      ATH_MSG_INFO( "Retrieved " << m_muGirlTool );
    }else{
      ATH_MSG_FATAL( "Could not get " << m_muGirlTool );
      return HLT::BAD_JOB_SETUP;
    }
  }

  //if ( m_caloTagOnly ) {
  //  //Retreive CaloTagTool
  //  StatusCode sc = m_caloTagTool.retrieve();
  //  if(sc.isSuccess()){
  //    ATH_MSG_INFO( "Retrieved " << m_caloTagTool );
  //  }else{
  //    ATH_MSG_FATAL("Could not get " << m_caloTagTool);
  //    return HLT::BAD_JOB_SETUP;
  //  }
  //}

  if (!m_caloTagOnly) {
    ATH_MSG_INFO("Output MS-only track particle container = " << m_msTrackParticleContName);
    ATH_MSG_INFO("Output SA track particle container = " << m_saTrackParticleContName);
    ATH_MSG_INFO("Output CB track particle container = " << m_cbTrackParticleContName);
    ATH_MSG_INFO("Output muon container = " << m_muonContName);
  }
  else {
    ATH_MSG_INFO("Output CT track particle container = " << m_ctTrackParticleContName);
  }

  ATH_MSG_DEBUG("End of init TrigMuSuperEF");
  return HLT::OK;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

HLT::ErrorCode
TrigMuSuperEF::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* TEout)
{
  ResetTimers(m_TMEF_SATimers);
  ResetTimers(m_TMEF_CBTimers);
  ResetTimers(m_TrigMuGirl_Timers);

  if(doTiming()) m_totalTime->start(); //Start the clock

  ++m_counter.total;
  unsigned int teID = TEout->getId();
  ++m_counter_perTE[teID].total;

  clearRoiCache();

  HLT::ErrorCode hltStatus = HLT::OK;
  //
  // some debug printout
  //
  if (m_debug) {
    std::string teInLabel;
    std::string teOutLabel;
    TrigConf::HLTTriggerElement::getLabel( inputTE->getId(), teInLabel );
    TrigConf::HLTTriggerElement::getLabel( TEout->getId(), teOutLabel );
    ATH_MSG_DEBUG("hltExecute() TE inID=" << inputTE->getId() << " TE outID=" << TEout->getId()
                  << " inLabel=" << teInLabel << " outLabel=" << teOutLabel);    
  }

  //
  // prepare for execute
  //

  auto muonContainerOwn = CxxUtils::make_unique<xAOD::MuonContainer>();
  m_muonContainer = muonContainerOwn.get();
  xAOD::MuonAuxContainer muonAuxContainer;
  muonContainerOwn->setStore( &muonAuxContainer );
  ATH_MSG_DEBUG("Created xAOD::MuonContainer");
  

  HLT::ErrorCode ec = HLT::OK;
  if( m_combinerOnly) {
    //@todo need to implement mode to run only the combiner
    ec = runCombinerOnly(inputTE, TEout,muonContainerOwn);
  } else if (m_caloTagOnly) {
    // run calotag mode
    ec = runCaloTagOnly(inputTE, TEout);
  } else { // not in combiner only mode
    ec = runStandardChain(inputTE, TEout,muonContainerOwn);
  }
  if(ec!=HLT::OK && ec!=HLT::MISSING_FEATURE) { // allow processing to finish for missing feature (e.g. when no muon found)
    ATH_MSG_ERROR("Problem in running TrigMuSuperEF");
    return ec;
  }

  //
  // post processing
  //
  
  if (m_caloTagOnly) {
    for (auto tp : *m_ctTrackParticleContainer) {
        ATH_MSG_DEBUG("Track Particle: pt, eta, phi = " << tp->pt() << ", " << tp->eta() << ", " << tp->phi());
    }

    if (m_ctTrackParticleContainer->size() > 0) {
        ++m_counter.pass;
        ++m_counter_perTE[teID].pass;
    }
  }

  else {
    for( auto muon : *m_muonContainer ) {
      ATH_MSG_DEBUG("Muon: author = " << muon->author() << ", " << "type = " << muon->muonType());
      ATH_MSG_DEBUG(" pt, eta, phi = " << muon->pt() << ", " << muon->eta() << ", " << muon->phi());
    }

    if (m_muonContainer->size()>0) {//may want to make this more clever looking e.g. at combined vs standalone
      ++m_counter.pass;
      ++m_counter_perTE[teID].pass;
    }
  }

  // always fill monitoring histograms, even if no muon found
  if(!m_caloTagOnly) {
    fillMonitoringVars( );
    fillCBMonitoringVars( );

  } else {
    fillCTMonitoringVars( *m_ctTrackParticleContainer );
  }

  // clear any collections that we created and were not taken by someone else
  clearCacheVector( m_tracksCache );

  // no muon found is OK
  if (hltStatus == HLT::MISSING_FEATURE) hltStatus = HLT::OK;
  
  if(doTiming()) m_totalTime->stop(); //Stop the clock

  return hltStatus;
}

  
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
HLT::ErrorCode
TrigMuSuperEF::hltFinalize()
{
  HLT::ErrorCode hltStatus = HLT::OK;
  ATH_MSG_DEBUG( "Finalizing TrigMuSuperEF" );
  // call hltFinalize on all sub-algorithms

  ATH_MSG_INFO( "------------- TrigMuSuperEF Output ----------------" );
  if ( m_doInsideOut && m_doOutsideIn ) {
    if ( m_forceBoth ) {
      ATH_MSG_INFO("TrigMuGirl and TrigMuonEF were always run both");
      printTrigMuGirlStats();
       printTrigMuonEFStats();
      printTotalStats();
    } else if ( m_insideOutFirst ) {
      ATH_MSG_INFO("TrigMuGirl was run first and TrigMuonEF was run only if no muon found by TrigMuGirl");
      printTrigMuGirlStats();
      printTrigMuonEFStats();
      printTotalStats();
    } else {
      ATH_MSG_INFO("TrigMuonEF was run first and TrigMuGirl was run only if no muon found by TrigMuonEF");
      printTrigMuonEFStats();
      printTrigMuGirlStats();
      printTotalStats();
    }
  } else if ( m_doInsideOut ) {
      ATH_MSG_INFO("Only TrigMuGirl was run");
      printTrigMuGirlStats();
  } else if ( m_doOutsideIn ) {
      ATH_MSG_INFO("Only TrigMuonEF was run");
      printTrigMuonEFStats();
  } else if ( m_caloTagOnly ) {
     ATH_MSG_INFO("Only CaloTag was run");
  }
  printTEStats();

  return  hltStatus;
}

HLT::ErrorCode TrigMuSuperEF::runCombinerOnly(const HLT::TriggerElement* inputTE, HLT::TriggerElement* TEout,std::unique_ptr<xAOD::MuonContainer> &muonContainerOwn) {
    // get xAOD ID tracks
    ElementLinkVector<xAOD::TrackParticleContainer> elv_xaodidtrks;
    HLT::ErrorCode hltStatus = getIDTrackParticleLinks(inputTE, elv_xaodidtrks);
    if(hltStatus != HLT::OK && hltStatus!=HLT::MISSING_FEATURE) {
      ATH_MSG_ERROR("Problem getting ID tracks");
      return hltStatus;
    }  
           // build InDetCandidates
    InDetCandidateCollection inDetCandidates;
    for(unsigned int itrack=0; itrack<elv_xaodidtrks.size(); ++itrack) {
      // Use ElementLink so the xAOD::TrackParticle has a link back to the Trk::Track
      inDetCandidates.push_back( new MuonCombined::InDetCandidate( elv_xaodidtrks.at(itrack) ) );
    }//loop on ID Trk::Tracks

    //Fill ID track monitoring variables
    fillIDMonitoringVars(elv_xaodidtrks);

    // get muons from the last step
    const xAOD::MuonContainer* lastMuons=0;
    hltStatus = getFeature(inputTE, lastMuons);
    if(hltStatus != HLT::OK || !lastMuons) {
      ATH_MSG_WARNING("Could not get xAOD::MuonContainer");
      return hltStatus;
    }

    // build muon candidates from the xAOD muons
    MuonCandidateCollection muonCandidates;
    for(auto muon : *lastMuons) {
      const ElementLink<xAOD::TrackParticleContainer>& saTrackLink = muon->muonSpectrometerTrackParticleLink();
      if(saTrackLink.isValid() && muon->isAvailable< ElementLink<xAOD::TrackParticleContainer> >("msTrackLink")) {
	const ElementLink<xAOD::TrackParticleContainer> msTrackLink = muon->auxdata< ElementLink<xAOD::TrackParticleContainer> >("msTrackLink");
	// for now have to deep copy the extrapolated track (MuonCandidate takes ownership)
	// Upgraded MuonCandidate in future should allow MuonCandidate( msTrackLink, saTrackLink ) without the copy
	if(msTrackLink.isValid()) muonCandidates.push_back( new MuonCombined::MuonCandidate( msTrackLink, new Trk::Track(*((*saTrackLink)->track())) ) );
	else ATH_MSG_WARNING("xAOD::Muon has msTrackLink, but it is not valid");
      }//valid Element Links
    }//loop on muons

    hltStatus = buildCombinedTracks( &muonCandidates, inDetCandidates, m_TMEF_monVars.CB,m_TMEF_CBTimers);

    ATH_MSG_DEBUG( "Call buildMuons, n(inDetCandidates) =  " << inDetCandidates.size());

    // make output containers
    xAOD::TrackParticleContainer* combTrackParticleCont = new xAOD::TrackParticleContainer();
    xAOD::TrackParticleAuxContainer combTrackParticleAuxCont;
    combTrackParticleCont->setStore( &combTrackParticleAuxCont );
    // can remove the extrapolated track containers once the MuonCandidate no longer takes ownership of the track
    TrackCollection* extrapolatedTracks = new TrackCollection();
    xAOD::TrackParticleContainer* saTrackParticleCont = new xAOD::TrackParticleContainer();
    xAOD::TrackParticleAuxContainer saTrackParticleAuxCont;
    saTrackParticleCont->setStore( &saTrackParticleAuxCont );

    hltStatus = buildMuons( &muonCandidates, &inDetCandidates, combTrackParticleCont, extrapolatedTracks, saTrackParticleCont);
    if(hltStatus!=HLT::OK) {
      ATH_MSG_ERROR("Problem building muons");
      return hltStatus;
    }

    // attach output
    if(attachOutput( TEout, combTrackParticleCont, extrapolatedTracks, saTrackParticleCont, std::move(muonContainerOwn))!=HLT::OK) {
      ATH_MSG_WARNING("Problem attaching output");
      return HLT::MISSING_FEATURE;
    }

    return hltStatus;
}// runCombinedReconstruction

HLT::ErrorCode TrigMuSuperEF::runCaloTagOnly(const HLT::TriggerElement* inputTE, HLT::TriggerElement* TEout) {
    ATH_MSG_DEBUG("Running CaloTag only mode");
    
    // get xAOD ID tracks
    ElementLinkVector<xAOD::TrackParticleContainer> elv_xaodidtrks;
    HLT::ErrorCode hltStatus = getIDTrackParticleLinks(inputTE, elv_xaodidtrks);
    if(hltStatus != HLT::OK && hltStatus!=HLT::MISSING_FEATURE) {
      ATH_MSG_ERROR("Problem getting ID tracks");
      return hltStatus;
    } 

    // build InDetCandidates
    InDetCandidateCollection inDetCandidates;
    for(unsigned int itrack=0; itrack<elv_xaodidtrks.size(); ++itrack) {
      // Use ElementLink so the xAOD::TrackParticle has a link back to the Trk::Track
      inDetCandidates.push_back( new MuonCombined::InDetCandidate( elv_xaodidtrks.at(itrack) ) );
    }//loop on ID xAOD::TrackParticles
    
    ATH_MSG_DEBUG( "Inner detector muon candidates, n(inDetCandidates) =  " << inDetCandidates.size());

    //Fill ID track monitoring variables
    fillIDMonitoringVars(elv_xaodidtrks);

    //Retrieve CaloCellContainer
    const CaloCellContainer* caloCellContainer = nullptr;
    if( getFeature(TEout, caloCellContainer, "") != HLT::OK || !caloCellContainer ) {
        ATH_MSG_ERROR("Failed to retrieve CaloCellContainer");
        return HLT::MISSING_FEATURE;
    }
    //Retrieve CaloClusterContainer
    const xAOD::CaloClusterContainer* caloClusterContainer = nullptr;
    if( getFeature(TEout, caloClusterContainer, "") != HLT::OK || !caloClusterContainer ) {
        ATH_MSG_ERROR("Failed to retrieve CaloClusterContainer");
        return HLT::MISSING_FEATURE;
    }
    
    //Do calotagging
    m_caloTagTool->extend(inDetCandidates, caloCellContainer, caloClusterContainer);

    ATH_MSG_DEBUG("Finished CaloTag");

    //Create output container
    //xAOD::TrackParticleContainer* idTrackParticleCont = new xAOD::TrackParticleContainer();
    //xAOD::TrackParticleAuxContainer idTrackParticleAuxCont;
    //idTrackParticleCont->setStore( &idTrackParticleAuxCont );
    //
    m_ctTrackParticleContainer = new xAOD::TrackParticleContainer();
    xAOD::TrackParticleAuxContainer* tpAuxCont = new xAOD::TrackParticleAuxContainer();
    m_ctTrackParticleContainer->setStore( tpAuxCont );

    for (auto idCandidate : inDetCandidates) {
        // Get calotag and select tracks with successful tag
        const MuonCombined::CaloTag* calotag = dynamic_cast<const MuonCombined::CaloTag*>(idCandidate->lastCombinedDataTag());
        
        if (calotag == nullptr) {
            ATH_MSG_WARNING("CaloTag was not attached to InDetCandidate");
            continue;
        }

        ATH_MSG_DEBUG("Calotag = " << calotag->caloMuonIdTag());

        if (calotag) {
            //attach to output container
            ATH_MSG_DEBUG("Attaching TrackParticle to TE");
            
            //Convoluted copy due to TrackParticle copy constructor not working correctly
            xAOD::TrackParticle* tp = new xAOD::TrackParticle();
            tp->makePrivateStore(idCandidate->indetTrackParticle());

            //Decorate tracks with Calotag results
            tp->auxdata<unsigned short>("CaloTag") = calotag->caloMuonIdTag();
            tp->auxdata<double>("CaloTagLH") = calotag->caloLRLikelihood();


            ATH_MSG_DEBUG("TPCont: " << m_ctTrackParticleContainer);
            //idTrackParticleCont->push_back( tp );
            //
            ATH_MSG_DEBUG("Size: " << m_ctTrackParticleContainer->size());
            m_ctTrackParticleContainer->push_back( tp );
            
            ATH_MSG_DEBUG("pT: " << tp->pt() << ", " << idCandidate->indetTrackParticle().pt()); 
        }
    } // loop over InDetCandidates

    //if (idTrackParticleCont->size() == 0) {
    if (m_ctTrackParticleContainer->size() == 0) {
        ATH_MSG_INFO("No muons found");
    }

    // attach output
    ATH_MSG_DEBUG("Attaching TrackParticles Container: " << m_ctTrackParticleContName);
    //if (attachFeature(TEout, idTrackParticleCont, m_ctTrackParticleContName) != HLT::OK) {
    if (attachFeature(TEout, m_ctTrackParticleContainer, m_ctTrackParticleContName) != HLT::OK) {
        ATH_MSG_WARNING("Problem attaching output");
        return HLT::MISSING_FEATURE;
    }

    return hltStatus;
} //runCaloTagOnly

//////////////////////// Run TrigMuonEF standalone reconstruction
HLT::ErrorCode TrigMuSuperEF::runMSReconstruction(const IRoiDescriptor* muonRoI, HLT::TriggerElement* TEout, MuonCandidateCollection& muonCandidates, std::unique_ptr<xAOD::MuonContainer>& muonContainerOwn) {
  ATH_MSG_DEBUG("Call getExtrapolatedTracks");

  // Use standalone track tool to find muon candidates
  HLT::ErrorCode hltStatus = m_TrigMuonEF_saTrackTool->getExtrapolatedTracks(muonRoI, muonCandidates, m_TMEF_monVars, m_TMEF_SATimers);

  // only bail out in case of real problem. So still attach features if no muon found: important for caching!
  if ( hltStatus != HLT::OK && hltStatus != HLT::MISSING_FEATURE ) return hltStatus;

  const MuonSegmentCombinationCollection* combiCol = m_TrigMuonEF_saTrackTool->segmentCombisToAttach();
  if (combiCol) {
    hltStatus = attachFeature(TEout, combiCol, "forTB");
    if(hltStatus!=HLT::OK) {
      ATH_MSG_WARNING("Attaching SegmentCombinationCollection to TEout unsuccessful");
      delete combiCol;
      return hltStatus;
    } else {
      ATH_MSG_DEBUG("Successfully attached to TEout the container of combined segments with size " << combiCol->size());
    }
  } else {
    ATH_MSG_DEBUG("SegmentCombinationCollection pointer zero, not attached to TE");
  }

  const Trk::SegmentCollection* segments = m_TrigMuonEF_saTrackTool->segmentsToAttach();
  if(segments) {
    hltStatus = attachFeature(TEout, segments, "forMT");
    if(hltStatus!=HLT::OK) {
      ATH_MSG_WARNING("Attaching segments to TEout: unsuccessful");
      delete segments;
      return hltStatus;
    } else {
      ATH_MSG_DEBUG("Successfully attached to TEout the container of segments with size " << segments->size());
    }
  } else {
    ATH_MSG_DEBUG("segments pointer zero, not attached to TE");
  }

  const TrackCollection* msTracks = m_TrigMuonEF_saTrackTool->spectrometerTracksToAttach();
  if (msTracks) {
    hltStatus = attachFeature(TEout, msTracks , "forSA");
    if(hltStatus!=HLT::OK) {
      msg() << MSG::WARNING << "Attaching spectrometer tracks to TEout: unsuccessful" << endreq;
      return hltStatus;
    } else {
      if (m_debug) msg() << MSG::DEBUG << "Successfully attached to TEout the spectrometer track container with size " << msTracks->size() << endreq;
    } 
  }
  
  // extract the MS track particles
  const xAOD::TrackParticleContainer* msTrackParticleCont = m_TrigMuonEF_saTrackTool->trackParticleContainerToAttach();
  const xAOD::TrackParticleAuxContainer* msTrackParticleAuxCont = m_TrigMuonEF_saTrackTool->trackParticleAuxContainerToAttach();


  if( msTrackParticleCont && msTrackParticleAuxCont ) {
    // attach MS track particles to the TE
    attachTrackParticleContainer( TEout, msTrackParticleCont, m_msTrackParticleContName);
  } else {
    ATH_MSG_WARNING("Problem with MS track particle container");
    if(msTrackParticleCont) delete msTrackParticleCont;
  }
  if(msTrackParticleAuxCont) delete msTrackParticleAuxCont; // always clean up Aux container

  // record collections  
  if (m_recordSegments)
    m_TrigMuonEF_saTrackTool->recordSegments();
  if (m_recordPatterns)
    m_TrigMuonEF_saTrackTool->recordPatterns();
  if (m_recordTracks)
    m_TrigMuonEF_saTrackTool->recordSpectrometerTracks();  

  if (!msTrackParticleCont || msTrackParticleCont->empty()) {
    ATH_MSG_DEBUG( "No muon tracks found: stop here to process the RoI." );
    return HLT::MISSING_FEATURE;
  } else {
    ATH_MSG_DEBUG( "Muon tracks container retrieved with size : " << msTrackParticleCont->size() );
  }

  if ( m_standaloneOnly ) {
    ATH_MSG_DEBUG("In standalone only mode, call buildMuons");
    // create xAOD track particle container for extrapolated tracks
    ATH_MSG_DEBUG("Create xAOD::TrackParticleContainer for extrapolated tracks");
    xAOD::TrackParticleContainer* saTrackParticleCont = new xAOD::TrackParticleContainer();
    xAOD::TrackParticleAuxContainer saTrackParticleAuxCont;
    saTrackParticleCont->setStore( &saTrackParticleAuxCont );
    // and the track container
    TrackCollection* extrapolatedTracks = new TrackCollection();
    // call muon builder, so muons are available to the hypos
    hltStatus = buildMuons( &muonCandidates, 0, 0, extrapolatedTracks, saTrackParticleCont );
    if(hltStatus!=HLT::OK) {
      ATH_MSG_ERROR("Error building muons");
      return hltStatus;
    }

    hltStatus = attachOutput( TEout, 0, extrapolatedTracks, saTrackParticleCont, std::move(muonContainerOwn) );
    if(hltStatus!=HLT::OK) {
      ATH_MSG_ERROR("Problem attaching output");
      return hltStatus;
    }
  }//standalone only

  return HLT::OK;
}//runMSReconstruction

HLT::ErrorCode
TrigMuSuperEF::runStandardChain(const HLT::TriggerElement* inputTE, HLT::TriggerElement* TEout, std::unique_ptr<xAOD::MuonContainer> &muonContainerOwn) 
{

  HLT::ErrorCode hltStatus = HLT::OK;
 
  const IRoiDescriptor* muonRoI = getRoiDescriptor(inputTE,TEout);

  if ( not muonRoI ) {
    std::string teInLabel;
    std::string teOutLabel;
    TrigConf::HLTTriggerElement::getLabel( inputTE->getId(), teInLabel );
    TrigConf::HLTTriggerElement::getLabel( TEout->getId(), teOutLabel );
    ATH_MSG_WARNING("No RoI found in inputTE=\"" << teInLabel << "\" outputTE=\"" << teOutLabel << "\". Not running TrigMuonEF");
    return HLT::MISSING_FEATURE;
  }

  // pre-define objects that are needed in muon building
  xAOD::TrackParticleContainer* combTrackParticleCont = 0;
  xAOD::TrackParticleAuxContainer combTrackParticleAuxCont;
  InDetCandidateCollection inDetCandidates;
  MuonCandidateCollection* muonCandidates= 0;

  if(m_standaloneOnly) {
    // Standalone only
    ++m_counter_TrigMuonEF.total;
    muonCandidates = new MuonCandidateCollection();
    hltStatus = runMSReconstruction(muonRoI, TEout, *muonCandidates, muonContainerOwn );

    ++m_counter_TrigMuonEF.pass;// check this

    delete muonCandidates;
    
    return hltStatus;
  } else {     // some kind of combined reco
    // get xAOD ID tracks
    ElementLinkVector<xAOD::TrackParticleContainer> elv_xaodidtrks;  
    hltStatus = getIDTrackParticleLinks(inputTE, elv_xaodidtrks);
    if(hltStatus != HLT::OK && hltStatus!=HLT::MISSING_FEATURE) {
      ATH_MSG_ERROR("Problem getting ID tracks");
      return hltStatus;
    }  
    // build InDetCandidates
    for(unsigned int itrack=0; itrack<elv_xaodidtrks.size(); ++itrack) {
      // Use ElementLink so the xAOD::TrackParticle has a link back to the Trk::Track
      inDetCandidates.push_back( new MuonCombined::InDetCandidate( elv_xaodidtrks.at(itrack) ) );
    }//loop on ID Trk::Tracks

    //Fill ID track monitoring variables
    fillIDMonitoringVars(elv_xaodidtrks);

    // prepare combined track particle container
    combTrackParticleCont = new xAOD::TrackParticleContainer();
    combTrackParticleCont->setStore( &combTrackParticleAuxCont );

    if(m_doOutsideIn && (!m_insideOutFirst || !m_doInsideOut) ) { // run TrigMuonEF

      ++m_counter_TrigMuonEF.total;
      // TrigMuonEF MS+CB
      muonCandidates = new MuonCandidateCollection();
      hltStatus = runMSCBReconstruction( muonRoI, TEout, *muonCandidates, inDetCandidates,  muonContainerOwn );

      if(hltStatus==HLT::OK) ++m_counter_TrigMuonEF.pass;
      else if(hltStatus!=HLT::MISSING_FEATURE) { // missing feature indicates no muon - which we don't report as an error
	ATH_MSG_ERROR("Problem running runMSCBReconstruction");
	return hltStatus;
      }

      if(m_doInsideOut) { // now run MuGirl
	if(m_forceBoth || hltStatus==HLT::MISSING_FEATURE) { 
	  ATH_MSG_DEBUG( "Executing extend()" );
	  ++m_counter_TrigMuGirl.total;
	  m_muGirlTool->extend(inDetCandidates);
	  ++m_counter_TrigMuGirl.pass;//@todo fix this counter
	}// need to run MuGirl
      }// doInsideOut

    }//TrigMuonEF first
    if(m_doInsideOut && (m_insideOutFirst || !m_doOutsideIn)) {
      ATH_MSG_DEBUG( "Executing extend()" );
      ++m_counter_TrigMuGirl.total;
      m_muGirlTool->extend(inDetCandidates);
      ++m_counter_TrigMuGirl.pass;//@todo fix this counter      

      if(m_doOutsideIn) {
	bool foundMuon=false;      // @todo: need to check if MuGirl tagged a muon - always assume it didn't for now
	if(m_forceBoth || !foundMuon) {
	  // now run TrigMuonEF MS+CB
	  ++m_counter_TrigMuonEF.total;
	  muonCandidates = new MuonCandidateCollection();
	  hltStatus = runMSCBReconstruction( muonRoI, TEout, *muonCandidates, inDetCandidates,  muonContainerOwn );
	  
	  if(hltStatus==HLT::OK) ++m_counter_TrigMuonEF.total;
	  else if(hltStatus!=HLT::MISSING_FEATURE) { // missing feature indicates no muon - which we don't report as an error
	    ATH_MSG_ERROR("Problem building combined tracks");
	    return hltStatus;
	  }
	}// need to run TrigMuonEF
      }//doOutsideIn
    }// MuGirl first

  }//combined running

  /////////////////////////// Build all output containers
  // includes xAOD muons, track particles and filling the SA TrackCollection
  TrackCollection* extrapolatedTracks = 0;
  xAOD::TrackParticleContainer* saTrackParticleCont = 0;
  xAOD::TrackParticleAuxContainer saTrackParticleAuxCont;
  if(muonCandidates) {
    extrapolatedTracks = new TrackCollection();
    saTrackParticleCont = new xAOD::TrackParticleContainer();
    saTrackParticleCont->setStore( &saTrackParticleAuxCont );
  }
  ATH_MSG_DEBUG( "Call buildMuons, n(inDetCandidates) =  " << inDetCandidates.size());
  hltStatus = buildMuons( muonCandidates, &inDetCandidates, combTrackParticleCont, extrapolatedTracks, saTrackParticleCont);
  if(hltStatus!=HLT::OK) {
    ATH_MSG_ERROR("Problem building muons");
    return hltStatus;
  }

  ATH_MSG_DEBUG("saTrackParticleCont size after buildMuons = " << saTrackParticleCont->size());

  // clean up muon candidates
  if(muonCandidates) delete muonCandidates;

  // attach output
  if(attachOutput( TEout, combTrackParticleCont, extrapolatedTracks, saTrackParticleCont, std::move(muonContainerOwn) )!=HLT::OK) {
    ATH_MSG_WARNING("Problem attaching output");
    return HLT::MISSING_FEATURE;
  }

  if (hltStatus!=HLT::OK) return hltStatus;
  
  // no muon found
  if(!m_combTrkTrackColl || m_combTrkTrackColl->empty()) {
    ATH_MSG_DEBUG( "No combined tracks found by TrigMuonEF, return HLT::MISSING_FEATURE");
    return HLT::MISSING_FEATURE;
  }

  ATH_MSG_DEBUG("Found " << m_combTrkTrackColl->size() << " combined tracks, return HLT::OK");
  return HLT::OK; // found muon
}

/// Function to run TrigMuonEF MS and then combined reco
HLT::ErrorCode TrigMuSuperEF::runMSCBReconstruction(const IRoiDescriptor* muonRoI,
						    HLT::TriggerElement* TEout, 
						    MuonCandidateCollection& muonCandidates,
						    InDetCandidateCollection& inDetCandidates, std::unique_ptr<xAOD::MuonContainer>& muonContainerOwn) {
  
  HLT::ErrorCode hltStatus = runMSReconstruction(muonRoI, TEout, muonCandidates, muonContainerOwn);
  if(hltStatus!=HLT::OK) {
    ATH_MSG_WARNING("Stop processing before looking for combined muons");
    return hltStatus;
  }

  ATH_MSG_DEBUG( "Executing buildCombinedTracks()" );
  
  // build combined tracks
  hltStatus = buildCombinedTracks(&muonCandidates,
				  inDetCandidates,
				  m_TMEF_monVars.CB,m_TMEF_CBTimers);

  return hltStatus;
}

/// Function to build combined tracks
HLT::ErrorCode TrigMuSuperEF::buildCombinedTracks(const MuonCandidateCollection* muonCandidates,
						  InDetCandidateCollection& inDetCandidates,
						  TrigMuonEFCBMonVars& ,
						  std::vector<TrigTimer*>& timers) {

  TrigTimer* trackFinderTime  = 0;
  TrigTimer* dataOutputTime = 0;
  if ( timers.size() >= 1 ) trackFinderTime = timers[0];
  if ( timers.size() >= 2 ) dataOutputTime  = timers[1];

  if(trackFinderTime) trackFinderTime->start();

  // call the combiner
  ATH_MSG_DEBUG("Call  m_muonCombinedTool->combine, n(muon cand)=" << muonCandidates->size() << " n(ID cand)=" << inDetCandidates.size());
  m_muonCombinedTool->combine( *muonCandidates,  inDetCandidates ) ;
    

  if(trackFinderTime) trackFinderTime->stop();
  if(dataOutputTime) dataOutputTime->start();
  for(auto candidate : inDetCandidates) {
    std::vector<const MuonCombined::TagBase*> tags = candidate->combinedDataTags();
    for(auto tag : tags) {
      if( tag->type() == xAOD::Muon::Combined ){
	if(dataOutputTime) dataOutputTime->stop();
	return HLT::OK; // found combined muon
      }
    }
  }

   if(dataOutputTime) dataOutputTime->stop();
   return HLT::MISSING_FEATURE; // if we got to here - no combined muon found
}

/**
 * Function to attach all the output info to the trigger element
 */
HLT::ErrorCode TrigMuSuperEF::attachOutput(HLT::TriggerElement* TEout,
					   xAOD::TrackParticleContainer* combinedTrackParticles,
					   const TrackCollection* extrapolatedTracks,
					   xAOD::TrackParticleContainer* extrapolatedTrackParticles,  std::unique_ptr<xAOD::MuonContainer> muonContainerOwn) {
  // attach TrackCollection for extrapolated tracks
  if(extrapolatedTracks) {
    HLT::ErrorCode hltStatus = attachFeature(TEout, extrapolatedTracks, "forCB");
    if( hltStatus!=HLT::OK ) {
      ATH_MSG_WARNING("attaching muon standalone tracks to TEout: unsuccessful");
      return hltStatus;
    } else {
      ATH_MSG_DEBUG("attached muon standalone tracks. Size: " << extrapolatedTracks->size());
    }
  }//valid extrapolated tracks

  // attach extrapolated track particles
  if(extrapolatedTrackParticles) attachTrackParticleContainer( TEout, extrapolatedTrackParticles, m_saTrackParticleContName );

  // attach info for combined tracks
  if(combinedTrackParticles) {
    attachTrackParticleContainer( TEout, combinedTrackParticles, m_cbTrackParticleContName);
  }
  
  if(m_combTrkTrackColl) {
    HLT::ErrorCode hltStatus = attachFeature(TEout, m_combTrkTrackColl, "MuonEFCombTrkTracks");
    if(hltStatus!=HLT::OK) {
      ATH_MSG_WARNING( "attaching combined tracks to TEout: unsuccessful" );
      return hltStatus;
    } else {
      ATH_MSG_DEBUG( "attached combined tracks. Size: " << m_combTrkTrackColl->size() );
      removeElement( m_tracksCache, m_combTrkTrackColl ); // make sure we don't try to delete it later
    }
    
  }

  // now pull back element links to the track particles and assign to the muons
  std::vector< std::pair< std::string, xAOD::Muon::TrackParticleType > > tpReassignVec;
  if(extrapolatedTrackParticles) tpReassignVec.push_back( std::make_pair(m_saTrackParticleContName, xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle) );
  if(combinedTrackParticles) tpReassignVec.push_back( std::make_pair(m_cbTrackParticleContName, xAOD::Muon::CombinedTrackParticle) );

  // loop on the containers we need to do this for
  for(auto reassignEle : tpReassignVec) {

    ATH_MSG_DEBUG("Start EleLink reassignment for " << reassignEle.first);
    ElementLinkVector<xAOD::TrackParticleContainer> elv_xaodtrks;
    HLT::ErrorCode hltStatus = getFeaturesLinks<xAOD::TrackParticleContainer,xAOD::TrackParticleContainer>(TEout, elv_xaodtrks, reassignEle.first);
    if( hltStatus != HLT::OK ) {
      ATH_MSG_WARNING("Failed to get ElementLinkVector<xAOD::TrackParticleContainer>, " << reassignEle.first << " links in xAOD::Muon will be bad after writing to disk");
    } else {
      // loop on muons
      for(auto muon : *muonContainerOwn) {
	// skip if muon has no MS TP
	if(!muon->trackParticleLink(reassignEle.second).isValid()) continue;

	bool foundmatch=false;
	const xAOD::TrackParticle* tp = *(muon->trackParticleLink(reassignEle.second));
	ATH_MSG_DEBUG("Try to find ElementLink for TP with pT = " << tp->pt());

	// loop on the vector of element links
	for(auto trklink : elv_xaodtrks) {
	  if(!trklink.isValid()) continue;
	  ATH_MSG_VERBOSE("Try next EleLink, pt = " << (*trklink)->pt());
	  if( *trklink == tp ) {
	    ATH_MSG_DEBUG("Found matching element link for this track particle, re-assign to xAOD::Muon, pt = " << (*trklink)->pt());
	    muon->setTrackParticleLink( reassignEle.second, trklink);
	    foundmatch=true;
	    break; // break from for loop
	  }// match
	}//loop on element links to track particles
	if(!foundmatch) ATH_MSG_WARNING("Did not find matching element link for this track particle, link in xAOD::Muon will be bad after writing to disk");

      }//loop on muons
    }
  }// loop on the trackparticles we need to reassign

  // attach xAOD muon container to TE

  if(muonContainerOwn) {
    size_t sz = muonContainerOwn->size();
    HLT::ErrorCode hltStatus = attachFeature(TEout, muonContainerOwn.release() , m_muonContName);
    if(hltStatus!=HLT::OK) {
      msg() << MSG::WARNING << "Attaching xAOD::MuonContainer to TEout: unsuccessful" << endreq;
      return hltStatus;
    } else {
      ATH_MSG_DEBUG( "Successfully attached to TEout the muon container with size " << sz );
    } 
  }

  return HLT::OK;
}

/**
 * Function to build xAOD muons from the results of the trigger reconstruction
 */
HLT::ErrorCode TrigMuSuperEF::buildMuons(const MuonCandidateCollection* muonCandidates,
					 InDetCandidateCollection* inDetCandidates,
					 xAOD::TrackParticleContainer* combinedTrackParticles,
					 TrackCollection* extrapolatedTracks,
					 xAOD::TrackParticleContainer* extrapolatedTrackParticles) {
  
  if(combinedTrackParticles) {
    // Track Collection for combined tracks
    m_combTrkTrackColl = new TrackCollection();
    // store the new combined TrackCollection for deletion at end of event if no-one takes ownership
    addElement( m_tracksCache, m_combTrkTrackColl);
  }

  // call muon creator tool to fill all xAOD objects
  // note, this also does duplicate removal

  ATH_MSG_DEBUG("Call m_muonCreatorTool->create");
  MuonCombined::IMuonCreatorTool::OutputData output(*m_muonContainer);
  output.combinedTrackParticleContainer = combinedTrackParticles;
  output.combinedTrackCollection = m_combTrkTrackColl;

  output.extrapolatedTrackParticleContainer = extrapolatedTrackParticles;
  output.extrapolatedTrackCollection = extrapolatedTracks;

  m_muonCreatorTool->create( muonCandidates, inDetCandidates, output);
  if( !m_stauCreatorTool.empty() ) m_stauCreatorTool->create( 0, inDetCandidates, output);

  ATH_MSG_DEBUG("N(input SA) = " << muonCandidates->size() << " N(SA from muon creator tool) = " << extrapolatedTracks->size());

  if(combinedTrackParticles) ATH_MSG_DEBUG("n(xAOD combinedTrackParticles)=" << combinedTrackParticles->size());
  if(extrapolatedTrackParticles) ATH_MSG_DEBUG("n(xAOD extrapolatedTrackParticles)=" << extrapolatedTrackParticles->size());
  ATH_MSG_DEBUG("n(xAOD muons)=" << m_muonContainer->size());  
  if(m_combTrkTrackColl) ATH_MSG_DEBUG("n(tracks) in m_combTrkTrackColl = " << m_combTrkTrackColl->size());

  return HLT::OK;
}

void TrigMuSuperEF::fillIDMonitoringVars(const ElementLinkVector<xAOD::TrackParticleContainer>& elv_idtrks){

  ATH_MSG_DEBUG( "Inner detector track monitoring started, container has size : " << elv_idtrks.size() );
  m_monVars.nIDTrks.push_back(elv_idtrks.size());
  for(auto elidtrk : elv_idtrks) {
    if(elidtrk.isValid()) {
      const xAOD::TrackParticle* idtrk = *elidtrk;
      m_monVars.IDTrk_phi.push_back(idtrk->phi());
      m_monVars.IDTrk_eta.push_back(idtrk->eta());
      m_monVars.IDTrk_pT.push_back( idtrk->pt()/1000.0 );
    }
  }//loop on ID tracks
}


void TrigMuSuperEF::fillCTMonitoringVars( const xAOD::TrackParticleContainer& idTrks )
{
    ATH_MSG_DEBUG("Filling CaloTag monitoring variables");

    m_TrigCaloTag_monVars.numberOfTracks.push_back(idTrks.size());
    
    for ( auto trk : idTrks ) {
    
        uint8_t numberOfPixelHits=0;
        uint8_t numberOfSCTHits=0;
        uint8_t numberOfTRTHits=0;
        if( trk->summaryValue(numberOfPixelHits,xAOD::numberOfPixelHits) ){
          ATH_MSG_DEBUG("Successfully retrieved the integer value, numberOfPixelHits");
        }
        if( trk->summaryValue(numberOfSCTHits,xAOD::numberOfSCTHits) ){
          ATH_MSG_DEBUG("Successfully retrieved the integer value, numberOfSCTHits");
        }
        if( trk->summaryValue(numberOfTRTHits,xAOD::numberOfTRTHits) ){
          ATH_MSG_DEBUG("Successfully retrieved the integer value, numberOfTRTHits");
        }

        //m_TrigCaloTag_monVars.nHit.push_back(0);
        m_TrigCaloTag_monVars.nSct.push_back(numberOfSCTHits);
        m_TrigCaloTag_monVars.nPixel.push_back(numberOfPixelHits);
        m_TrigCaloTag_monVars.nTrt.push_back(numberOfTRTHits);
        m_TrigCaloTag_monVars.IDTrk_CaloTag.push_back(trk->auxdata<unsigned short>("CaloTag"));
        m_TrigCaloTag_monVars.d0.push_back( trk->d0() );
        m_TrigCaloTag_monVars.z0.push_back( trk->z0() );
        m_TrigCaloTag_monVars.IDTrk_phi.push_back(trk->phi());
        m_TrigCaloTag_monVars.IDTrk_eta.push_back(trk->eta());
        m_TrigCaloTag_monVars.IDTrk_pT.push_back(trk->pt() / CLHEP::GeV);
        m_TrigCaloTag_monVars.IDTrk_CaloLH.push_back(trk->auxdata<double>("CaloTagLH"));
    }
    return;
}



void TrigMuSuperEF::fillMonitoringVars(  ) {


  //To be fixed: Still need to get some hit information.

  unsigned int nTracks = 0;
  for(auto muon : *m_muonContainer) {

    const xAOD::TrackParticle *trkpart = muon->primaryTrackParticle();


    ATH_MSG_DEBUG("muon type " << muon->muonType() << " muon pt " << muon->pt() << " trk d0 " << trkpart->d0() << " trk z0 " << trkpart->z0() << " trk chi2 " <<trkpart->chiSquared() << " trk charge "<<trkpart->charge());
    ++nTracks;
    if(m_debug) {
      if(muon->isAvailable< ElementLink<xAOD::TrackParticleContainer> >("msTrackLink")) {
	const ElementLink<xAOD::TrackParticleContainer> msTrackLink = muon->auxdata< ElementLink<xAOD::TrackParticleContainer> >("msTrackLink");
	if(msTrackLink.isValid()) ATH_MSG_DEBUG(" muon MS track before extrapolation pt = " << (*msTrackLink)->pt());
	else ATH_MSG_DEBUG(" muon MS track before extrapolation is not valid");
      } else ATH_MSG_DEBUG( " no MS track link available for this muon");
    }

    uint8_t numberOfPixelHits=0;
    uint8_t numberOfSCTHits=0;
    uint8_t numberOfTRTHits=0;
    if( trkpart->summaryValue(numberOfPixelHits,xAOD::numberOfPixelHits) ){
      ATH_MSG_DEBUG("Successfully retrieved the integer value, numberOfPixelHits");
    }
    if( trkpart->summaryValue(numberOfSCTHits,xAOD::numberOfSCTHits) ){
      ATH_MSG_DEBUG("Successfully retrieved the integer value, numberOfSCTHits");
    }
    if( trkpart->summaryValue(numberOfTRTHits,xAOD::numberOfTRTHits) ){
      ATH_MSG_DEBUG("Successfully retrieved the integer value, numberOfTRTHits");
    }

    m_monVars.type.push_back(muon->muonType());
    m_monVars.charge.push_back( trkpart->charge() );
    m_monVars.chi2.push_back( trkpart->chiSquared() );
    if(trkpart->numberDoF()>0) m_monVars.chi2Prob.push_back( trkpart->chiSquared()/trkpart->numberDoF() );
    float matchchi2 = 0.0;
    muon->parameter(matchchi2,xAOD::Muon::msOuterMatchChi2);
    m_monVars.matchChi2.push_back( matchchi2 );
    m_monVars.pt.push_back( muon->pt() / CLHEP::GeV );
    m_monVars.phi.push_back( muon->phi() );
    m_monVars.eta.push_back( muon->eta() );
    m_monVars.d0.push_back( trkpart->d0() );
    m_monVars.z0.push_back( trkpart->z0() );
    /*m_monVars.nMdt.push_back( trkpart->NMdtHits() );
    m_monVars.nRpcEta.push_back( trkpart->NRpcHitsEta() );
    m_monVars.nRpcPhi.push_back( trkpart->NRpcHitsPhi() );
    m_monVars.nCscEta.push_back( trkpart->NCscHitsEta() );
    m_monVars.nCscPhi.push_back( trkpart->NCscHitsPhi() );
    m_monVars.nTgcEta.push_back( trkpart->NTgcHitsEta() );
    m_monVars.nTgcPhi.push_back( trkpart->NTgcHitsPhi() );*/
    m_monVars.nSct.push_back( numberOfSCTHits );
    m_monVars.nPixel.push_back( numberOfPixelHits );
    m_monVars.nTrt.push_back( numberOfTRTHits );
    /*m_monVars.nHit.push_back( trkpart->NMdtHits() +
			      trkpart->NRpcHitsEta() +
			      trkpart->NRpcHitsPhi() +
			      trkpart->NCscHitsEta() +
			      trkpart->NCscHitsPhi() +
			      trkpart->NTgcHitsEta() +
			      trkpart->NTgcHitsPhi() +
			      trkpart->NIdSctHits() +
			      trkpart->NIdPixelHits() +
			      trkpart->NTrtHits() );
    */
  }//loop over muons
  m_monVars.numberOfTracks.push_back(nTracks);
  
}


void TrigMuSuperEF::fillCBMonitoringVars() {


  //To be fixed: Still need to get some hit information.

  unsigned int nTracks = 0;
  for(auto muon : *m_muonContainer) {
    if(muon->muonType()!=0) continue;
    const xAOD::TrackParticle *trkpart = muon->primaryTrackParticle();


    ATH_MSG_DEBUG("muon type " << muon->muonType() << " muon pt " << muon->pt() << " trk d0 " << trkpart->d0() << " trk z0 " << trkpart->z0() << " trk chi2 " <<trkpart->chiSquared() << " trk charge "<<trkpart->charge());
    ++nTracks;
    if(m_debug) {
      if(muon->isAvailable< ElementLink<xAOD::TrackParticleContainer> >("msTrackLink")) {
	const ElementLink<xAOD::TrackParticleContainer> msTrackLink = muon->auxdata< ElementLink<xAOD::TrackParticleContainer> >("msTrackLink");
	if(msTrackLink.isValid()) ATH_MSG_DEBUG(" muon MS track before extrapolation pt = " << (*msTrackLink)->pt());
	else ATH_MSG_DEBUG(" muon MS track before extrapolation is not valid");
      } else ATH_MSG_DEBUG( " no MS track link available for this muon");
    }

    uint8_t numberOfPixelHits=0;
    uint8_t numberOfSCTHits=0;
    uint8_t numberOfTRTHits=0;
    if( trkpart->summaryValue(numberOfPixelHits,xAOD::numberOfPixelHits) ){
      ATH_MSG_DEBUG("Successfully retrieved the integer value, numberOfPixelHits");
    }
    if( trkpart->summaryValue(numberOfSCTHits,xAOD::numberOfSCTHits) ){
      ATH_MSG_DEBUG("Successfully retrieved the integer value, numberOfSCTHits");
    }
    if( trkpart->summaryValue(numberOfTRTHits,xAOD::numberOfTRTHits) ){
      ATH_MSG_DEBUG("Successfully retrieved the integer value, numberOfTRTHits");
    }

     m_TMEF_monVars.CB.chi2.push_back( trkpart->chiSquared() );
    float matchchi2 = 0.0;
    muon->parameter(matchchi2,xAOD::Muon::msOuterMatchChi2);
     m_TMEF_monVars.CB.matchChi2.push_back( matchchi2 );
     m_TMEF_monVars.CB.pt.push_back( muon->pt() / CLHEP::GeV );
     m_TMEF_monVars.CB.phi.push_back( muon->phi() );
     m_TMEF_monVars.CB.eta.push_back( muon->eta() );
     m_TMEF_monVars.CB.d0.push_back( trkpart->d0() );
     m_TMEF_monVars.CB.z0.push_back( trkpart->z0() );
    /* m_TMEF_monVars.CB.nMdt.push_back( trkpart->NMdtHits() );
     m_TMEF_monVars.CB.nRpcEta.push_back( trkpart->NRpcHitsEta() );
     m_TMEF_monVars.CB.nRpcPhi.push_back( trkpart->NRpcHitsPhi() );
     m_TMEF_monVars.CB.nCscEta.push_back( trkpart->NCscHitsEta() );
     m_TMEF_monVars.CB.nCscPhi.push_back( trkpart->NCscHitsPhi() );
     m_TMEF_monVars.CB.nTgcEta.push_back( trkpart->NTgcHitsEta() );
     m_TMEF_monVars.CB.nTgcPhi.push_back( trkpart->NTgcHitsPhi() );*/
     m_TMEF_monVars.CB.nSct.push_back( numberOfSCTHits );
     m_TMEF_monVars.CB.nPixel.push_back( numberOfPixelHits );
     m_TMEF_monVars.CB.nTrt.push_back( numberOfTRTHits );
    /* m_TMEF_monVars.CB.nHit.push_back( trkpart->NMdtHits() +
			      trkpart->NRpcHitsEta() +
			      trkpart->NRpcHitsPhi() +
			      trkpart->NCscHitsEta() +
			      trkpart->NCscHitsPhi() +
			      trkpart->NTgcHitsEta() +
			      trkpart->NTgcHitsPhi() +
			      trkpart->NIdSctHits() +
			      trkpart->NIdPixelHits() +
			      trkpart->NTrtHits() );
    */
  }//loop over muons
   m_TMEF_monVars.CB.numberOfTracks.push_back(nTracks);
  
}


void TrigMuSuperEF::printCounterStats( const PassCounters& counter, const std::string& source, int precision ) const {
  double ratio = 0.0;
  if ( counter.total ) ratio = ((double)counter.pass) / ((double)counter.total);
  ATH_MSG_INFO("Number of ROI's processed " << source << ": " << counter.total
	       << "  found muons: " << counter.pass
	       << "  fraction: " << std::setprecision(precision) << ratio);
}

void TrigMuSuperEF::printTrigMuonEFStats() const {
  printCounterStats( m_counter_TrigMuonEF, "by TrigMuonEF" );
}

void TrigMuSuperEF::printTrigMuGirlStats() const {
  printCounterStats( m_counter_TrigMuGirl, "by TrigMuGirl" );
}

void TrigMuSuperEF::printTotalStats() const {
  printCounterStats( m_counter, "in total", 4 );
}

void TrigMuSuperEF::printTEStats() const {
  std::map<unsigned int,PassCounters>::const_iterator itC = m_counter_perTE.begin(), itC_end = m_counter_perTE.end();
  std::string tePrefix = "in TE ";
  std::string teLabel;
  for ( ; itC != itC_end; ++itC ) {
    TrigConf::HLTTriggerElement::getLabel( itC->first, teLabel );
    printCounterStats( itC->second, tePrefix + teLabel, 4 );
  }
}


const IRoiDescriptor* TrigMuSuperEF::getRoiDescriptor(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE) {
  if (!m_roi) {
    HLT::ErrorCode hltStatus = HLT::OK;
    const TrigRoiDescriptor* trigRoI = 0;
    const TrigRoiDescriptor* superRoI = 0;
     TrigRoiDescriptor* newTrigRoI = 0;
    // only get RoI if not in full scan mode. For full scan mode, 0 pointer for RoI is fine

    if(m_fullScan) {
      // first try to retrieve (partial) full-scan SuperRoI
      hltStatus = getFeature(inputTE, superRoI, "forMSFS");
      if ( hltStatus != HLT::OK ) {
	ATH_MSG_DEBUG("Failure in getFeature((composite)TrigRoiDescriptor,\"forMSFS\") due to internal navigation error");
      } else if (superRoI) { 
	if ( !superRoI->composite() ) {
	  ATH_MSG_DEBUG("Retrieved TrigRoiDescriptor \"forMFFS\" is not a composite RoI");
	  if(superRoI->isFullscan()) m_roi = superRoI;
	}
	else { 
	  ATH_MSG_VERBOSE("Retrieved (composite)TrigRoiDescriptor \"forMFFS\"");
	  m_roi = superRoI;
	}
      } else {
	ATH_MSG_DEBUG("Could not find (composite)TrigRoiDescriptor \"forMSFS\", trying to get the default one (\"\")");
	// try with default SuperRoI (latest added?)
	hltStatus = getFeature(inputTE, superRoI, "");
	if ( hltStatus != HLT::OK ) {
	  ATH_MSG_DEBUG("Failure in getFeature((composite)TrigRoiDescriptor,\"\") due to internal navigation error");
	} else if (superRoI) {
	  if ( !superRoI->composite() ) {
	    ATH_MSG_DEBUG("Retrieved TrigRoiDescriptor \"forMFFS\" is not a composite RoI, fullScan setting of RoI= " << superRoI->isFullscan());

	    if(superRoI->isFullscan()) m_roi = superRoI;
	  }
	  else { 
	    ATH_MSG_VERBOSE("Retrieved (composite)TrigRoiDescriptor \"\"");
	    m_roi = superRoI;
	  }
	}
	if(m_roi==0) {
	  // no RoI found, create new full-scan RoI
	  ATH_MSG_VERBOSE("No (composite)TrigRoiDescriptor found. Creating new full-scan RoI");
	  newTrigRoI = new TrigRoiDescriptor(true);
	  /// newTrigRoI->set_roiId(10000); /// do we need this identifier set ???
	  m_Roi_StillToBeAttached = newTrigRoI;
	  m_roi = newTrigRoI;
	}
      }
      // end of if m_fullScan
    } else {
      // RoI mode
      hltStatus = getFeature(inputTE, trigRoI, "forMS");  // RoI based
      if (hltStatus != HLT::OK) {
	ATH_MSG_DEBUG("Failure in getFeature(TrigRoIDescriptor,\"forMS\") due to internal navigation error");
	m_roi = 0;
	return 0;
      } else if (trigRoI) { // Found it!
	ATH_MSG_VERBOSE("Found RoIDescriptor \"forMS\"");
      } else { // did not find it
	ATH_MSG_DEBUG("Could not find RoIDescriptor \"forMS\", trying to recover the L1 one (with \"\")");
	hltStatus = getFeature(inputTE, trigRoI, "");
	if (hltStatus != HLT::OK) {
	  ATH_MSG_DEBUG("Failure in getFeature(TrigRoIDescriptor) due to internal navigation error");
	  m_roi = 0;
	  return 0;
	} else if (trigRoI) { // found it!
	  ATH_MSG_VERBOSE("Found (L1) RoIDescriptor \"\"");
	} else { // did not find it
	  ATH_MSG_DEBUG("Could not find (L1) RoIDescriptor \"\"");
	  m_roi = 0;
	  return 0;
	}
      }
      
      if (trigRoI) {
	newTrigRoI = createSingleTrigRoiDescriptor( trigRoI->eta(), trigRoI->phi(), m_dEtaRoI, m_dPhiRoI, trigRoI->roiId() );
	m_Roi_StillToBeAttached = newTrigRoI;
	m_roi = newTrigRoI;
      } // if trigRoI
      
    } // RoI mode
  } //if(!m_roi)

  if(outputTE && m_Roi_StillToBeAttached){
    attachFeature(outputTE, m_Roi_StillToBeAttached, "forSA");
    m_Roi_StillToBeAttached = 0;
  }
  
  return m_roi;
}

TrigRoiDescriptor* TrigMuSuperEF::createSingleTrigRoiDescriptor( double eta, double phi, double etaWidth, double phiWidth, int roiNum ) {
  double minphi = phi - std::abs(phiWidth);
  double maxphi = phi + std::abs(phiWidth);
  if(minphi <  -M_PI) minphi += 2.0*M_PI;
  if(minphi >=  M_PI) minphi -= 2.0*M_PI;
  if(maxphi <= -M_PI) maxphi += 2.0*M_PI;
  if(maxphi >   M_PI) maxphi -= 2.0*M_PI;
  double minEta = eta - std::abs(etaWidth);
  double maxEta = eta + std::abs(etaWidth);
  TrigRoiDescriptor* muonRoI = new TrigRoiDescriptor(unsigned(0), unsigned(roiNum), eta, minEta, maxEta, phi, minphi, maxphi);
  //  muonRoI->set_roiId(roiNum);

  ATH_MSG_VERBOSE( "Creating Roi #" << roiNum << " with center " << " (eta,phi) = (" << eta << "," << phi
                   << ") and size (dEta,dPhi) = (" << etaWidth << "," << phiWidth << ")" <<
                   " ==> etaRange = " << minEta << " -> " << maxEta <<
                   " phiRange = " << minphi << " -> " << maxphi );
 
  return muonRoI;
}


void TrigMuSuperEF::ResetTimers(std::vector<TrigTimer*>& timers){
  for(std::vector<TrigTimer*>::const_iterator time_it=timers.begin(); time_it!=timers.end(); time_it++){
    (*time_it)->reset();
  }
}

void TrigMuSuperEF::attachTrackParticleContainer( HLT::TriggerElement* TEout, const xAOD::TrackParticleContainer* trackParticleCont, const string& name) {

  if(trackParticleCont) {
    HLT::ErrorCode hltStatus = attachFeature(TEout, trackParticleCont , name);
    if(hltStatus!=HLT::OK) {
      msg() << MSG::WARNING << "Attaching xAOD::TrackParticleContainer " << trackParticleCont << " with name " << name << " to TEout: unsuccessful" << endreq;
      if(trackParticleCont) delete trackParticleCont;
    } else {
      if (m_debug) msg() << MSG::DEBUG << "Successfully attached to TEout the xAOD track particle container " << name << " with size " << trackParticleCont->size() << endreq;
    } 
  }//valid xAOD::TrackParticle container
  else {
    msg() << MSG::WARNING << "Called attachTrackParticleContainer with null TrackParticleContainer, name = " << name << ", likely a problem in the code" << endreq;
  }
}

// declare all monitored variables relating to combined muons
void TrigMuSuperEF::declareCombinedMonitoringVariables(TrigMuonEFCBMonVars& monVars)
{
  declareMonitoredStdContainer ("EF_cbtrks_n",               monVars.numberOfTracks, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cbtrks_chi2",            monVars.chi2,           IMonitoredAlgo::AutoClear);
  //declareMonitoredStdContainer ("EF_cbtrks_chi2Prob",        monVars.chi2Prob,     IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cbtrks_match_chi2",      monVars.matchChi2,      IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cbtrks_pt",              monVars.pt,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cbtrks_phi",             monVars.phi,            IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cbtrks_eta",             monVars.eta,            IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cbtrks_d0",              monVars.d0,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cbtrks_z0",              monVars.z0,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cbtrks_nMdt",            monVars.nMdt,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cbtrks_nRpcEta",         monVars.nRpcEta,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cbtrks_nRpcPhi",         monVars.nRpcPhi,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cbtrks_nCscEta",         monVars.nCscEta,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cbtrks_nCscPhi",         monVars.nCscPhi,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cbtrks_nTgcEta",         monVars.nTgcEta,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cbtrks_nTgcPhi",         monVars.nTgcPhi,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cbtrks_nHit",            monVars.nHit,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cbtrks_nIdSctHit",       monVars.nSct,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cbtrks_nIdPixelHit",     monVars.nPixel,         IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cbtrks_nIdTrtHit",       monVars.nTrt,           IMonitoredAlgo::AutoClear);


  return;
}


void TrigMuSuperEF::declareCaloTagMonitoringVariables(TrigMuonCaloTagMonVars& monVars)
{
  ATH_MSG_DEBUG("Declaring monitored CaloTag variables");

  declareMonitoredStdContainer ("EF_cttrks_n",               monVars.numberOfTracks, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cttrks_nHit",            monVars.nHit,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cttrks_nIdSctHit",       monVars.nSct,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cttrks_nIdPixelHit",     monVars.nPixel,         IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cttrks_nIdTrtHit",       monVars.nTrt,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cttrks_caloTag",         monVars.IDTrk_CaloTag,  IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cttrks_d0",              monVars.d0,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cttrks_z0",              monVars.z0,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cttrks_phi",             monVars.IDTrk_phi,      IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cttrks_eta",             monVars.IDTrk_eta,      IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cttrks_pT",              monVars.IDTrk_pT,       IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer ("EF_cttrks_caloLH",          monVars.IDTrk_CaloLH,   IMonitoredAlgo::AutoClear);

  return;
}



HLT::ErrorCode TrigMuSuperEF::getIDTrackParticleLinks(const HLT::TriggerElement* te, ElementLinkVector<xAOD::TrackParticleContainer>& elv_xaodidtrks) {
  // get element links to the ID tracks
  HLT::ErrorCode hltStatus = getFeaturesLinks<xAOD::TrackParticleContainer,xAOD::TrackParticleContainer>(te, elv_xaodidtrks, m_idTrackParticlesName);
  if( hltStatus != HLT::OK ) {
    ATH_MSG_WARNING("Failed to get ElementLinkVector<xAOD::TrackParticleContainer>, " << m_idTrackParticlesName << " aborting this ROI");
  } 
  return hltStatus;
}

void TrigMuSuperEF::setCombinedTimers(HLT::Algo* fexAlgo, std::vector<TrigTimer*>& timers){
  ATH_MSG_INFO("Setting TMEF Combined Timers");
  timers.push_back( fexAlgo->addTimer("TMEF_CBFinderTime") );
  timers.push_back( fexAlgo->addTimer("TMEF_CBDataOutputTime") );
}
