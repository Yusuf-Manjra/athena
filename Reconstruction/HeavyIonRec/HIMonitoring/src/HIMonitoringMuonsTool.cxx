/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */



#include "AthenaMonitoring/AthenaMonManager.h"
#include "HIMonitoringMuonsTool.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODMuon/MuonContainer.h"

HIMonitoringMuonsTool::
 HIMonitoringMuonsTool(const std::string& type, const std::string& name,
                       const IInterface* parent) : ManagedMonitorToolBase(type, name, parent) {
}

HIMonitoringMuonsTool::~HIMonitoringMuonsTool() {
}

// Description: Used for rebooking unmanaged histograms
StatusCode HIMonitoringMuonsTool::bookHistogramsRecurrent( ) {
  return StatusCode::SUCCESS;
}

// Description: Used for re-booking managed histograms
StatusCode HIMonitoringMuonsTool::bookHistograms( ) {
  if (m_environment == AthenaMonManager::online) {
    // book histograms that are only made in the online environment...
  }

  if (m_dataType == AthenaMonManager::cosmics) {
    // book histograms that are only relevant for cosmics data...
  }

  book_hist();


  return StatusCode::SUCCESS;
}

StatusCode HIMonitoringMuonsTool::fillHistograms() {
  StatusCode sc;

  /// HI event shape
  const xAOD::HIEventShapeContainer* evtShape;

  sc = evtStore()->retrieve(evtShape, "HIEventShape");
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not find HIEventShape");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("HIEventShape retrieved from StoreGate");
  }


  const xAOD::MuonContainer* muons = 0;
  sc = evtStore()->retrieve(muons, "Muons");
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not find Muons");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Muons retrieved from StoreGate");
  }

  xAOD::MuonContainer::const_iterator muon_itr = muons->begin();
  xAOD::MuonContainer::const_iterator muon_end = muons->end();

  for (; muon_itr != muon_end; ++muon_itr) {
    // pt cut
    if ((*muon_itr)->pt() * 1.e-3 < 6.) continue;

    xAOD::Muon::Author auth = (*muon_itr)->author();
    float type = (*muon_itr)->muonType();



    const xAOD::TrackParticle* primTrackParticle = (*muon_itr)->primaryTrackParticle();
    const xAOD::TrackParticle* MStrackParticle = (*muon_itr)->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
    const xAOD::TrackParticle* MEtrackParticle = (*muon_itr)->trackParticle(
      xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);
    const xAOD::TrackParticle* IDtrackParticle = (*muon_itr)->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);


    float chi2ndof = 0;
    float chi2ndofID = 0;
    float chi2ndofMS = 0;
    float chi2ndofME = 0;
    if (primTrackParticle) {
      chi2ndof = primTrackParticle->chiSquared() / primTrackParticle->numberDoF();
    }
    if (IDtrackParticle) {
      chi2ndofID = IDtrackParticle->chiSquared() / IDtrackParticle->numberDoF();
    }
    if (MStrackParticle) {
      chi2ndofMS = MStrackParticle->chiSquared() / MStrackParticle->numberDoF();
    }
    if (MEtrackParticle) {
      chi2ndofME = MEtrackParticle->chiSquared() / MEtrackParticle->numberDoF();
    }





    m_h_Auth_fcal->Fill(auth);
    m_h_Type_fcal->Fill(type);
    m_h_Chi2ndf_fcal->Fill(chi2ndof);
    m_h_Chi2ndfID_fcal->Fill(chi2ndofID);
    m_h_Chi2ndfMS_fcal->Fill(chi2ndofMS);
    m_h_Chi2ndfME_fcal->Fill(chi2ndofME);
  }//for



  return StatusCode::SUCCESS;
}

StatusCode HIMonitoringMuonsTool::procHistograms( ) {
  //if( endOfLowStatFlag() || endOfLumiBlock() ) { }

  if (endOfRunFlag()) {
    if (m_h_Auth_fcal->GetEntries() > 0) m_h_Auth_fcal->Scale(1. / m_h_Auth_fcal->GetEntries());
    if (m_h_Type_fcal->GetEntries() > 0) m_h_Type_fcal->Scale(1. / m_h_Type_fcal->GetEntries());
    if (m_h_Chi2ndf_fcal->GetEntries() > 0) m_h_Chi2ndf_fcal->Scale(1. / m_h_Chi2ndf_fcal->GetEntries());
    if (m_h_Chi2ndfID_fcal->GetEntries() > 0) m_h_Chi2ndfID_fcal->Scale(1. / m_h_Chi2ndfID_fcal->GetEntries());
    if (m_h_Chi2ndfMS_fcal->GetEntries() > 0) m_h_Chi2ndfMS_fcal->Scale(1. / m_h_Chi2ndfMS_fcal->GetEntries());
    if (m_h_Chi2ndfME_fcal->GetEntries() > 0) m_h_Chi2ndfME_fcal->Scale(1. / m_h_Chi2ndfME_fcal->GetEntries());
  }

  return StatusCode::SUCCESS;
}

/// *** FCal sum Et *** ///
void HIMonitoringMuonsTool::book_hist() {
  std::string path = "HeavyIon/Muons";

  m_h_Auth_fcal = new TH1D("h_Auth_fcal", ";author;counts", 12, 0, 12);
  regHist(m_h_Auth_fcal, path, run).ignore();

  m_h_Type_fcal = new TH1D("h_Type_fcal", ";type;counts", 5, 0, 5);
  regHist(m_h_Type_fcal, path, run).ignore();

  m_h_Chi2ndf_fcal = new TH1D("h_Chi2ndf_fcal", ";chi2/ndf;counts", 100, 0, 10);
  regHist(m_h_Chi2ndf_fcal, path, run).ignore();

  m_h_Chi2ndfID_fcal = new TH1D("h_Chi2ndfID_fcal", ";ID chi2/ndf;counts", 100, 0, 10);
  regHist(m_h_Chi2ndfID_fcal, path, run).ignore();

  m_h_Chi2ndfMS_fcal = new TH1D("h_Chi2ndfMS_fcal", ";MS chi2/ndf;counts", 100, 0, 10);
  regHist(m_h_Chi2ndfMS_fcal, path, run).ignore();

  m_h_Chi2ndfME_fcal = new TH1D("h_Chi2ndfME_fcal", ";ME chi2/ndf;counts", 100, 0, 10);
  regHist(m_h_Chi2ndfME_fcal, path, run).ignore();
}
