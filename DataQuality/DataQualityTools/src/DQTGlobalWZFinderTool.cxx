/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTGlobalWZFinderTool.cxx
// PACKAGE:  DataQualityTools  
// 
// AUTHORS:   Jahred Adelman (jahred.adelman@cern.ch)
//            Simon Viel (svielcern.ch)
//            Sam King (samking@physics.ubc.ca)
//            Koos van Nieuwkoop (jvannieu@cern.ch)
//
// ********************************************************************

#include "DataQualityTools/DQTGlobalWZFinderTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODEventInfo/EventInfo.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "MuonSelectorTools/IMuonSelectionTool.h"
#include "IsolationSelection/IIsolationSelectionTool.h"

#include <vector>

#include "xAODEventInfo/EventInfo.h"
#include "TProfile.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

using xAOD::Jet;
using xAOD::JetContainer;
using Gaudi::Units::GeV;
using Gaudi::Units::mm;

//----------------------------------------------------------------------------------
DQTGlobalWZFinderTool::DQTGlobalWZFinderTool(const std::string & type, 
		   const std::string & name,
		   const IInterface* parent)
   : DataQualityFatherMonTool(type, name, parent),
     m_muon_Pt(nullptr),
     m_muon_Eta(nullptr),
     m_ele_Et(nullptr),
     m_ele_Eta(nullptr),
     m_electronContainerName("Electrons"),
     m_egDetailContainerName("egDetailAOD"),
     //m_VxPrimContainerName("VxPrimaryCandidate"), //Kshort
     m_VxPrimContainerName("PrimaryVertices"),
     m_VxContainerName("SecVertices"), //Kshort
     m_METName("MET_Reference_AntiKt4EMTopo"),
     m_muonContainerName("Muons"),
     m_jetCollectionName("AntiKt4EMTopoJets"),
     m_tracksName("InDetTrackParticles"), //Kshort
     m_electronEtCut(25),
     m_muonPtCut(25),
     m_metCut(25),
     m_zCutLow(66.0),
     m_zCutHigh(116.0),
     m_muonSelectionTool("CP::MuonSelectionTool/MuonSelectionTool"),
     m_isolationSelectionTool("CP::IsolationSelectionTool/IsolationSelectionTool"),
     m_Jpsi_mm_trigger{"CATEGORY_primary_bphys"},
     m_Z_mm_trigger{"CATEGORY_monitoring_muonIso", "CATEGORY_monitoring_muonNonIso"},
     m_Z_ee_trigger{"CATEGORY_primary_single_ele"}
//----------------------------------------------------------------------------------

{
  m_path = "GLOBAL/DQTGlobalWZFinder";
  declareProperty("ElectronContainerName", m_electronContainerName);
  declareProperty("ElectronDetailContainerName", m_egDetailContainerName);
  declareProperty("MetName", m_METName);
  declareProperty("MuonContainerName",m_muonContainerName);
  declareProperty("JetCollectionName", m_jetCollectionName);
  declareProperty("ElectronEtCut",m_electronEtCut);
  declareProperty("MuonPtCut",m_muonPtCut);
  declareProperty("MetCut",m_metCut);
  declareProperty("ZCutLow", m_zCutLow);
  declareProperty("ZCutHigh", m_zCutHigh);
  declareProperty("doTrigger", m_doTrigger = 1);    
  declareProperty("ZmmTriggers", m_Z_mm_trigger);
  declareProperty("ZeeTriggers", m_Z_ee_trigger);
  declareProperty("MuonSelectionTool", m_muonSelectionTool);
  declareProperty("IsolationSelectionTool", m_isolationSelectionTool);

  m_W_mt_ele = 0;
  m_W_mt_mu = 0;
  m_W_pt_v_met_ele = 0;
  m_W_pt_v_met_mu = 0;
  m_Z_mass_opsele = 0;
  m_Z_mass_opsmu = 0;
  m_Z_mass_ssele = 0;
  m_Z_mass_ssmu = 0;
  m_Z_Q_ele = 0;
  m_Z_Q_mu = 0;
  m_metHist = 0;
  m_metPhiHist = 0;
  
  m_livetime_lb = nullptr;
  m_lblength_lb = nullptr;
  m_mu_lb = nullptr;
  m_Z_ee_trig_ps = nullptr;
  m_Z_mm_trig_ps = nullptr;


}

//----------------------------------------------------------------------------------
DQTGlobalWZFinderTool::~DQTGlobalWZFinderTool()
//----------------------------------------------------------------------------------
{
}

StatusCode DQTGlobalWZFinderTool::bookHistogramsRecurrent()
{ 
  updateTriggersForGroups(m_Jpsi_mm_trigger);
  updateTriggersForGroups(m_Z_mm_trigger);
  updateTriggersForGroups(m_Z_ee_trigger);

  bool failure(false);

  std::string  fullPathDQTGlobalWZFinder=m_path;
  

  const xAOD::EventInfo* thisEventInfo;
  StatusCode sc = evtStore()->retrieve(thisEventInfo);
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Could not find EventInfo in evtStore()");
    return sc;
  } else {
    this_lb = thisEventInfo->lumiBlock() ;
  }

  failure |= registerHist(fullPathDQTGlobalWZFinder, m_livetime_lb = new TProfile("m_livetime_lb", "Livetime", 1, this_lb-0.5, this_lb+0.5), lumiBlock, ATTRIB_UNMANAGED, "merge").isFailure();
  failure |= registerHist(fullPathDQTGlobalWZFinder, m_lblength_lb = new TProfile("m_lblength_lb", "LB length", 1, this_lb-0.5, this_lb+0.5), lumiBlock, ATTRIB_UNMANAGED, "merge").isFailure();
  failure |= registerHist(fullPathDQTGlobalWZFinder, m_mu_lb = new TProfile("m_mu_lb", "#mu", 1, this_lb-0.5, this_lb+0.5), lumiBlock, ATTRIB_UNMANAGED, "merge").isFailure();
  failure |= registerHist(fullPathDQTGlobalWZFinder, m_Z_ee_trig_ps = new TProfile("m_Z_ee_trig_ps", "Z->ee trigger PS", 1, this_lb-0.5, this_lb+0.5), lumiBlock, ATTRIB_UNMANAGED, "merge").isFailure();
  failure |= registerHist(fullPathDQTGlobalWZFinder, m_Z_mm_trig_ps = new TProfile("m_Z_mm_trig_ps", "Z->#mu#mu trigger PS", 1, this_lb-0.5, this_lb+0.5), lumiBlock, ATTRIB_UNMANAGED, "merge").isFailure();
  failure |= registerHist(fullPathDQTGlobalWZFinder, m_ZBosonCounter_El = TH1F_LW::create("m_Z_Counter_el","Z to e e Count per Lumi Block", 1, this_lb-0.5, this_lb+0.5), lumiBlock, ATTRIB_UNMANAGED, "merge").isFailure();
  failure |= registerHist(fullPathDQTGlobalWZFinder, m_ZBosonCounter_Mu = TH1F_LW::create("m_Z_Counter_mu","Z to mu mu Count per Lumi Block", 1, this_lb-0.5, this_lb+0.5), lumiBlock, ATTRIB_UNMANAGED, "merge").isFailure();
  
  if (!failure) {
    m_livetime_lb->Fill(this_lb, lbAverageLivefraction());
    m_lblength_lb->Fill(this_lb, lbDuration());
    m_mu_lb->Fill(this_lb, lbAverageInteractionsPerCrossing());
  }  

  return (failure ? StatusCode::FAILURE : StatusCode::SUCCESS);
}

//----------------------------------------------------------------------------------
StatusCode DQTGlobalWZFinderTool::bookHistograms( )
//StatusCode DQTGlobalWZFinderTool::bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun )
//----------------------------------------------------------------------------------
{
  bool failure(false);

  MsgStream log(msgSvc(), name());
  printedErrorEleContainer = false;
  printedErrorMuColl = false;
  printedErrorMet = false;
  printedErrorTrackContainer = false;

  //Initialize counters for rate monitors
  for (int index =0 ; index < 2 ; index++) {
    JPsiCounterSBG[index]=0;
    JPsiCounter2BG[index]=0;
    JPsiCounterSignal[index]=0;

    UpsilonCounterSBG[index]=0;
    UpsilonCounterBG[index]=0;
    UpsilonCounterSignal[index]=0;

    ZBosonCounterSBG_El[index]=0;
    ZBosonCounterSBG_Mu[index]=0;
  }
  
  //if (newRun) {
  ATH_MSG_DEBUG("in bookHistograms() and m_doRunCosmics = " << m_doRunCosmics << " and m_doRunBeam = " << m_doRunBeam);
  ATH_MSG_DEBUG("Using base path " << m_path);
     
     failure = bookDQTGlobalWZFinderTool();
  //}
  //else if (newEventsBlock || newLumiBlock) {
  //  return StatusCode::SUCCESS;
  //}
  if (failure) {return  StatusCode::FAILURE;}
  else {return StatusCode::SUCCESS;}
}	

		

//----------------------------------------------------------------------------------
bool DQTGlobalWZFinderTool::bookDQTGlobalWZFinderTool()
//----------------------------------------------------------------------------------
{
  bool failure(false);
  //  if (isNewEventsBlock || isNewLumiBlock || isNewRun) {
  MsgStream log(msgSvc(), name());
 
  std::string  fullPathDQTGlobalWZFinder=m_path;
  
  if (m_doRunBeam) {
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_W_mt_ele = TH1F_LW::create("m_W_mt_ele","W#rightarrowe#nu M_{T} ",40,20*1000,400*1000)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_W_mt_mu = TH1F_LW::create("m_W_mt_mu","W#rightarrow#mu#nu M_{T} ",40,20*1000,400*1000)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_W_pt_v_met_ele = TH2F_LW::create("m_W_pt_v_met_ele","W#rightarrowe#nu e E_{T} vs met ",100,5*1000,200*1000, 100, 5*1000, 200*1000)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_W_pt_v_met_mu = TH2F_LW::create("m_W_pt_v_met_mu","W#rightarrow#mu#nu #mu p_{T} vs met ",100,5*1000,200*1000, 100, 5*1000, 200*1000)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_metHist = TH1F_LW::create("m_metHist","MET",50,0*1000,500*1000)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_metPhiHist = TH1F_LW::create("m_metPhiHist","MET phi",10,-1.6,1.6)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_JPsi_mass_opsmu = TH1F_LW::create("m_JPsi_mass_opsmu","JPsi#rightarrow#mu#mu (op. sign) Mass ",40, 2600, 3600)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Upsilon_mass_opsmu = TH1F_LW::create("m_Upsilon_mass_opsmu", "Upsilon#rightarrow#mu#mu", 40, 4500, 14500)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Z_Q_ele = TH1F_LW::create("m_Z_Q_ele","Z#rightarrowee Charge ",7, -3, 3)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Z_Q_mu = TH1F_LW::create("m_Z_Q_mu","Z#rightarrow#mu#mu Charge ",7, -3, 3)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_muon_Pt = TH1F_LW::create("m_muon_Pt", "Selected Muon p_{T}", 50, 0, 200)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_muon_Eta = TH1F_LW::create("m_muon_Eta", "Selected Muon #eta", 50, -2.5, 2.5)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_ele_Et = TH1F_LW::create("m_ele_Et", "Selected Electron E_{T}", 50, 0, 200)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_ele_Eta = TH1F_LW::create("m_ele_Eta", "Selected Electron #eta", 50, -2.5, 2.5)).isFailure();

     int nzbins = int(ceilf( m_zCutHigh - m_zCutLow ));
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Z_mass_opsele = TH1F_LW::create("m_Z_mass_opsele","Z#rightarrowee (op. sign) Mass ", nzbins, m_zCutLow*GeV, m_zCutHigh*GeV), lumiBlock).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Z_mass_opsmu = TH1F_LW::create("m_Z_mass_opsmu","Z#rightarrow#mu#mu (op. sign) Mass ", nzbins, m_zCutLow*GeV, m_zCutHigh*GeV), lumiBlock).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Z_mass_ssele = TH1F_LW::create("m_Z_mass_ssele","Z#rightarrowee (same sign) Mass ", nzbins, m_zCutLow*GeV, m_zCutHigh*GeV), lumiBlock).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Z_mass_ssmu = TH1F_LW::create("m_Z_mass_ssmu","Z#rightarrow#mu#mu (same sign) Mass ", nzbins, m_zCutLow*GeV, m_zCutHigh*GeV), lumiBlock).isFailure();
     
     //Resonance particle rate monitoring
     minLumiBlock  = 0.0;
     maxLumiBlock = 1200.0;
     numBins = 30.0;
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_JPsiCounter_Mu   = TH1F_LW::create("m_JPsi_Counter_mu", "JPsi to mu mu Count per Lumi Block", numBins, minLumiBlock+0.5, maxLumiBlock+0.5)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_UpsilonCounter_Mu   = TH1F_LW::create("m_Upsilon_Counter_mu", "Upsilon to mu mu Count per Lumi Block", numBins, minLumiBlock+0.5, maxLumiBlock+0.5)).isFailure();

  }
   
  if (failure) {
     log << MSG::ERROR << "Error Booking histograms " << endreq;
  }

  return failure;

}


//----------------------------------------------------------------------------------
StatusCode DQTGlobalWZFinderTool::fillHistograms()
//----------------------------------------------------------------------------------
{
  MsgStream log(msgSvc(), name());

  if (m_doRunBeam) {  
     StatusCode sc = StatusCode::SUCCESS;

     //Get LumiBlock and EventNumber
     const xAOD::EventInfo* thisEventInfo;
     sc = evtStore()->retrieve(thisEventInfo);
     if(sc.isFailure()) 
       {
	 log << MSG::ERROR << "Could not find EventInfo in evtStore()" << endreq;
	 return sc;
       }
     else
       {
	 this_lb = thisEventInfo->lumiBlock() ;
	 eventNumber  = thisEventInfo->eventNumber();
       }

     Float_t evtWeight = 1;
     if (thisEventInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
       evtWeight = thisEventInfo->mcEventWeight();
       ATH_MSG_DEBUG("Event Weight: " << evtWeight);
     }
  
     //Get MET
     
     Double_t phiMet = 0, metMet = 0;
     //     const MissingET *missET;
     const xAOD::MissingETContainer *missETcont(0);
     const xAOD::MissingET* missET;
     if ( evtStore()->contains<xAOD::MissingETContainer>(m_METName) ) {
       evtStore()->retrieve(missETcont,m_METName);
       missET = (*missETcont)["FinalClus"];
       if (!missET){
	 ATH_MSG_WARNING("Cannot retrieve xAOD::MissingET " << m_METName);
	 if (!printedErrorMet)
	   ATH_MSG_WARNING("Cannot retrieve " << m_METName);
	 printedErrorMet = true;
       }
       else {
	 phiMet = missET->phi();
	 metMet = missET->met();
       }
     }
     else {
        if (!printedErrorMet)
           log << MSG::WARNING << "Cannot retrieve xAOD::MissingETContainer " << m_METName << endreq;
        printedErrorMet = true;
     }

     ATH_MSG_DEBUG(" MET = " << metMet << " and met phi = " << phiMet);


     //Get Electrons
     
     //     const ElectronContainer* elecTES = 0;
     const xAOD::ElectronContainer* elecTES = 0;
     if ( evtStore()->contains<xAOD::ElectronContainer>(m_electronContainerName) ) {
        sc=evtStore()->retrieve( elecTES, m_electronContainerName);
        if( sc.isFailure()  ||  !elecTES ) {
	  if (!printedErrorEleContainer) log << MSG::WARNING << "No electron container" <<  m_electronContainerName << " found in evtStore" << endreq; 
           printedErrorEleContainer = true;
           return StatusCode::SUCCESS;
        }
     }
     else {
        if (!printedErrorEleContainer) log << MSG::WARNING << "No electron container" <<  m_electronContainerName << " found in evtStore" << endreq; 
        printedErrorEleContainer = true;
        return StatusCode::SUCCESS;
     }
     
     ATH_MSG_DEBUG("ElectronContainer successfully retrieved");


     //Get Muons
        
     //     const Analysis::MuonContainer* muons  = 0;
     const xAOD::MuonContainer* muons  = 0;
     if(evtStore()->contains<xAOD::MuonContainer>(m_muonContainerName)){
        sc = evtStore()->retrieve(muons,m_muonContainerName);
        if (sc.isFailure()) {
           if (!printedErrorMuColl) log << MSG::WARNING << "No muon collection with name " << m_muonContainerName << " found in evtStore" << endreq;
           printedErrorMuColl = true;
           return StatusCode::SUCCESS;
        }
     } else {
        if (!printedErrorMuColl) log << MSG::WARNING << "evtStore() does not contain muon Collection with name "<< m_muonContainerName <<" " <<endreq;
        printedErrorMuColl = true;
        return StatusCode::SUCCESS;
     }

     ATH_MSG_DEBUG("Got muon collection! ");

     std::vector<const xAOD::Electron*> goodelectrons;
     std::vector<const xAOD::Muon*> goodmuonsZ;
     std::vector<CLHEP::HepLorentzVector> goodmuonsJPsi;
     std::vector<Int_t> goodmuonJPsicharge;

     //get primary vertex info
     //const VxContainer *m_vertices;
     const xAOD::VertexContainer* m_vertices(0);
     //bool vertexCut = false;
     
     //EP CHECK( m_xaodConverter.retrieve() );

     xAOD::Vertex* pVtx(0);
     if ( evtStore()->contains<xAOD::VertexContainer>(m_VxPrimContainerName)) {
       sc = evtStore()->retrieve(m_vertices,m_VxPrimContainerName);

       if (sc.isFailure()) {
	 log << MSG::WARNING << "No collection with name " << m_VxPrimContainerName << " found in evtStore()" << endreq;
	 //return StatusCode::SUCCESS;
       }
       else {
	 ATH_MSG_DEBUG("Collection with name " << m_VxPrimContainerName << " with size " << m_vertices->size() << " found in evtStore()");
	 xAOD::VertexContainer::const_iterator vxItr = m_vertices->begin();
	 xAOD::VertexContainer::const_iterator vxItrE = m_vertices->end();
	 for (; vxItr != vxItrE; ++vxItr) {
           //if (! ((*vxItr)->vxTrackAtVertexAvailable())) continue;
	   if ((*vxItr)->vertexType() == xAOD::VxType::PriVtx) {
	     pVtx = *vxItr;
	   }
	 }
       }
     }



     //MET cleaning
     bool isBad = false;
     const JetContainer* jetTES;
     //std::string m_jetCollectionName = "AntiKt4TopoEMJets";
     bool printedErrorJetCollection = false;
     if (evtStore()->contains<JetContainer>(m_jetCollectionName)) {
       //       log << MSG::DEBUG << "Found JetCollection!" << endreq;
       sc=evtStore()->retrieve(jetTES,m_jetCollectionName);
	      
       //MET cleaning
       //JetContainer::const_iterator fJet = jetTES->begin();
       //JetContainer::const_iterator fJetE = jetTES->end();
       
       //       for (; fJet != fJetE; ++fJet) //loop over jet collection
	 //	 {
       //	   Jet::signalstate_t s = (*fJet)->signalState(); //save the signal state
       //	   JetSignalState::setState(*fJet, P4SignalState::JETEMSCALE); //switch to the em scale
       //	   JetSignalState::setState(*fJet, s); //switch to the initial scale
       //	   isBad = JetCaloQualityUtils::isBad(*fJet); //test jet
       //	   if (isBad) break;
       //	 }
       //
     }
     else {
       log << MSG::WARNING << "Didn't find JetCollection" << endreq;  
       if (!printedErrorJetCollection) log << MSG::WARNING << "No jet collection found in TDS" << endreq;
       printedErrorJetCollection = true;
       isBad = true;
       return StatusCode::SUCCESS;
     }
     ATH_MSG_DEBUG("JetCollection successfully retrieved");
     

     // Electron Cut Flow
     Int_t El_N = 0;
     ATH_MSG_DEBUG("Start electron selection");

     //TLorentzVector* tlv_e = new TLorentzvector();
     for (xAOD::ElectronContainer::const_iterator itr=elecTES->begin(); itr != elecTES->end(); ++itr) {
       //Float_t eta = (*itr)->eta();
        bool passSel = false;
        if (!((*itr)->passSelection(passSel, "LHLoose"))) ATH_MSG_WARNING("Electron ID WP Not Defined");
	auto elTrk = (*itr)->trackParticle();
	if (!elTrk) {
	  ATH_MSG_WARNING("Electron with no track particle " << thisEventInfo->runNumber() << " " << thisEventInfo->eventNumber());
	  continue;
	}
	float d0sig;
	try {
	  d0sig = xAOD::TrackingHelpers::d0significance(elTrk, thisEventInfo->beamPosSigmaX(), thisEventInfo->beamPosSigmaY(), thisEventInfo->beamPosSigmaXY());
	} catch (...) {
	  ATH_MSG_DEBUG("Invalid beamspot - electron");
	   try {
	     d0sig = xAOD::TrackingHelpers::d0significance(elTrk);
	   } catch (...) {
	     ATH_MSG_WARNING("Ridiculous exception thrown - electron");
	     continue;
	   }
	}
	/*
	ATH_MSG_INFO("Electron accept: " << passSel);
	ATH_MSG_INFO("Electron eta: " << (*itr)->caloCluster()->etaBE(2));
	ATH_MSG_INFO("Electron pt: " << (*itr)->pt() << " " << m_electronEtCut*GeV);
	ATH_MSG_INFO("Electron iso: " << m_isolationSelectionTool->accept(**itr));
	ATH_MSG_INFO("Electron d0sig: " << d0sig);
	ATH_MSG_INFO("Electron Good vtx: " << pVtx);
	if (pVtx) 
	  ATH_MSG_INFO("Electron z0sinth: " << fabs((elTrk->z0()+elTrk->vz()-pVtx->z())*std::sin(elTrk->theta())) << " " << 0.5*mm);
	ATH_MSG_INFO("Electron isBad: " << isBad);
	*/

	if ( ((*itr)->pt() > m_electronEtCut*GeV) &&
             ((*itr)->caloCluster()->etaBE(2) < 2.47) &&
	     passSel &&
	     m_isolationSelectionTool->accept(**itr) &&
	     fabs(d0sig) < 5 &&
	     pVtx && 
	     fabs((elTrk->z0()+elTrk->vz()-pVtx->z())*std::sin(elTrk->theta())) < 0.5*mm &&
	     !isBad
	     ) {

	  ATH_MSG_DEBUG("Good electron");
	  El_N++;
	  
	  m_ele_Et->Fill((*itr)->pt()/GeV, evtWeight);
	  m_ele_Eta->Fill((*itr)->eta(), evtWeight);
	  goodelectrons.push_back(*itr);

        }
     }

     // Muon Cut Flow

     Int_t MuZ_N = 0;
     Int_t MuJPsi_N = 0;
     ATH_MSG_DEBUG("Start muon selection");

     xAOD::MuonContainer::const_iterator muonItr;
     for (muonItr=muons->begin(); muonItr != muons->end(); ++muonItr) {
         Float_t minptCutJPsi(1.0*CLHEP::GeV);
	 auto muTrk = (*muonItr)->primaryTrackParticle();
	 float d0sig;
	 if (!muTrk) {
	   ATH_MSG_WARNING("No muon track! " << thisEventInfo->runNumber() << " " << thisEventInfo->eventNumber());
	   continue;
	 }
	 try {
	   d0sig = xAOD::TrackingHelpers::d0significance(muTrk, thisEventInfo->beamPosSigmaX(), thisEventInfo->beamPosSigmaY(), thisEventInfo->beamPosSigmaXY());
	 } catch (...) {
	   ATH_MSG_DEBUG("Invalid beamspot - muon");
	   try {
	     d0sig = xAOD::TrackingHelpers::d0significance(muTrk);
	   } catch (...) {
	     ATH_MSG_WARNING("Ridiculous exception thrown - muon");
	     continue;
	   }
	 }
	 
	 
	 ATH_MSG_DEBUG("Muon accept: " << m_muonSelectionTool->accept(**muonItr));
	 ATH_MSG_DEBUG("Muon pt: " << (*muonItr)->pt() << " " << m_muonPtCut*GeV);
	 ATH_MSG_DEBUG("Muon iso: " << m_isolationSelectionTool->accept(**muonItr));
	 ATH_MSG_DEBUG("Muon d0sig: " << d0sig);
	 ATH_MSG_DEBUG("Muon Good vtx: " << pVtx);
	 if (pVtx) 
	   ATH_MSG_DEBUG("Muon z0sinth: " << fabs((muTrk->z0()+muTrk->vz()-pVtx->z())*std::sin(muTrk->theta())) << " " << 0.5*mm);
	 ATH_MSG_DEBUG("Muon isBad: " << isBad);
	 
         if ( m_muonSelectionTool->accept(**muonItr) &&
              ((*muonItr)->pt() > m_muonPtCut*GeV) &&
	      m_isolationSelectionTool->accept(**muonItr) &&
	      fabs(d0sig) < 3 &&
	      pVtx &&
	      fabs((muTrk->z0()+muTrk->vz()-pVtx->z())*std::sin(muTrk->theta())) < 0.5*mm &&
	      !isBad
	      ) {

             MuZ_N++;

	     m_muon_Pt->Fill((*muonItr)->pt()/GeV, evtWeight);
	     m_muon_Eta->Fill((*muonItr)->eta(), evtWeight);
             goodmuonsZ.push_back(*muonItr);
         }
         if ( ((*muonItr)->pt() > minptCutJPsi) ) {
             MuJPsi_N++;

             Float_t px = (*muonItr)->p4().Px();
             Float_t py = (*muonItr)->p4().Py();
             Float_t pz = (*muonItr)->p4().Pz();
             Float_t e = (*muonItr)->p4().E();
             Int_t charge = 0;//(Int_t)((*muonItr)->charge()); // TODO update when xAODMuon-00-06-00

             CLHEP::HepLorentzVector thislepton;
             thislepton.setPx(px);
             thislepton.setPy(py);
             thislepton.setPz(pz);
             thislepton.setE(e);
             goodmuonsJPsi.push_back(thislepton);
             goodmuonJPsicharge.push_back(charge);

         }


     }


     // Check Sum of Candidate Leptons, Return if None


     ATH_MSG_DEBUG("Candidate e+mu = " << El_N+MuZ_N+MuJPsi_N); 
     if ((El_N + MuZ_N + MuJPsi_N) < 1) return sc;

     // Sort Candidates by Pt


     const xAOD::Electron* leadingEle(0);
     const xAOD::Electron*  subleadingEle(0);
     const xAOD::Muon* leadingMuZ(0);
     const xAOD::Muon* subleadingMuZ(0);
     CLHEP::HepLorentzVector leadingMuJPsi;
     CLHEP::HepLorentzVector subleadingMuJPsi;
     
     leadingMuJPsi.setPerp(0);
     subleadingMuJPsi.setPerp(0);
     
     Int_t leadingMuJPsicharge = 0;
     Int_t subleadingMuJPsicharge = 0;


     ATH_MSG_DEBUG("Beginning ele loop");
     for (UInt_t iEle = 0; iEle < goodelectrons.size(); iEle++) {
        Float_t pt = goodelectrons[iEle]->pt();
	ATH_MSG_DEBUG("Ele pt " << pt);
        if (! leadingEle || pt > leadingEle->pt()) {
           subleadingEle = leadingEle;
           leadingEle = goodelectrons[iEle];
        }
        else if (! subleadingEle || pt > subleadingEle->pt()) {
           subleadingEle = goodelectrons[iEle];
        }
     }
     ATH_MSG_DEBUG("Done ele loop");
     ATH_MSG_DEBUG("Start mu Z loop");
     for (UInt_t iMu = 0; iMu < goodmuonsZ.size(); iMu++) {
        Float_t pt = goodmuonsZ[iMu]->pt();
        if (! leadingMuZ || pt > leadingMuZ->pt()) {
           subleadingMuZ = leadingMuZ;
           leadingMuZ = goodmuonsZ[iMu];
        }
        else if (! subleadingMuZ || pt > subleadingMuZ->pt()) {
           subleadingMuZ = goodmuonsZ[iMu];
        }
     }
     ATH_MSG_DEBUG("Done mu Z loop");
     ATH_MSG_DEBUG("Start mu J/psi loop");
     for (UInt_t iMu = 0; iMu < goodmuonsJPsi.size(); iMu++) {
        Float_t pt = goodmuonsJPsi[iMu].perp();
        if (pt > leadingMuJPsi.perp()) {
           subleadingMuJPsi = leadingMuJPsi;
           subleadingMuJPsicharge = leadingMuJPsicharge;
           leadingMuJPsi = goodmuonsJPsi[iMu];
           leadingMuJPsicharge = goodmuonJPsicharge[iMu];
        }
        else if (pt > subleadingMuJPsi.perp()) {
           subleadingMuJPsi = goodmuonsJPsi[iMu];
           subleadingMuJPsicharge = goodmuonJPsicharge[iMu];
        }
     }
     ATH_MSG_DEBUG("End mu J/psi loop");
     
     ATH_MSG_DEBUG("Fill MET");
     //MET histograms
     m_metHist->Fill(metMet, evtWeight);
     m_metPhiHist->Fill(phiMet, evtWeight);
     ATH_MSG_DEBUG("End MET");
     
     // Z Mass

     bool isZee = (goodelectrons.size() > 1);
     bool isZmumu = (goodmuonsZ.size() > 1);
     bool isJPsimumu = (goodmuonsJPsi.size() > 1);
     ATH_MSG_DEBUG("Evaluated Event");

     if (isZee && trigChainsArePassed(m_Z_ee_trigger)) {
       ATH_MSG_DEBUG("Zee found");
       TLorentzVector Zee = (leadingEle->p4() + subleadingEle->p4());
       Float_t mass = Zee.M();
       Int_t Zeecharge = leadingEle->charge() + subleadingEle->charge();
       
       if (mass > m_zCutLow*GeV && mass < m_zCutHigh*GeV ) {
	 m_Z_Q_ele->Fill(Zeecharge, evtWeight);
	 ATH_MSG_DEBUG( "Found a Z to ee candidate!  Mass = " << mass << ", and charge = " << Zeecharge );
	 if (Zeecharge == 0) {
	   m_Z_mass_opsele->Fill(mass, evtWeight);
	   m_ZBosonCounter_El->Fill(this_lb, evtWeight);
	   ++ZBosonCounterSBG_El[0];
	 } else {
	   m_Z_mass_ssele->Fill(mass, evtWeight);
	 }
       }
     }

     if (isZmumu && trigChainsArePassed(m_Z_mm_trigger)) {
       ATH_MSG_DEBUG("Zmumu found");
       TLorentzVector Zmumu = leadingMuZ->p4() + subleadingMuZ->p4();
       Float_t mass = Zmumu.M();
       Int_t Zmumucharge = leadingMuZ->charge() + subleadingMuZ->charge();

	if (mass > m_zCutLow*GeV && mass < m_zCutHigh*GeV) {
	  m_Z_Q_mu->Fill(Zmumucharge, evtWeight);
	  ATH_MSG_DEBUG("Found a Z to mumu candidate!  Mass = " << mass << ", and charge = " << Zmumucharge);
	  if (Zmumucharge == 0) {
	    m_Z_mass_opsmu->Fill(mass, evtWeight);
	    m_ZBosonCounter_Mu->Fill(this_lb, evtWeight);
	    ++ZBosonCounterSBG_Mu[0];
	  } else {
	    m_Z_mass_ssmu->Fill(mass, evtWeight);
	  }
	}
     }     
     

     //JPsi and Upsilon counter
     if (isJPsimumu && trigChainsArePassed(m_Jpsi_mm_trigger)) {
       ATH_MSG_DEBUG("Jpsi mm found");
         CLHEP::HepLorentzVector JPsimumu = leadingMuJPsi + subleadingMuJPsi;
         Float_t mass = JPsimumu.m();
         Int_t JPsimumucharge = leadingMuJPsicharge + subleadingMuJPsicharge;

        
         if (JPsimumucharge == 0) {
             m_JPsi_mass_opsmu->Fill(mass, evtWeight);
             m_Upsilon_mass_opsmu->Fill(mass, evtWeight);

		 

             if (mass > 3038 && mass < 3156) {
                 ++JPsiCounterSBG[0];
		 if(m_doTrigger) {
		   ATH_MSG_DEBUG("ABOUT TO DO BAD THINGS");
		   //ATH_MSG_DEBUG("Pointer?" << getTrigDecTool());
		   if( trigChainsArePassed(m_Jpsi_mm_trigger) ) {
		     ++JPsiCounterSBG[1];
		   }
		 }
             }
             if ((mass > 2802 && mass < 2920) || (mass > 3274 && mass < 3391)) {
                 ++JPsiCounter2BG[0];
		 if(m_doTrigger) {
		   //if( m_trigDecTool->isPassed("EF_2mu6_Jpsimumu") ) {
		   //  ++JPsiCounter2BG[1];
		   //}
		 }
             }
             //assuming JPsi mass of 3 097 MeV and range of 2 sigma (sigma = 59 MeV, based on root best fit of distribution)
	     if (mass > 8242 && mass < 10678) {
                 ++UpsilonCounterSBG[0];
		 if(m_doTrigger) {
		   //if( m_trigDecTool->isPassed("EF_2mu6_Upsimumu") ) {
		   //  ++UpsilonCounterSBG[1];
		   //}
		 }
             }
             if ((mass > 4588 && mass < 5806) || (mass > 13114 && mass < 14332)) {
                 ++UpsilonCounterBG[0];
		 if(m_doTrigger) {
		   //if( m_trigDecTool->isPassed("EF_2mu6_Upsimumu") ) {
		   //  ++UpsilonCounterBG[1];
		   //}
		 }
             }
             //assuming Upsilon mass of 9 460 MeV and range of 2 sigma (sigma = 1218 MeV)


         }
         //else {
         //    m_Z_mass_ssmu->Fill(mass);
         //}
         ATH_MSG_DEBUG("Found a JPsi/Upsilon to mumu candidate!  Mass = " << mass << ", and charge = " << JPsimumucharge);
     }  

     // W Transverse Mass

     Float_t metx = metMet*cos(phiMet);
     Float_t mety = metMet*sin(phiMet);
     Float_t m_mtCut = 40*GeV;

     bool isWenu =  ((goodelectrons.size()  > 0) && (metMet > m_metCut*GeV) && !isZee);
     bool isWmunu = ((goodmuonsZ.size() > 0) && (metMet > m_metCut*GeV) && !isZmumu);


     if (isWenu) {
       ATH_MSG_DEBUG("Wenu found " << leadingEle);
       Float_t px = leadingEle->p4().Px();
       Float_t py = leadingEle->p4().Py();
       Float_t pz = leadingEle->p4().Pz();
        Float_t p = sqrt(px*px+py*py+pz*pz);
        Float_t et = leadingEle->e()*leadingEle->pt()/p;
        Float_t mt = sqrt(2*(et*metMet-px*metx-py*mety));     

	if (mt > m_mtCut)
	{
	  m_W_pt_v_met_ele->Fill(leadingEle->pt(), metMet, evtWeight);
	  m_W_mt_ele->Fill(mt, evtWeight);
	  ATH_MSG_DEBUG("Found a W to enu candidate!  M_T = " << mt << ", and MET = " << metMet << ", and ele_pt = " << leadingEle->pt());
	}
     }

     if (isWmunu) {
       ATH_MSG_DEBUG("Wmunu found");
       Float_t px = leadingMuZ->p4().Px();
       Float_t py = leadingMuZ->p4().Py();
       Float_t pz = leadingMuZ->p4().Pz();
       Float_t p = sqrt(px*px+py*py+pz*pz);
       Float_t et = leadingMuZ->e()*leadingMuZ->pt()/p;
       Float_t mt = sqrt(2*(et*metMet-px*metx-py*mety));
       
       if (mt > m_mtCut)
	 {
	   m_W_pt_v_met_mu->Fill(leadingMuZ->pt(), metMet, evtWeight);
	   m_W_mt_mu->Fill(mt, evtWeight);
	   ATH_MSG_DEBUG("Found a W to munu candidate!  M_T = " << mt << ", and MET = " << metMet << ", and mu_pt = " << leadingMuZ->pt());
	 }
     }

     return sc;
 
  } //end ifBeam  
  
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------
StatusCode DQTGlobalWZFinderTool::procHistograms( )
//StatusCode DQTGlobalWZFinderTool::procHistograms( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun )
//----------------------------------------------------------------------------------
{
  //if ( endOfEventsBlock || endOfLumiBlock || endOfRun ) {
  if ( endOfLumiBlockFlag() || endOfRunFlag() ) {
  MsgStream log(msgSvc(), name());

  ATH_MSG_DEBUG("in finalHists()");
  }


   //---Filling rate monitoring histograms---//
  if (endOfLumiBlockFlag()) {
         JPsiCounterSignal[0] = JPsiCounterSBG[0] - JPsiCounter2BG[0]/2;
         JPsiCounterSignal[1] = JPsiCounterSBG[1] - JPsiCounter2BG[1]/2;

         UpsilonCounterSignal[0] = UpsilonCounterSBG[0] - UpsilonCounterBG[0];
         UpsilonCounterSignal[1] = UpsilonCounterSBG[1] - UpsilonCounterBG[1];

         if (m_doRunBeam) {
             //Map lumiBlock to corresponding bin
             int binNumber = int((this_lb-0.50)/((maxLumiBlock - minLumiBlock)/numBins))+1;

	     //JPsi->mu mu
	     if(JPsiCounterSignal[0] < 0) {
	       JPsiCounterSignal[0] = 0;
	     }
	     if(JPsiCounterSignal[1] < 0) {
	       JPsiCounterSignal[1] = 0;
	     }

	     for(int i = 0 ; i < JPsiCounterSignal[0] ; i++) {
	       m_JPsiCounter_Mu->Fill(this_lb); 
	     }
	     m_JPsiCounter_Mu->SetBinError(binNumber, TMath::Sqrt(m_JPsiCounter_Mu->GetBinContent( binNumber)));

	     //Upsilon->mu mu
             if(UpsilonCounterSignal[0] < 0) {
	       UpsilonCounterSignal[0]= 0;
             }
             if(UpsilonCounterSignal[1] < 0) {
	       UpsilonCounterSignal[1]= 0;
             }

	     for(int i = 0 ; i < UpsilonCounterSignal[0] ; i++) {
	       m_UpsilonCounter_Mu->Fill(this_lb);
	     }
	     m_UpsilonCounter_Mu->SetBinError(binNumber, TMath::Sqrt(m_UpsilonCounter_Mu->GetBinContent( binNumber)));

             //Z->e e
	     if(ZBosonCounterSBG_El[0] < 0) {
	       ZBosonCounterSBG_El[0] = 0;
	     }
	     if(ZBosonCounterSBG_El[1] < 0) {
	       ZBosonCounterSBG_El[1] = 0;
	     }
        }
	 for (int index =0 ; index < 2 ; index++) {
	   JPsiCounterSBG[index]=0;
	   JPsiCounterSBG[index]=0;
	   JPsiCounter2BG[index]=0;
	   JPsiCounterSignal[index]=0;
	   
	   UpsilonCounterSBG[index]=0;
	   UpsilonCounterBG[index]=0;
	   UpsilonCounterSignal[index]=0;
	   
	   ZBosonCounterSBG_El[index]=0;
	   ZBosonCounterSBG_Mu[index]=0;
	 }
   }





  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------
StatusCode DQTGlobalWZFinderTool::checkHists(bool /* fromFinalize */)
//----------------------------------------------------------------------------------
{
  MsgStream log(msgSvc(), name());

  ATH_MSG_DEBUG("in checkHists()");

  return StatusCode::SUCCESS;
}

