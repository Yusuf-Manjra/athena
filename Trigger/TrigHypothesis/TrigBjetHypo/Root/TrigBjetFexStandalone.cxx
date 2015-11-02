/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetFex.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#include "TrigBjetFexStandalone.h"
#include "TaggerHelper.h"
#include "MsgStream.h"

#include "TrigBjetHypo/TrigBjetTagger.h"
#include "TrigBjetHypo/TuningLikelihood.h"

//#include "TrigInDetEvent/TrigInDetTrackCollection.h"
//#include "Particle/TrackParticleContainer.h"
//#include "TrigInDetEvent/TrigVertexCollection.h"
//#include "VxVertex/VxContainer.h"
//#include "VxSecVertex/VxSecVertexInfo.h"
//#include "VxSecVertex/VxSecVKalVertexInfo.h"
//#include "TrigSteeringEvent/TrigRoiDescriptor.h"
//#include "EventInfo/EventInfo.h"
//#include "EventInfo/EventID.h"

//#include "TrigParticle/TrigL2BjetContainer.h"
//#include "TrigParticle/TrigEFBjetContainer.h"

//#include "InDetBeamSpotService/IBeamCondSvc.h"

//#include "TrigInDetEvent/TrigVertex.h"



//** ----------------------------------------------------------------------------------------------------------------- **//


TrigBjetFexStandalone::TrigBjetFexStandalone(const std::string& name, bool negativeWeights) :
  HLT::FexAlgo(name),
  //m_trackJetFinderTool("TrigTrackJetFinderTool"),      	
  //m_trigL2BjetColl(0),
  //m_trigEFBjetColl(0),
  m_trigBjetTagger(0),
  m_constTrigBjetTagger(0),
  m_trigBjetPrmVtxInfo(0),
  m_trigBjetSecVtxInfo(0),
  m_trigBjetJetInfo(0),
  m_totTracks(0),
  m_totSelTracks(0),
  m_tuningLikelihoodIP1D(0),
  m_tuningLikelihoodIP2D(0),
  m_tuningLikelihoodIP3D(0),
  m_tuningLikelihoodIP1D_lowSiHits(0),
  m_tuningLikelihoodIP2D_lowSiHits(0),
  m_tuningLikelihoodIP3D_lowSiHits(0),
  m_tuningLikelihoodMVtx(0),
  m_tuningLikelihoodEVtx(0),
  m_tuningLikelihoodNVtx(0),
  m_tuningLikelihoodSV(0),
  m_negativeWeights(negativeWeights)
{
  declareProperty ("AlgoId",             m_algo);
  declareProperty ("Instance",           m_instance);
  declareProperty ("Taggers",            m_taggers);

  declareProperty ("par_0_MC",           m_par_0_MC);
  declareProperty ("par_1_MC",           m_par_1_MC);
  declareProperty ("par_0_DT",           m_par_0_DT);
  declareProperty ("par_1_DT",           m_par_1_DT);


  declareProperty ("UseJetDirection",    m_useJetDirection);

  declareProperty ("useLowSiHits",                 m_useLowSiHits = false);
  declareProperty ("SizeIP1D_lowSiHits",           m_sizeIP1D_lowSiHits);
  declareProperty ("bIP1D_lowSiHits",              m_bIP1D_lowSiHits);
  declareProperty ("uIP1D_lowSiHits",              m_uIP1D_lowSiHits);
  declareProperty ("SizeIP2D_lowSiHits",           m_sizeIP2D_lowSiHits);
  declareProperty ("bIP2D_lowSiHits",              m_bIP2D_lowSiHits);
  declareProperty ("uIP2D_lowSiHits",              m_uIP2D_lowSiHits);
  declareProperty ("SizeIP3D_lowSiHits",           m_sizeIP3D_lowSiHits);
  declareProperty ("bIP3D_lowSiHits",              m_bIP3D_lowSiHits);
  declareProperty ("uIP3D_lowSiHits",              m_uIP3D_lowSiHits);

  declareProperty ("SizeIP1D",           m_sizeIP1D);
  declareProperty ("bIP1D",              m_bIP1D);
  declareProperty ("uIP1D",              m_uIP1D);
  declareProperty ("SizeIP2D",           m_sizeIP2D);
  declareProperty ("bIP2D",              m_bIP2D);
  declareProperty ("uIP2D",              m_uIP2D);
  declareProperty ("SizeIP3D",           m_sizeIP3D);
  declareProperty ("bIP3D",              m_bIP3D);
  declareProperty ("uIP3D",              m_uIP3D);

  declareProperty ("SizeMVtx",           m_sizeMVtx);
  declareProperty ("bMVtx",              m_bMVtx);
  declareProperty ("uMVtx",              m_uMVtx);
  declareProperty ("SizeEVtx",           m_sizeEVtx);
  declareProperty ("bEVtx",              m_bEVtx);
  declareProperty ("uEVtx",              m_uEVtx);
  declareProperty ("SizeNVtx",           m_sizeNVtx);
  declareProperty ("bNVtx",              m_bNVtx);
  declareProperty ("uNVtx",              m_uNVtx);
  declareProperty ("SizeSV",             m_sizeSV);
  declareProperty ("bSV",                m_bSV);
  declareProperty ("uSV",                m_uSV);

  declareProperty ("UseBeamSpotFlag",    m_useBeamSpotFlag    = false);
  declareProperty ("SetBeamSpotWidth",   m_setBeamSpotWidth   = 0.05);

  declareProperty ("UseParamFromData",   m_useParamFromData   = false);

  declareProperty ("UseErrIPParam",      m_useErrIPParam      = false);
  declareProperty ("JetDirection",       m_jetDirection       = true);
  declareProperty ("L2PrmVtxAtEF",       m_l2PrmVtxAtEF       = true);
  declareProperty ("UseEtaPhiTrackSel",  m_useEtaPhiTrackSel  = false);

  declareProperty ("TrkSel_Chi2",        m_trkSelChi2         = 0.001);
  declareProperty ("TrkSel_BLayer",      m_trkSelBLayer       = 1);
  declareProperty ("TrkSel_PixHits",     m_trkSelPixHits      = 2);
  declareProperty ("TrkSel_SiHits",      m_trkSelSiHits       = 4);
  declareProperty ("TrkSel_D0",          m_trkSelD0           = 1*mm);
  declareProperty ("TrkSel_Z0",          m_trkSelZ0           = 2*mm);
  declareProperty ("TrkSel_Pt",          m_trkSelPt           = 1*GeV);

  //declareMonitoredStdContainer("trk_a0",            m_mon_trk_a0,        AutoClear);
  //declareMonitoredStdContainer("trk_a0_sel",        m_mon_trk_a0_sel,    AutoClear);
  //declareMonitoredStdContainer("trk_S(a0)_sel",     m_mon_trk_Sa0_sel,   AutoClear);
  //declareMonitoredStdContainer("trk_z0",            m_mon_trk_z0,        AutoClear);
  //declareMonitoredStdContainer("trk_z0_sel",        m_mon_trk_z0_sel,    AutoClear);
  //declareMonitoredStdContainer("trk_z0_sel_PV",     m_mon_trk_z0_sel_PV, AutoClear);
  //declareMonitoredStdContainer("trk_S(z0)_sel",     m_mon_trk_Sz0_sel,   AutoClear);
  //declareMonitoredStdContainer("trk_prob",          m_mon_trk_prob,      AutoClear);

  //declareMonitoredVariable    ("roi_nTracks",       m_totTracks);
  //declareMonitoredVariable    ("roi_nTracks_sel",   m_totSelTracks);
  //declareMonitoredStdContainer("roi_stepsToSelect", m_listCutApplied, AutoClear);
  //declareMonitoredObject      ("roi_selectedTracks", *this, &TrigBjetFex::totSelectedTracks);

  //declareMonitoredObject("X(IP1D)", m_constTrigBjetTagger, &TrigBjetTagger::getXIP1D);
  //declareMonitoredObject("X(IP2D)", m_constTrigBjetTagger, &TrigBjetTagger::getXIP2D);
  //declareMonitoredObject("X(IP3D)", m_constTrigBjetTagger, &TrigBjetTagger::getXIP3D);
  //declareMonitoredObject("X(SVTX)", m_constTrigBjetTagger, &TrigBjetTagger::getXSVTX);
  //declareMonitoredObject("X(COMB)", m_constTrigBjetTagger, &TrigBjetTagger::getXCOMB);
  //declareMonitoredObject("X(CHI2)", m_constTrigBjetTagger, &TrigBjetTagger::getXCHI2);

  m_taggerHelper = new TaggerHelper(msg(), msgLvl(), m_negativeWeights);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigBjetFexStandalone::~TrigBjetFexStandalone() {

  if (m_taggerHelper)            delete m_taggerHelper;
  if (m_trigBjetTagger)          delete m_trigBjetTagger;
  if (m_trigBjetPrmVtxInfo)      delete m_trigBjetPrmVtxInfo;
  if (m_trigBjetSecVtxInfo)      delete m_trigBjetSecVtxInfo;
  if (m_trigBjetJetInfo)         delete m_trigBjetJetInfo;
  if (m_tuningLikelihoodIP1D)    delete m_tuningLikelihoodIP1D;
  if (m_tuningLikelihoodIP2D)    delete m_tuningLikelihoodIP2D;
  if (m_tuningLikelihoodIP3D)    delete m_tuningLikelihoodIP3D;
  if (m_tuningLikelihoodIP1D_lowSiHits)    delete m_tuningLikelihoodIP1D_lowSiHits;
  if (m_tuningLikelihoodIP2D_lowSiHits)    delete m_tuningLikelihoodIP2D_lowSiHits;
  if (m_tuningLikelihoodIP3D_lowSiHits)    delete m_tuningLikelihoodIP3D_lowSiHits;
  if (m_tuningLikelihoodMVtx)    delete m_tuningLikelihoodMVtx;
  if (m_tuningLikelihoodEVtx)    delete m_tuningLikelihoodEVtx;
  if (m_tuningLikelihoodNVtx)    delete m_tuningLikelihoodNVtx;
  if (m_tuningLikelihoodSV)      delete m_tuningLikelihoodSV;
  if (m_trigBjetTrackInfoVectorPreSel) delete m_trigBjetTrackInfoVectorPreSel;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFexStandalone::hltInitialize() {
  
  //* Get message service *//
  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigBjetFex, version " << PACKAGE_VERSION << endreq;

  //* declareProperty overview *//
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endreq;
    msg() << MSG::DEBUG << " AlgoId = "              << m_algo << endreq; 
    msg() << MSG::DEBUG << " Instance = "            << m_instance << endreq;
 
    msg() << MSG::DEBUG << " UseBeamSpotFlag = "     << m_useBeamSpotFlag << endreq; 
    msg() << MSG::DEBUG << " SetBeamSpotWidth = "    << m_setBeamSpotWidth << endreq;

    msg() << MSG::DEBUG << " UseParamFromData = "     << m_useParamFromData << endreq; 

    msg() << MSG::DEBUG << " Taggers = "             << m_taggers << endreq; 
    msg() << MSG::DEBUG << " UseErrIPParam = "       << m_useErrIPParam << endreq; 
    msg() << MSG::DEBUG << " JetDirection = "        << m_jetDirection << endreq; 
    msg() << MSG::DEBUG << " L2PrmVtxAtEF = "        << m_l2PrmVtxAtEF << endreq;
    msg() << MSG::DEBUG << " UseEtaPhiTrackSel = "   << m_useEtaPhiTrackSel << endreq;

    msg() << MSG::DEBUG << " JetProb 0 MC = "      << m_par_0_MC << endreq; 
    msg() << MSG::DEBUG << " JetProb 1 MC = "      << m_par_1_MC << endreq; 
    msg() << MSG::DEBUG << " JetProb 0 DT = "      << m_par_0_DT << endreq; 
    msg() << MSG::DEBUG << " JetProb 1 DT = "      << m_par_1_DT << endreq; 

    msg() << MSG::DEBUG << " SizeIP1D = "          << m_sizeIP1D << endreq; 
    msg() << MSG::DEBUG << " bIP1D = "             << m_bIP1D << endreq; 
    msg() << MSG::DEBUG << " uIP1D = "             << m_uIP1D << endreq; 
    msg() << MSG::DEBUG << " SizeIP2D = "          << m_sizeIP2D << endreq;
    msg() << MSG::DEBUG << " bIP2D = "             << m_bIP2D << endreq; 
    msg() << MSG::DEBUG << " uIP2D = "             << m_uIP2D << endreq;  
    msg() << MSG::DEBUG << " SizeIP3D = "          << m_sizeIP3D << endreq; 
    msg() << MSG::DEBUG << " bIP3D = "             << m_bIP3D << endreq; 
    msg() << MSG::DEBUG << " uIP3D = "             << m_uIP3D << endreq; 

    msg() << MSG::DEBUG << " TrkSel_Chi2 = "     << m_trkSelChi2 << endreq; 
    msg() << MSG::DEBUG << " TrkSel_BLayer = "   << m_trkSelBLayer << endreq; 
    msg() << MSG::DEBUG << " TrkSel_SiHits = "   << m_trkSelSiHits << endreq; 
    msg() << MSG::DEBUG << " TrkSel_D0 = "       << m_trkSelD0 << endreq; 
    msg() << MSG::DEBUG << " TrkSel_Z0 = "       << m_trkSelZ0 << endreq; 
    msg() << MSG::DEBUG << " TrkSel_Pt = "       << m_trkSelPt << endreq; 

    if (m_instance == "EF") {
  
      msg() << MSG::DEBUG << " SizeMVtx = "       << m_sizeMVtx << endreq; 
      msg() << MSG::DEBUG << " bMVtx = "          << m_bMVtx << endreq; 
      msg() << MSG::DEBUG << " uMVtx = "          << m_uMVtx << endreq;
      msg() << MSG::DEBUG << " SizeEVtx = "       << m_sizeEVtx << endreq; 
      msg() << MSG::DEBUG << " bEVtx = "          << m_bEVtx << endreq; 
      msg() << MSG::DEBUG << " uEVtx = "          << m_uEVtx << endreq;  
      msg() << MSG::DEBUG << " SizeNVtx = "       << m_sizeNVtx << endreq; 
      msg() << MSG::DEBUG << " bNVtx = "          << m_bNVtx << endreq; 
      msg() << MSG::DEBUG << " uNVtx = "          << m_uNVtx << endreq;
      msg() << MSG::DEBUG << " SizeSV = "         << m_sizeSV << endreq; 
      msg() << MSG::DEBUG << " bSV = "            << m_bSV << endreq; 
      msg() << MSG::DEBUG << " uSV = "            << m_uSV << endreq;  
    }
  }

  m_trigBjetTagger = new TrigBjetTagger(this, msg(), msgLvl(), m_negativeWeights);
  m_constTrigBjetTagger  = const_cast<const TrigBjetTagger*>(m_trigBjetTagger);

  if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Retrieving tuning likelihoods." << endreq;

  m_tuningLikelihoodIP1D = new TuningLikelihood(&m_sizeIP1D[0], &m_bIP1D[0], &m_uIP1D[0], m_sizeIP1D.size());
  m_trigBjetTagger->fillLikelihoodMap("IP1D", m_tuningLikelihoodIP1D);
    
  m_tuningLikelihoodIP2D = new TuningLikelihood(&m_sizeIP2D[0], &m_bIP2D[0], &m_uIP2D[0], m_sizeIP2D.size());   
  m_trigBjetTagger->fillLikelihoodMap("IP2D", m_tuningLikelihoodIP2D);

  m_tuningLikelihoodIP3D = new TuningLikelihood(&m_sizeIP3D[0], &m_bIP3D[0], &m_uIP3D[0], m_sizeIP3D.size()); 
  m_trigBjetTagger->fillLikelihoodMap("IP3D", m_tuningLikelihoodIP3D);

  if (m_useLowSiHits) {
    m_tuningLikelihoodIP1D_lowSiHits = new TuningLikelihood(&m_sizeIP1D_lowSiHits[0], &m_bIP1D_lowSiHits[0], &m_uIP1D_lowSiHits[0], m_sizeIP1D_lowSiHits.size());
    m_trigBjetTagger->fillLikelihoodMap("IP1D_lowSiHits", m_tuningLikelihoodIP1D_lowSiHits);
    
    m_tuningLikelihoodIP2D_lowSiHits = new TuningLikelihood(&m_sizeIP2D_lowSiHits[0], &m_bIP2D_lowSiHits[0], &m_uIP2D_lowSiHits[0], m_sizeIP2D_lowSiHits.size());   
    m_trigBjetTagger->fillLikelihoodMap("IP2D_lowSiHits", m_tuningLikelihoodIP2D_lowSiHits);

    m_tuningLikelihoodIP3D_lowSiHits = new TuningLikelihood(&m_sizeIP3D_lowSiHits[0], &m_bIP3D_lowSiHits[0], &m_uIP3D_lowSiHits[0], m_sizeIP3D_lowSiHits.size()); 
    m_trigBjetTagger->fillLikelihoodMap("IP3D_lowSiHits", m_tuningLikelihoodIP3D_lowSiHits);
  }

  m_tuningLikelihoodMVtx = new TuningLikelihood(&m_sizeMVtx[0], &m_bMVtx[0], &m_uMVtx[0], m_sizeMVtx.size()); 
  m_trigBjetTagger->fillLikelihoodMap("MVTX", m_tuningLikelihoodMVtx);
    
  m_tuningLikelihoodEVtx = new TuningLikelihood(&m_sizeEVtx[0], &m_bEVtx[0], &m_uEVtx[0], m_sizeEVtx.size()); 
  m_trigBjetTagger->fillLikelihoodMap("EVTX", m_tuningLikelihoodEVtx);

  m_tuningLikelihoodNVtx = new TuningLikelihood(&m_sizeNVtx[0], &m_bNVtx[0], &m_uNVtx[0], m_sizeNVtx.size()); 
  m_trigBjetTagger->fillLikelihoodMap("NVTX", m_tuningLikelihoodNVtx);

  m_tuningLikelihoodSV = new TuningLikelihood(&m_sizeSV[0], &m_bSV[0], &m_uSV[0], m_sizeSV.size()); 
  m_trigBjetTagger->fillLikelihoodMap("SVTX", m_tuningLikelihoodSV);

  //* Retrieve TrigTrackJetFinder tool *//
  //StatusCode sc = m_trackJetFinderTool.retrieve();
  //if(sc.isFailure()) {
  //  msg() << MSG::FATAL << "Failed to locate tool " << m_trackJetFinderTool << endreq;
  //  return HLT::BAD_JOB_SETUP;
  //} else
  //  msg() << MSG::INFO << "Retrieved tool " << m_trackJetFinderTool << endreq;
  
  m_trackJetFinderTool = new TrigTrackJetFinderTool("TrigTrackJetFinderTool()");

  m_trigBjetPrmVtxInfo = new TrigBjetPrmVtxInfo();
  m_trigBjetSecVtxInfo = new TrigBjetSecVtxInfo();
  m_trigBjetJetInfo    = new TrigBjetJetInfo();
  m_trigBjetTrackInfoVectorPreSel  = new std::vector<TrigBjetTrackInfo>();
  m_trigBjetTrackInfoVectorPreSel->reserve(1000);

  return HLT::OK;
}



//** ----------------------------------------------------------------------------------------------------------------- **//


bool TrigBjetFexStandalone::l2TrackSel(const TrigBjetTrackInfo &track, unsigned int i) {

  float zv = m_trigBjetPrmVtxInfo->zPrmVtx();

  float d0  = track.d0();
  float z0  = track.z0();
  float theta = 2*TMath::ATan(TMath::Exp(-track.eta()));

  m_taggerHelper->IPCorr(track.d0(),track.z0(), d0, z0, track.phi(), track.eta(), track.pT(), 
			 m_trigBjetPrmVtxInfo->xBeamSpot(), m_trigBjetPrmVtxInfo->yBeamSpot());

  if (msgLvl() <= MSG::VERBOSE) {
    msg() << MSG::VERBOSE << "l2TrackSel method" << endreq;    
    msg() << MSG::VERBOSE << " Track number " << i+1 << " to be selected must be:" << endreq;
    msg() << MSG::VERBOSE << "  Pt " << track.pT() << " >= " << m_trkSelPt << endreq;
    msg() << MSG::VERBOSE << "  d0 " << fabs(d0) << " <= " << m_trkSelD0 << endreq;
    msg() << MSG::VERBOSE << "  z0*sin(theta) " << fabs(z0-zv)*TMath::Sin(theta) << " <= " << m_trkSelZ0 << endreq;
    msg() << MSG::VERBOSE << "  SiHit " << (int)track.siHits() << " >= " << m_trkSelSiHits << endreq;
    msg() << MSG::VERBOSE << "  isBLayer " << track.Blayer() << " = 0" << endreq;
    msg() << MSG::VERBOSE << "  Prob(chi2) " << TMath::Prob(track.chi2(),(int)track.siHits()*3-5) << " > " << m_trkSelChi2 << endreq;
  }

  if(m_useEtaPhiTrackSel) {

    if (fabs(track.eta() - m_trigBjetJetInfo->etaRoI()) > 0.2) {
      if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  track " << i+1 << " not selected (eta matching)" << endreq;;
      m_listCutApplied.push_back(2); return false;
    }

    if (fabs(m_taggerHelper->phiCorr(m_taggerHelper->phiCorr(track.phi()) - m_trigBjetJetInfo->phiRoI())) > 0.2) {
      if (msgLvl() <= MSG::VERBOSE)
	msg() << MSG::VERBOSE << "  track " << i+1 << " not selected (phi matching)" << endreq;;
      m_listCutApplied.push_back(3); return false;
    }
  }

  if (fabs(track.pT()) < m_trkSelPt) {
    if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  track " << i+1 << " not selected (pT cut)" << endreq;
    m_listCutApplied.push_back(4); return false;
  }

  if (fabs(d0) > m_trkSelD0) {
    if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  track " << i+1 << " not selected (d0 cut)" << endreq;
    m_listCutApplied.push_back(5); return false;
  }

  if (fabs(z0-zv)*TMath::Sin(theta) > m_trkSelZ0) {
    if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  track " << i+1 << " not selected (z0 cut)" << endreq;
    m_listCutApplied.push_back(6); return false;
  }

  if (track.Blayer() == 0) {
    if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  track " << i+1 << " not selected (missing b-layer hit)" << endreq;
    m_listCutApplied.push_back(7); return false;
  }

  if ((int)track.siHits() < m_trkSelSiHits) {
    if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  track " << i+1 << " not selected (too few silicon hits)" << endreq;
    m_listCutApplied.push_back(8); return false;
  }

  if (TMath::Prob(track.chi2(),(int)track.siHits()*3-5) <= m_trkSelChi2) {
    if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  track " << i+1 << " not selected (chi2 cut)" << endreq;
    m_listCutApplied.push_back(9); return false;
  }

  if (msgLvl() <= MSG::VERBOSE)
    msg() << MSG::VERBOSE << "  track " << i+1 << " selected" << endreq;

  m_listCutApplied.push_back(10);
  return true;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


bool TrigBjetFexStandalone::efTrackSel(const TrigBjetTrackInfo &track, unsigned int i) {

  float zv = m_trigBjetPrmVtxInfo->zPrmVtx();

  //const Trk::TrackSummary *summary = track->trackSummary();
  //const Trk::FitQuality *quality   = track->fitQuality();

  int numberOfSiHits = track.pixHits()+track.sctHits();
  float theta = 2*TMath::ATan(TMath::Exp(-track.eta()));
  float pT = track.pT();
  
  float d0 = track.d0();
  float z0 = track.z0();
  m_taggerHelper->IPCorr(track.d0(), track.z0(),
			 d0,z0,track.phi(), track.eta(), pT, m_trigBjetPrmVtxInfo->xBeamSpot(), m_trigBjetPrmVtxInfo->yBeamSpot());

  if (msgLvl() <= MSG::VERBOSE) {
    msg() << MSG::VERBOSE << "efTrackSel method" << endreq;
    msg() << MSG::VERBOSE << "  Track number " << i+1 << " to be selected must be:" << endreq;
    msg() << MSG::VERBOSE << "    Pt " << fabs(pT) << " >= " << m_trkSelPt << endreq;
    msg() << MSG::VERBOSE << "    d0 " << fabs(d0) << " <= " << m_trkSelD0 << endreq;
    msg() << MSG::VERBOSE << "    z0*sin(theta) " << fabs(z0-zv)*TMath::Sin(theta) << " <= " << m_trkSelZ0 << endreq;
    msg() << MSG::VERBOSE << "    bLayer " << track.Blayer() << " >= " << m_trkSelBLayer << endreq;
    msg() << MSG::VERBOSE << "    pixelHit " << track.pixHits() << " >= " << m_trkSelPixHits << endreq;
    msg() << MSG::VERBOSE << "    SiHit " << (track.pixHits() + track.sctHits()) << " >= " 
	  << m_trkSelSiHits << endreq;
    msg() << MSG::VERBOSE << "    Prob(chi2) " << TMath::Prob(track.chi2(), (int)numberOfSiHits*3-5) << " > " << m_trkSelChi2 << endreq;
  }

  if(m_useEtaPhiTrackSel) {

    if (fabs(track.eta() - m_trigBjetJetInfo->etaRoI()) > 0.2) {
      if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  track " << i+1 << " is not selected (eta matching)" << endreq;
      m_listCutApplied.push_back(2); return false;
    }

    if (fabs(m_taggerHelper->phiCorr(m_taggerHelper->phiCorr(track.phi()) - m_trigBjetJetInfo->phiRoI())) > 0.2) {
      if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  track " << i+1 << " is not selected (phi matching)" << endreq;
      m_listCutApplied.push_back(3); return false;
    }
  }

  if (fabs(pT) < m_trkSelPt) {
    if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  track " << i+1 << " not selected (pT cut)" << endreq;
    m_listCutApplied.push_back(4); return false;
  }

  if (fabs(d0) > m_trkSelD0) {
    if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  track " << i+1 << " not selected (d0 cut)" << endreq;
    m_listCutApplied.push_back(5); return false;
  }

  if (fabs(z0-zv)*TMath::Sin(theta) > m_trkSelZ0) {
    if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  track " << i+1 << " not selected (z0 cut)" << endreq;
    m_listCutApplied.push_back(6); return false;
  }

  if (track.Blayer() < m_trkSelBLayer) {
    if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  track " << i+1 << " not selected (missing b-layer hit)" << endreq;
    m_listCutApplied.push_back(7); return false;
  }

  if (track.pixHits() < m_trkSelPixHits) {
    if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  track " << i+1 << " not selected (too few pixel hits)" << endreq;
    m_listCutApplied.push_back(8); return false;
  }

  if (numberOfSiHits < m_trkSelSiHits) {
    if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  track " << i+1 << " not selected (too few silicon hits)" << endreq;
    m_listCutApplied.push_back(9); return false;
  }

  if (TMath::Prob(track.chi2(), (int)numberOfSiHits*3-5) <= m_trkSelChi2) {
    if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "  track " << i+1 << " not selected (chi2 cut)" << endreq;
    m_listCutApplied.push_back(10); return false;
  }
  
  if (msgLvl() <= MSG::VERBOSE)
    msg() << MSG::VERBOSE << "    track " << i+1 << " is selected" << endreq;

  m_listCutApplied.push_back(11);
  return true;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFexStandalone::setRoI(float eta, float phi) {
  m_trigBjetJetInfo->clear();
  m_trigBjetJetInfo->setEtaPhiRoI(eta, m_taggerHelper->phiCorr(phi));
  m_trigBjetJetInfo->setEtaPhiJet(eta, m_taggerHelper->phiCorr(phi));
  return HLT::OK;
}

HLT::ErrorCode TrigBjetFexStandalone::setBeamSpot(float beamSpot[3],
                                                  float beamTilt[2],
                                                  float beamSigma[3],
                                                  int   beamStatus) {
  m_trigBjetPrmVtxInfo->clear();
  m_trigBjetPrmVtxInfo->setBeamSpot(beamSpot[0], beamSpot[1], beamSpot[2]);
  m_trigBjetPrmVtxInfo->setBeamSpotTilt(beamTilt[0], beamTilt[1]);
  m_trigBjetPrmVtxInfo->setBeamSpotWidth(beamSigma[0], beamSigma[1], beamSigma[2]);
  m_trigBjetPrmVtxInfo->setBeamSpotStatus(beamStatus);
  return HLT::OK;
}

HLT::ErrorCode TrigBjetFexStandalone::setSV(float vtxMass, float sign3D, float eFrac, int nTracks, int n2TrkVtx) {
  m_trigBjetSecVtxInfo->clear();
  m_trigBjetSecVtxInfo->setVtxMass(vtxMass);
  m_trigBjetSecVtxInfo->setDecayLengthSignificance(sign3D);
  m_trigBjetSecVtxInfo->setEnergyFraction(eFrac);
  m_trigBjetSecVtxInfo->setNTrksInVtx(nTracks);
  m_trigBjetSecVtxInfo->setN2TrkVtx(n2TrkVtx);
  m_trigBjetSecVtxInfo->isValid(true);
  return HLT::OK;
}

HLT::ErrorCode TrigBjetFexStandalone::setPrmVtx(float pos[3]) {
  m_pvX = pos[0];
  m_pvY = pos[1];
  m_pvZ = pos[2];
  return HLT::OK;
}

HLT::ErrorCode TrigBjetFexStandalone::clearTracks() {
  m_trigBjetTrackInfoVectorPreSel->clear();
  return HLT::OK;
}

HLT::ErrorCode TrigBjetFexStandalone::loadTrack(float pT, float eta, float phi, float d0, float z0,
                                                float ed0, float ez0, int pixHits, int sctHits, int siHits,
                                                float chi2, int blayer) {
  TrigBjetTrackInfo t;
  t.setPT(pT); t.setEta(eta); t.setPhi(phi); t.setD0(d0); t.setZ0(z0);
  t.setED0(ed0); t.setEZ0(ez0); t.setPixHits(pixHits); t.setSCTHits(sctHits); t.setSiHits(siHits);
  t.setChi2(chi2); t.setBlayer(blayer);

  m_trigBjetTrackInfoVectorPreSel->push_back(t);
  return HLT::OK;
}

HLT::ErrorCode TrigBjetFexStandalone::hltExecute() {

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Executing TrigBjetFex" << endreq;

  m_totSelTracks = 0;
  m_totTracks    = 0;

  std::vector<TrigBjetTrackInfo> trigBjetTrackInfoVector;
  m_trigBjetTrackInfoVector = &trigBjetTrackInfoVector;

  //* Retrieve beamspot information *//

  int m_beamSpotBitMap = m_trigBjetPrmVtxInfo->beamSpotStatus();

  //* Check if beam spot is from online algorithms *//
  int m_beamSpotStatus = ((m_beamSpotBitMap & 0x4) == 0x4);
    
  //* Check if beam spot fit converged *//
  if (m_beamSpotStatus)
    m_beamSpotStatus = ((m_beamSpotBitMap & 0x3) == 0x3);

  m_trigBjetPrmVtxInfo->setBeamSpotStatus(m_beamSpotStatus);

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << *m_trigBjetPrmVtxInfo << endreq;
 
  //* Apply beam spot correction for tilt *//
  float m_xPrmVtx=0, m_yPrmVtx=0, m_zPrmVtx=0;

  m_xPrmVtx = m_pvX;
  m_yPrmVtx = m_pvY;
  m_zPrmVtx = m_pvZ;

  float m_xBeamSpot = m_trigBjetPrmVtxInfo->xBeamSpot() + tan(m_trigBjetPrmVtxInfo->xBeamSpotTilt()) * (m_zPrmVtx-m_trigBjetPrmVtxInfo->zBeamSpot());
  float m_yBeamSpot = m_trigBjetPrmVtxInfo->yBeamSpot() + tan(m_trigBjetPrmVtxInfo->yBeamSpotTilt()) * (m_zPrmVtx-m_trigBjetPrmVtxInfo->zBeamSpot());
  float m_zBeamSpot = m_trigBjetPrmVtxInfo->zBeamSpot();

  m_trigBjetPrmVtxInfo->setBeamSpot(m_xBeamSpot, m_yBeamSpot, m_zBeamSpot);
  m_trigBjetPrmVtxInfo->setPrmVtx(m_xPrmVtx, m_yPrmVtx, m_zPrmVtx);

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << *m_trigBjetPrmVtxInfo << endreq;

  m_trackJetFinderTool->clear();
  m_trackJetFinderTool->inputPrimaryVertexZ(m_trigBjetPrmVtxInfo->zPrmVtx());

  if (m_instance == "L2")  {

    m_totTracks = m_taggerHelper->getTrackNumber(*m_trigBjetTrackInfoVectorPreSel);

    for (unsigned int j = 0; j < m_totTracks; j++) {

      const TrigBjetTrackInfo &track = (*m_trigBjetTrackInfoVectorPreSel)[j];
      
      m_mon_trk_a0.push_back(track.d0());
      m_mon_trk_z0.push_back(track.z0());

      if (l2TrackSel(track, j)) {
	m_totSelTracks++;
	TrigBjetTrackInfo trigBjetTrackInfo = track;
	float d0Corr=0, z0Corr=0;
        m_taggerHelper->IPCorr(track.d0(), track.z0(), d0Corr, z0Corr,
			       track.phi(),track.eta(), track.pT(), 
			       m_trigBjetPrmVtxInfo->xBeamSpot(),m_trigBjetPrmVtxInfo->yBeamSpot());
	trigBjetTrackInfo.setIPCorr(d0Corr, z0Corr);
	if (msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Track " << j+1 << " selected." << endreq;
	  msg() << MSG::DEBUG << "  " << trigBjetTrackInfo << endreq;
	}
	trigBjetTrackInfoVector.push_back(trigBjetTrackInfo);
	m_trackJetFinderTool->addTrack(track, j);
      }
    }

  } else if (m_instance == "EF") {

    m_totTracks = m_taggerHelper->getTrackNumber(*m_trigBjetTrackInfoVectorPreSel);

    for (unsigned int j = 0; j < m_totTracks; j++) {
      
      const TrigBjetTrackInfo &track = (*m_trigBjetTrackInfoVectorPreSel)[j];
      
      m_mon_trk_a0.push_back(track.d0());
      m_mon_trk_z0.push_back(track.z0());

      if (efTrackSel(track, j)) {
	m_totSelTracks++;
	TrigBjetTrackInfo trigBjetTrackInfo = track;
	float d0Corr=0, z0Corr=0;
	m_taggerHelper->IPCorr(track.d0(), track.z0(), d0Corr, z0Corr,
			       track.phi(),track.eta(), track.pT(),
			       m_trigBjetPrmVtxInfo->xBeamSpot(),m_trigBjetPrmVtxInfo->yBeamSpot());
	trigBjetTrackInfo.setIPCorr(d0Corr, z0Corr);
	if (msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Track " << j+1 << " selected." << endreq;
	  msg() << MSG::DEBUG << "  " << trigBjetTrackInfo << endreq;
	}
	trigBjetTrackInfoVector.push_back(trigBjetTrackInfo);
	m_trackJetFinderTool->addTrack(track, j);
      }
    }
  }

  std::vector<int> tracksTrackJet;
  float etaTrackJet, phiTrackJet;
  
  m_trackJetFinderTool->findJet(tracksTrackJet, etaTrackJet, phiTrackJet);

  if (etaTrackJet != -99 && phiTrackJet != -99) {
    
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "eta RoI = " << m_trigBjetJetInfo->etaRoI() << " ; eta Jet at EF = " << etaTrackJet << endreq;
      msg() << MSG::DEBUG << "phi RoI = " << m_trigBjetJetInfo->phiRoI() << " ; phi Jet at EF = " << phiTrackJet << endreq;
    }
    
    m_trigBjetJetInfo->setEtaPhiTrkJet(etaTrackJet, phiTrackJet);
    
  } else {
    
    m_trigBjetJetInfo->setEtaPhiTrkJet(m_trigBjetJetInfo->etaRoI(), m_trigBjetJetInfo->phiRoI());
    
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "eta Jet = eta RoI" << endreq;
  }
  
  msg() << MSG::DEBUG << *m_trigBjetJetInfo << endreq;

  std::vector<std::string>::iterator pTagger    = m_taggers.begin();
  std::vector<std::string>::iterator lastTagger = m_taggers.end();

  if (m_useBeamSpotFlag && !m_trigBjetPrmVtxInfo->beamSpotStatus()) {

    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Beam spot status flag set to " << m_trigBjetPrmVtxInfo->beamSpotStatus() << ". Discriminant weights are not computed." << endreq;
      msg() << MSG::DEBUG << "Beam spot flag set to " << m_useBeamSpotFlag << ". Discriminant weights are not computed." << endreq;
    }

    m_listCutApplied.push_back(0);
    m_trigBjetTagger->getWeights();

  } else if (m_trigBjetPrmVtxInfo->xBeamSpotWidth()>m_setBeamSpotWidth || m_trigBjetPrmVtxInfo->yBeamSpotWidth()>m_setBeamSpotWidth) {

    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Beam spot width is more than " << m_setBeamSpotWidth << "um. Discriminant weights are not computed." << endreq;

    m_listCutApplied.push_back(1);
    m_trigBjetTagger->getWeights();

  } else {
    
    if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "Computing discriminant weights using taggers: " << m_taggers 
	    << " and using calibration from " << (m_useParamFromData==0 ? "MC" : "data") << " for CHI2" << endreq;
    
    m_trigBjetTagger->getWeights(m_trigBjetTrackInfoVector, m_trigBjetPrmVtxInfo, m_trigBjetSecVtxInfo, m_trigBjetJetInfo);
    
  }
  
  if (msgLvl() <= MSG::DEBUG) {
     
      msg() << MSG::DEBUG << "REGTEST:  Tracks: " << m_totTracks << " reconstructed and " << m_totSelTracks <<" selected" << endreq;
      
      if (m_instance == "EF") {
	msg() << MSG::DEBUG << "REGTEST:  "
	      << " (x,y,z) = (" << m_trigBjetPrmVtxInfo->xPrmVtx() << "," << m_trigBjetPrmVtxInfo->yPrmVtx() << "," << m_trigBjetPrmVtxInfo->zPrmVtx() << ")" << endreq;
        /*
	if (pointerToEFSecVtxCollections)
	  msg() << MSG::DEBUG << "REGTEST:  Secondary vertex: " << pointerToEFSecVtxCollections->size() << " reconstructed" << endreq;
	else
	  msg() << MSG::DEBUG << "REGTEST:  Secondary vertex: 0 reconstructed" << endreq;
        */
      } else {
	msg() << MSG::DEBUG << "REGTEST:  "
	      << " z = " << m_trigBjetPrmVtxInfo->zPrmVtx() << endreq;
        /*
	if (pointerToL2SecVtxCollections)
	  msg() << MSG::DEBUG << "REGTEST:  Secondary vertex: " << pointerToL2SecVtxCollections->size() << " reconstructed" << endreq;
	else
	  msg() << MSG::DEBUG << "REGTEST:  Secondary vertex: 0 reconstructed" << endreq;
         */
      }
      
      msg() << MSG::DEBUG << "REGTEST: List weights stored probability and likelihood objects:" << endreq;

      pTagger = m_taggers.begin();
      for ( ; pTagger != lastTagger; pTagger++)
	msg() << MSG::DEBUG << "REGTEST:  X(" << (*pTagger) << ") = " << m_trigBjetTagger->taggersXMap((*pTagger))  << endreq;

  }

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFexStandalone::hltFinalize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Finalizing TrigBjetFex" << endreq;

  return HLT::OK;
}



