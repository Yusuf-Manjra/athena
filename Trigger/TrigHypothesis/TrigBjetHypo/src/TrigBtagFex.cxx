/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBtagFex.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// ************************************************

#include "TrigBjetHypo/TrigBtagFex.h"

// ONLINE INFRASTRUCTURE
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h" // TO BE REMOVED

// BEAMSPOT
#include "InDetBeamSpotService/IBeamCondSvc.h"

// EDM

#include "xAODBase/IParticle.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "xAODTracking/TrackParticleContainer.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"

#include "xAODBTagging/BTagVertex.h"
#include "xAODBTagging/BTagVertexContainer.h"
#include "xAODBTagging/BTagVertexAuxContainer.h"

#include "BTagging/BTagTrackAssociation.h"
#include "BTagging/BTagSecVertexing.h"
#include "BTagging/BTagTool.h"

// ----------------------------------------------------------------------------------------------------------------- 


TrigBtagFex::TrigBtagFex(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::FexAlgo(name, pSvcLocator),
  m_bTagTrackAssocTool("Analysis::BTagTrackAssociation"),
  m_bTagSecVtxTool("Analysis::BTagSecVertexing")

{
  declareProperty("setupOfflineTools",  m_setupOfflineTools = false);

  declareProperty("TaggerBaseNames",    m_TaggerBaseNames);
  declareProperty("TrackKey",           m_trackKey = "");
  declareProperty("JetKey",             m_jetKey = "");
  declareProperty("PriVtxKey",          m_priVtxKey = "");

  declareProperty("BTagTool",           m_bTagTool);
  declareProperty("BTagTrackAssocTool", m_bTagTrackAssocTool);
  declareProperty("BTagSecVertexing",   m_bTagSecVtxTool);

  declareProperty ("UseBeamSpotFlag",    m_useBeamSpotFlag    = false);

  // Run-2 monitoring

  declareMonitoredVariable("sv_mass", m_mon_sv_mass, AutoClear);
  declareMonitoredVariable("sv_evtx", m_mon_sv_evtx, AutoClear);
  declareMonitoredVariable("sv_nvtx", m_mon_sv_nvtx, AutoClear);

  declareMonitoredVariable("tag_IP2D",    m_mon_tag_IP2D,    AutoClear);
  declareMonitoredVariable("tag_IP3D",    m_mon_tag_IP3D,    AutoClear);
  declareMonitoredVariable("tag_SV1",     m_mon_tag_SV1,     AutoClear);
  declareMonitoredVariable("tag_IP3DSV1", m_mon_tag_IP3DSV1, AutoClear);
  declareMonitoredVariable("tag_MV2c00",  m_mon_tag_MV2c00,  AutoClear);
  declareMonitoredVariable("tag_MV2c10",  m_mon_tag_MV2c10,  AutoClear);
  declareMonitoredVariable("tag_MV2c20",  m_mon_tag_MV2c20,  AutoClear);

}


// ----------------------------------------------------------------------------------------------------------------- 


TrigBtagFex::~TrigBtagFex() {

}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigBtagFex::hltInitialize() {

  // Get message service 
  if (msgLvl() <= MSG::INFO)
    msg() << MSG::INFO << "Initializing TrigBtagFex, version " << PACKAGE_VERSION << endreq;

  // declareProperty overview 
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endreq;
    // msg() << MSG::DEBUG << " AlgoId = "              << m_algo << endreq;
    // msg() << MSG::DEBUG << " Instance = "            << m_instance << endreq;

    msg() << MSG::DEBUG << " UseBeamSpotFlag = "     << m_useBeamSpotFlag << endreq;
    // msg() << MSG::DEBUG << " SetBeamSpotWidth = "    << m_setBeamSpotWidth << endreq;

    // msg() << MSG::DEBUG << " UseParamFromData = "    << m_useParamFromData << endreq;

    // msg() << MSG::DEBUG << " Using Offline Tools = " << m_setupOfflineTools << endreq;
    // msg() << MSG::DEBUG << " Taggers = "             << m_taggers << endreq;
    msg() << MSG::DEBUG << " Offline Taggers = "     << m_TaggerBaseNames << endreq;
    // msg() << MSG::DEBUG << " UseErrIPParam = "       << m_useErrIPParam << endreq;
    // msg() << MSG::DEBUG << " UseJetDirection = "     << m_useJetDirection << endreq;
    // msg() << MSG::DEBUG << " RetrieveHLTJets = "     << m_retrieveHLTJets << endreq;
    // msg() << MSG::DEBUG << " TagHLTJets = "          << m_tagHLTJets << endreq;
    // msg() << MSG::DEBUG << " HistoPrmVtxAtEF = "        << m_histoPrmVtxAtEF << endreq;
    // msg() << MSG::DEBUG << " UseEtaPhiTrackSel = "   << m_useEtaPhiTrackSel << endreq;

    // msg() << MSG::DEBUG << " JetProb 0 MC = "      << m_par_0_MC << endreq;
    // msg() << MSG::DEBUG << " JetProb 1 MC = "      << m_par_1_MC << endreq;
    // msg() << MSG::DEBUG << " JetProb 0 DT = "      << m_par_0_DT << endreq;
    // msg() << MSG::DEBUG << " JetProb 1 DT = "      << m_par_1_DT << endreq;

    // msg() << MSG::DEBUG << " SizeIP1D = "          << m_sizeIP1D << endreq;
    // msg() << MSG::DEBUG << " bIP1D = "             << m_bIP1D << endreq;
    // msg() << MSG::DEBUG << " uIP1D = "             << m_uIP1D << endreq;
    // msg() << MSG::DEBUG << " SizeIP2D = "          << m_sizeIP2D << endreq;
    // msg() << MSG::DEBUG << " bIP2D = "             << m_bIP2D << endreq;
    // msg() << MSG::DEBUG << " uIP2D = "             << m_uIP2D << endreq;
    // msg() << MSG::DEBUG << " SizeIP3D = "          << m_sizeIP3D << endreq;
    // msg() << MSG::DEBUG << " bIP3D = "             << m_bIP3D << endreq;
    // msg() << MSG::DEBUG << " uIP3D = "             << m_uIP3D << endreq;

    // msg() << MSG::DEBUG << " SizeIP1D_lowSiHits = "          << m_sizeIP1D_lowSiHits << endreq;
    // msg() << MSG::DEBUG << " bIP1D_lowSiHits = "             << m_bIP1D_lowSiHits << endreq;
    // msg() << MSG::DEBUG << " uIP1D_lowSiHits = "             << m_uIP1D_lowSiHits << endreq;
    // msg() << MSG::DEBUG << " SizeIP2D_lowSiHits = "          << m_sizeIP2D_lowSiHits << endreq;
    // msg() << MSG::DEBUG << " bIP2D_lowSiHits = "             << m_bIP2D_lowSiHits << endreq;
    // msg() << MSG::DEBUG << " uIP2D_lowSiHits = "             << m_uIP2D_lowSiHits << endreq;
    // msg() << MSG::DEBUG << " SizeIP3D_lowSiHits = "          << m_sizeIP3D_lowSiHits << endreq;
    // msg() << MSG::DEBUG << " bIP3D_lowSiHits = "             << m_bIP3D_lowSiHits << endreq;
    // msg() << MSG::DEBUG << " uIP3D_lowSiHits = "             << m_uIP3D_lowSiHits << endreq;

    // msg() << MSG::DEBUG << " SizeIP1D = "          << m_sizeIP1D << endreq;
    // msg() << MSG::DEBUG << " bIP1D = "             << m_bIP1D << endreq;
    // msg() << MSG::DEBUG << " uIP1D = "             << m_uIP1D << endreq;

    // msg() << MSG::DEBUG << " TrkSel_Chi2 = "     << m_trkSelChi2 << endreq;
    // msg() << MSG::DEBUG << " TrkSel_BLayer = "   << m_trkSelBLayer << endreq;
    // msg() << MSG::DEBUG << " TrkSel_SiHits = "   << m_trkSelSiHits << endreq;
    // msg() << MSG::DEBUG << " TrkSel_D0 = "       << m_trkSelD0 << endreq;
    // msg() << MSG::DEBUG << " TrkSel_Z0 = "       << m_trkSelZ0 << endreq;
    // msg() << MSG::DEBUG << " TrkSel_Pt = "       << m_trkSelPt << endreq;

    // msg() << MSG::DEBUG << " SizeMVtx = "       << m_sizeMVtx << endreq;
    // msg() << MSG::DEBUG << " bMVtx = "          << m_bMVtx << endreq;
    // msg() << MSG::DEBUG << " uMVtx = "          << m_uMVtx << endreq;
    // msg() << MSG::DEBUG << " SizeEVtx = "       << m_sizeEVtx << endreq;
    // msg() << MSG::DEBUG << " bEVtx = "          << m_bEVtx << endreq;
    // msg() << MSG::DEBUG << " uEVtx = "          << m_uEVtx << endreq;
    // msg() << MSG::DEBUG << " SizeNVtx = "       << m_sizeNVtx << endreq;
    // msg() << MSG::DEBUG << " bNVtx = "          << m_bNVtx << endreq;
    // msg() << MSG::DEBUG << " uNVtx = "          << m_uNVtx << endreq;
    // msg() << MSG::DEBUG << " SizeSV = "         << m_sizeSV << endreq;
    // msg() << MSG::DEBUG << " bSV = "            << m_bSV << endreq;
    // msg() << MSG::DEBUG << " uSV = "            << m_uSV << endreq;
  }

  if(m_setupOfflineTools) {

    // Retrieve the offline track association tool
    if(!m_bTagTrackAssocTool.empty()) {
      if(m_bTagTrackAssocTool.retrieve().isFailure()) {
	msg() << MSG::FATAL << "Failed to locate tool " << m_bTagTrackAssocTool << endreq;
	return HLT::BAD_JOB_SETUP;
      } else
	msg() << MSG::INFO << "Retrieved tool " << m_bTagTrackAssocTool << endreq;
    } else if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "No track association tool to retrieve" << endreq;

    // Retrieve the bTagSecVtxTool
    if(m_bTagSecVtxTool.retrieve().isFailure()) {
      msg() << MSG::FATAL << "Failed to locate tool " << m_bTagSecVtxTool << endreq;
      return HLT::BAD_JOB_SETUP;
    } else
      msg() << MSG::INFO << "Retrieved tool " << m_bTagSecVtxTool << endreq;

    // Retrieve the main BTagTool
    if(m_bTagTool.retrieve().isFailure()) {
      msg() << MSG::FATAL << "Failed to locate tool " << m_bTagTool << endreq;
      return HLT::BAD_JOB_SETUP;
    } else
      msg() << MSG::INFO << "Retrieved tool " << m_bTagTool << endreq;

  }

  return HLT::OK;
}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigBtagFex::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE) {

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Executing TrigBtagFex" << endreq;

  // RETRIEVE INPUT CONTAINERS

  // Get EF jet 
  const xAOD::JetContainer* jets = nullptr;
  if(getFeature(inputTE, jets, m_jetKey) == HLT::OK && jets != nullptr) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - xAOD::JetContainer: " << "nJets = " << jets->size() << endreq;
  } else {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - No xAOD::JetContainer" << endreq;
    return HLT::MISSING_FEATURE;
  }

  // Get primary vertex 
  const xAOD::VertexContainer* vertexes = nullptr;
  if (getFeature(outputTE, vertexes, m_priVtxKey) == HLT::OK && vertexes != nullptr) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - xAOD::VertexContainer: " << "nVertexes = " << vertexes->size() << endreq;
  } else {
    if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "INPUT - No xAOD::VertexContainer" << endreq;
    return HLT::MISSING_FEATURE;
  }

  // Get tracks 
  const xAOD::TrackParticleContainer* tracks = nullptr;
  if(getFeature(outputTE, tracks, m_trackKey) == HLT::OK && tracks != nullptr) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - xAOD::TrackParticleContainer: " << "nTracks = " << tracks->size() << endreq;
  } else {
    if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "INPUT - No xAOD::TrackParticleContainer" << endreq;
    return HLT::MISSING_FEATURE;
  }

  // PREPARE PROCESSING AND OUTPUT CONTAINERS

  // Prepare jet tagging - get primary vertex 
  auto vtxitr = vertexes->begin();
  const auto primaryVertex = *vtxitr;

  // Prepare jet tagging - create temporary jet copy 
  auto jetitr=jets->begin();
  xAOD::Jet jet;
  jet.makePrivateStore(**jetitr);

  // Prepare jet tagging - create SV output 
  auto    trigVertexContainer = new xAOD::VertexContainer();
  xAOD::VertexAuxContainer   trigSecVertexAuxContainer;
  trigVertexContainer->setStore(&trigSecVertexAuxContainer);

  // Prepare jet tagging - create JF output 
  auto   trigBTagVertexContainer = new xAOD::BTagVertexContainer();
  xAOD::VertexAuxContainer     trigBTagSecVertexAuxContainer;
  trigBTagVertexContainer->setStore(&trigBTagSecVertexAuxContainer);

  // Prepare jet tagging - create BTagging output 
  xAOD::BTaggingAuxContainer trigBTaggingAuxContainer;
  auto trigBTaggingContainer = new xAOD::BTaggingContainer();
  trigBTaggingContainer->setStore(&trigBTaggingAuxContainer);
  auto trigBTagging = new xAOD::BTagging();
  trigBTaggingContainer->push_back(trigBTagging);

  // EXECUTE OFFLINE TOOLS

  if(m_setupOfflineTools) {

    // Execute track association 
    if (!m_bTagTrackAssocTool.empty()) {
      StatusCode jetIsAssociated;

      // Link the BTagging object to the jet for track association
      ElementLink< xAOD::BTaggingContainer> linkBTagger;
      linkBTagger.toContainedElement(*trigBTaggingContainer, trigBTagging);
      jet.setBTaggingLink(linkBTagger);

      std::vector<xAOD::Jet*> jetsList;
      jetsList.push_back(&jet);
      if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "#BTAG# Track association tool is not empty" << endreq;
      // We must pass the tracks explicitly to the track associator
      jetIsAssociated = m_bTagTrackAssocTool->BTagTrackAssociation_exec(&jetsList, tracks);

      if ( jetIsAssociated.isFailure() ) {
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "#BTAG# Failed to associate tracks to jet" << endreq;
	return StatusCode::FAILURE;
      }
    }
    else {
      if(msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "#BTAG# Empty track association tool" << endreq;
    }

    // Execute secondary vertexing 
    StatusCode sc = m_bTagSecVtxTool->BTagSecVtx_exec(jet, trigBTagging, trigVertexContainer, trigBTagVertexContainer, primaryVertex);
    if(sc.isFailure()) {
      if(msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "#BTAG# Failed to reconstruct sec vtx" << endreq;
    }

    // Tag jet 
    sc = m_bTagTool->tagJet(jet, trigBTagging, primaryVertex);
    if(sc.isFailure()) {
      if(msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "#BTAG# Failed in taggers call" << endreq;
    }
  }

  // Fill monitoring variables
  trigBTagging->variable<float>("SV1", "masssvx",  m_mon_sv_mass);
  trigBTagging->variable<float>("SV1", "efracsvx", m_mon_sv_evtx);
  trigBTagging->variable<int>  ("SV1", "N2Tpair",  m_mon_sv_nvtx);

  m_mon_tag_IP2D    = trigBTagging->IP2D_loglikelihoodratio();
  m_mon_tag_IP3D    = trigBTagging->IP3D_loglikelihoodratio();
  m_mon_tag_SV1     = trigBTagging->SV1_loglikelihoodratio();
  m_mon_tag_IP3DSV1 = trigBTagging->SV1plusIP3D_discriminant();

  m_mon_tag_MV2c00 = trigBTagging->auxdata<double>("MV2c00_discriminant");
  m_mon_tag_MV2c10 = trigBTagging->auxdata<double>("MV2c10_discriminant");
  m_mon_tag_MV2c20 = trigBTagging->auxdata<double>("MV2c20_discriminant");

  // Dump results 
  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "IP2D u/b: " << trigBTagging->IP2D_pu() << "/" << trigBTagging->IP2D_pb()
          << "   IP3D u/b: " << trigBTagging->IP3D_pu() << "/" << trigBTagging->IP3D_pb()
          << "   SV1 u/b: " << trigBTagging->SV1_pu() << "/" << trigBTagging->SV1_pb()
          << "   MV2c20 var: " << trigBTagging->auxdata<double>("MV2c20_discriminant") << endreq;

  // ATTACH FEATURES AND CLEAR TEMPORARY OBJECTS

  // Create element link from the BTagging to the Jet container
  auto &jetAssociationLinks = trigBTagging->auxdata<std::vector<ElementLink<xAOD::IParticleContainer> > >("BTagBtagToJetAssociator");
  if (jets) {
    jetAssociationLinks.resize (jets->size());
    for(size_t i = 0; i < jets->size(); ++i) {
      jetAssociationLinks[i].toIndexedElement (*jets, i);
    }
  }

  // Attach BTagContainer as feature 
  if(attachFeature(outputTE, trigBTaggingContainer, "HLTBjetFex") == HLT::OK) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "OUTPUT - Attached xAOD::BTaggingContainer" << endreq;
  } else {
    if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "OUTPUT - Failed to attach xAOD::BTaggingContainer" << endreq;
    return HLT::NAV_ERROR;
  }
  if(attachFeature(outputTE, trigVertexContainer, "HLT_BjetSecondaryVertexFex") == HLT::OK) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "OUTPUT - Attached xAOD::VertexContainer" << endreq;
  } else {
    if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "OUTPUT - Failed to attach xAOD::VertexContainer" << endreq;
    return HLT::NAV_ERROR;
  }
  if(attachFeature(outputTE, trigBTagVertexContainer, "HLT_BjetVertexFex") == HLT::OK) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "OUTPUT - Attached xAOD::BTagVertexContainer" << endreq;
  } else {
    if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "OUTPUT - Failed to attach xAOD::BTagVertexContainer" << endreq;
    return HLT::NAV_ERROR;
  }

  return HLT::OK;
}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigBtagFex::hltFinalize() {

  if (msgLvl() <= MSG::INFO)
    msg() << MSG::INFO << "Finalizing TrigBtagFex" << endreq;

  return HLT::OK;
}
