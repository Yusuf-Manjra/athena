/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonInputProvider.h"

#include <math.h>

#include "GaudiKernel/ITHistSvc.h"

#include "L1TopoEvent/TopoInputEvent.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/MuCTPIL1Topo.h"
#include "TrigT1Interfaces/MuCTPIL1TopoCandidate.h"

#include "TrigT1Result/MuCTPIRoI.h"
#include "TrigT1Result/Header.h"
#include "TrigT1Result/Trailer.h"

#include "TrigConfData/L1Menu.h"

#include "xAODTrigger/MuonRoI.h"
#include "xAODTrigger/MuonRoIContainer.h"

#include "TrigT1MuctpiBits/HelpersPhase1.h"


using namespace LVL1;
using namespace xAOD;

MuonInputProvider::MuonInputProvider( const std::string& type, const std::string& name, 
                                      const IInterface* parent) :
   base_class(type, name, parent),
   m_histSvc("THistSvc", name)
{
   declareInterface<LVL1::IInputTOBConverter>( this );
}

StatusCode
MuonInputProvider::initialize() {

   // Get the RPC and TGC RecRoI tool
   ATH_CHECK( m_recRPCRoiTool.retrieve() );
   ATH_CHECK( m_recTGCRoiTool.retrieve() );

   CHECK(m_histSvc.retrieve());

   ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "MuonInputProvider");
   CHECK(incidentSvc.retrieve());
   incidentSvc->addListener(this,"BeginRun", 100);
   incidentSvc.release().ignore();

   //This is a bit ugly but I've done it so the job options can be used to determine 
   //use of storegate
   CHECK(m_MuCTPItoL1TopoLocation.initialize(!m_MuCTPItoL1TopoLocation.key().empty()));
   
   if(!m_MuCTPItoL1TopoLocationPlusOne.key().empty())
      m_MuCTPItoL1TopoLocationPlusOne = m_MuCTPItoL1TopoLocation.key()+std::to_string(1);
   
   CHECK(m_MuCTPItoL1TopoLocationPlusOne.initialize(!m_MuCTPItoL1TopoLocationPlusOne.key().empty()));

   CHECK(m_muonROILocation.initialize(!m_muonROILocation.key().empty()));
   CHECK(m_roibLocation.initialize(!m_roibLocation.key().empty()));

   return StatusCode::SUCCESS;
}

void
MuonInputProvider::handle(const Incident& incident) {
   if (incident.type()!="BeginRun") return;
   ATH_MSG_DEBUG( "In BeginRun incident");

   std::string histPath = "/EXPERT/" + name() + "/";
   std::replace( histPath.begin(), histPath.end(), '.', '/'); 

   auto hPt = std::make_unique<TH1I>("MuonTOBPt", "Muon TOB Pt", 40, 0, 40);
   hPt->SetXTitle("p_{T} [GeV]");

   auto hPtTGC = std::make_unique<TH1I>("MuonTOBPtTGC", "TGC Muon TOB Pt", 40, 0, 40);
   hPtTGC->SetXTitle("p_{T} [GeV]");

   auto hPtRPC = std::make_unique<TH1I>("MuonTOBPtRPC", "RPC Muon TOB Pt", 40, 0, 40);
   hPtRPC->SetXTitle("p_{T} [GeV]");

   auto hPtEta = std::make_unique<TH2I>("MuonTOBPtEta", "Muon TOB Pt vs Eta", 200, -200, 200, 40, 0, 40);
   hPtEta->SetXTitle("#eta#times40");
   hPtEta->SetYTitle("p_{T} [GeV]");

   auto hPhiEta = std::make_unique<TH2I>("MuonTOBPhiEta", "Muon TOB Location", 50, -200, 200, 64, 0, 128);
   hPhiEta->SetXTitle("#eta#times40");
   hPhiEta->SetYTitle("#phi#times20");

   auto hBW2or3Eta = std::make_unique<TH2I>("MuonTOBBW2or3Eta", "Muon TOB BW2or3 vs Eta", 200, -200, 200, 3, -1, 2);
   hBW2or3Eta->SetXTitle("#eta#times40");
   hBW2or3Eta->SetYTitle("TGC full-station coincidence");

   auto hInnerCoinEta = std::make_unique<TH2I>("MuonTOBInnerCoinEta", "Muon TOB InnerCoin vs Eta", 200, -200, 200, 3, -1, 2);
   hInnerCoinEta->SetXTitle("#eta#times40");
   hInnerCoinEta->SetYTitle("TGC inner coincidence");

   auto hGoodMFEta = std::make_unique<TH2I>("MuonTOBGoodMFEta", "Muon TOB GoodMF vs Eta", 200, -200, 200, 3, -1, 2);
   hGoodMFEta->SetXTitle("#eta#times40");
   hGoodMFEta->SetYTitle("good magnetic field");

   auto hChargeEta = std::make_unique<TH2I>("MuonTOBChargeEta", "Muon TOB Charge vs Eta", 200, -200, 200, 3, -1, 2);
   hChargeEta->SetXTitle("#eta#times40");
   hChargeEta->SetYTitle("charge");

   auto hIs2candEta = std::make_unique<TH2I>("MuonTOBIs2candEta", "Muon TOB Is2cand vs Eta", 200, -200, 200, 3, -1, 2);
   hIs2candEta->SetXTitle("#eta#times40");
   hIs2candEta->SetYTitle(">1 cand. in RPC pad");

   auto hIsTGCEta = std::make_unique<TH2I>("MuonTOBIsTGCEta", "Muon TOB IsTGC vs Eta", 200, -200, 200, 2, -0.5, 1.5);
   hIsTGCEta->SetXTitle("#eta#times40");
   hIsTGCEta->SetYTitle("Is a TGC muon");

   auto hLateMuonPt = std::make_unique<TH1I>("LateMuonTOBPt", "LateMuon TOB Pt", 40, 0, 40);
   hLateMuonPt->SetXTitle("p_{T} [GeV]");

   auto hLateMuonPtTGC = std::make_unique<TH1I>("LateMuonTOBPtTGC", "TGC LateMuon TOB Pt", 40, 0, 40);
   hLateMuonPtTGC->SetXTitle("p_{T} [GeV]");

   auto hLateMuonPtRPC = std::make_unique<TH1I>("LateMuonTOBPtRPC", "RPC LateMuon TOB Pt", 40, 0, 40);
   hLateMuonPtRPC->SetXTitle("p_{T} [GeV]");

   auto hLateMuonPtEta = std::make_unique<TH2I>("LateMuonTOBPtEta", "LateMuon TOB Pt vs Eta", 200, -200, 200, 40, 0, 40);
   hLateMuonPtEta->SetXTitle("#eta#times40");
   hLateMuonPtEta->SetYTitle("p_{T} [GeV]");

   auto hLateMuonPhiEta = std::make_unique<TH2I>("LateMuonTOBPhiEta", "LateMuon TOB Location", 50, -200, 200, 64, 0, 128);
   hLateMuonPhiEta->SetXTitle("#eta#times40");
   hLateMuonPhiEta->SetYTitle("#phi#times20");

   auto hLateMuonBW2or3Eta = std::make_unique<TH2I>("LateMuonTOBBW2or3Eta", "LateMuon TOB BW2or3 vs Eta", 200, -200, 200, 3, -1, 2);
   hLateMuonBW2or3Eta->SetXTitle("#eta#times40");
   hLateMuonBW2or3Eta->SetYTitle("TGC full-station coincidence");

   auto hLateMuonInnerCoinEta = std::make_unique<TH2I>("LateMuonTOBInnerCoinEta", "LateMuon TOB InnerCoin vs Eta", 200, -200, 200, 3, -1, 2);
   hLateMuonInnerCoinEta->SetXTitle("#eta#times40");
   hLateMuonInnerCoinEta->SetYTitle("TGC inner coincidence");

   auto hLateMuonGoodMFEta = std::make_unique<TH2I>("LateMuonTOBGoodMFEta", "LateMuon TOB GoodMF vs Eta", 200, -200, 200, 3, -1, 2);
   hLateMuonGoodMFEta->SetXTitle("#eta#times40");
   hLateMuonGoodMFEta->SetYTitle("good magnetic field");

   auto hLateMuonChargeEta = std::make_unique<TH2I>("LateMuonTOBChargeEta", "LateMuon TOB Charge vs Eta", 200, -200, 200, 3, -1, 2);
   hLateMuonChargeEta->SetXTitle("#eta#times40");
   hLateMuonChargeEta->SetYTitle("charge");

   auto hLateMuonIs2candEta = std::make_unique<TH2I>("LateMuonTOBIs2candEta", "LateMuon TOB Is2cand vs Eta", 200, -200, 200, 3, -1, 2);
   hLateMuonIs2candEta->SetXTitle("#eta#times40");
   hLateMuonIs2candEta->SetYTitle(">1 cand. in RPC pad");

   auto hLateMuonIsTGCEta = std::make_unique<TH2I>("LateMuonTOBIsTGCEta", "LateMuon TOB IsTGC vs Eta", 200, -200, 200, 2, -0.5, 1.5);
   hLateMuonIsTGCEta->SetXTitle("#eta#times40");
   hLateMuonIsTGCEta->SetYTitle("Is a TGC muon");

  
   if (m_histSvc->regShared( histPath + "MuonTOBPt", std::move(hPt), m_hPt ).isSuccess()){
      ATH_MSG_DEBUG("MuonTOBPt histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register MuonTOBPt histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "MuonTOBPtTGC", std::move(hPtTGC), m_hPtTGC ).isSuccess()){
      ATH_MSG_DEBUG("MuonTOBPtTGC histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register MuonTOBPtTGC histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "MuonTOBPtRPC", std::move(hPtRPC), m_hPtRPC ).isSuccess()){
      ATH_MSG_DEBUG("MuonTOBPtRPC histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register MuonTOBPtRPC histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "MuonTOBPtEta", std::move(hPtEta), m_hPtEta ).isSuccess()){
      ATH_MSG_DEBUG("MuonTOBPtEta histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register MuonTOBPtEta histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "MuonTOBPhiEta", std::move(hPhiEta), m_hPhiEta ).isSuccess()){
      ATH_MSG_DEBUG("MuonTOBPhiEta histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register MuonTOBPhiEta histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "MuonTOBBW2or3Eta", std::move(hBW2or3Eta), m_hBW2or3Eta ).isSuccess()){
      ATH_MSG_DEBUG("MuonTOBBW2or3Eta histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register MuonTOBBW2or3Eta histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "MuonTOBInnerCoinEta", std::move(hInnerCoinEta), m_hInnerCoinEta ).isSuccess()){
      ATH_MSG_DEBUG("MuonTOBInnerCoinEta histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register MuonTOBInnerCoinEta histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "MuonTOBGoodMFEta", std::move(hGoodMFEta), m_hGoodMFEta ).isSuccess()){
      ATH_MSG_DEBUG("MuonTOBGoodMFEta histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register MuonTOBGoodMFEta histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "MuonTOBChargeEta", std::move(hChargeEta), m_hChargeEta ).isSuccess()){
      ATH_MSG_DEBUG("MuonTOBChargeEta histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register MuonTOBChargeEta histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "MuonTOBIs2candEta", std::move(hIs2candEta), m_hIs2candEta ).isSuccess()){
      ATH_MSG_DEBUG("MuonTOBIs2candEta histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register MuonTOBIs2candEta histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "MuonTOBIsTGCEta", std::move(hIsTGCEta), m_hIsTGCEta ).isSuccess()){
      ATH_MSG_DEBUG("MuonTOBIsTGCEta histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register LateMuonTOBIsTGCEta histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "LateMuonTOBPt", std::move(hLateMuonPt), m_hLateMuonPt ).isSuccess()){
      ATH_MSG_DEBUG("LateMuonTOBPt histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register LateMuonTOBPt histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "LateMuonTOBPtTGC", std::move(hLateMuonPtTGC), m_hLateMuonPtTGC ).isSuccess()){
      ATH_MSG_DEBUG("LateMuonTOBPtTGC histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register LateMuonTOBPtTGC histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "LateMuonTOBPtRPC", std::move(hLateMuonPtRPC), m_hLateMuonPtRPC ).isSuccess()){
      ATH_MSG_DEBUG("LateMuonTOBPtRPC histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register LateMuonTOBPtRPC histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "LateMuonTOBPtEta", std::move(hLateMuonPtEta), m_hLateMuonPtEta ).isSuccess()){
      ATH_MSG_DEBUG("LateMuonTOBPtEta histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register LateMuonTOBPtEta histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "LateMuonTOBPhiEta", std::move(hLateMuonPhiEta), m_hLateMuonPhiEta ).isSuccess()){
      ATH_MSG_DEBUG("LateMuonTOBPhiEta histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register LateMuonTOBPhiEta histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "LateMuonTOBBW2or3Eta", std::move(hLateMuonBW2or3Eta), m_hLateMuonBW2or3Eta ).isSuccess()){
      ATH_MSG_DEBUG("LateMuonTOBBW2or3Eta histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register LateMuonTOBBW2or3Eta histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "LateMuonTOBInnerCoinEta", std::move(hLateMuonInnerCoinEta), m_hLateMuonInnerCoinEta ).isSuccess()){
      ATH_MSG_DEBUG("LateMuonTOBInnerCoinEta histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register LateMuonTOBInnerCoinEta histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "LateMuonTOBGoodMFEta", std::move(hLateMuonGoodMFEta), m_hLateMuonGoodMFEta ).isSuccess()){
      ATH_MSG_DEBUG("LateMuonTOBGoodMFEta histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register LateMuonTOBGoodMFEta histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "LateMuonTOBChargeEta", std::move(hLateMuonChargeEta), m_hLateMuonChargeEta ).isSuccess()){
      ATH_MSG_DEBUG("LateMuonTOBChargeEta histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register LateMuonTOBChargeEta histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "LateMuonTOBIs2candEta", std::move(hLateMuonIs2candEta), m_hLateMuonIs2candEta ).isSuccess()){
      ATH_MSG_DEBUG("LateMuonTOBIs2candEta histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register LateMuonTOBIs2candEta histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "LateMuonTOBIsTGCEta", std::move(hLateMuonIsTGCEta), m_hLateMuonIsTGCEta ).isSuccess()){
      ATH_MSG_DEBUG("LateMuonTOBIsTGCEta histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register LateMuonTOBIsTGCEta histogram for MuonProvider");
   }
}

TCS::MuonTOB MuonInputProvider::createMuonTOB(const xAOD::MuonRoI & muonRoI, const std::vector<unsigned int> & rpcPtValues, const std::vector<unsigned int> & tgcPtValues) const{


   float et;
   float eta = muonRoI.eta();
   float phi = muonRoI.phi();

   // WARNING:: 
   // This uses a mapping for thrNumber : thrValue , where the thresholds
   // can change per run, and so the menu used might be different.
   // This should be changed to read from threshold value as soon
   // as it is available.
   // See: https://its.cern.ch/jira/browse/ATR-26165

   int thrNumber = muonRoI.getThrNumber();

   if (muonRoI.getSource() == xAOD::MuonRoI::RoISource::Barrel) { //RPC
      et = rpcPtValues[thrNumber]; //map is in GeV
    } else {
      et = tgcPtValues[thrNumber]; //map is in GeV
    }

   unsigned int EtTopo = et*10;
   int etaTopo = topoIndex(eta,40);
   int phiTopo = topoIndex(phi,20);

   if (phiTopo < 0){ phiTopo += 128; }
   
   TCS::MuonTOB muon( EtTopo, 0, etaTopo, static_cast<unsigned int>(phiTopo), muonRoI.getRoI() );
   muon.setEtDouble(static_cast<double>(EtTopo/10.));
   muon.setEtaDouble(static_cast<double>(etaTopo/40.));
   muon.setPhiDouble(static_cast<double>(phiTopo/20.));

   // Muon flags
   if ( muonRoI.getSource() != xAOD::MuonRoI::RoISource::Barrel) { // TGC ( endcap (E) + forward (F) )
      muon.setBW2or3( topoFlag(muonRoI.getBW3Coincidence()) ); //Needs checking if this is the right flag
      muon.setInnerCoin( topoFlag(muonRoI.getInnerCoincidence()) );
      muon.setGoodMF( topoFlag(muonRoI.getGoodMF()) );
      muon.setCharge( topoFlag(muonRoI.getCharge()) );
      muon.setIs2cand( 0 );
      muon.setIsTGC( 1 );
   }
   else { // RPC ( barrel (B) )
      muon.setBW2or3( 0 );
      muon.setInnerCoin( 0 );
      muon.setGoodMF( 0 );
      muon.setCharge( 0 );
      muon.setIs2cand( topoFlag(muonRoI.isMoreCandInRoI()) );  //Needs checking if this is the right flag
      muon.setIsTGC( 0 );
   }

   m_hPt->Fill( muon.EtDouble() );
   if ( muon.isTGC() ) { m_hPtTGC->Fill( muon.EtDouble() ); }
   else                { m_hPtRPC->Fill( muon.EtDouble() ); }
   m_hPtEta->Fill( muon.eta(), muon.EtDouble() );
   m_hPhiEta->Fill( muon.eta(), muon.phi() );

   m_hBW2or3Eta->Fill( muon.eta(), muon.bw2or3() );
   m_hInnerCoinEta->Fill( muon.eta(), muon.innerCoin() );
   m_hGoodMFEta->Fill( muon.eta(), muon.goodMF() );
   m_hChargeEta->Fill( muon.eta(), muon.charge() );
   m_hIs2candEta->Fill( muon.eta(), muon.is2cand() );
   m_hIsTGCEta->Fill( muon.eta(), muon.isTGC() );

   return muon;
}

TCS::MuonTOB
MuonInputProvider::createMuonTOB(uint32_t roiword, const TrigConf::L1Menu * l1menu) const {

   LVL1::RecMuonRoI roi( roiword, m_recRPCRoiTool.get(), m_recTGCRoiTool.operator->(), l1menu );

   ATH_MSG_DEBUG("Muon ROI: thrvalue = " << roi.getThresholdValue() << " eta = " << roi.eta() << " phi = " << roi.phi() << ", w   = " << MSG::hex << std::setw( 8 ) << roi.roiWord() << MSG::dec);
         
   TCS::MuonTOB muon( roi.getThresholdValue(), 0, int(10*roi.eta()), int(10*roi.phi()), roi.roiWord() );
   muon.setEtaDouble( roi.eta() );
   muon.setPhiDouble( roi.phi() );

   m_hPt->Fill(muon.Et());
   m_hPhiEta->Fill(muon.eta(),muon.phi());
   
   return muon;
}


TCS::MuonTOB
MuonInputProvider::createMuonTOB(const MuCTPIL1TopoCandidate & roi) const {
   ATH_MSG_DEBUG("Muon ROI (MuCTPiToTopo): thr ID = " << roi.getptThresholdID() << " eta = " << roi.geteta() << " phi = " << roi.getphi()  
                  << ", w   = " << MSG::hex << std::setw( 8 ) << roi.getRoiID() << MSG::dec );
   ATH_MSG_DEBUG("                            Oct = " << roi.getMioctID() << " etacode=" <<  roi.getetacode() << " phicode= " <<  
                  roi.getphicode()<< ", Sector="<< roi.getSectorName() );

   // roi.geteta() and roi.getphi() return the the exact geometrical coordinates of the trigger chambers
   // L1Topo granularities are 0.025 for eta (=> inverse = 40) and 0.05 for phi (=> inverse = 20)
   // L1Topo simulation uses positive phi (from 0 to 2pi) => transform phiTopo
   float fEta = roi.geteta();
   float fPhi = roi.getphi();
  
   unsigned int EtTopo = roi.getptValue()*10;
   int etaTopo = topoIndex(fEta,40);
   int phiTopo = topoIndex(fPhi,20);

   if (phiTopo < 0){ phiTopo += 128; }
   
   TCS::MuonTOB muon( EtTopo, 0, etaTopo, static_cast<unsigned int>(phiTopo), roi.getRoiID() );
   muon.setEtDouble(static_cast<double>(EtTopo/10.));
   muon.setEtaDouble(static_cast<double>(etaTopo/40.));
   muon.setPhiDouble(static_cast<double>(phiTopo/20.));

   // Muon flags
   if ( roi.getSectorName().at(0) != 'B' ) { // TGC ( endcap (E) + forward (F) )
      muon.setBW2or3( topoFlag(roi.getbw2or3()) );
      muon.setInnerCoin( topoFlag(roi.getinnerCoin()) );
      muon.setGoodMF( topoFlag(roi.getgoodMF()) );
      muon.setCharge( topoFlag(roi.getcharge()) );
      muon.setIs2cand( 0 );
      muon.setIsTGC( 1 );
   }
   else { // RPC ( barrel (B) )
      muon.setBW2or3( 0 );
      muon.setInnerCoin( 0 );
      muon.setGoodMF( 0 );
      muon.setCharge( 0 );
      muon.setIs2cand( topoFlag(roi.getis2cand()) );
      muon.setIsTGC( 0 );
   }

   m_hPt->Fill( muon.EtDouble() );
   if ( muon.isTGC() ) { m_hPtTGC->Fill( muon.EtDouble() ); }
   else                { m_hPtRPC->Fill( muon.EtDouble() ); }
   m_hPtEta->Fill( muon.eta(), muon.EtDouble() );
   m_hPhiEta->Fill( muon.eta(), muon.phi() );

   m_hBW2or3Eta->Fill( muon.eta(), muon.bw2or3() );
   m_hInnerCoinEta->Fill( muon.eta(), muon.innerCoin() );
   m_hGoodMFEta->Fill( muon.eta(), muon.goodMF() );
   m_hChargeEta->Fill( muon.eta(), muon.charge() );
   m_hIs2candEta->Fill( muon.eta(), muon.is2cand() );
   m_hIsTGCEta->Fill( muon.eta(), muon.isTGC() );

   return muon;
}

TCS::LateMuonTOB
MuonInputProvider::createLateMuonTOB(const MuCTPIL1TopoCandidate & roi) const {


   ATH_MSG_DEBUG("Late Muon ROI (MuCTPiToTopo):bcid=1 thr pt = " << roi.getptThresholdID() << " eta = " << roi.geteta() << " phi = " << roi.getphi() << ", w   = " << MSG::hex << std::setw( 8 ) << roi.getRoiID() << MSG::dec);

   unsigned int EtTopo = roi.getptValue()*10;
   int etaTopo = topoIndex(roi.geteta(),40);
   int phiTopo = topoIndex(roi.getphi(),20);

   if (phiTopo < 0){ phiTopo += 128; }
 
   TCS::LateMuonTOB muon( EtTopo, 0, etaTopo, static_cast<unsigned int>(phiTopo), roi.getRoiID() );

   muon.setEtDouble(static_cast<double>(EtTopo/10.));
   muon.setEtaDouble(static_cast<double>(etaTopo/40.));
   muon.setPhiDouble(static_cast<double>(phiTopo/20.));

   // Muon flags
   if ( roi.getSectorName().at(0) != 'B' ) { // TGC ( endcap (E) + forward (F) )
      muon.setBW2or3( topoFlag(roi.getbw2or3()) );
      muon.setInnerCoin( topoFlag(roi.getinnerCoin()) );
      muon.setGoodMF( topoFlag(roi.getgoodMF()) );
      muon.setCharge( topoFlag(roi.getcharge()) );
      muon.setIs2cand( 0 );
      muon.setIsTGC( 1 );
   }
   else { // RPC ( barrel (B) )
      muon.setBW2or3( 0 );
      muon.setInnerCoin( 0 );
      muon.setGoodMF( 0 );
      muon.setCharge( 0 );
      muon.setIs2cand( topoFlag(roi.getis2cand()) );
      muon.setIsTGC( 0 );
   }

   m_hLateMuonPt->Fill(muon.EtDouble());
   if ( muon.isTGC() ) { m_hLateMuonPtTGC->Fill( muon.EtDouble() ); }
   else                { m_hLateMuonPtRPC->Fill( muon.EtDouble() ); }
   m_hLateMuonPtEta->Fill( muon.eta(), muon.EtDouble() );
   m_hLateMuonPhiEta->Fill( muon.eta(), muon.phi() );

   m_hLateMuonBW2or3Eta->Fill( muon.eta(), muon.bw2or3() );
   m_hLateMuonInnerCoinEta->Fill( muon.eta(), muon.innerCoin() );
   m_hLateMuonGoodMFEta->Fill( muon.eta(), muon.goodMF() );
   m_hLateMuonChargeEta->Fill( muon.eta(), muon.charge() );
   m_hLateMuonIs2candEta->Fill( muon.eta(), muon.is2cand() );
   m_hLateMuonIsTGCEta->Fill( muon.eta(), muon.isTGC() );

   ATH_MSG_DEBUG("LateMuon created");
   return muon;
}

int
MuonInputProvider::topoIndex(float x, int g) const {
  float tmp = x*g;
  float index;
  if ( (abs(tmp)-0.5)/2. == std::round((abs(tmp)-0.5)/2.) ) {
    if ( tmp>0 ) { index = std::floor(tmp); }               
    else { index = std::ceil(tmp); }                      
  }
  else { index = std::round(tmp); }
  return static_cast<int>(index);
}

int
MuonInputProvider::topoFlag(bool flag) const {
  if ( flag == true ) { return 1; }
  else { return -1; }
}

StatusCode
MuonInputProvider::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {

      if (!m_MuonL1RoIKey.empty()) {

        ATH_MSG_DEBUG("Using muon inputs from L1 RoI");

        const TrigConf::L1Menu * l1menu = nullptr;
        ATH_CHECK( detStore()->retrieve(l1menu) );
          
        //Read mapping from menu
        const auto & exMU = l1menu->thrExtraInfo().MU();
        auto rpcPtValues = exMU.knownRpcPtValues();
        auto tgcPtValues = exMU.knownTgcPtValues();

        SG::ReadHandle<xAOD::MuonRoIContainer> muonROIs (m_MuonL1RoIKey);
        for (auto muonRoi : *muonROIs) {

            inputEvent.addMuon( MuonInputProvider::createMuonTOB( *muonRoi, rpcPtValues, tgcPtValues) );

         }
      } else{

      ATH_MSG_DEBUG("Use MuCTPiToTopo granularity Muon ROIs.");

      // first see if L1Muctpi simulation already ran and object is in storegate, if not throw an error

      const LVL1::MuCTPIL1Topo* l1topo  {nullptr};

      if(m_MuCTPItoL1TopoLocation.key().empty()==false){
         SG::ReadHandle<LVL1::MuCTPIL1Topo> l1topoh(m_MuCTPItoL1TopoLocation);
         if( l1topoh.isValid() ) l1topo = l1topoh.cptr();
      }

      if( l1topo ) {
         ATH_MSG_DEBUG("Use MuCTPiToTopo granularity Muon ROIs: retrieve from SG");

         const std::vector<MuCTPIL1TopoCandidate> & candList = l1topo->getCandidates();
         for( const MuCTPIL1TopoCandidate & muCand : candList) {
            inputEvent.addMuon( MuonInputProvider::createMuonTOB( muCand ) );
            if(muCand.moreThan2CandidatesOverflow()){
               inputEvent.setOverflowFromMuonInput(true);
               ATH_MSG_DEBUG("setOverflowFromMuonInput : true (MuCTPIL1TopoCandidate from SG)");
            }
         }
      } else {
 	 ATH_MSG_ERROR("Couldn't retrieve L1Topo inputs from StoreGate");
	 return StatusCode::FAILURE;
      }

      //BC+1 ... this can only come from simulation, in data taking this is collected by the L1Topo at its input
      // so no need to do anything else here
      if(m_MuCTPItoL1TopoLocationPlusOne.key().empty()==false) {
         SG::ReadHandle<LVL1::MuCTPIL1Topo> l1topoBC1(m_MuCTPItoL1TopoLocationPlusOne);
         if( l1topoBC1.isValid() ) {
            ATH_MSG_DEBUG( "Contains L1Topo LateMuons L1Muctpi object from StoreGate!" );
            const std::vector<MuCTPIL1TopoCandidate> & candList = l1topoBC1->getCandidates();
            for( const MuCTPIL1TopoCandidate& muCand : candList)
            {
               ATH_MSG_DEBUG("MuonInputProvider addLateMuon ");
               inputEvent.addLateMuon( MuonInputProvider::createLateMuonTOB( muCand ) );	   
            }
         }
      }
   }
   return StatusCode::SUCCESS;
}
