/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigT2HistoPrmVtx.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2HistoPrmVtx
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#include "TrigT2HistoPrmVtx/TrigT2HistoPrmVtx.h"
#include "TrigT2HistoPrmVtx/HistoVertexHelper.h"
#include "TrigT2HistoPrmVtxBase.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "Particle/TrackParticleContainer.h"
#include "TrigInDetEvent/TrigVertexCollection.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODBase/IParticle.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"



//** ----------------------------------------------------------------------------------------------------------------- **//


TrigT2HistoPrmVtx::TrigT2HistoPrmVtx(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::FexAlgo(name, pSvcLocator),
  c( new TrigT2HistoPrmVtxBase(msg(), msgLvl()) ),
  m_constTrigT2HistoPrmVtxBase(0),
  m_trigVertexColl(0)
{
  declareProperty ("AlgoId",             c->m_algo);
  declareProperty ("Instance",           c->m_instance);

  declareProperty ("NumBins",            c->m_nBins);

  declareProperty ("UseBeamSpot",        c->m_useBeamSpot        = false);
  declareProperty ("UseEtaPhiTrackSel",  c->m_useEtaPhiTrackSel  = false);

  declareProperty ("L2TrkSel_Chi2",      c->m_l2TrkSelChi2       = 0.001);
  declareProperty ("L2TrkSel_BLayer",    c->m_l2TrkSelBLayer     = 1);
  declareProperty ("L2TrkSel_SiHits",    c->m_l2TrkSelSiHits     = 4);
  declareProperty ("L2TrkSel_D0",        c->m_l2TrkSelD0         = 1*CLHEP::mm);
  declareProperty ("L2TrkSel_Pt",        c->m_l2TrkSelPt         = 2*CLHEP::GeV);

  declareProperty ("EFTrkSel_Chi2",      c->m_efTrkSelChi2       = 0.0);
  declareProperty ("EFTrkSel_BLayer",    c->m_efTrkSelBLayer     = 1);
  declareProperty ("EFTrkSel_PixHits",   c->m_efTrkSelPixHits    = 2);
  declareProperty ("EFTrkSel_SiHits",    c->m_efTrkSelSiHits     = 7);
  declareProperty ("EFTrkSel_D0",        c->m_efTrkSelD0         = 1*CLHEP::mm);
  declareProperty ("EFTrkSel_Pt",        c->m_efTrkSelPt         = 1*CLHEP::GeV);

  declareMonitoredVariable("PrmVtx",          c->zPrmVtx[0]          = -1);
  declareMonitoredVariable("PrmVtxSigmaAll",  c->zPrmVtxSigmaAll[0]  = -1);
  declareMonitoredVariable("PrmVtxSigma2Trk", c->zPrmVtxSigma2Trk[0] = -1);
  declareMonitoredVariable("TracksPerVtx",    c->nTrackVtx[0]        = -1);
  declareMonitoredVariable("NVtxFound",       c->nVtxFound           = -1);

  declareMonitoredObject("Trk_selectedTracks", m_constTrigT2HistoPrmVtxBase, &TrigT2HistoPrmVtxBase::totSelectedTracks);
  declareMonitoredStdContainer("Trk_stepsToSelect", c->m_listCutApplied);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigT2HistoPrmVtx::~TrigT2HistoPrmVtx() {}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigT2HistoPrmVtx::hltInitialize() {

  //* Get message service *//
  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigT2HistoPrmVtx, version " << PACKAGE_VERSION << endreq;

 if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endreq;
    msg() << MSG::DEBUG << " AlgoId = "              << c->m_algo << endreq; 
    msg() << MSG::DEBUG << " Instance = "            << c->m_instance << endreq; 
    msg() << MSG::DEBUG << " Numbins = "             << c->m_nBins << endreq; 
    msg() << MSG::DEBUG << " UseBeamSpot = "         << c->m_useBeamSpot << endreq; 
    msg() << MSG::DEBUG << " UseEtaPhiTrackSel = "   << c->m_useEtaPhiTrackSel << endreq;

    if (c->m_instance == "_L2") {

      msg() << MSG::DEBUG << " L2TrkSel_Chi2 = "     << c->m_l2TrkSelChi2 << endreq; 
      msg() << MSG::DEBUG << " L2TrkSel_BLayer = "   << c->m_l2TrkSelBLayer << endreq; 
      msg() << MSG::DEBUG << " L2TrkSel_SiHits = "   << c->m_l2TrkSelSiHits << endreq; 
      msg() << MSG::DEBUG << " L2TrkSel_D0 = "       << c->m_l2TrkSelD0 << endreq; 
      msg() << MSG::DEBUG << " L2TrkSel_Pt = "       << c->m_l2TrkSelPt << endreq; 

    } else if (c->m_instance == "_EF") {

      msg() << MSG::DEBUG << " EFTrkSel_Chi2 = "     << c->m_efTrkSelChi2 << endreq; 
      msg() << MSG::DEBUG << " EFTrkSel_BLayer = "   << c->m_efTrkSelBLayer << endreq; 
      msg() << MSG::DEBUG << " EFTrkSel_PixHits = "  << c->m_efTrkSelPixHits << endreq; 
      msg() << MSG::DEBUG << " EFTrkSel_SiHits = "   << c->m_efTrkSelSiHits << endreq;    
      msg() << MSG::DEBUG << " EFTrkSel_D0 = "       << c->m_efTrkSelD0 << endreq; 
      msg() << MSG::DEBUG << " EFTrkSel_Pt = "       << c->m_efTrkSelPt << endreq; 

    }
  }

  m_constTrigT2HistoPrmVtxBase = const_cast<const TrigT2HistoPrmVtxBase*>(c);

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigT2HistoPrmVtx::getCollection(const TrigInDetTrackCollection*& pointerToL2TrackCollections, const HLT::TriggerElement* outputTE) {

    //* Create TrigInDetTrack standard vector *// 
    std::vector<const TrigInDetTrackCollection*> vectorOfL2TrackCollections;
    
    HLT::ErrorCode status = getFeatures(outputTE, vectorOfL2TrackCollections, ""); 
    if (status != HLT::OK)
      msg() << MSG::ERROR << "Failed to get InDetTrackCollection from the trigger element" << endreq;
    else if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "Got " << vectorOfL2TrackCollections.size() << " InDetTrackCollection" << endreq;

    //* Create iterator on TrigInDetTrack collections *// 
    std::vector<const TrigInDetTrackCollection*>::iterator pTrackColl =  vectorOfL2TrackCollections.begin();
    std::vector<const TrigInDetTrackCollection*>::iterator lastTrackColl = vectorOfL2TrackCollections.end();
  
    //* Loop over TrigInDetTrack collections *// 
    for ( ; pTrackColl != lastTrackColl; pTrackColl++) {
      if ((*pTrackColl)->size() != 0) {
	if ((*pTrackColl)->front()->algorithmId() == c->m_algo) {
	  if (msgLvl() <= MSG::DEBUG && c->m_algo == 1)
	    msg() << MSG::DEBUG << "Selected collection with SiTrack label" << endreq;
	  else if (msgLvl() <= MSG::DEBUG && c->m_algo == 2)
	    msg() << MSG::DEBUG << "Selected collection with IdScan label" << endreq;
	  else if (msgLvl() <= MSG::DEBUG && c->m_algo == 5)
	    msg() << MSG::DEBUG << "Selected collection with L2Star label (strategy A)" << endreq;
	  else if (msgLvl() <= MSG::DEBUG && c->m_algo == 6)
	    msg() << MSG::DEBUG << "Selected collection with L2Star label (strategy B)" << endreq;
	  else if (msgLvl() <= MSG::DEBUG && c->m_algo == 8)
	    msg() << MSG::DEBUG << "Selected collection with L2Star label (strategy F)" << endreq;
	  else if (msgLvl() <= MSG::DEBUG && c->m_algo == 13)
	    msg() << MSG::DEBUG << "Selected collection with L2Star label (strategy F with refit)" << endreq;
	  break;
	}
      }
    }

    if (pTrackColl == lastTrackColl) {
      pointerToL2TrackCollections = 0;
      return HLT::ERROR;
    } else {
      pointerToL2TrackCollections = *pTrackColl;
      return HLT::OK;
    }    
}

//** ----------------------------------------------------------------------------------------------------------------- **//


unsigned int TrigT2HistoPrmVtx::getTrackNumbers(const TrigInDetTrackCollection* pointerToL2TrackCollections) {

  unsigned int nL2tracks;

  if (pointerToL2TrackCollections) {
    nL2tracks = pointerToL2TrackCollections->size();
    if (msgLvl() <= MSG::DEBUG)  
      msg() << MSG::DEBUG << "Found " << nL2tracks << " tracks in the RoI" << endreq;
  } else {
    nL2tracks = 0;
    if (msgLvl() <= MSG::DEBUG)  
      msg() << MSG::DEBUG << "No tracks in the RoI" << endreq;
  }
  
  return nL2tracks;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


unsigned int TrigT2HistoPrmVtx::getTrackNumbers(const xAOD::TrackParticleContainer* pointerToEFTrackCollections) {

  unsigned int nEFtracks;

  if (pointerToEFTrackCollections) {
    nEFtracks = pointerToEFTrackCollections->size();
    if (msgLvl() <= MSG::DEBUG)  
      msg() << MSG::DEBUG << "Found " << nEFtracks << " tracks in the RoI" << endreq;
  } else {
    nEFtracks = 0;
    if (msgLvl() <= MSG::DEBUG)  
      msg() << MSG::DEBUG << "No tracks in the RoI" << endreq;
  }

  return nEFtracks;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigT2HistoPrmVtx::hltExecute(const HLT::TriggerElement* /*inputTE*/, HLT::TriggerElement* outputTE) {

  if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Executing TrigT2HistoPrmVtx" << endreq;    

  float zFirstTrack=0;
  float zFirstTrackError=0;

  c->m_totTracks=0;
  c->m_totSelTracks=0;

  c->m_hisVtx->reset();

  for (int i = 0; i <3; i++) {
    c->zPrmVtx.at(i)          = -200;
    c->zPrmVtxSigmaAll.at(i)  = -200;
    c->zPrmVtxSigma2Trk.at(i) = -200;
    c->nTrackVtx.at(i)        = -200;
  }

  //* Retrieve beamspot information *//
  c->m_xBeamSpot = 0;      
  c->m_yBeamSpot = 0;

  if(c->m_useBeamSpot) {

    IBeamCondSvc* m_iBeamCondSvc; 
    StatusCode scBS = service("BeamCondSvc", m_iBeamCondSvc);

    if (scBS.isFailure() || m_iBeamCondSvc == 0) {
      m_iBeamCondSvc = 0;

      if (msgLvl() <= MSG::WARNING) {
	msg() << MSG::WARNING << "Could not retrieve Beam Conditions Service. " << endreq;
	msg() << MSG::WARNING << "Using origin at ( " << c->m_xBeamSpot << " , " << c->m_yBeamSpot << " ) " << endreq;
      }

    } else {

      Amg::Vector3D m_beamSpot = m_iBeamCondSvc->beamPos();

      c->m_xBeamSpot = m_beamSpot.x();
      c->m_yBeamSpot = m_beamSpot.y();
      c->m_zBeamSpot = m_beamSpot.z();

      if(msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Beam spot from service: x=" << c->m_xBeamSpot 
	      << ", y=" << c->m_yBeamSpot << ", z=" << c->m_zBeamSpot << endreq;
    }
  }

  //* Get RoI descriptor *//
  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(outputTE, roiDescriptor) == HLT::OK ) {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Using outputTE: "
	    << "RoI id " << roiDescriptor->roiId()
	    << ", Phi = " <<  roiDescriptor->phi()
	    << ", Eta = " << roiDescriptor->eta() << endreq;
    }
  } else {
    if (msgLvl() <= MSG::WARNING) 
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element " << endreq;

    return HLT::NAV_ERROR;
  }

  //* Create pointers to collections *// 
  const TrigInDetTrackCollection* pointerToL2TrackCollections = 0;
  const xAOD::TrackParticleContainer* pointerToEFTrackCollections = 0;

  if (c->m_instance == "_L2")  { 

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Executing TrigT2HistoPrmVtx at L2" << endreq;

    //* Get collection from TrigT2HistoPrmVtx::getCollection *//
    if (getCollection(pointerToL2TrackCollections, outputTE) != HLT::OK)
      msg() << MSG::DEBUG << "No tracks retrieved from TrigT2HistoPrmVtx::getCollection" << endreq;
    else if (msgLvl() <= MSG::DEBUG)  
      msg() << MSG::DEBUG << "Got collection from TrigT2HistoPrmVtx::getCollection" << endreq;   

    //* Get tracks number *//
    c->m_totTracks = getTrackNumbers(pointerToL2TrackCollections);

    c->m_listCutApplied.clear();

    //* Retrieve z0 parameters *//  
    for (unsigned int i = 0; i < c->m_totTracks; i++) {
      
      if (msgLvl() <= MSG::DEBUG && i == 0) 
	msg() << MSG::DEBUG << "Loop over tracks: retrieving z0" << endreq;

      const TrigInDetTrack* track = (*pointerToL2TrackCollections)[i];
      float z0  = track->param()->z0();
      float ez0 = track->param()->ez0();

//       float d0  = track->param()->a0();
//       if (c->m_useBeamSpot) 
//         c->IPCorr(track->param()->a0(),track->param()->z0(),d0,z0,track->param()->phi0(), track->param()->eta(), track->param()->pT());

      if (!c->l2TrackSel(track, i, roiDescriptor->eta(), roiDescriptor->phi())) continue;

      c->m_totSelTracks++;

      if (c->m_totSelTracks == 1) {
	zFirstTrack=z0;
	zFirstTrackError=ez0;
      }

      if (msgLvl() <= MSG::VERBOSE)
	msg() << MSG::VERBOSE << "Track number " << i+1 << " with z0 = " << z0 << " and ez0 = " << ez0 << endreq;
      
      c->m_hisVtx->fill(z0);
    }  
  }

  else if (c->m_instance == "_EF") { 

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Executing TrigT2HistoPrmVtx at EF" << endreq;

    HLT::ErrorCode status = getFeature(outputTE, pointerToEFTrackCollections, ""); 
    if (status != HLT::OK) {
      msg() << MSG::DEBUG << "No tracks retrieved from TrigT2HistoPrmVtx" << endreq;
    } 
    else if (msgLvl() <= MSG::DEBUG)  
      msg() << MSG::DEBUG << "Got collection from TrigT2HistoPrmVtx" << endreq;   

    //* Get tracks number *//
    c->m_totTracks = getTrackNumbers(pointerToEFTrackCollections);

    c->m_listCutApplied.clear();

    //* Retrieve z0 parameters *//  
    for (unsigned int i = 0; i < c->m_totTracks; i++) {
      
      if (msgLvl() <= MSG::DEBUG && i == 0) 
	msg() << MSG::DEBUG << "Loop over tracks: retrieving z0" << endreq;

      const xAOD::TrackParticle* track = (*pointerToEFTrackCollections)[i];
      float z0 =  track->z0();
      const xAOD::ParametersCovMatrix_t covTrk = track->definingParametersCovMatrix();
      float ez0 = Amg::error(covTrk, 1);
      
//       float pT = track->pt();
//       float d0 =  track->d0();
//       if(c->m_useBeamSpot)
//         c->IPCorr(track->d0(),track->z0(),d0,z0,track->phi(),track->eta(), pT);

      if (!c->efTrackSel(track, i, roiDescriptor->eta(), roiDescriptor->phi())) continue;

      c->m_totSelTracks++;

      if (c->m_totSelTracks == 1) {
	zFirstTrack=z0;
	zFirstTrackError=ez0;
      }

      if (msgLvl() <= MSG::VERBOSE)
	msg() << MSG::VERBOSE << "Track number " << i+1 << " with z0 = " << z0 << " and ez0 = " << ez0 << endreq;
      
      c->m_hisVtx->fill(z0);
    }  
  }

  //* Get primary vertex from TrigT2HistoPrmVtx::findPrmVtx *//
  if (c->m_totSelTracks == 1) {
    c->zPrmVtx.at(0)          = zFirstTrack;
    c->zPrmVtxSigmaAll.at(0)  = zFirstTrackError;
    c->nTrackVtx.at(0)        = 1;
    c->nVtxFound = 1;
  } else {
    c->findPrmVtx();
  }

  if (msgLvl() <= MSG::DEBUG) {
    for (int i = 0; i <3; i++)
      msg() << MSG::DEBUG << "Primary vertex " << i << ": z=" << c->zPrmVtx.at(i) << ", sigma=" << c->zPrmVtxSigmaAll.at(i) << ", nTracks=" << c->nTrackVtx.at(i) << endreq;
  }

  if (msgLvl() <= MSG::DEBUG) {

    const EventInfo* pEventInfo;
 
    if ( !store() || store()->retrieve(pEventInfo).isFailure() ) {
      msg()  << MSG::DEBUG << "Failed to get EventInfo " << endreq;
    } else {
      msg() << MSG::DEBUG << "PrmVtx summary (Run " << pEventInfo->event_ID()->run_number()
	    << "; Event "<< pEventInfo->event_ID()->event_number() << ")" << endreq;
      msg() << MSG::DEBUG << "REGTEST: RoI " << roiDescriptor->roiId() << ", Phi = "   << roiDescriptor->phi() << ", Eta = "   << roiDescriptor->eta() << endreq;
      msg() << MSG::DEBUG << "REGTEST: Tracks: " << c->m_totTracks << " reconstructed and " << c->m_totSelTracks <<" selected" << endreq;
      msg() << MSG::DEBUG << "REGTEST: Primary vertex: " << c->zPrmVtx.at(0) << " +/ " << c->zPrmVtxSigmaAll.at(0) << " with " << c->nTrackVtx.at(0) << " tracks" << endreq;
    }
  }

  //* Create TrigVertex collection *// 
  m_trigVertexColl = new TrigVertexCollection();

  TrigVertex* trigVertex = 0;

  if(name().find("Tau") != string::npos) {
    
    if (c->m_algo == 1 || c->m_algo == 6)
      trigVertex = new TrigVertex(c->zPrmVtx.at(0), c->zPrmVtxSigmaAll.at(0)*c->zPrmVtxSigmaAll.at(0), TrigVertex::HISTOPRMVTXSITRACKIDTAU);  // FIX for covariance definition
    else if (c->m_algo == 8)
      trigVertex = new TrigVertex(c->zPrmVtx.at(0), c->zPrmVtxSigmaAll.at(0)*c->zPrmVtxSigmaAll.at(0), TrigVertex::HISTOPRMVTXFTKIDTAU);  // FIX for covariance definition
    else if (c->m_algo == 3)
      trigVertex = new TrigVertex(c->zPrmVtx.at(0), c->zPrmVtxSigmaAll.at(0)*c->zPrmVtxSigmaAll.at(0), TrigVertex::HISTOPRMVTXEFIDTAU);  // FIX for covariance definition
    
  } else {

    if (c->m_algo == 1 || c->m_algo == 6)
      trigVertex = new TrigVertex(c->zPrmVtx.at(0), c->zPrmVtxSigmaAll.at(0)*c->zPrmVtxSigmaAll.at(0), TrigVertex::HISTOPRMVTXSITRACKID);  // FIX for covariance definition
    else if (c->m_algo == 2 || c->m_algo == 5)
      trigVertex = new TrigVertex(c->zPrmVtx.at(0), c->zPrmVtxSigmaAll.at(0)*c->zPrmVtxSigmaAll.at(0), TrigVertex::HISTOPRMVTXIDSCANID);  // FIX for covariance definition
    else if (c->m_algo == 8)
      trigVertex = new TrigVertex(c->zPrmVtx.at(0), c->zPrmVtxSigmaAll.at(0)*c->zPrmVtxSigmaAll.at(0), TrigVertex::HISTOPRMVTXFTKID);  // FIX for covariance definition
    else if (c->m_algo == 13)
      trigVertex = new TrigVertex(c->zPrmVtx.at(0), c->zPrmVtxSigmaAll.at(0)*c->zPrmVtxSigmaAll.at(0), TrigVertex::HISTOPRMVTXFTKID_REFIT);  // FIX for covariance definition
    else if (c->m_algo == 3)
      trigVertex = new TrigVertex(c->zPrmVtx.at(0), c->zPrmVtxSigmaAll.at(0)*c->zPrmVtxSigmaAll.at(0), TrigVertex::HISTOPRMVTXEFID);  // FIX for covariance definition
  }

  m_trigVertexColl->push_back(trigVertex);

  string key;

  if (c->m_instance == "_L2")      key = "T2HistoPrmVtx";
  else if (c->m_instance == "_EF") key = "EFHistoPrmVtx";

  HLT::ErrorCode stat = attachFeature(outputTE, m_trigVertexColl, key);
  if (stat != HLT::OK) {
    if (msgLvl() <= MSG::WARNING) 
      msg() << MSG::WARNING << "Failed to attach TrigVertexCollection to navigation" << endreq;
    return stat;
  }

  return HLT::OK;
}

//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigT2HistoPrmVtx::hltFinalize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Finalizing TrigT2HistoPrmVtx" << endreq;

  return HLT::OK;
}

 

