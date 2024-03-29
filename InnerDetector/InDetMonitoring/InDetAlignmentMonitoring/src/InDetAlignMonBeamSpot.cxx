/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

// $Id: InDetAlignMonBeamSpot.cxx,v 1.6 2009-02-05 20:55:08 beringer Exp $

#include <sstream>
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

//#include "GaudiKernel/MsgStream.h"

#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODTracking/Vertex.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "InDetAlignMonBeamSpot.h"



InDetAlignMonBeamSpot::InDetAlignMonBeamSpot(const std::string& type, const std::string& name, const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent),
  m_hasBeamCondSvc(false),
  m_hTrNPt(nullptr),
  m_hTrPt(nullptr),
  m_hTrDPhi(nullptr),
  m_hTrDPhiCorr(nullptr),
  m_hBsX(nullptr),
  m_hBsY(nullptr),
  m_hBsZ(nullptr),
  m_hBsTiltX(nullptr),
  m_hBsTiltY(nullptr),
  m_hPvN(nullptr),
  m_hPvNPriVtx(nullptr),
  m_hPvNPileupVtx(nullptr),
  m_hPvX(nullptr),
  m_hPvY(nullptr),
  m_hPvZ(nullptr),
  m_hPvErrX(nullptr),
  m_hPvErrY(nullptr),
  m_hPvErrZ(nullptr),
  m_hPvChiSqDoF(nullptr),
  m_hPvXZ(nullptr),
  m_hPvYZ(nullptr),
  m_hPvYX(nullptr),
  m_hPvNTracksAll(nullptr),
  m_hPvNTracks(nullptr),
  m_hPvTrackPt(nullptr),
  m_hPvTrackEta(nullptr) {
  declareProperty("useBeamspot", m_useBeamspot = true);
  declareProperty("vxContainerWithBeamConstraint", m_vxContainerWithBeamConstraint = false);
  declareProperty("minTracksPerVtx", m_minTracksPerVtx = 10);
  declareProperty("minTrackPt", m_minTrackPt = 1500);  // MeV
  declareProperty("triggerChainName", m_triggerChainName = "NoTriggerSelection");
  declareProperty("histFolder", m_histFolder = "IDAlignMon/BeamSpot/" + m_triggerChainName);
}

InDetAlignMonBeamSpot::~InDetAlignMonBeamSpot() {
}

StatusCode InDetAlignMonBeamSpot::initialize() {
  StatusCode sc;

  sc = ManagedMonitorToolBase::initialize();
  if (!sc.isSuccess()) return sc;

  if (m_beamSpotKey.initialize().isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve beamspot service - will use nominal beamspot at (0,0,0)");
  } else {
    m_hasBeamCondSvc = true;
    ATH_MSG_INFO("Retrieved service beaspot ");
  }

  ATH_CHECK(m_vxContainerName.initialize());
  ATH_CHECK(m_trackContainerName.initialize());

  return StatusCode::SUCCESS;
}

StatusCode InDetAlignMonBeamSpot::bookHistograms() {
  MonGroup al_beamspot_mon(this, m_histFolder, run);


  if (AthenaMonManager::environment() == AthenaMonManager::online) {
    // book histograms that are only made in the online environment...
  }

  if (AthenaMonManager::dataType() == AthenaMonManager::cosmics) {
    // book histograms that are only relevant for cosmics data...
  }

  if (newLowStatFlag() || newLumiBlockFlag()) {
  }

  if (newRunFlag()) {
    // Histograms for track-based beam spot monitoring
    m_hTrDPhi = makeAndRegisterTH2F(al_beamspot_mon, "trkDPhi", "DCA vs Phi wrt (0,0);#varphi (rad);d_{0} (mm)", 100,
                                    -3.5, 3.5, 100, -5, 5);
    m_hTrDPhiCorr = makeAndRegisterTH2F(al_beamspot_mon, "trkDPhiCorr",
                                        "DCA vs Phi wrt Beamspot;#varphi (rad);d_{0} (#mum)", 100, -3.5, 3.5, 100, -200,
                                        200);

    m_hTrPt = makeAndRegisterTH1F(al_beamspot_mon, "trkPt", "Track Pt;P_{t} (GeV)", 100, 0, 5);
    m_hTrNPt = makeAndRegisterTH1F(al_beamspot_mon, "trkNPt",
                                   "Number of Tracks per event (after Pt cut);Number of tracks", 50, 0, 50);

    // Histograms of assumed beam spot position
    if (m_useBeamspot) {
      m_hBsX = makeAndRegisterTH1F(al_beamspot_mon, "bsX", "Beam spot position: x;x (mm)", 100, -3, 3);
      m_hBsY = makeAndRegisterTH1F(al_beamspot_mon, "bsY", "Beam spot position: y;y (mm)", 100, -3, 3);
      m_hBsZ = makeAndRegisterTH1F(al_beamspot_mon, "bsZ", "Beam spot position: z;z (mm)", 100, -300, 300);
      m_hBsTiltX = makeAndRegisterTH1F(al_beamspot_mon, "bsTiltX",
                                       "Beam spot tile angle: x-z plane; Tilt angle (#murad)", 100, -1e3, 1e3);
      m_hBsTiltY = makeAndRegisterTH1F(al_beamspot_mon, "bsTiltY",
                                       "Beam spot tile angle: y-z plane; Tilt angle (#murad)", 100, -1e3, 1e3);
    }

    // Histograms for vertex-based beam spot monitoring
    if (!m_vxContainerWithBeamConstraint) {
      ATH_MSG_DEBUG("Container with Beam constraint");
      // The following histograms are made either relative to the current beamspot (from BeamCondSvc),
      // or relative to the nomial beamspot at (0,0,0) without any tilt.
      if (m_useBeamspot) {
        m_hPvX = makeAndRegisterTH1F(al_beamspot_mon, "pvX", "Primary vertex: x - x_{beam};x-x_{beam} (#mum)", 100,
                                     -200, 200);
        m_hPvY = makeAndRegisterTH1F(al_beamspot_mon, "pvY", "Primary vertex: y - y_{beam};y-y_{beam} (#mum)", 100,
                                     -200, 200);
        m_hPvZ = makeAndRegisterTH1F(al_beamspot_mon, "pvZ", "Primary vertex: z - z_{beam};z-z_{beam} (mm)", 100, -200,
                                     200);
      } else {
        m_hPvX = makeAndRegisterTH1F(al_beamspot_mon, "pvX", "Primary vertex: x;x (mm)", 600, -5, 5);
        m_hPvY = makeAndRegisterTH1F(al_beamspot_mon, "pvY", "Primary vertex: y;y (mm)", 600, -5, 5);
        m_hPvZ = makeAndRegisterTH1F(al_beamspot_mon, "pvZ", "Primary vertex: z;z (mm)", 100, -200, 200);
      }

      // Histograms that are independent of the useBeamSpot parameters
      m_hPvXZ = makeAndRegisterTH2F(al_beamspot_mon, "pvXZ", "Primary vertex: x vs z;z (mm);x (mm)", 100, -200, 200,
                                    100, -5, 5);
      m_hPvYZ = makeAndRegisterTH2F(al_beamspot_mon, "pvYZ", "Primary vertex: y vs z;z (mm);y (mm)", 100, -200, 200,
                                    100, -5, 5);
      m_hPvYX = makeAndRegisterTH2F(al_beamspot_mon, "pvYX", "Primary vertex: y vs x;x (mm);y (mm)", 100, -5, 5, 100,
                                    -5, 5);
      m_hPvN = makeAndRegisterTH1F(al_beamspot_mon, "pvN",
                                   "Number of vertices (" + m_vxContainerName.key() + ", excluding dummy vertex);Number of vertices", 10, 0,
                                   10);
      m_hPvNPriVtx = makeAndRegisterTH1F(al_beamspot_mon, "pvNPriVtx",
                                         "Number of primary vertices (" + m_vxContainerName.key() + ");Number of vertices", 2, 0,
                                         2);
      m_hPvNPileupVtx = makeAndRegisterTH1F(al_beamspot_mon, "pvNPileupVtx",
                                            "Number of pileup vertices (" + m_vxContainerName.key() + ");Number of vertices", 10, 0,
                                            10);
      m_hPvErrX = makeAndRegisterTH1F(al_beamspot_mon, "pvErrX", "Primary vertex: #sigma_{x}; #sigma_{x} (mm)", 100, 0,
                                      .5);
      m_hPvErrY = makeAndRegisterTH1F(al_beamspot_mon, "pvErrY", "Primary vertex: #sigma_{y}; #sigma_{y} (mm)", 100, 0,
                                      .5);
      m_hPvErrZ = makeAndRegisterTH1F(al_beamspot_mon, "pvErrZ", "Primary vertex: #sigma_{z}; #sigma_{z} (mm)", 100, 0,
                                      .5);
      m_hPvChiSqDoF = makeAndRegisterTH1F(al_beamspot_mon, "pvChiSqDof",
                                          "Primary vertex: #Chi^{2}/DoF of vertex fit;#Chi^{2}/DoF", 100, 0, 10);
      m_hPvNTracksAll = makeAndRegisterTH1F(al_beamspot_mon, "pvNTracksAll",
                                            "Number of tracks in primary vertex;Number of tracks", 50, 0, 50);
      m_hPvNTracks = makeAndRegisterTH1F(al_beamspot_mon, "pvNTracks",
                                         "Number of tracks in primary vertex;Number of tracks", 50, 0, 50);
      m_hPvTrackPt = makeAndRegisterTH1F(al_beamspot_mon, "pvTrackPt",
                                         "Primary vertex: original track p_{t};p_{t} (GeV)", 100, 0, 20);
      m_hPvTrackEta = makeAndRegisterTH1F(al_beamspot_mon, "pvTrackEta", "Primary vertex: original track #eta; #eta",
                                          100, -3, 3);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode InDetAlignMonBeamSpot::fillHistograms() {
  // Get beamspot information, if available
  float beamSpotX = 0.;
  float beamSpotY = 0.;
  float beamSpotZ = 0.;
  float beamTiltX = 0.;
  float beamTiltY = 0.;
  float scaleFactor = 1.;

  if (m_useBeamspot && m_hasBeamCondSvc) {
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle {
      m_beamSpotKey
    };
    Amg::Vector3D bpos = beamSpotHandle->beamPos();
    beamSpotX = bpos.x();
    beamSpotY = bpos.y();
    beamSpotZ = bpos.z();
    beamTiltX = beamSpotHandle->beamTilt(0);
    beamTiltY = beamSpotHandle->beamTilt(1);
    scaleFactor = 1000.;   // Use microns for some histograms when showing distance relative to beamspot
    m_hBsX->Fill(beamSpotX);
    m_hBsY->Fill(beamSpotY);
    m_hBsZ->Fill(beamSpotZ);
    m_hBsTiltX->Fill(1e6 * beamTiltX);
    m_hBsTiltY->Fill(1e6 * beamTiltY);
    ATH_MSG_DEBUG("Beamspot : x0 = " << beamSpotX << ", y0 = " << beamSpotY
                                     << ", z0 = " << beamSpotZ << ", tiltX = " << beamTiltX
                                     << ", tiltY = " << beamTiltY);
  }

  SG::ReadHandle<xAOD::TrackParticleContainer> trackCollection {
    m_trackContainerName
  };
  if (not trackCollection.isValid()) {
    ATH_MSG_DEBUG("Could not retrieve TrackParticleContainer container with key " + m_trackContainerName.key());
    return StatusCode::SUCCESS;
  }





  // Track monitoring
  int nTracks = 0;
  for (xAOD::TrackParticleContainer::const_iterator trkItr = trackCollection->begin(); trkItr != trackCollection->end();
       ++trkItr) {
    const xAOD::TrackParticle* tpb = *trkItr;
    if (!tpb) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Null pointer to TrackParticleBase" << endmsg;
      continue;
    }
    const Trk::Perigee* perigee = &(tpb->perigeeParameters());
    if (!perigee) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Null pointer to track perigee" << endmsg;
      continue;
    }

    float theta = perigee->parameters()[Trk::theta];
    float qOverPt = perigee->parameters()[Trk::qOverP] / sin(theta);
    float charge = perigee->charge();
    float phi0 = perigee->parameters()[Trk::phi0];
    float d0 = perigee->parameters()[Trk::d0];
    float z0 = perigee->parameters()[Trk::z0];
    if (qOverPt != 0) {
      float pT = (1 / qOverPt) * (charge);
      // For all tracks
      m_hTrPt->Fill(pT / 1000.);

      // Select tracks to use for remaining histograms
      if (pT < m_minTrackPt) continue;
    }

    nTracks++;
    m_hTrDPhi->Fill(phi0, d0 * 1000.);

    float beamX = beamSpotX + tan(beamTiltX) * (z0 - beamSpotZ);
    float beamY = beamSpotY + tan(beamTiltY) * (z0 - beamSpotZ);
    float d0corr = d0 - (-sin(phi0) * beamX + cos(phi0) * beamY);
    m_hTrDPhiCorr->Fill(phi0, d0corr * 1e3);
  }
  m_hTrNPt->Fill(nTracks);



  // Primary vertex monitoring - only if we have a primary vertex collection determined
  // without beam constraint
  if (!m_vxContainerWithBeamConstraint) {
    SG::ReadHandle<xAOD::VertexContainer> vxContainer {
      m_vxContainerName
    };
    if (not vxContainer.isValid()) {
      ATH_MSG_DEBUG("Could not retrieve primary vertex container with key " + m_vxContainerName.key());
      return StatusCode::SUCCESS;
    }

    m_hPvN->Fill(vxContainer->size() - 1);  // exclude dummy vertex
    int nPriVtx = 0;
    int nPileupVtx = 0;
    for (const xAOD::Vertex* vx : *vxContainer) {
      // Count different types of vertices
      if (vx->vertexType() == xAOD::VxType::PriVtx) nPriVtx++;
      if (vx->vertexType() == xAOD::VxType::PileUp) nPileupVtx++;

      // Select good primary vertex
      if (vx->vertexType() != xAOD::VxType::PriVtx) continue;
      if (vx->numberDoF() <= 0) continue;
      //      std::vector<Trk::VxTrackAtVertex*>* vxTrackAtVertex = (*vxIter)->vxTrackAtVertex();
      m_hPvNTracksAll->Fill(vx->nTrackParticles() != 0 ? vx->nTrackParticles() : -1.);
      if (vx->nTrackParticles() == 0 || vx->nTrackParticles() < m_minTracksPerVtx) continue;

      // Found good VxCandidate to monitor - now fill histograms
      float x = vx->position().x();
      float y = vx->position().y();
      float z = vx->position().z();
      float beamX = beamSpotX + std::tan(beamTiltX) * (z - beamSpotZ);
      float beamY = beamSpotY + std::tan(beamTiltY) * (z - beamSpotZ);
      float beamZ = beamSpotZ;

      m_hPvX->Fill((x - beamX) * scaleFactor);
      m_hPvY->Fill((y - beamY) * scaleFactor);
      m_hPvZ->Fill(z - beamZ);
      //m_hPvErrX->Fill( (*vxIter)->recVertex().errorPosition().error(Trk::x) );  //Check with Anthony
      //m_hPvErrY->Fill( (*vxIter)->recVertex().errorPosition().error(Trk::y) );
      //m_hPvErrZ->Fill( (*vxIter)->recVertex().errorPosition().error(Trk::z) );

      m_hPvErrX->Fill(std::sqrt(vx->covariancePosition()(Trk::x, Trk::x)));
      m_hPvErrY->Fill(std::sqrt(vx->covariancePosition()(Trk::y, Trk::y)));
      m_hPvErrZ->Fill(std::sqrt(vx->covariancePosition()(Trk::z, Trk::z)));

      //m_hPvErrX->Fill(Amg::error((*vxIter)->recVertex().covariancePosition(),Trk::x));   //Why this doesn't work?
      //m_hPvErrY->Fill(Amg::error((*vxIter)->recVertex().covariancePosition(),Trk::y));
      //m_hPvErrZ->Fill(Amg::error((*vxIter)->recVertex().covariancePosition(),Trk::z));



      m_hPvChiSqDoF->Fill(vx->chiSquared() / vx->numberDoF());
      //Coverity report. vxTrackAtVertex cannot be NULL. taking out the ? : and setting to thee vxTrackAtVertex->size()
      // (pbutti: 28Jan15)
      //m_hPvNTracks->Fill( vxTrackAtVertex!=0 ? vxTrackAtVertex->size() : -1. );

      m_hPvNTracks->Fill(vx->nTrackParticles());

      m_hPvXZ->Fill(z, x);
      m_hPvYZ->Fill(z, y);
      m_hPvYX->Fill(x, y);

      // Histograms on original tracks used for primary vertex
      const std::vector< ElementLink< xAOD::TrackParticleContainer > >& tpLinks = vx->trackParticleLinks();
      if (!tpLinks.empty()) {
        for (const auto& tp_elem : tpLinks) {
          const xAOD::TrackParticle* trkp = *tp_elem;
          const Trk::Track* trk = trkp->track();
          //      std::vector<Trk::VxTrackAtVertex*>::iterator trkIter;
          //      for (trkIter=vxTrackAtVertex->begin(); trkIter!=vxTrackAtVertex->end(); ++trkIter) {
          //	  const Trk::ITrackLink* trkLink = (*trkIter)->trackOrParticleLink();
          //	  const Trk::MeasuredPerigee* measuredPerigee = dynamic_cast<const
          // Trk::MeasuredPerigee*>(trk->initialPerigee());
          //	  const Trk::TrackParameters* measuredPerigee = trk->initialPerigee();
          const Trk::TrackParameters* measuredPerigee = trk->perigeeParameters();
          const AmgSymMatrix(5) * covariance = measuredPerigee ? measuredPerigee->covariance() : nullptr;
          m_hPvTrackEta->Fill(measuredPerigee != nullptr && covariance != nullptr ? measuredPerigee->eta() : -999.);
          m_hPvTrackPt->Fill(measuredPerigee != nullptr && covariance ? measuredPerigee->pT() / 1000. : -999.);   // Histo
                                                                                                                  // is
                                                                                                                  // in
                                                                                                                  // GeV,
                                                                                                                  // not
                                                                                                                  // MeV
        }
      }
    }
    m_hPvNPriVtx->Fill(nPriVtx);
    m_hPvNPileupVtx->Fill(nPileupVtx);
  }

  return StatusCode::SUCCESS;
}

StatusCode InDetAlignMonBeamSpot::procHistograms() {
//bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun )
//if( endOfLowStatFlag() || endOfLumiBlockFlag() ) { }

  //if( endOfRunFlag() ) { }

  return StatusCode::SUCCESS;
}

TH1F* InDetAlignMonBeamSpot::makeAndRegisterTH1F(MonGroup& mon,
                                                 const char* hName, const std::string& hTitle, int nBins, float minX,
                                                 float maxX) {
  TH1F* h = new TH1F(hName, hTitle.c_str(), nBins, minX, maxX);

  h->Sumw2();
  if (mon.regHist(h).isFailure()) {
    ATH_MSG_WARNING("Unable to book histogram with name = " + std::string(hName));
  }
  return h;
}

TH2F* InDetAlignMonBeamSpot::makeAndRegisterTH2F(MonGroup& mon,
                                                 const char* hName, const std::string& hTitle,
                                                 int nBinsX, float minX, float maxX,
                                                 int nBinsY, float minY, float maxY) {
  TH2F* h = new TH2F(hName, hTitle.c_str(), nBinsX, minX, maxX, nBinsY, minY, maxY);

  h->Sumw2();
  h->SetOption("colz");
  if (mon.regHist(h).isFailure()) {
    ATH_MSG_WARNING("Unable to book histogram with name = " + std::string(hName));
  }
  return h;
}
