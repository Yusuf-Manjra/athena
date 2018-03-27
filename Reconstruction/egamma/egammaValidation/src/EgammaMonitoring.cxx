/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */

#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AsgTools/MessageCheck.h"
#include "egammaValidation/EgammaMonitoring.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include <AsgTools/AnaToolHandle.h>
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"

#include <vector>
#include <cmath>


EgammaMonitoring :: EgammaMonitoring (const std::string& name, ISvcLocator *pSvcLocator) : 
  AthAlgorithm (name, pSvcLocator),
  m_LooseLH ("AsgElectronLikelihoodTool/LooseLH"),
  m_MediumLH("AsgElectronLikelihoodTool/MediumLH"),
  m_TightLH ("AsgElectronLikelihoodTool/TightLH") 
{
  declareProperty( "sampleType", m_sampleType = "Unknown", "Descriptive name for the processed type of particle" );
}

// ******

StatusCode EgammaMonitoring :: initialize ()
{
  ANA_MSG_INFO ("******************************* Initializing *******************************");

  /// Get Histogram Service ///
  ATH_CHECK(service("THistSvc", rootHistSvc));
  
  ANA_MSG_INFO ("******************* Running over " << m_sampleType << "*******************");

  ANA_MSG_DEBUG ("*******************************  Histo INIT  *******************************");

  evtNmb = new TH1D(); evtNmb->SetName("evtNmb"); evtNmb->SetTitle("Event Number"); 
  CHECK( rootHistSvc->regHist("/MONITORING/evtNmb", evtNmb));
    
  if("electron" == m_sampleType) {

    evtNmb->SetBins(2000, 85000, 87000);

    pT_ElTrk_All  = new TH1D(); pT_ElTrk_All ->SetName("pT_ElTrk_All") ; pT_ElTrk_All ->SetTitle("Electron Pt Track All"); pT_ElTrk_All ->SetBins(200,  0, 200);
    CHECK( rootHistSvc->regHist("/MONITORING/pT_ElTrk_All", pT_ElTrk_All));
    
    pT_ElTrk_LLH  = new TH1D(); pT_ElTrk_LLH ->SetName("pT_ElTrk_LLH") ; pT_ElTrk_LLH ->SetTitle("Electron Pt Track LLH"); pT_ElTrk_LLH ->SetBins(200,  0, 200);
    CHECK( rootHistSvc->regHist("/MONITORING/pT_ElTrk_LLH", pT_ElTrk_LLH));
    
    pT_ElTrk_MLH  = new TH1D(); pT_ElTrk_MLH ->SetName("pT_ElTrk_MLH") ; pT_ElTrk_MLH ->SetTitle("Electron Pt Track MLH"); pT_ElTrk_MLH ->SetBins(200,  0, 200);
    CHECK( rootHistSvc->regHist("/MONITORING/pT_ElTrk_MLH", pT_ElTrk_MLH));
    
    pT_ElTrk_TLH  = new TH1D(); pT_ElTrk_TLH ->SetName("pT_ElTrk_TLH") ; pT_ElTrk_TLH ->SetTitle("Electron Pt Track TLH"); pT_ElTrk_TLH ->SetBins(200,  0, 200);
    CHECK( rootHistSvc->regHist("/MONITORING/pT_ElTrk_TLH", pT_ElTrk_TLH));
    
    eta_ElTrk_All = new TH1D(); eta_ElTrk_All->SetName("eta_ElTrk_All"); eta_ElTrk_All->SetTitle("Electron Eta All")     ; eta_ElTrk_All->SetBins(200, -3,   3);
    CHECK( rootHistSvc->regHist("/MONITORING/eta_ElTrk_All", eta_ElTrk_All));
    
    phi_ElTrk_All = new TH1D(); phi_ElTrk_All->SetName("phi_ElTrk_All"); phi_ElTrk_All->SetTitle("Electron Phi All")     ; 
    phi_ElTrk_All->SetBins(100, -TMath::Pi(), TMath::Pi());    
    CHECK( rootHistSvc->regHist("/MONITORING/phi_ElTrk_All", phi_ElTrk_All));
  
  } // electron Hists

  if("gamma" == m_sampleType) {

    evtNmb->SetBins(250, 33894000, 33896000);
    
    pT_Phot_All  = new TH1D(); pT_Phot_All ->SetName("pT_Phot_All") ; pT_Phot_All ->SetTitle("Photon Pt All") ; pT_Phot_All ->SetBins(200,  0,  200);
    CHECK( rootHistSvc->regHist("/MONITORING/pT_Phot_All", pT_Phot_All));
    
    eta_Phot_All = new TH1D(); eta_Phot_All->SetName("eta_Phot_All"); eta_Phot_All->SetTitle("Photon Eta All"); eta_Phot_All->SetBins(200, -3,    3);
    CHECK( rootHistSvc->regHist("/MONITORING/eta_Phot_All", eta_Phot_All));
    
    phi_Phot_All = new TH1D(); phi_Phot_All->SetName("phi_Phot_All"); phi_Phot_All->SetTitle("Photon Phi All"); phi_Phot_All->SetBins(100, -TMath::Pi(), TMath::Pi());
    CHECK( rootHistSvc->regHist("/MONITORING/phi_Phot_All", phi_Phot_All));
  
  } // gamma Hists

  //*****************LLH Requirement********************
  ANA_CHECK(m_LooseLH.setProperty("WorkingPoint", "LooseLHElectron"));
  ANA_CHECK(m_LooseLH.initialize());
  ANA_MSG_DEBUG ("*******************************  OK LLH Req  *******************************");
  //****************************************************
    
  
  //*****************MLH Requirement********************
  ANA_CHECK(m_MediumLH.setProperty("WorkingPoint", "MediumLHElectron"));
  ANA_CHECK(m_MediumLH.initialize());
  ANA_MSG_DEBUG ("*******************************  OK MLH Req  *******************************");
  //****************************************************

  
  //*****************TLH Requirement********************
  ANA_CHECK(m_TightLH.setProperty("WorkingPoint", "TightLHElectron"));
  ANA_CHECK(m_TightLH.initialize());
  ANA_MSG_DEBUG ("*******************************  OK TLH Req  *******************************");
  //****************************************************

  
  ANA_MSG_DEBUG ("*******************************   END INIT   *******************************");  
  return StatusCode::SUCCESS;
}

// ******

StatusCode EgammaMonitoring :: beginInputFile ()
{
  return StatusCode::SUCCESS;
}

// ******

StatusCode EgammaMonitoring :: firstExecute ()
{
  return StatusCode::SUCCESS;
}

// ******

StatusCode EgammaMonitoring :: execute ()
{
  
  // Retrieve things from the event store

  const xAOD::EventInfo *eventInfo = nullptr;
  ANA_CHECK (evtStore()->retrieve(eventInfo, "EventInfo"));

  if("electron" == m_sampleType) {
    
    const xAOD::ElectronContainer* RecoEl = nullptr;
    if( !evtStore()->retrieve(RecoEl, "Electrons").isSuccess() ) {
      ATH_MSG_ERROR("Failed to retrieve electron container. Exiting.");
      return StatusCode::FAILURE;
    }
    
    for(auto elrec : *RecoEl) {
      
      const xAOD::TrackParticle* tp = elrec->trackParticle();

      if((tp->pt())/1000. > 0) {
	
	pT_ElTrk_All->Fill((tp->pt())/1000.); 
	
	if(m_LooseLH ->accept(elrec)) pT_ElTrk_LLH->Fill((tp->pt())/1000.);
	if(m_MediumLH->accept(elrec)) pT_ElTrk_MLH->Fill((tp->pt())/1000.);
	if(m_TightLH ->accept(elrec)) pT_ElTrk_TLH->Fill((tp->pt())/1000.);
	
      }

      eta_ElTrk_All->Fill(tp->eta());
      phi_ElTrk_All->Fill(tp->phi());
      
    } // RecoEl Loop

  } // if electron


  if("gamma" == m_sampleType) {
    
    const xAOD::PhotonContainer* RecoPh = nullptr;
    if( !evtStore()->retrieve(RecoPh, "Photons").isSuccess() ) {
      ATH_MSG_ERROR("Failed to retrieve photon container. Exiting.");
      return StatusCode::FAILURE;
    }  
    
    for(auto phrec : *RecoPh) {
      
      if((phrec->pt())/1000. > 0) pT_Phot_All->Fill((phrec->pt())/1000.);
      eta_Phot_All->Fill(phrec->eta());
      phi_Phot_All->Fill(phrec->phi());
      
    } // RecoPh Loop

  } // if gamma

  evtNmb->Fill(eventInfo->eventNumber());

  return StatusCode::SUCCESS;
}

// ******

StatusCode EgammaMonitoring :: finalize ()
{
  ANA_MSG_INFO ("******************************** Finalizing ********************************");
  return StatusCode::SUCCESS;
}
